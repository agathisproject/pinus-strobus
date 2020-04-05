// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/uart1.h"

#define FCY (_XTAL_FREQ / 2)
#include <libpic30.h>

#include "src/FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "src/agathis.h"
#include "src/hw.h"
#include "src/cli.h"

void task_main(void *pvParameters) {
    while (1) {
        LED_green(1);
        vTaskDelay(500);
        LED_green(0);
        vTaskDelay(500);
    }
}

void task_CLI(void *pvParameters) {
    uint8_t parseSts = 1;

    while (1) {
        printf("%s", CLI_PROMPT);
        cli_get_cmd();
        parseSts = cli_parse_cmd();
        if (parseSts == 0) {
            cli_execute();
        }
        vTaskDelay(100);
    }
}

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    MGMT_NODE_Initialize();
    __delay_ms(10);
    printf("boot OK\n");
    while (!UART1_IsTxDone()) {};

    BaseType_t xRes = pdFAIL;

    xRes = xTaskCreate(task_main,
                       (const char *)"main",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       NULL);
    if (xRes != pdPASS) {
        LED_red(1);
    }

    xRes = xTaskCreate(task_CLI,
                       (const char *)"CLI",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       NULL);
    if (xRes != pdPASS) {
        LED_red(1);
    }

    printf("starting tasks\n");
    while (!UART1_IsTxDone()) {};
    vTaskStartScheduler();

    printf("!!! FAIL !!!\n");
    while (!UART1_IsTxDone()) {};
}
