// SPDX-License-Identifier: GPL-3.0-or-later

#include "agathis.h"

#include <stdio.h>

#include "constants.h"
#include "hw.h"

MgmtState MGMT = {0, 0, 0};

void MGMT_NODE_Initialize() {
    MGMT.addr_d = Get_Addr_D();
    if (MGMT.addr_d == 0) {
        MGMT.TMC = 1;
        MGMT.addr_u = (MGMT.addr_d + 1);
    } else if (MGMT.addr_d == (NODE_MAX_CNT - 1)) {
        MGMT.TMC = 1;
        MGMT.addr_u = 1;
    } else {
        MGMT.addr_u = (MGMT.addr_d + 1);
    }
    Set_Addr_U(MGMT.addr_u);
}

void MGMT_NODE_Show() {
    if (MGMT.TMC) {
        printf(" TMC: y\n");
    } else {
        printf(" TMC: n\n");
    }
    printf("addr: %#03x/%#03x\n", MGMT.addr_d, MGMT.addr_u);
}
