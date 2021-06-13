// SPDX-License-Identifier: GPL-3.0-or-later

#include "callback.h"

#include "agathis/base.h"

static uint8_t rx_i2c1_flag = 0;
static uint8_t rx_i2c1_buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t scratch = 0;

extern TaskHandle_t xHandle0;

void tx_i2c1(I2CXfer_t *i2c_xfer) {
}

void rx_i2c1(I2CXfer_t *i2c_xfer) {
    uint8_t cmd = i2c_getRXByte(1, 0);

    if (i2c_xfer->nb == 1) {
        switch (cmd) {
            case AG_CMD_SUMMARY:
                i2c_setTXByte(1, 0, AG_CMD_SUMMARY_NB);
                i2c_setTXByte(1, 1, AG_PROTO_VER);
                i2c_setTXByte(1, 2, MOD_STATE.flags);
                i2c_setTXByte(1, 3, MOD_STATE.last_err);
                i2c_setTXByte(1, 4, 0x00);
                break;
            case AG_CMD_TYPE:
                i2c_setTXByte(1, 0, AG_CMD_TYPE_NB);
                i2c_setTXByte(1, 1, (uint8_t) (MOD_STATE.type & 0xFF));
                i2c_setTXByte(1, 2, (uint8_t) ((MOD_STATE.type >> 8) & 0xFF));
                break;
            case AG_CMD_CAP:
                i2c_setTXByte(1, 0, AG_CMD_CAP_NB);
                i2c_setTXByte(1, 1, MOD_STATE.caps);
                i2c_setTXByte(1, 2, 0x00);
                break;
            case AG_CMD_MFR:
                i2c_setTXByte(1, 0, AG_CMD_MFR_NB);
                i2c_setTXByte(1, 1, (uint8_t) (scratch & 0xFF));
                i2c_setTXByte(1, 2, (uint8_t) ((scratch >> 8) & 0xFF));
                i2c_setTXByte(1, 3, (uint8_t) ((scratch >> 16) & 0xFF));
                i2c_setTXByte(1, 4, (uint8_t) ((scratch >> 24) & 0xFF));
                break;
            default:
                break;
        }
    } else {
        if (i2c_xfer->nb == (i2c_getRXByte(1, 1) + 2)) {
            i2c_getRXData(1, i2c_xfer->nb, rx_i2c1_buff);
            switch (rx_i2c1_buff[0]) {
                case AG_CMD_MFR:
                    scratch <<= 8;
                    scratch |= rx_i2c1_buff[5];
                    scratch <<= 8;
                    scratch |= rx_i2c1_buff[4];
                    scratch <<= 8;
                    scratch |= rx_i2c1_buff[3];
                    scratch <<= 8;
                    scratch |= rx_i2c1_buff[2];
                    break;
                default:
                    rx_i2c1_flag = 1;
                    break;
            }
        }
    }
}

uint8_t getRX1(void) {
    return rx_i2c1_flag;
}

void clrRX1(void) {
    rx_i2c1_flag = 0;
}
