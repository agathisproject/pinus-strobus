/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "hw/callback.h"
#include "agathis/tmc.h"

void scan_tree() {
    if (xSemaphoreTake(xSemaphore_MMC, 10) != pdTRUE) {
        return;
    }

    //TMC_Scan();

    xSemaphoreGive(xSemaphore_MMC);
}

void task_mc(void *pvParameters) {
    while (1) {
        //scan_tree();
        vTaskDelay(1);
    }
}
