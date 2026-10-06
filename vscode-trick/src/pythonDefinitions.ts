import * as fs from 'fs';
import * as path from 'path';
import * as vscode from 'vscode';
import { symbolMatchesWord } from './sdefineDefinitions';
import { SimConfigProvider } from './simConfig';
import { SieModel, parseSieResource, unwrapContainerElementType } from './sieResource';

/**
 * Go to Definition for sim variables (`ball.state.input.mass`) and `trick.*`
 * names in input.py/.dr files, pointed at the C++ declaration the SWIG
 * binding was generated from, rather than the generated `.pyi` stub Pylance
 * resolves to (which just re-declares the same name with no useful body).
 *
 * Neither S_sie.resource nor any other Trick-generated artifact records file/
 * line info for a class or member - only `<sim>/build/class_map.cpp` (written
 * by ICG's PrintFileContents10::printClassMap) records the header a class
 * came from, as a `// <path>` comment above its `extern ATTRIBUTES attrX[]`.
 * There's still no line number, so the declaration line is found with a text
 * heuristic scan of that header once it's resolved.
 */

function escapeRegExp(text: string): string {
  return text.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function lineOf(text: string, offset: number): number {
  let line = 0;
  for (let i = 0; i < offset && i < text.length; i++) {
    if (text.charCodeAt(i) === 10 /* \n */) {
      line++;
    }
  }
  return line;
}

// A dotted/indexed access chain, e.g. `ball.state.input.position[0]`. Scans
// raw line text rather than requiring real Python tokenization, so it also
// matches chains written as plain text inside a string literal - e.g. `.dr`
// files' `drg0.add_variable("ball.state.output.position[0]")`, which Pylance
// can't resolve at all since it's just a string to the Python parser.
const CHAIN_RE = /[A-Za-z_]\w*(?:\s*\[[^\]\n]*\])?(?:\s*\.\s*[A-Za-z_]\w*(?:\s*\[[^\]\n]*\])?)*/g;
const SEGMENT_RE = /([A-Za-z_]\w*)(?:\s*\[[^\]\n]*\])?/g;

/**
 * Walks the chain containing [wordStart, wordEnd) on lineText and returns the
 * segment names from the chain's root up to (and truncated at) the clicked
 * segment - e.g. clicking "input" in `ball.state.input.mass` returns
 * `['ball', 'state', 'input']`, since that's the segment being navigated to.
 */
export function parseAccessChain(
  lineText: string,
  wordStart: number,
  wordEnd: number
): string[] | undefined {
  CHAIN_RE.lastIndex = 0;
  let m: RegExpExecArray | null;
  while ((m = CHAIN_RE.exec(lineText))) {
    const chainStart = m.index;
    const chainEnd = chainStart + m[0].length;
    if (wordStart < chainStart || wordEnd > chainEnd) {
      continue;
    }
    const segRe = new RegExp(SEGMENT_RE);
    const segments: { name: string; start: number; end: number }[] = [];
    let sm: RegExpExecArray | null;
    while ((sm = segRe.exec(m[0]))) {
      const start = chainStart + sm.index;
      segments.push({ name: sm[1], start, end: start + sm[1].length });
    }
    const idx = segments.findIndex((s) => wordStart >= s.start && wordStart < s.end);
    if (idx === -1) {
      return undefined;
    }
    return segments.slice(0, idx + 1).map((s) => s.name);
  }
  return undefined;
}

/** Maps mangled SIE class name -> absolute header path, from a class_map.cpp. */
export function parseClassMap(text: string): Map<string, string> {
  const map = new Map<string, string>();
  const re = /\/\/\s*(\S.*?)\s*\r?\n\s*extern\s+ATTRIBUTES\s+attr(\w+)\s*\[\s*\]/g;
  let m: RegExpExecArray | null;
  while ((m = re.exec(text))) {
    map.set(m[2], m[1]);
  }
  return map;
}

export interface ResolvedChainTarget {
  className: string;
  memberName?: string;
}

/**
 * Resolves a chain of sim-variable names (as returned by parseAccessChain)
 * against a parsed SieModel. The chain's root must be a top_level_object;
 * each subsequent name is looked up as a member of the previous class,
 * descending through unwrapped container/pointer types. Returns the class the
 * final (clicked) name belongs to, and that name itself as the member - or,
 * when only the root was clicked, just its class with no member.
 */
export function resolveSimChain(model: SieModel, chain: string[]): ResolvedChainTarget | undefined {
  const root = model.topLevel.find((o) => o.name === chain[0]);
  if (!root) {
    return undefined;
  }
  if (chain.length === 1) {
    return { className: root.type };
  }

  let currentClass = root.type;
  for (let i = 1; i < chain.length; i++) {
    const members = model.classes.get(currentClass);
    const member = members?.find((m) => m.name === chain[i]);
    if (!member) {
      return undefined;
    }
    if (i === chain.length - 1) {
      return { className: currentClass, memberName: member.name };
    }
    const elementType = unwrapContainerElementType(member.type);
    const nextClass = (elementType ?? member.type).replace(/\*+$/, '').trim();
    if (!model.classes.has(nextClass)) {
      return undefined;
    }
    currentClass = nextClass;
  }
  return undefined;
}

function simpleClassName(mangledName: string): string {
  const parts = mangledName.split('__');
  return parts[parts.length - 1];
}

interface ClassRegion {
  /** Offset of the "class"/"struct"/typedef keyword that starts the declaration. */
  declStart: number;
  bodyStart: number;
  bodyEnd: number;
}

function findClassRegion(text: string, name: string): ClassRegion | undefined {
  const classRe = new RegExp(`\\b(?:class|struct)\\s+${escapeRegExp(name)}\\b[^{;]*\\{`);
  const cm = classRe.exec(text);
  if (cm) {
    const bodyStart = cm.index + cm[0].length;
    const closeRe = /\}\s*;/g;
    closeRe.lastIndex = bodyStart;
    const close = closeRe.exec(text);
    return { declStart: cm.index, bodyStart, bodyEnd: close ? close.index : text.length };
  }

  // C-style `typedef struct { ... } Name ;` - no name follows the keyword, so
  // every typedef'd struct in the file has to be scanned for its trailing name.
  const typedefRe = /typedef\s+struct\s*\{([\s\S]*?)\}\s*(\w+)\s*;/g;
  let tm: RegExpExecArray | null;
  while ((tm = typedefRe.exec(text))) {
    if (tm[2] === name) {
      const bodyStart = tm.index + tm[0].indexOf('{') + 1;
      return { declStart: tm.index, bodyStart, bodyEnd: bodyStart + tm[1].length };
    }
  }
  return undefined;
}

export interface DeclarationMatch {
  line: number;
  matchedMember: boolean;
}

/**
 * Finds the line a class (or one of its own members) is declared on, within
 * already-resolved header text. `className` is the mangled SIE name
 * (`Trick__IntegLoop`); only its last `::`-segment is searched for, since
 * `::` isn't how it's actually spelled in the header (namespaces wrap the
 * body instead). Inheritance is flattened by SIE, so a member may actually
 * live in a base class outside this class's own region - callers should fall
 * back to a broader search (e.g. a workspace symbol lookup) when
 * `matchedMember` comes back false but a member name was requested.
 */
export function findDeclarationLine(
  headerText: string,
  className: string,
  memberName?: string
): DeclarationMatch {
  const name = simpleClassName(className);
  const region = findClassRegion(headerText, name);
  if (!region) {
    return { line: 0, matchedMember: false };
  }
  if (memberName) {
    const body = headerText.slice(region.bodyStart, region.bodyEnd);
    const memberRe = new RegExp(`\\b${escapeRegExp(memberName)}\\s*(?:\\[|;|=|,|\\))`);
    const mm = memberRe.exec(body);
    if (mm) {
      return { line: lineOf(headerText, region.bodyStart + mm.index), matchedMember: true };
    }
  }
  return { line: lineOf(headerText, region.declStart), matchedMember: false };
}

/** Finds a top-level sim object's own declaration line in S_define, e.g. `ballSimObject ball ;`. */
export function findTopLevelObjectLine(sDefineText: string, objectName: string): number {
  const re = new RegExp(`^\\s*[\\w:<>,\\s]+\\b${escapeRegExp(objectName)}\\s*;`, 'm');
  const m = re.exec(sDefineText);
  return m ? lineOf(sDefineText, m.index) : 0;
}

export interface TrickDeclaration {
  file: string;
  line: number;
}

/**
 * Finds a `trick.X` name's C++ declaration across a set of header texts
 * (path -> content), in priority order: function prototype, then class/struct
 * definition, then enumerator. Names that are Python-only (e.g. shortcuts.py
 * helpers like `var_get`) correctly find nothing here, leaving Pylance's own
 * result as the sole one.
 */
export function findTrickDeclaration(
  name: string,
  files: Map<string, string>
): TrickDeclaration | undefined {
  const protoRe = new RegExp(`^(?!\\s*(?:\\*|//|@)).*\\b${escapeRegExp(name)}\\s*\\([^;{]*\\)\\s*;`, 'm');
  const classRe = new RegExp(`\\b(?:class|struct)\\s+${escapeRegExp(name)}\\b`);
  const enumRe = new RegExp(`^\\s*${escapeRegExp(name)}\\s*(?:=|,)`, 'm');

  for (const matcher of [protoRe, classRe, enumRe]) {
    for (const [file, text] of files) {
      const m = matcher.exec(text);
      if (m) {
        return { file, line: lineOf(text, m.index) };
      }
    }
  }
  return undefined;
}

interface CacheEntry<T> {
  mtimeMs: number;
  value: T;
}

export class TrickPythonDefinitionProvider implements vscode.DefinitionProvider {
  private readonly sieCache = new Map<string, CacheEntry<SieModel>>();
  private readonly classMapCache = new Map<string, CacheEntry<Map<string, string>>>();
  private trickHeaderCache: { trickHome: string; files: Map<string, string> } | undefined;

  constructor(private readonly simConfigs: SimConfigProvider) {}

  async provideDefinition(
    document: vscode.TextDocument,
    position: vscode.Position,
    token: vscode.CancellationToken
  ): Promise<vscode.Definition | undefined> {
    const wordRange = document.getWordRangeAtPosition(position);
    if (!wordRange) {
      return undefined;
    }
    const lineText = document.lineAt(position.line).text;
    const chain = parseAccessChain(lineText, wordRange.start.character, wordRange.end.character);
    if (!chain || chain.length === 0) {
      return undefined;
    }

    const simRoot = this.simConfigs.findSimRoot(document.uri.fsPath);

    if (chain[0] === 'trick') {
      if (chain.length < 2 || !simRoot) {
        return undefined;
      }
      return this.resolveTrickTarget(simRoot, chain[1]);
    }

    if (!simRoot || token.isCancellationRequested) {
      return undefined;
    }

    return this.resolveSimTarget(simRoot, chain);
  }

  private getSieModel(simRoot: string): SieModel | undefined {
    const siePath = path.join(simRoot, 'S_sie.resource');
    return this.readCached(this.sieCache, siePath, (text) => parseSieResource(text));
  }

  private getClassMap(filePath: string): Map<string, string> | undefined {
    return this.readCached(this.classMapCache, filePath, (text) => parseClassMap(text));
  }

  private readCached<T>(
    cache: Map<string, CacheEntry<T>>,
    filePath: string,
    parse: (text: string) => T
  ): T | undefined {
    let stat: fs.Stats;
    try {
      stat = fs.statSync(filePath);
    } catch {
      return undefined;
    }
    const cached = cache.get(filePath);
    if (cached && cached.mtimeMs === stat.mtimeMs) {
      return cached.value;
    }
    const value = parse(fs.readFileSync(filePath, 'utf8'));
    cache.set(filePath, { mtimeMs: stat.mtimeMs, value });
    return value;
  }

  private resolveHeaderForClass(
    simRoot: string,
    className: string
  ): { file: string; redirectToSDefine: boolean } | undefined {
    const simMap = this.getClassMap(path.join(simRoot, 'build', 'class_map.cpp'));
    let file = simMap?.get(className);
    if (!file) {
      const trickHome = this.simConfigs.resolveTrickHome(simRoot);
      if (trickHome) {
        const frameworkMap = this.getClassMap(
          path.join(trickHome, 'trick_source', 'sim_services', 'include', 'io_src', 'class_map.cpp')
        );
        file = frameworkMap?.get(className);
      }
    }
    if (!file) {
      return undefined;
    }
    return { file, redirectToSDefine: path.basename(file) === 'S_source.hh' };
  }

  private async resolveSimTarget(simRoot: string, chain: string[]): Promise<vscode.Location | undefined> {
    const model = this.getSieModel(simRoot);
    if (!model) {
      return undefined;
    }
    const target = resolveSimChain(model, chain);
    if (!target) {
      return undefined;
    }

    if (!target.memberName) {
      const sDefinePath = path.join(simRoot, 'S_define');
      if (!fs.existsSync(sDefinePath)) {
        return undefined;
      }
      const text = fs.readFileSync(sDefinePath, 'utf8');
      const line = findTopLevelObjectLine(text, chain[0]);
      return new vscode.Location(vscode.Uri.file(sDefinePath), new vscode.Position(line, 0));
    }

    const header = this.resolveHeaderForClass(simRoot, target.className);
    if (!header) {
      return undefined;
    }
    // S_source.hh is itself generated (CP copies each S_define sim-object
    // class's body into it verbatim) - redirect to S_define, the file the
    // user actually wrote the class in.
    const file = header.redirectToSDefine ? path.join(simRoot, 'S_define') : header.file;
    if (!fs.existsSync(file)) {
      return undefined;
    }
    const text = fs.readFileSync(file, 'utf8');
    const match = findDeclarationLine(text, target.className, target.memberName);
    if (match.matchedMember) {
      return new vscode.Location(vscode.Uri.file(file), new vscode.Position(match.line, 0));
    }

    const viaSymbols = await this.findViaWorkspaceSymbols(target.memberName);
    return viaSymbols ?? new vscode.Location(vscode.Uri.file(file), new vscode.Position(match.line, 0));
  }

  private async findViaWorkspaceSymbols(word: string): Promise<vscode.Location | undefined> {
    try {
      const symbols = await vscode.commands.executeCommand<vscode.SymbolInformation[]>(
        'vscode.executeWorkspaceSymbolProvider',
        word
      );
      const match = symbols?.find((s) => symbolMatchesWord(s.name, word));
      return match ? new vscode.Location(match.location.uri, match.location.range) : undefined;
    } catch {
      return undefined;
    }
  }

  private async resolveTrickTarget(simRoot: string, name: string): Promise<vscode.Location | undefined> {
    const trickHome = this.simConfigs.resolveTrickHome(simRoot);
    if (!trickHome) {
      return undefined;
    }
    const files = this.getTrickHeaderTexts(trickHome);
    if (!files) {
      return undefined;
    }
    const found = findTrickDeclaration(name, files);
    if (!found) {
      return undefined;
    }
    return new vscode.Location(vscode.Uri.file(found.file), new vscode.Position(found.line, 0));
  }

  // Indexed once per TRICK_HOME and kept for the session - there's no good
  // invalidation signal (these headers change far less often than sim files),
  // and re-walking/re-reading the whole tree on every Ctrl+click would be slow.
  private getTrickHeaderTexts(trickHome: string): Map<string, string> | undefined {
    if (this.trickHeaderCache?.trickHome === trickHome) {
      return this.trickHeaderCache.files;
    }
    const dir = path.join(trickHome, 'include', 'trick');
    if (!fs.existsSync(dir)) {
      return undefined;
    }
    const files = new Map<string, string>();
    const walk = (current: string): void => {
      const entries = fs.readdirSync(current, { withFileTypes: true }).sort((a, b) =>
        a.name.localeCompare(b.name)
      );
      for (const entry of entries) {
        const full = path.join(current, entry.name);
        if (entry.isDirectory()) {
          walk(full);
        } else if (/\.hh?$/.test(entry.name)) {
          try {
            files.set(full, fs.readFileSync(full, 'utf8'));
          } catch {
            // Unreadable (permissions, broken symlink, etc.) - skip it.
          }
        }
      }
    };
    walk(dir);
    this.trickHeaderCache = { trickHome, files };
    return files;
  }
}
