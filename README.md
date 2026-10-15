## Open Board Viewer ++ [![Build Status](../../actions/workflows/make_packages.yml/badge.svg?event=push)](../../actions/)

Linux SDL/ImGui edition software for viewing .brd files, intended as a drop-in
replacement for the "Test_Link" software and "Landrex".

> **OpenBoardView++** — a fork of OpenBoardView with a headless CLI for scripts and AI
> agents. See the [fork note](#fork-note--openboardview), the [Headless CLI](#headless-cli),
> the [changelog](CHANGELOG.md) and [related projects](#related-projects).


[![Walkthrough of OpenBoardView R7.2](http://img.youtube.com/vi/6CrNRo1UP5g/0.jpg)](http://www.youtube.com/watch?v=6CrNRo1UP5g "OpenBoardView R7.2 demonstration, with voice-over")

[![Demo Video](https://github.com/OpenBoardView/OpenBoardView/blob/master/asset/screenshot.png)]()


![Common net pin halo](https://github.com/OpenBoardView/OpenBoardView/blob/master/asset/screenshot-halo.png)

![Part searching](https://github.com/OpenBoardView/OpenBoardView/blob/master/asset/screenshot-partsearch.png)


### Features

- Headless CLI (`--cli`) for scripts and AI agents — query parts, nets, pins and geometry
  without opening a window, with TSV or JSON output and a batch mode (see
  [Headless CLI](#headless-cli))
- Annotations (per board database file)
- Part and pin sizes better represented
- Better outlining of irregular objects (such as connectors)
- Drag and drop
- Recently used file history
- Non-orthagonally orientated caps/resistors/diodes now drawn more realistically
- Adjustable DPI (for working on 2K/4K screens)
- Works with multiple concurrent instances


### TODO

- Decode more board formats
- Compound project/file format


### Prerequisites

#### macOS
	$ brew install cmake sdl2

If you get the following error after running `build.sh`:

	urllib.error.URLError: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get local issuer certificate (_ssl.c:997)>


Run the script at `/Applications/Python3.x/Install Certificates.command`.


#### Ubuntu

For Ubuntu developers, you'll need the following packages at a minimum;

	$ apt-get install git build-essential cmake libsdl2-dev libgtk-3-dev

#### Fedora

For Fedora, you will need the following packages at a minimum;

	$ dnf install @development-tools

	$ dnf install SDL2-devel gtk3-devel fontconfig-devel cmake

### Installation

1. Clone the project — **this fork**, the one that contains the headless CLI
```
$ git clone --recursive 'https://github.com/Teycir/OpenBoardView-plus-plus'
```
   Upstream releases and prebuilt packages do **not** contain `--cli`; build this fork to get
   it. (For the plain upstream viewer, clone
   [OpenBoardView](https://github.com/OpenBoardView/OpenBoardView) instead.)

2. Build it
```
$ ./build.sh
```
3. Sign it (skip if you're not on running macOS)
```
$ codesign --force --deep --sign - ./openboardview.app
```
4. Run it!
```
$ ./bin/openboardview
```
...or...
```
$ ./openboardview.sh
```
...or...
```
$ open ./openboardview.app
```

### Usage

- Ctrl-O: Open file select dialog

- w/a/s/d: pan viewport over board
- x: Reset zoom and center
- Mouse scroll, -/=: Zoom out/in
- Mouse click-hold-drag, Numeric pad up/down/left/right: pan viewport over board
- Numeric pad +/-: zoom board
- Numeric pad 5: Reset zoom and center
- Space, Middle mouse click: Flip board
- R/./Numpad-Del: Rotate clockwise
- ,/Numpad-Ins: Rotate counter-clockwise

- /, Ctrl-F: Search
- ESC: Clear search results and selected parts

- p: Toggle pin display
- m: Mirror board across Y-axis

- L: Show net list
- K: Show part list

### Headless CLI

Non-interactive query mode for scripts and AI agents. No window, no GL, machine-readable output.
It lives in the **same binary as the GUI** — there is nothing extra to install or enable.

```
$ openboardview --cli <board_file> [query] [options]
```

#### Running it

Build this fork first ([Installation](#installation)), then invoke the binary with `--cli`:

```bash
./bin/openboardview --cli board.brd --info    # from the build tree
./openboardview.sh --cli board.brd --info     # wrapper: builds if needed, sets lib path
openboardview --cli board.brd --info          # once installed / on your PATH
```

`--version` confirms you have a build with the CLI — an upstream build does not know the flag
and will fail with a usage error:

```bash
$ openboardview --version
openboardview 10.0.0 (headless CLI)
```

#### Your first query

```bash
# what is this board? components, pins, nets, bounding box
openboardview --cli LA-K971P_boardview.cad --info

# which pins are on GND, as JSON
openboardview --cli LA-K971P_boardview.cad --net GND --json | jq '.npins'

# every part with its position, as TSV
openboardview --cli LA-K971P_boardview.cad --parts --units mm --limit 50
```

Queries (`--info` is the default):

| Flag | Output |
|---|---|
| `--info` | file summary, component/pin/net counts, dummy count, bounding box |
| `--bbox` | board bounding box only |
| `--parts` | TSV: refdes, side, mount, npins, nnets, x, y, mfgcode, nets |
| `--nets` | TSV: net, number, is_ground, side, npins |
| `--net <name>` | pins on the given net (case-insensitive) |
| `--find <refdes>` | one component plus its pins (case-insensitive) |
| `--json` | full board dump as JSON |

Options:

| Flag | Meaning |
|---|---|
| `--units <mm\|thou\|in>` | coordinate unit (default `mm`; board data is stored in thou) |
| `--limit <n>` | cap listed rows and per-component pin lists (`0` = unlimited) |
| `--json` | output format flag — combines with any query (`--json --net GND`) |
| `--stdin` | batch mode: read queries from stdin, one per line, parse the board once |
| `--version` | print CLI version and exit |

Exit codes: `0` ok, `1` usage error, `2` unreadable file, `3` unknown/bad format, `4` query miss.
Errors go to stderr; stdout stays clean for piping. `--json` alone prints an indented full dump;
with `--stdin` it emits one compact JSON object per line (JSONL).

```bash
# single query
openboardview --cli board.brd --net GND --json | jq '.npins'

# tabular extraction
openboardview --cli board.brd --parts --units mm --limit 50 | cut -f1,6,7
```

#### Scripting and AI agents

`--stdin` parses the board **once** for any number of queries, and `--json` renders each result
as one line of JSON, so a whole batch is a single pipe:

```bash
# batch: 51x faster than one process per query on a 1 MB board
printf -- '--net GND\n--find U1\n--bbox\n' | openboardview --cli board.cad --stdin --json

# JSONL — one compact object per line, easy to consume
printf -- '--net GND\n--net VCC\n' | openboardview --cli board.cad --stdin --json | jq -c '.npins'

# stream a query list in
openboardview --cli board.cad --stdin < queries.txt
```

A failing query does not abort the batch: it reports on stderr, the remaining queries still run,
and the exit code is the worst result seen.

Supported formats are auto-detected from file content, not extension: GenCAD, CAD, Allegro,
BRD/BRD2, BDV, BVR/BVR3, FZ, CST, CAE, ASC, AD, XZZPCB.

### Fork note — OpenBoardView++

This is **OpenBoardView++**, a fork of [OpenBoardView](https://github.com/OpenBoardView/OpenBoardView)
that adds the headless CLI above. It is **not** the upstream project — for the original
SDL/ImGui viewer, packaging and releases, use upstream. The upstream MIT license and
`Copyright (c) 2016 Chloridite and OpenBoardView contributors` notice are retained unchanged;
the CLI additions are released under the same MIT terms.

The default branch `main` carries a fresh history created for this fork (a single root commit
at the time of writing); the upstream line of development was not carried over. Upstream
branches inherited by the fork (e.g. `unix`, `bdv`) remain available for reference but are not
maintained here. Because `main` shares no ancestry with upstream, diffs against upstream are
best read per-file rather than as a linear log.

### Changelog

Fork changes are tracked in **[CHANGELOG.md](CHANGELOG.md)** — newest first, in
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) format. It starts at the fork point,
so it does not repeat upstream release notes; those live in
[upstream releases](https://github.com/OpenBoardView/OpenBoardView/releases). The base version
forked here is 10.0.0.

### Related projects

- **[schematic-fetch](https://github.com/Teycir/schematic-fetch)** — fetches free full PDF
  schematics and boardviews (`.brd`, `.cad`, boardview files) from open sources, verifies and
  deduplicates them, and archives them with a manifest. Handy as the source of the board files
  you feed to `--cli`; the real-board validation in `cli_fixtures/VERIFICATION.md` used
  boardviews it retrieved.

  ```bash
  ./scripts/fetch.sh "LA-K971P" /tmp/schem_LA-K971P   # probe free sources for the board
  openboardview --cli <boardview.cad> --info          # then query the file you saved
  ```
