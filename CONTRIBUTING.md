# Contributing to Trick

Trick is a simulation development framework maintained by a small team at the
NASA Johnson Space Center. It is used in flight software development, vehicle
design, hardware-in-the-loop testing, and crew training at NASA and at other
agencies, companies, universities, and by individuals around the world.
Changes to Trick affect the correctness of simulations that people depend on.

If you use Trick and have hit a problem or a limitation, we want to hear from
you. The one thing we ask is that every issue and pull request be grounded in
actual use of Trick.

This document describes what the Trick team accepts, how to submit it, and how
submissions that do not meet these guidelines are handled. Please read it in
full before opening an issue or pull request. Maintainers will point to this
document when closing issues or pull requests that do not follow it.

- [Who this project is for](#who-this-project-is-for)
- [Before you open anything](#before-you-open-anything)
- [Issues](#issues)
- [Pull requests](#pull-requests)
- [Use of AI and automated tooling](#use-of-ai-and-automated-tooling)
- [Review and maintainer discretion](#review-and-maintainer-discretion)
- [Contributions that are not accepted](#contributions-that-are-not-accepted)
- [Enforcement](#enforcement)

## Who this project is for

Trick's issue tracker and pull request queue exist to serve **people who build
and run simulations with Trick**. The maintainers are a small team, and their
time is valuable. We want to spend it on problems Trick users actually encounter.

We welcome contributions from anyone, inside or outside NASA, who:

- has installed Trick, built it, and run a simulation with it;
- has encountered a concrete problem or need in the course of doing so; and
- can describe that problem in terms of Trick's behavior and their use of it.

We are not looking for general-purpose "improvements" to the repository from
people who do not use Trick. Contributions are evaluated on the value they
provide to Trick's users, not on volume, effort, or good intentions.

## Before you open anything

1. **Read the documentation.** Start with the
   [Install Guide](https://nasa.github.io/trick/documentation/install_guide/Install-Guide),
   the [Tutorial](https://nasa.github.io/trick/tutorial/Tutorial), the
   [FAQ](https://nasa.github.io/trick/faq/FAQ), and the
   [Documentation home](https://nasa.github.io/trick/documentation/Documentation-Home).
2. **Search existing issues and pull requests**, including closed ones.
3. **Reproduce the problem** against the current `master` branch or the latest
   release, either while building or installing Trick itself, in a Trick
   simulation you actually run, or while following the Trick documentation.
   Problems that are only observed by reading the source, by a static analysis
   tool, or by a language model are not bug reports until they have been
   reproduced.
4. **Be prepared to engage.** Issues and pull requests are conversations.
   Maintainers will ask questions. If you cannot answer them, the submission
   will be closed.

## Issues

Issues must be opened using one of the provided templates. Blank issues are
disabled. Each template asks for specific information; all required fields
must be completed truthfully. Issues with placeholder, boilerplate, or
evidently untrue answers to required fields are closed without further review.

### Bug reports

A bug report must include:

- The Trick version (or commit), operating system and version, and the
  versions of your compiler, LLVM, SWIG, and Python.
- A minimal reproduction. For a build or install failure of Trick itself, the
  exact `configure` and `make` commands and installed dependency versions. For
  a simulation problem, a sim in `trick_sims/` or `test/` plus an input file,
  or a minimal sim of your own. For a documentation error, the page URL, the
  incorrect statement quoted exactly, and the expected correction, with what
  happened when you followed the documentation. "It seems like this could
  break" is not a reproduction.
- What you expected, what happened, and relevant output or logs.
- A brief description of how you use Trick and how this bug affects that use.
  This is required. It is how we prioritize work.

### Feature requests

A feature request must describe the **problem** you cannot solve with Trick
today and the simulation context in which you hit it. Proposed solutions are
welcome but secondary. Requests for features you do not personally need,
generic modernization proposals, and lists of "suggested improvements" are
closed.

A feature request being open does not mean it has been accepted. Do not begin
implementing anything until a maintainer has agreed to it in the issue.

### Questions

Usage questions are acceptable **after** you have checked the documentation.
State what you tried and which documentation you consulted. Questions that
are answered directly by the documentation will be closed with a link.

## Pull requests

### Process

1. **Open an issue first, for every pull request.** There is no exception
   for small or "trivial" changes. Wait for a maintainer to agree in the
   issue that the change is wanted and that the approach is acceptable before
   opening a pull request. This protects your time as much as ours: it is far
   better to learn in a two-line issue that a change will not be accepted, or
   should be done differently, than after writing the code. Pull requests
   without an accepted issue are closed without review, regardless of size or
   quality.
2. **Keep pull requests small and focused.** One logical change per pull
   request. Do not combine unrelated fixes, and do not reformat or "clean up"
   code you are not otherwise changing.
3. **Build Trick and run the tests locally** on at least one supported
   platform before opening the pull request. See
   [Testing](https://nasa.github.io/trick/developer_docs/Testing). New
   behavior requires new or updated tests. Bug fixes should include a test
   that fails without the fix where practical.
4. **Follow the style checks.** CI runs `clang-format` on changed C/C++
   lines, `perltidy` on changed Perl, `ruff` on changed Python, and
   `spotless` on changed Java. Run these locally before pushing.
5. **Use a conventional-commit PR title**, e.g. `fix: correct sleep timer
overflow on 32-bit` or `feat: add variable server byte-order query`. CI
   enforces this and release tooling uses it to generate the changelog. If
   your pull request has a single commit, that commit message must match the
   title. Allowed types are listed in
   [`.github/workflows/pr-title-lint.yml`](.github/workflows/pr-title-lint.yml).
6. **Complete the pull request template.** Every checklist item is an
   attestation. Do not check a box that is not true.
7. **Respond to review.** Pull requests with unanswered maintainer questions
   or unaddressed review comments for 30 days may be closed. You may reopen
   when you are ready to continue.

### Trivial and cosmetic changes

Trick does not accept pull requests for trivial or cosmetic changes. This
includes, but is not limited to: typo or grammar corrections in comments,
documentation, or messages; whitespace or formatting-only changes; comment
rewording; renaming for "clarity"; reordering imports or includes; replacing
constructs with equivalent constructs; README badge or wording changes; and
"modernization" that does not fix a demonstrated problem.

If you notice a typo or similar defect that genuinely affects users (for
example, an incorrect command in the Install Guide), open an issue describing
it. Maintainers will fix it, or will fold the fix into related work. Pull
requests for such changes are closed even if the change is correct.

### Requirements for every pull request

A pull request will be reviewed only if all of the following are true:

- It links to an issue in which a maintainer has agreed to the change.
- The author has built Trick with the change and run the relevant tests, and
  says which tests and on which platform.
- The author personally understands every line of the change and can explain
  it when asked.
- The description explains _why_ the change is needed for Trick users, not
  just _what_ it does.
- CI passes, or the author explains a failure they believe is unrelated.

### Maintainer-configured automation

Pull requests opened by automation that the maintainers have configured in
this repository, such as dependency updates and release preparation, are
exempt from the linked-issue requirement and from the author attestations in
the pull request template.

This exemption does not extend to bots, agents, or pipelines configured by
anyone else.

## Use of AI and automated tooling

You may use AI assistants and other tools to help you write code, tests, and
prose. The following rules apply regardless of the tools used:

- **A human must be accountable for the submission.** The person whose
  account opens the issue or pull request is the author. They must have run
  the code, understood the change, and be able to discuss it in their own
  words. "The tool said so" is not an answer to a review question.
- **Disclose material AI involvement** in the pull request template. Failing
  to disclose it when it is later evident is treated as a truthfulness
  problem, which is worse than the AI use itself.
- **Fully automated submissions are not accepted.** Issues or pull requests
  generated and opened by an agent, bot, or pipeline without a human
  reviewing and standing behind each one are closed on sight, and the account
  is subject to [enforcement](#enforcement). This includes bulk submissions
  across many repositories, submissions that are evidently templated, and
  submissions whose author cannot answer basic questions about the change.
  The only exception is
  [maintainer-configured automation](#maintainer-configured-automation).
- **Automated "findings" are not bug reports.** Output from static analyzers,
  linters, fuzzers, or language models is welcome only after a human has
  confirmed the problem is real, reproduced it in Trick, and assessed its
  impact. Post the reproduction, not the tool output.

The purpose of these rules is not to gatekeep on tooling. It is to ensure that
maintainer time is spent talking to someone who can engage with the work.

## Review and maintainer discretion

Maintainers review contributions as their primary responsibilities allow.
There is no service-level commitment on response time. Please do not ping,
re-open, or open duplicate issues to draw attention to a submission.

**Maintainers are under no obligation to review, respond to, or merge any
contribution.** Meeting every requirement in this document makes a
contribution eligible for review; it does not entitle it to acceptance. A
change may be declined because it does not fit the project's direction,
because its maintenance cost exceeds its benefit, because it is being
addressed another way, or for other reasons at the maintainers' judgment.
Maintainers will try to say why, briefly. "Closed" means "not accepted into
Trick," not "wrong."

These guidelines are applied uniformly. They apply equally to NASA employees,
contractors, external users, students, and anyone else.

## Contributions that are not accepted

The following are closed when identified, with a link to this section. They
are listed so that the criteria are public and predictable.

1. **Drive-by submissions**: issues or pull requests from accounts with no
   evident use of Trick that do not identify a concrete problem encountered
   while using it.
2. **Trivial and cosmetic changes**: the changes listed under
   [Trivial and cosmetic changes](#trivial-and-cosmetic-changes), and any
   change whose main effect is on the diff rather than on behavior.
3. **Pull requests without an accepted issue**, of any size.
4. **Tool output presented as a finding**: scanner, linter, or model output
   without a human reproduction and impact assessment.
5. **Fully automated or bulk submissions**, as described in
   [Use of AI and automated tooling](#use-of-ai-and-automated-tooling).
6. **Dependency bumps**: Dependabot is configured for this. Do not open
   manual dependency updates unless fixing a specific reproduced problem.
7. **Submissions with untrue attestations** in a template.
8. **Support requests disguised as bugs**, or questions answered by the
   documentation.
9. **Anything relating to a project that merely uses Trick** rather than to
   Trick itself. File it with that project.
10. **Duplicates** of open or closed issues and pull requests.
11. **Repeat submissions** of something that was already closed under this
    section, without a material change in the submission.

## Enforcement

Enforcement exists to protect maintainer time so that it can be spent on
Trick's users. It is about the submissions, not the person, and it is applied
the same way to everyone. A first issue from a real user that is missing some
information will simply get a request for that information. The actions below
are for submissions that do not meet these guidelines. Maintainers may take
any of them at their discretion:

- **Close** the issue or pull request with a comment linking to the relevant
  section of this document. Closed items are normally locked.
- **Warn**: on a further submission that does not meet these guidelines,
  state explicitly that continued submissions of this kind will result in
  the account being blocked.
- **Limit interactions** on the repository using GitHub's
  [interaction limits](https://docs.github.com/en/communities/moderating-comments-and-conversations/limiting-interactions-in-your-repository).
- **Block** the account from the repository or the organization using
  GitHub's
  [blocking tools](https://docs.github.com/en/communities/maintaining-your-safety-on-github/blocking-a-user-from-your-organization),
  including for automated or inauthentic activity, untrue attestations, or
  abusive conduct. Blocks may be indefinite.

Maintainers record the reason for closures, warnings, and blocks in the
closing comment.
