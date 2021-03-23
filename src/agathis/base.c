/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "base.h"

#include <stdio.h>
#if defined (__AVR__)
#include <avr/wdt.h>
#elif defined (__XC16__)
#endif
#include "hw/gpio.h"

AG_MC_STATE_t MC = {0, 0, 0,
                    0, 0, 0,
                    0, 0xFFFF
                   };

AG_MC_SCAN_INFO_t RmtMC[MC_MAX_CNT - 1] = {
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
    {MC_NOT_PRESENT, 0, 0},
};

#if defined (__XC16__)
SemaphoreHandle_t xSemaphore_MMC = NULL;
#endif

#if defined(__AVR__)
uint8_t p_gpio_addr_d(void) {
    uint8_t res = ((gpio_get(6) << 2) + (gpio_get(5) << 1) + gpio_get(4));
    return res;
}
#elif defined(__XC16__)
uint8_t p_gpio_addr_d(void) {
    // TODO:
    return 0;
}
#endif

#if defined(__AVR__)
void p_gpio_addr_u(uint8_t addr) {
    gpio_set(7, (addr & 0x01));
    gpio_set(8, ((addr >> 1) & 0x01));
    gpio_set(9, ((addr >> 2) & 0x01));
}
#elif defined(__XC16__)
void p_gpio_addr_u(uint8_t addr) {
    // TODO:
}
#endif

void ag_update_pwr(void) {

}

void ag_enable_caps(uint8_t val) {
    MC.caps_en = (val & MC.caps);
}

void ag_init() {
    MC.flags = AG_EEPROM_FLAG;
    MC.caps = (AG_CAP_PCIE_FLAG | AG_CAP_USB_FLAG | AG_CAP_JTAG_FLAG |
               AG_CAP_CLK100_FLAG);

    MC.addr_d = p_gpio_addr_d();
    MC.addr_i2c = (I2C_OFFSET + MC.addr_d);
    if (MC.addr_d == 0) {
        MC.flags |= AG_TMC_FLAG;
        MC.addr_u = (MC.addr_d + 1);
        MC.addr_i2c = (I2C_OFFSET + 3); // TODO: for test, remove on release
    } else if (MC.addr_d == (MC_MAX_CNT - 1)) {
        MC.flags |= AG_TMC_FLAG;
        MC.addr_u = 1;
    } else {
        MC.addr_u = (MC.addr_d + 1);
    }
    p_gpio_addr_u(MC.addr_u);

    ag_enable_caps(0x00);
    MC.last_err = 0;
    ag_update_pwr();
    printf("%s addr: 0x%02X / 0x%02X (0x%02X)\n", PREFIX_MC,
           MC.addr_d, MC.addr_u, MC.addr_i2c);
}

#if defined(__AVR__)
void ag_reset() {
    printf("reset\n");
    wdt_enable(WDTO_15MS);
}
#elif defined(__XC16__)
void ag_reset() {

}
#endif
