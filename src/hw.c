// SPDX-License-Identifier: GPL-3.0-or-later

#include "hw.h"

#include <stdio.h>

#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/i2c1.h"
#include "../mcc_generated_files/i2c2.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "constants.h"
#include "agathis.h"
#include "agathis_mc.h"

uint8_t GPIO_Get_Addr_D() {
    // TODO:
    return 0;
}

void GPIO_Set_Addr_U(uint8_t addr) {
    // TODO:
}

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

static uint8_t i2c_nb_rx = 0;
static uint8_t i2c_tx_idx = 0;
static uint8_t i2c_buff[4] = {0, 0, 0, 0};

bool I2C2_StatusCallback(I2C2_SLAVE_DRIVER_STATUS status) {
    switch (status) {
        case I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED:
            //printf("t\n");
            I2C2_ReadPointerSet(&i2c_buff[i2c_tx_idx++]);
            break;
        case I2C2_SLAVE_RECEIVE_REQUEST_DETECTED:
            //printf("r\n");
            i2c_nb_rx = 0;
            i2c_tx_idx = 0;
            I2C2_WritePointerSet(i2c_buff);
            break;
        case I2C2_SLAVE_RECEIVED_DATA_DETECTED:
            //printf("d\n");
            if (i2c_buff[i2c_nb_rx] == MC_CMD_ID) {
                MC_cmdID_reply(i2c_buff);
            }
            i2c_nb_rx ++;
            break;
        case I2C2_SLAVE_10BIT_RECEIVE_REQUEST_DETECTED:
            break;
        default:
            break;
    }
    return true;
}

void SMBus_Rd_Block (I2CXfer_t *i2c_xfer, uint8_t cmd, uint8_t nb,
                     uint8_t *data) {
    I2C1_MESSAGE_STATUS sts = I2C1_MESSAGE_PENDING;

    while (!I2C1_MasterQueueIsEmpty()) {
        vTaskDelay(1);
    }
    I2C1_MasterWrite(&cmd, 1, i2c_xfer->i2c_addr, &sts);
    I2C1_MasterRead(data, nb, i2c_xfer->i2c_addr, &sts);
    while (sts == I2C1_MESSAGE_PENDING) {
        vTaskDelay(1);
    }

    if (sts == I2C1_MESSAGE_COMPLETE) {
        i2c_xfer->sts = I2C_XFER_OK;
    } else {
        i2c_xfer->sts = I2C_XFER_FAIL;
    }
}

void SMBus_Wr_Block (I2CXfer_t *i2c_xfer, uint8_t nb, uint8_t *data) {
    // TODO:
}
