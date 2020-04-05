// SPDX-License-Identifier: GPL-3.0-or-later

#include "cli_types.h"

#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include "agathis.h"

void _info_SW() {
    printf("   OS: %s\n", tskKERNEL_VERSION_NUMBER);
    printf("   FW: %d\n", APP_VERSION);
    //printf("build: %s\n", APP_BUILD);
}

void _info_HW() {
    unsigned int devid, devrev;

    TBLPAG = 0xFF;
    devid = __builtin_tblrdl(0x0);
    devrev = __builtin_tblrdl(0x2);
    printf(" DEVID: %#06x\n", devid);
    printf("DEVREV: %#06x\n", devrev);
}

CliCmdReturn_t info(ParsedCmd_t *cmdp) {
    if (cmdp->nParams != 1) {
        return CMD_WRONG_N;
    }

    if (strncmp(cmdp->params[0], "sw", CLI_PARAM_SIZE) == 0) {
        _info_SW();
    } else if (strncmp(cmdp->params[0], "hw", CLI_PARAM_SIZE) == 0) {
        _info_HW();
    } else if (strncmp(cmdp->params[0], "mc", CLI_PARAM_SIZE) == 0) {
        MC_Show();
    } else {
        return CMD_WRONG_PARAM;
    }
    return CMD_DONE;
}

CliCmdReturn_t show(ParsedCmd_t *cmdp) {
    return CMD_DONE;
}

#define CMD_CNT 2

unsigned int Get_Cmd_Cnt() {
    return CMD_CNT;
}

static CliCmd_t _CMDS_ARRAY[CMD_CNT] = {
    {"info", "[sw|hw|mc]", "show info", &info},
    {"show", "", "show ???", &show},
};

CliCmd_t *CMDS = _CMDS_ARRAY;
