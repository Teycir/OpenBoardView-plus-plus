# OpenBoardView headless CLI — verification record

Generated (UTC): 2026-10-15T20:49:34Z
Revised (UTC): 2026-10-15T20:58:00Z (real-board pass + dummy/limit fixes)

| artifact | path | sha256 |
|---|---|---|
| binary | /home/teycir/.local/bin/openboardview | 40febc30b48f259661c42a7d573f12a91223bfdb2ccdc1d4416db0fa5e3a6415 |
| fixture | /home/teycir/Repos/OpenBoardView/cli_fixtures/fixture.cad | 7e971e5e92aae27bef31df9a7a1a0dddb642b118f6d24d4362140c5864632053 |
| source | src/openboardview/CLI.cpp | f8dcd5aec0c29c4db01b661d893b936cabb3455b242271cce7b6048cfa1fa269 |
| source | src/openboardview/CLI.h | b061af7ea789398ec4e856549b7dad309c47cfda4d44b1ad4f6c0199ce6fd96e |
| source | src/openboardview/main_opengl.cpp | 0ee1c2206d1c022aeb0afdfeaf8180a16b0e0da3fb7bd9c54523fb1166767e87 |

## Command matrix (all run from ~/Repos/OpenBoardView/cli_fixtures)

```
COMMAND                                    RESULT
--cli fixture.cad --info                   rc=0, components 2 / dummy_components 1 / pins 4 / nets 4 / nets_connected 3 / bbox_mm present
--cli fixture.cad --bbox                   rc=0, bbox_mm: 24.8920,24.8920 53.8480,51.3080
--cli fixture.cad --parts                  rc=0, R1 top SMD 2 pins x=26.6700 y=25.4000
--cli fixture.cad --nets                   rc=0, GND is_ground=yes, VCC npins=2
--cli fixture.cad --net VCC                rc=0, R1:2 + U1:1
--cli fixture.cad --find r1                rc=0, case-insensitive; 2 pins w/ nets GND,VCC
--cli fixture.cad --json                   rc=0, valid JSON, dummy component flagged
--cli fixture.cad --parts --units thou     rc=0, x=1050 thou
--cli fixture.cad --bbox --units in        rc=0, 0.9800,0.9800 2.1200,2.0200
--cli fixture.cad --parts --limit 1        rc=0, 1 row
--cli (no file)                            rc=1
--cli /nonexistent.brd --info              rc=2
--cli bogus.brd --info                     rc=3 (unrecognized format)
--cli fixture.cad --net NOPE               rc=4
--cli fixture.cad --find ZZZ               rc=4
--cli --help                               rc=0
fixture.cad (no CLI flag)                  GUI launched (timeout rc=124)
-i fixture.cad                             GUI launched (timeout rc=124)
(no args)                                  GUI launched (timeout rc=124)
```

## Notes / limitations
- GUI regression: checked via timeout; rc=124 means the process stayed alive (window open), i.e. hook correctly returned -1.
- Exit codes: 0 ok, 1 usage, 2 unreadable file, 3 unrecognized/invalid format, 4 query target not found.
- Units: board values are thou; mm = thou * 0.0254, in = thou * 0.001. Default mm.
- Component position is computed by the CLI from the pin bounding-box centre (the data model stores no component position/rotation).
- Component rotation is not available in the data model and is not reported.
- '"..."' component = BRDBoard::kComponentDummyName, upstream holder for loose pads/vias; JSON marks it "dummy": true.
- Change (2026-10-15T20:58Z): the dummy is excluded from `components`/`--parts` and reported separately as `dummy_components`. On 820-00840 it holds 1233 pads/1138 nets, so listing it emitted a single ~40 KB TSV row that broke tabular parsing.
- Change (2026-10-15T20:58Z): `--limit N` now also caps the per-component `pins` array in `--json` (uniform row cap). Component counts (`npins`/`nnets`) stay full-size.
- Known gap: the CLI does not honour '-c <config>'; it reads the default config dir (encrypted-format keys only).
- Known gap: no headless PNG render (needs a GL context).

## Real-board validation (2026-10-15T20:58Z)

Fetched with the schematic-fetch skill (archive.org, keyless, FREE_ONLY) — boardviews, not PDFs, since
OpenBoardView only opens board files. Archived+hashed under ~/BugBounty/schematics/.

| board | file | sha256 | format routed |
|---|---|---|---|
| Compal LA-K971P (Lenovo IdeaPad Gaming 3-15ACH6) | LA-K971P_boardview.cad | 830d8dcac85ca03ee05c29ae2dcd9984452adbb02f21ba2ee039dc11c9b1fdab | GenCAD 1.4 (ASCII), via GenCADFile |
| Apple 820-00840-A (A1708) | 820-00840_boardview.brd | 40fbb87ed6e9da00f82a749a53cade3184573fabe4ab416132401f09a4e94f17 | BRD (binary) |

```
COMMAND                                   RESULT
LA-K971P --info                     rc=0  valid yes / components 2641 / dummy 1 / pins 12397 / nets 2110 / nets_connected 2109
                                          bbox_mm: 26.8986,-189.9412 343.1667,-22.2504 (316.2681 x 167.6908)
820-00840 --info                    rc=0  valid yes / components 2837 / dummy 1 / pins 11833 / nets 2038 / outline_points 146
                                          bbox_mm: 0.0000,0.0000 206.0956,86.6140 (206.0956 x 86.6140)
LA-K971P --parts --limit 12         rc=0  real refdes (C1, C10..C17, C232, C233), sides/smds, nets (+1.8VS, GND, EDP_TXP0_C)
LA-K971P --nets --limit 8           rc=0  +0.6VS, +0.75VALW, +0.75VS ... is_ground=no
LA-K971P --bbox --units thou        rc=0  1059.0000,-7478.0000 13510.5000,-876.0000 (consistency: 1059 thou == 26.8986 mm)
LA-K971P --find C1                  rc=0  2 pins, nets +1.8VS / GND, centre_mm 171.0055,-70.3580
LA-K971P --net EDP_TXP0             rc=0  2 pins (C10:1, UC1:D11)
LA-K971P --net GND --limit 5        rc=0  5 rows shown, header reports pins=3643
LA-K971P --json --limit 20          rc=0  jq valid; counts present; stderr 0 bytes; 20 comps + 20 nets emitted
820-00840 --parts --limit 4         rc=0  C1000..C1003 (dummy row gone)
820-00840 --json --limit 2          rc=0  1240 bytes; jq counts.components==2837, dummy_components==1; pins array capped at 2
```

Findings from real data:
- Loader dispatch is correct on real files: `.cad` containing GenCAD is routed by content to GenCADFile, not CADFile (extension-first dispatch would have mis-parsed it).
- GenCAD outline yields 0 outline_points (no OUTLINE block); BRD yields 146. `--bbox` still works on both (falls back to component centres).
- GenCAD coordinates are negative-Y (board origin differs per vendor) — not a parsing error; thou/mm output stays internally consistent.
- Only real defect found: the dummy component row + uncapped pins array (both fixed above).

## Batch mode, version and JSON format (second pass)

Built after the first pass; installed binary sha256 `5f303459f8b5a2157d6577d22ba5026613882f42a359b218fc100b7658e0f4b0`.

New: `--stdin` (batch), `--version`, and `--json` promoted from a query mode to an orthogonal
output-format flag.

| Check | Result |
|---|---|
| `--version` | `openboardview 10.0.0 (headless CLI)`, rc=0 |
| `-v` | same, rc=0 |
| `--stdin` TSV | `# query: <text>` header per query; blank and `#` lines skipped; rc=0 |
| `--stdin --json` | one compact JSON object per line (JSONL); every line parses under `jq`; rc=0 |
| `--stdin` query miss | `error: ...` on stderr, batch continues, exit = worst rc (4) |
| `--json --net GND` | single shot: `{"query":"net","found":true,...,"pins":[...]}`, rc=0 |
| `--json --find U1` | single shot: `{"query":"refdes","found":true,"component":{...}}`, jq valid |
| `--json` alone | unchanged indented full dump; `cli_version` first field; rc=0 |
| `--json` + `--parts/--bbox/--info` | JSON for that query, arrays capped by `--limit`, rc=0 |

Regression on the archived real boards — counts identical to the first pass:

| Board | components / dummy / pins / nets / nets_connected |
|---|---|
| LA-K971P (GenCAD, 1.14 MB) | 2641 / 1 / 12397 / 2110 / 2109 |
| 820-00840 (BRD, 635 KB) | 2837 / 1 / 11833 / 2038 / 2037 |

Perf, 50 real net queries against LA-K971P:

    50 separate processes   real 0m32.072s
    --stdin (one process)   real 0m0.621s   -> 51.6x
    --stdin --json          real 0m0.626s

Two defects found and fixed during this pass: `--json` was modelled as a *query mode*, so a
batch `--net` line overwrote it and `--stdin --json` emitted TSV; and `--json --find` emitted a
keyless nested object (`"found": true, {...}`), which is invalid JSON. `--json` is now a format
flag (`CliOpts::json_out`) orthogonal to the query mode, and the `--find` wrapper carries an
explicit `"component"` key.
