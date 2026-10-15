# Changelog

All notable changes to **OpenBoardView++** (this fork) are documented in this file.

This changelog covers the **fork only** and starts at the fork point. Upstream OpenBoardView's
own release notes live at <https://github.com/OpenBoardView/OpenBoardView/releases>; the base
version forked here is **10.0.0**.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

#### Headless CLI (`--cli`)

Query a board file without opening a window. Results go to stdout, errors to stderr, so output
pipes cleanly.

- Queries: `--info` (default), `--bbox`, `--parts`, `--nets`, `--net <name>`,
  `--find <refdes>`, `--json`.
- `--units mm|thou|in` — coordinate unit (default `mm`; board data is stored in thou).
- `--limit <n>` — caps listed rows and per-component pin arrays (`0` = unlimited).
- `--json` — output-format flag, orthogonal to the query mode. Alone it prints a full indented
  dump; combined with a query it renders that query as JSON (`--json --net GND`,
  `--json --parts`, `--json --bbox`, ...).
- `--stdin` — batch mode. Reads queries from stdin, one per line, and parses the board
  **once**. TSV output gets a `# query:` header per query; with `--json` it emits one compact
  JSON object per line (JSONL). Blank lines and `#` comments are skipped. A failing query
  reports on stderr and the batch continues; the exit status is the worst query result.
- `--version` — print the CLI version and exit.
- Exit codes: `0` ok, `1` usage error, `2` unreadable file, `3` unknown/bad format,
  `4` query miss.
- Board formats are detected from file **content**, not extension: GenCAD, CAD, Allegro,
  BRD/BRD2, BDV, BVR/BVR3, FZ, CST, CAE, ASC, AD, XZZPCB.

#### Performance

- Batch mode removes the per-query parse. 50 net queries against a 1.14 MB GenCAD board
  (Compal LA-K971P) go from **32.07 s** (50 separate processes) to **0.62 s** (a single
  process) — a **51.6x** speed-up. JSONL output costs the same.

#### Documentation

- README: the CLI is now the first entry in the feature list, with a full "Headless CLI"
  reference and a *how to run it* section — build-tree, wrapper (`./openboardview.sh`) and
  PATH invocations, a `--version` check that proves the build has the CLI, first-query
  examples, and a scripting / AI-agent batch section.
- README: installation now clones **this fork**. Upstream releases and prebuilt packages do
  not contain `--cli`, so the previous upstream clone URL could never produce a CLI build.
- `cli_fixtures/`: a synthetic GenCAD fixture, a negative fixture, and `VERIFICATION.md`
  recording the fixture matrix, the real-board matrix (Compal LA-K971P and Apple 820-00840),
  the two defects fixed during validation, and the performance numbers.

### Fixed

- `--json` was modelled as a query *mode*, so a batch `--net` line overwrote it and
  `--stdin --json` emitted TSV instead of JSON. It is now an independent format flag.
- `--json --find <refdes>` emitted a keyless nested object (`"found": true, {...}`), which is
  not valid JSON. The component is now nested under an explicit `"component"` key.
- The placeholder `"..."` component (loose pads and vias) is no longer reported as a real
  component: it is excluded from `--parts` and from the `components` count, reported separately
  as `dummy_components` / `"dummy": true`, and its pin list no longer dominates `--json` output.

### Notes

- No new dependencies and no new build target: JSON is emitted by hand and the CLI ships inside
  the existing `openboardview` binary.
