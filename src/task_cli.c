/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "cli/cli.h"

void task_CLI(void *pvParameters) {
    uint8_t parseSts = 1;

    while (1) {
        printf("%s", CLI_PROMPT);
        CLI_Get_Cmd();
        parseSts = CLI_Parse_Cmd();
        if (parseSts == 0) {
            CLI_Execute();
        }
        //vTaskDelay(1);
    }
}
