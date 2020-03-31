// SPDX-License-Identifier: GPL-3.0-or-later

#include "cli.h"

#include <stdio.h>
#include <string.h>

#include "../mcc_generated_files/uart1.h"
#include "cmd.h"

static char _CLI_BUFF[CLI_BUFF_SIZE + 1] = "\0";

static ParsedCmd _PARSED_CMD = {"\0", 0, {"\0", "\0", "\0", "\0"}};

static CliCmd CMDS[CMD_CNT] = {
    {"ver", "", "show version", &ver},
    {"info", "[hw|sw]", "show component info", &info},
};

void cli_get_cmd() {
    uint8_t byteIn;
    uint8_t cmdDone = 0;
    uint8_t idx = 0;

    strncpy(_CLI_BUFF, "\0", (CLI_BUFF_SIZE + 1));
    while (!cmdDone) {
        if (!UART1_IsRxReady()) {
            byteIn = UART1_Read();
            //printf("%x\n", byteIn);

            // NL or CR end the command
            if ((byteIn == 10) || (byteIn == 13)) {
                if (idx > 0) {
                    cmdDone = 1;
                }
                break;
            }
            // BS removes last character from buffer
            if (byteIn == 8) {
                if (idx > 0) {
                    idx --;
                    _CLI_BUFF[idx] = '\0';
                }
                break;
            }
            // skip non-printable characters
            if ((byteIn < 32) || (byteIn > 126)) {
                break;
            }
            // if buffer full skip adding
            if (idx == CLI_BUFF_SIZE) {
                break;
            }
            _CLI_BUFF[idx] = (char)byteIn;
            idx ++;
            _CLI_BUFF[idx] = '\0';
        }
    }
}

uint8_t cli_parse_cmd() {
    strncpy(_PARSED_CMD.cmd, "\0", CLI_PARAM_SIZE);
    _PARSED_CMD.nParams = 0;
    strncpy(_PARSED_CMD.params[0], "\0", CLI_PARAM_SIZE);
    strncpy(_PARSED_CMD.params[1], "\0", CLI_PARAM_SIZE);
    strncpy(_PARSED_CMD.params[2], "\0", CLI_PARAM_SIZE);
    strncpy(_PARSED_CMD.params[3], "\0", CLI_PARAM_SIZE);

    //printf("DBG: parse ...\n");
    uint8_t j = 0;
    for (uint8_t i = 0; i < CLI_BUFF_SIZE; i ++) {
        if (_CLI_BUFF[i] == 32) {
            _PARSED_CMD.nParams ++;
            j = 0;
            continue;
        }
        if (_CLI_BUFF[i] == 0) {
            _PARSED_CMD.nParams ++;
            break;
        }

        if (j >= CLI_PARAM_SIZE) {
            printf("ERROR parsing (size)\n");
            return 1;
        }
        if (_PARSED_CMD.nParams >= CLI_PARAM_CNT) {
            printf("ERROR parsing (param)\n");
            return 2;
        }

        if (_PARSED_CMD.nParams == 0) {
            _PARSED_CMD.cmd[j] = _CLI_BUFF[i];
            j ++;
            _PARSED_CMD.cmd[j] = '\0';
        } else {
            _PARSED_CMD.params[_PARSED_CMD.nParams - 1][j] = _CLI_BUFF[i];
            j ++;
            _PARSED_CMD.params[_PARSED_CMD.nParams - 1][j] = '\0';
        }
    }
    if (_PARSED_CMD.nParams > 1) {
        _PARSED_CMD.nParams --;
    }
    return 0;
}

void cli_execute() {
    //printf("DBG: execute %s (%d params)\n", _PARSED_CMD.cmd, _PARSED_CMD.nParams);
    if (strlen(_PARSED_CMD.cmd) == 0) {
        return;
    }

    if (strncmp(_PARSED_CMD.cmd, "help", CLI_PARAM_SIZE) == 0) {
        for (uint8_t i = 0; i < CMD_CNT; i++) {
            printf("  %s - %s\n", CMDS[i].cmd, CMDS[i].help);
        }
        return;
    }

    uint8_t cmdDone = 0;

    for (uint8_t i = 0; i < CMD_CNT; i++) {
        if (strncmp(_PARSED_CMD.cmd, CMDS[i].cmd, CLI_PARAM_SIZE) == 0) {
            CMDS[i].fptr(&_PARSED_CMD);
            cmdDone = 1;
            break;
        }
    }
    if (cmdDone == 0) {
        printf("UNRECOGNIZED command\n");
    }
}
