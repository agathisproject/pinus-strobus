// SPDX-License-Identifier: GPL-3.0-or-later

#include "agathis.h"

#include <stdio.h>

#include "../mcc_generated_files/i2c2.h"

#include "constants.h"
#include "hw.h"

MCState_t MC = {0, 0, 0};
MCInfo_t MMC[MC_MAX_CNT] = {
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
};
SemaphoreHandle_t xSemaphore_MMC = NULL;

void MC_Initialize() {
    MC.addr_d = GPIO_Get_Addr_D();
    if (MC.addr_d == 0) {
        MC.TMC = 1;
        MC.addr_u = (MC.addr_d + 1);
    } else if (MC.addr_d == (MC_MAX_CNT - 1)) {
        MC.TMC = 1;
        MC.addr_u = 1;
    } else {
        MC.addr_u = (MC.addr_d + 1);
    }
    GPIO_Set_Addr_U(MC.addr_u);

    I2C2_SlaveAddressSet(I2C_OFFSET + MC.addr_u);
    I2C2_SlaveAddressMaskSet(0x7F);
}

void MC_Show() {
    if (MC.TMC) {
        printf(" TMC: y\n");
    } else {
        printf(" TMC: n\n");
    }
    printf("addr: %#03x/%#03x\n", MC.addr_d, MC.addr_u);
}
