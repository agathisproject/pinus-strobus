// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CLI_YP8GJVG3TCW7BWQW
#define CLI_YP8GJVG3TCW7BWQW
/** @file */

#include <stdint.h>

#define CLI_PROMPT "> "
#define CLI_BUFF_SIZE 32
#define CLI_PARAM_CNT 4 /**< number of parameters for a command */
#define CLI_PARAM_SIZE 12

typedef struct {
    char cmd[CLI_PARAM_SIZE];
    uint8_t nParams;
    char params[CLI_PARAM_CNT][CLI_PARAM_SIZE];
} ParsedCmd;

void cli_get_cmd();
uint8_t cli_parse_cmd();
void cli_execute();

#endif /* CLI_YP8GJVG3TCW7BWQW */
