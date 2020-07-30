// SPDX-License-Identifier: GPL-3.0-or-later

#include "cli_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "constants.h"
#include "agathis/mc.h"
#include "agathis/tmc.h"

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
    } else {
        return CMD_WRONG_PARAM;
    }
    return CMD_DONE;
}

void _ls_tree() {
    if (xSemaphoreTake(xSemaphore_MMC, 10) != pdTRUE) {
        printf("ongoing scan\n");
        return;
    }

    for (uint8_t i = 1; i < MC_MAX_CNT; i++) {
        printf("  MC %2d:", i);
        if (RmtMC[i - 1].state == MC_NOT_PRESENT) {
            printf(" -");
        } else if (RmtMC[i - 1].state == MC_INVALID) {
            printf(" ?");
        } else {
            printf(" R%d", (RmtMC[i - 1].pow_rst & MC_CMD_ID_RST_MASK) >>
                   MC_CMD_ID_RST_OFFS);
            printf(" P%d", (RmtMC[i - 1].pow_rst & MC_CMD_ID_PWR_MASK) >>
                   MC_CMD_ID_PWR_OFFS);
        }
        printf("\n");
    }
    xSemaphoreGive(xSemaphore_MMC);
}

CliCmdReturn_t ls(ParsedCmd_t *cmdp) {
    if (cmdp->nParams == 0) {
        _ls_tree();
        return CMD_DONE;
    }

    if (cmdp->nParams == 1) {
        uint8_t mc_id = strtol(cmdp->params[0], NULL, 10);
        if (mc_id == 0) {
            MC_Show();
        } else if (mc_id < MC_MAX_CNT) {
            TMC_Show(mc_id);
        } else {
            printf("id MUST BE lower than %d\n", MC_MAX_CNT);
        }
    }
    return CMD_DONE;
}

CliCmdReturn_t set(ParsedCmd_t *cmdp) {
    if (cmdp->nParams != 3) {
        return CMD_WRONG_N;
    }

    uint8_t mc_id = strtol(cmdp->params[0], NULL, 10);
    if (strncmp(cmdp->params[1], "pwr", CLI_PARAM_SIZE) == 0) {
        uint8_t val = strtol(cmdp->params[2], NULL, 16);
        if (mc_id == 0) {
            MC_SetPower(val);
        } else if (mc_id < MC_MAX_CNT) {
            // TODO: TMC_SetPower(mc_id, val);
        } else {
            printf("id MUST BE lower than %d\n", MC_MAX_CNT);
        }
    } else {
        return CMD_WRONG_PARAM;
    }
    return CMD_DONE;
}

#define CMD_CNT 3

unsigned int Get_Cmd_Cnt() {
    return CMD_CNT;
}

static CliCmd_t _CMDS_ARRAY[CMD_CNT] = {
    {"info", "[sw|hw]", "show HW/SW info", &info},
    {"ls", "[id]", "show MC info", &ls},
    {"set", "id [pwr] <val>", "set MC attribute", &set},
};

CliCmd_t *CMDS = _CMDS_ARRAY;
