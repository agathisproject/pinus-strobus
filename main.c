/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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

#include "hw/i2c.h"
#include "hw/callback.h"
#include "agathis/base.h"

void GPIO_LED_Red(uint8_t state) {
    if (state == 0) {
        IO_RA0_SetLow();
    } else  {
        IO_RA0_SetHigh();
    }
}

void GPIO_LED_Green(uint8_t state) {
    if (state == 0) {
        IO_RA1_SetLow();
    } else {
        IO_RA1_SetHigh();
    }
}

void GPIO_LED_Blue(uint8_t state) {
    if (state == 0) {
        IO_RC7_SetLow();
    } else {
        IO_RC7_SetHigh();
    }
}

void task_mc(void *pvParameters);
void task_CLI(void *pvParameters);
TaskHandle_t xHandle0 = NULL;
TaskHandle_t xHandle1 = NULL;

void i2c_init() {
    i2c_initMaster(i2c_getCtrl(0), 100);
    i2c_initSlave(i2c_getCtrl(1), 100, MC.addr_i2c);
    i2c_setTXCallback(&i2c2_tx);
    i2c_setRXCallback(&i2c2_rx);
    i2c_showCtrl(i2c_getCtrl(0));
    i2c_showCtrl(i2c_getCtrl(1));
}

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    __delay_ms(10);
    printf("boot OK\n");
    while (!UART1_IsTxDone()) {};
    ag_init();
    i2c_init();

    BaseType_t xRes = pdFAIL;

    xRes = xTaskCreate(task_mc,
                       (const char *)"main",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       (tskIDLE_PRIORITY + 1),
                       &xHandle0);
    if (xRes != pdPASS) {
        GPIO_LED_Red(1);
    }

    xRes = xTaskCreate(task_CLI,
                       (const char *)"CLI",
                       configMINIMAL_STACK_SIZE,
                       (void *)NULL,
                       tskIDLE_PRIORITY,
                       &xHandle1);
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
