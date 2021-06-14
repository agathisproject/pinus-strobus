// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include "system.h"
#include "clock.h"
#include "uart1.h"
#include "pin_manager.h"

#define FCY (CLOCK_SystemFrequencyGet() / 2)
#include <libpic30.h>

#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "platform/platform.h"
#include "agathis/base.h"

void task_mc(void *pvParameters);
void task_CLI(void *pvParameters);
TaskHandle_t xHandle0 = NULL;
TaskHandle_t xHandle1 = NULL;

void init() {
    SYSTEM_Initialize();
    __delay_ms(10);
    printf("boot OK\n");
    while (!UART1_IsTxDone()) {};
    cpu_init();

    gpio_init();
    ag_init();
    i2c_init();
}

int main(void) {
    init();

    BaseType_t xRes = pdFAIL;

    xRes = xTaskCreate(task_mc,
                       (const char *)"main",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       (tskIDLE_PRIORITY + 1),
                       &xHandle0);
    if (xRes != pdPASS) {
        gpio_set(PIN_LED_R, 1);
    }

    xRes = xTaskCreate(task_CLI,
                       (const char *)"CLI",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       &xHandle1);
    if (xRes != pdPASS) {
        gpio_set(PIN_LED_R, 1);
    }

    xSemaphore_MMC = xSemaphoreCreateBinary();
    if (xSemaphore_MMC == NULL) {
        gpio_set(PIN_LED_R, 1);
    }
    xSemaphoreGive(xSemaphore_MMC);

    printf("starting tasks\n");
    while (!UART1_IsTxDone()) {};
    vTaskStartScheduler();

    printf("!!! FAIL !!!\n");
    while (!UART1_IsTxDone()) {};
}
