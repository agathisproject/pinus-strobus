// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "hw.h"
#include "agathis.h"
#include "agathis_tmc.h"

void get_MC_info() {

}

void scan_tree() {
    if (xSemaphoreTake(xSemaphore_MMC, 10) != pdTRUE) {
        return;
    }

    uint8_t data[MC_CMD_ID_NB] = {0, 0, 0, 0};
    MCCmdStatus_t sts = MC_CMD_FAIL;

    for (uint8_t i = 0; i < MC_MAX_CNT; i++) {
        sts = TMC_cmdID(i, (uint8_t *) &data);
        if (sts == MC_CMD_OK) {
            if (data[0] != (MC_CMD_ID_NB - 1)) {
                RmtMC[i].state = MC_INVALID;
            } else {
                RmtMC[i].state = MC_PRESENT;
                RmtMC[i].pow_rst = data[1];
                RmtMC[i].io = data[2];
            }
        } else {
            RmtMC[i].state = MC_NOT_PRESENT;
        }
    }

    xSemaphoreGive(xSemaphore_MMC);
}

void task_mc(void *pvParameters) {
    while (1) {
        scan_tree();
        vTaskDelay(500);
    }
}
