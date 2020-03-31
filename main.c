// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"

#define FCY (_XTAL_FREQ / 2)
#include <libpic30.h>

#include "src/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "src/cli.h"

void LED_red_ON() {
    IO_RA0_SetHigh();
}

void LED_red_OFF() {
    IO_RA0_SetLow();
}

void LED_green_ON() {
    IO_RA1_SetHigh();
}

void LED_green_OFF() {
    IO_RA1_SetLow();
}

void LED_blue_ON() {
    IO_RC7_SetHigh();
}

void LED_blue_OFF() {
    IO_RC7_SetLow();
}

void task_main(void *pvParameters) {
    while (1) {
        LED_green_ON();
        vTaskDelay(500);
        LED_green_OFF();
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
        LED_red_ON();
    }

    xRes = xTaskCreate(task_CLI,
                       (const char *)"CLI",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       NULL);
    if (xRes != pdPASS) {
        LED_red_ON();
    }

    vTaskStartScheduler();

    printf("!!! FAIL !!!\n");
    while (!UART1_IsTxDone()) {};

}
