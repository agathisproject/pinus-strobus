/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "platform.h"

#include "callback.h"
#include "agathis/base.h"

void gpio_init(void) {
    gpio_cfg(PIN_LED_R, PIN_TYPE_OUT);
    gpio_cfg(PIN_LED_G, PIN_TYPE_OUT);
    gpio_cfg(PIN_LED_B, PIN_TYPE_OUT);

    gpio_cfg(PIN_D_ADDR0, PIN_TYPE_IN);
    gpio_cfg(PIN_D_ADDR1, PIN_TYPE_IN);
    gpio_cfg(PIN_D_ADDR2, PIN_TYPE_IN);

    gpio_cfg(PIN_U_ADDR0, PIN_TYPE_OUT);
    gpio_cfg(PIN_U_ADDR1, PIN_TYPE_OUT);
    gpio_cfg(PIN_U_ADDR2, PIN_TYPE_OUT);
    //printf("[GPIO] done\n");
    gpio_set(PIN_LED_R, 1);
}

void i2c_init(void) {
    i2c_initMaster(i2c_getCtrl(0), 100);
    i2c_initSlave(i2c_getCtrl(1), 100, MC.addr_i2c);
    i2c_setTXCallback(&i2c2_tx);
    i2c_setRXCallback(&i2c2_rx);
    i2c_showCtrl(i2c_getCtrl(0));
    i2c_showCtrl(i2c_getCtrl(1));
}
