// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CMD_7SJ8FR5XMGZZTMG7
#define CMD_7SJ8FR5XMGZZTMG7
/** @file */

#include "cli.h"

typedef enum {
    CMD_DONE,
    CMD_WRONG_N,
    CMD_WRONG_PARAM,
} ECliCmdReturn;

#define CMD_DESC_SIZE 32
#define CMD_HELP_SIZE 32

typedef struct {
    char cmd[CLI_PARAM_SIZE];
    char desc[CMD_DESC_SIZE];
    char help[CMD_HELP_SIZE];
    ECliCmdReturn (*fptr)(ParsedCmd *cmdp);
} CliCmd;

#define CMD_CNT 2

extern CliCmd *CMDS;

#endif /* CMD_7SJ8FR5XMGZZTMG7 */
