// SPDX-License-Identifier: GPL-3.0-or-later

#include "agathis.h"

#include <stdio.h>

#include "../mcc_generated_files/i2c2.h"

#include "constants.h"
#include "hw.h"

LclMCState_t MC = {0, 0, 0, 0, 0, 0};

MCInfo_t RmtMC[MC_MAX_CNT] = {
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

SemaphoreHandle_t xSemaphore_MMC = NULL;

void MC_Initialize() {
    MC.addr_d = GPIO_Get_Addr_D();
    MC.addr_i2c = (I2C_OFFSET + MC.addr_d);
    if (MC.addr_d == 0) {
        MC.TMC = 1;
        MC.addr_u = (MC.addr_d + 1);
        MC.addr_i2c = (I2C_OFFSET + 1); // TODO: for test, remove on release
    } else if (MC.addr_d == MC_MAX_CNT) {
        MC.TMC = 1;
        MC.addr_u = 1;
    } else {
        MC.addr_u = (MC.addr_d + 1);
    }

    GPIO_Set_Addr_U(MC.addr_u);
    I2C2_SlaveAddressSet(MC.addr_i2c);
}

void MC_Show() {
    if (MC.TMC) {
        printf("  TMC: y\n");
    } else {
        printf("  TMC: n\n");
    }
    printf(" addr: %d(d) %d(u) %#03x\n", MC.addr_d, MC.addr_u, MC.addr_i2c);
    printf("power: %d\n", ((MC.pow_rst & MC_CMD_ID_PWR_MASK) >>
                           MC_CMD_ID_PWR_OFFS));
}

void MC_SetPower(uint8_t val) {
    if (val > 7) {
        printf("INCORRECT power state: %d\n", val);
        return;
    }

    MC.pow_rst &= ~MC_CMD_ID_PWR_MASK;
    MC.pow_rst |= ((val << MC_CMD_ID_PWR_OFFS) & MC_CMD_ID_PWR_MASK);
    // TODO: do HW actions
}
