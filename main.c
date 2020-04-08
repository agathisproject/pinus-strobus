// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/uart1.h"

#define FCY (_XTAL_FREQ / 2)
#include <libpic30.h>

#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "agathis.h"
#include "hw.h"
#include "cli/cli.h"

void task_main(void *pvParameters) {
    while (1) {
        GPIO_LED_Green(1);
        vTaskDelay(500);
        GPIO_LED_Green(0);
        vTaskDelay(500);
    }
}

void task_CLI(void *pvParameters) {
    uint8_t parseSts = 1;

    while (1) {
        printf("%s", CLI_PROMPT);
        CLI_Get_Cmd();
        parseSts = CLI_Parse_Cmd();
        if (parseSts == 0) {
            CLI_Execute();
        }
        vTaskDelay(100);
    }
}

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    MC_Initialize();
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
        GPIO_LED_Red(1);
    }

    xRes = xTaskCreate(task_CLI,
                       (const char *)"CLI",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       NULL);
    if (xRes != pdPASS) {
        GPIO_LED_Red(1);
    }

    xSemaphore_MMC = xSemaphoreCreateBinary();
    if (xSemaphore_MMC == NULL) {
        GPIO_LED_Red(1);
    }
    xSemaphoreGive(xSemaphore_MMC);

    printf("starting tasks\n");
    while (!UART1_IsTxDone()) {};
    vTaskStartScheduler();

    printf("!!! FAIL !!!\n");
    while (!UART1_IsTxDone()) {};
}
