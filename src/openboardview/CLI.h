#pragma once

/*
 * Headless CLI mode.
 *
 * Lets agents/scripts query a board file without opening a window (no SDL,
 * no GL, no display). Enabled by long flags that the GUI arg parser does not
 * understand: --cli, --info, --json, --parts, --nets, --net, --find, --bbox.
 *
 * Returns -1 when the invocation is *not* a CLI call (GUI takes over),
 * otherwise the process exit code:
 *   0 = ok   1 = usage error   2 = file unreadable
 *   3 = unrecognized/invalid board format
 */
int obv_cli_try_run(int argc, char **argv, const char *config_file);
