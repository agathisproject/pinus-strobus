// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AGATHIS_6PLS6RVRFVYEP7NX
#define AGATHIS_6PLS6RVRFVYEP7NX
/** @file */

#include "cli.h"

#define I2C_OFFSET 0x20

typedef struct {
    uint8_t TMC; /**< Tree Management Controller enabled */
    uint8_t addr_d; /**< down-trunk management address */
    uint8_t addr_u; /**< up-trunk management address */
} MgmtState;

#define NODE_MAX_CNT 16

typedef struct {
    uint8_t type;
} MgmtNode;

extern MgmtState MGMT;

void MGMT_NODE_Initialize();

void MGMT_NODE_Show();

#endif /* AGATHIS_6PLS6RVRFVYEP7NX */
