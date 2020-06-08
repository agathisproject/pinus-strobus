// SPDX-License-Identifier: GPL-3.0-or-later

#include "agathis_mc.h"

MC_cmdID_reply(uint8_t *i2c_buff) {
    i2c_buff[0] = 0x03;
    i2c_buff[1] = MC.pow_rst;
    i2c_buff[2] = MC.io;
    i2c_buff[3] = 0x00;
}
