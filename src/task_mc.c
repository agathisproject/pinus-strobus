// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "agathis.h"
#include "hw.h"

void task_mc(void *pvParameters) {
    while (1) {
        GPIO_LED_Green(1);
        vTaskDelay(500);
        GPIO_LED_Green(0);
        vTaskDelay(500);
    }
}