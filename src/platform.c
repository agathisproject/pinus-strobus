// SPDX-License-Identifier: GPL-3.0-or-later

#include "platform.h"

#include "callback.h"
#include "agathis/base.h"

void cpu_init(void) {
    uint16_t tmp = RCON;
    printf("%s reset reason: %#x\n", PREFIX_MC, tmp);
}

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
}

void i2c_init(void) {
    i2c_initMaster(0, 100);
    i2c_setTXCallback(0, NULL);
    i2c_setTXCallback(0, NULL);

    i2c_initSlave(1, 100, MOD_STATE.addr_i2c);
    i2c_setTXCallback(1, &tx_i2c1);
    i2c_setRXCallback(1, &rx_i2c1);

    i2c_showCtrl(0);
    i2c_showCtrl(1);
}
