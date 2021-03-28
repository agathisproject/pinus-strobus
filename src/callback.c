/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "callback.h"

#include "agathis/base.h"

static uint8_t cmd_rcv = 0;
static uint8_t cmd_buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t scratch = 0;

extern TaskHandle_t xHandle0;

void tx_i2c2(I2CXfer_t *i2c_xfer) {
}

void rx_i2c2(I2CXfer_t *i2c_xfer) {
    uint8_t cmd = i2c_getRXByte(0);

    if (i2c_xfer->nb == 1) {
        switch (cmd) {
            case AG_CMD_SUMMARY:
                i2c_setTXByte(0, AG_CMD_SUMMARY_NB);
                i2c_setTXByte(1, AG_PROTO_VER);
                i2c_setTXByte(2, MC.flags);
                i2c_setTXByte(3, MC.last_err);
                i2c_setTXByte(4, 0x00);
                break;
            case AG_CMD_TYPE:
                i2c_setTXByte(0, AG_CMD_TYPE_NB);
                i2c_setTXByte(1, (uint8_t) (MC.type & 0xFF));
                i2c_setTXByte(2, (uint8_t) ((MC.type >> 8) & 0xFF));
                break;
            case AG_CMD_CAP:
                i2c_setTXByte(0, AG_CMD_CAP_NB);
                i2c_setTXByte(1, MC.caps);
                i2c_setTXByte(2, MC.caps_en);
                break;
            case AG_CMD_MFR:
                i2c_setTXByte(0, AG_CMD_MFR_NB);
                i2c_setTXByte(1, (uint8_t) (scratch & 0xFF));
                i2c_setTXByte(2, (uint8_t) ((scratch >> 8) & 0xFF));
                i2c_setTXByte(3, (uint8_t) ((scratch >> 16) & 0xFF));
                i2c_setTXByte(4, (uint8_t) ((scratch >> 24) & 0xFF));
                break;
            default:
                break;
        }
    } else {
        if (i2c_xfer->nb == (i2c_getRXByte(1) + 2)) {
            i2c_getRXData(i2c_xfer->nb, cmd_buff);
            cmd_rcv = 1;
            switch (cmd_buff[0]) {
                case AG_CMD_SUMMARY:
                    if (cmd_buff[3] & AG_RESET_FLAG) {
                        ag_reset();
                    }
                    break;
                case AG_CMD_CAP:
                    ag_enable_caps(cmd_buff[4]);
                    break;
                case AG_CMD_MFR:
                    scratch <<= 8;
                    scratch |= cmd_buff[5];
                    scratch <<= 8;
                    scratch |= cmd_buff[4];
                    scratch <<= 8;
                    scratch |= cmd_buff[3];
                    scratch <<= 8;
                    scratch |= cmd_buff[2];
                    break;
                default:
                    break;
            }
        }
    }
}

uint8_t isRcv(void) {
    return cmd_rcv;
}

void clrRcv(void) {
    cmd_rcv = 0;
}
