// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include "../mcc_generated_files/i2c1.h"
#include "../mcc_generated_files/i2c2.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "agathis.h"
#include "hw.h"

void get_MC_info() {

}

void scan_tree() {
    if (xSemaphoreTake(xSemaphore_MMC, 10) != pdTRUE) {
        return;
    }

    I2C1_MESSAGE_STATUS sts;
    uint8_t tx_data[2] = {0, 0};

    for (uint8_t i = 0; i < MC_MAX_CNT; i++) {
        sts = I2C1_MESSAGE_PENDING;

        while (!I2C1_MasterQueueIsEmpty()) {
            vTaskDelay(1);
        }
        I2C1_MasterWrite(tx_data, 2, (i + I2C_OFFSET + 1), &sts);
        while (sts == I2C1_MESSAGE_PENDING) {
            vTaskDelay(1);
        }

        if (sts == I2C1_MESSAGE_COMPLETE) {
            RmtMC[i].state = MC_INVALID;
            // TODO: read MC info
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
