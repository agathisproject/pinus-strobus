// SPDX-License-Identifier: GPL-3.0-or-later

#include "hw.h"

#include <stdio.h>

#include "constants.h"
#include "../mcc_generated_files/pin_manager.h"

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
