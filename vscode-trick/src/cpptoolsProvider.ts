import * as path from 'path';
import * as vscode from 'vscode';
import {
  CustomConfigurationProvider,
  SourceFileConfigurationItem,
  WorkspaceBrowseConfiguration,
  Version,
  getCppToolsApi,
  CppToolsApi,
} from 'vscode-cpptools';
import { SimConfig, SimConfigProvider } from './simConfig';

const C_EXTENSIONS = new Set(['.c']);
const CXX_EXTENSIONS = new Set(['.cpp', '.cc', '.cxx', '.hh', '.hpp', '.hxx', '.h']);
// .h is ambiguous between C and C++; Trick models are overwhelmingly C++, so
// treat plain .h as C++ unless the sibling .c file wins out - good enough for v1.

export class TrickCppConfigurationProvider implements CustomConfigurationProvider, vscode.Disposable {
  readonly name = 'Trick';
  readonly extensionId = 'trick-sim.vscode-trick';

  private api: CppToolsApi | undefined;

  constructor(private readonly simConfigs: SimConfigProvider, private readonly output: vscode.OutputChannel) {
    this.simConfigs.onDidInvalidate((simRoot) => {
      this.api?.didChangeCustomConfiguration(this);
      this.api?.didChangeCustomBrowseConfiguration(this);
      this.output.appendLine(`[${simRoot}] configuration invalidated, notified cpptools`);
    });
  }

  notifyBrowseConfigurationChanged(): void {
    this.api?.didChangeCustomBrowseConfiguration(this);
  }

  async activate(): Promise<void> {
    this.api = await getCppToolsApi(Version.v6);
    if (!this.api) {
      this.output.appendLine(
        'ms-vscode.cpptools not found or API version unsupported; skipping IntelliSense integration.'
      );
      return;
    }
    this.api.registerCustomConfigurationProvider(this);
    if (this.api.notifyReady) {
      this.api.notifyReady(this);
    } else {
      this.api.didChangeCustomConfiguration(this);
    }
  }

  dispose(): void {
    this.api?.dispose();
  }

  async canProvideConfiguration(uri: vscode.Uri): Promise<boolean> {
    const ext = path.extname(uri.fsPath);
    if (!C_EXTENSIONS.has(ext) && !CXX_EXTENSIONS.has(ext)) {
      return false;
    }
    return (await this.simConfigs.getConfigForFile(uri.fsPath)) !== undefined;
  }

  async provideConfigurations(uris: vscode.Uri[]): Promise<SourceFileConfigurationItem[]> {
    const items: SourceFileConfigurationItem[] = [];
    for (const uri of uris) {
      const config = await this.simConfigs.getConfigForFile(uri.fsPath);
      if (!config) {
        continue;
      }
      items.push(this.toConfigurationItem(uri, config));
    }
    return items;
  }

  private toConfigurationItem(uri: vscode.Uri, config: SimConfig): SourceFileConfigurationItem {
    const isC = C_EXTENSIONS.has(path.extname(uri.fsPath));
    return {
      uri,
      configuration: {
        includePath: isC ? config.cIncludes : config.cxxIncludes,
        defines: isC ? config.cDefines : config.cxxDefines,
        standard: isC ? 'c99' : (config.cxxStandard as any),
        compilerPath: (isC ? config.cCompilerPath : config.compilerPath) ?? undefined,
      },
    };
  }

  async canProvideBrowseConfiguration(): Promise<boolean> {
    return true;
  }

  async provideBrowseConfiguration(): Promise<WorkspaceBrowseConfiguration> {
    const all = this.simConfigs.getAllCached();
    const browsePath = [...new Set(all.flatMap((c) => [...c.cxxIncludes, ...c.cIncludes]))];
    return { browsePath };
  }

  async canProvideBrowseConfigurationsPerFolder(): Promise<boolean> {
    return false;
  }

  async provideFolderBrowseConfiguration(): Promise<WorkspaceBrowseConfiguration> {
    return this.provideBrowseConfiguration();
  }
}
