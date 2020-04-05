// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AGATHIS_6PLS6RVRFVYEP7NX
#define AGATHIS_6PLS6RVRFVYEP7NX
/** @file */

#include <stdint.h>

#define I2C_OFFSET 0x20

typedef struct {
    uint8_t TMC; /**< Tree Management Controller enabled */
    uint8_t addr_d; /**< down-trunk management address */
    uint8_t addr_u; /**< up-trunk management address */
} MCState_t;

#define NODE_MAX_CNT 16

typedef struct {
    uint8_t type;
} MCInfo_t;

extern MCState_t MC;

void MC_Initialize();

void MC_Show();

#endif /* AGATHIS_6PLS6RVRFVYEP7NX */
