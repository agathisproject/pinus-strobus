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

#include "hw/i2c.h"
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

uint32_t scratch = 0;

void tx_i2c(I2CXfer_t *i2c_xfer) {

}

void rx_i2c(I2CXfer_t *i2c_xfer) {
    uint8_t reply[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t cmd = i2c_getRXByte(0);

    if (i2c_xfer->nb == 1) {
        switch (cmd) {
            case AG_CMD_SUMMARY:
                reply[0] = AG_CMD_SUMMARY_NB;
                reply[1] = AG_PROTO_VER;
                reply[2] = MC.flags;
                reply[3] = MC.last_err;
                reply[4] = 0x00;
                i2c_setTXData((AG_CMD_SUMMARY_NB + 1), reply);
                break;
            case AG_CMD_TYPE:
                reply[0] = AG_CMD_TYPE_NB;
                reply[1] = (uint8_t) MC.type;
                reply[2] = (uint8_t) (MC.type >> 8);
                i2c_setTXData((AG_CMD_TYPE_NB + 1), reply);
                break;
            case AG_CMD_CAP:
                reply[0] = AG_CMD_CAP_NB;
                reply[1] = MC.caps;
                reply[2] = MC.caps_en;
                i2c_setTXData((AG_CMD_TYPE_NB + 1), reply);
                break;
            case AG_CMD_MFR:
                i2c_setTXByte(0, AG_CMD_MFR_NB);
                i2c_setTXByte(1, (uint8_t) (scratch & 0xFF));
                i2c_setTXByte(2, (uint8_t) ((scratch >> 8) & 0xFF));
                i2c_setTXByte(3, (uint8_t) ((scratch >> 16) & 0xFF));
                i2c_setTXByte(4, (uint8_t) ((scratch >> 24) & 0xFF));
            default:
                break;
        }
    } else {
        if (i2c_xfer->nb == (i2c_getRXByte(1) + 2)) {
            switch (cmd) {
                case AG_CMD_CAP:
                    break;
                case AG_CMD_MFR:
                    scratch <<= 8;
                    scratch |= i2c_getRXByte(5);
                    scratch <<= 8;
                    scratch |= i2c_getRXByte(4);
                    scratch <<= 8;
                    scratch |= i2c_getRXByte(3);
                    scratch <<= 8;
                    scratch |= i2c_getRXByte(2);
                    break;
                default:
                    break;
            }
        }
    }
}

void i2c_init() {
    i2c_initMaster(i2c_getCtrl(0), 100);
    i2c_initSlave(i2c_getCtrl(1), 100, MC.addr_i2c);
    i2c_setTXCallback(&tx_i2c);
    i2c_setRXCallback(&rx_i2c);
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
