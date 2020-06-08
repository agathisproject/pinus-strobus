// SPDX-License-Identifier: GPL-3.0-or-later

#include "agathis_tmc.h"

#include "hw.h"

MCCmdStatus_t TMC_cmdID(uint8_t mc_id, uint8_t *reply) {
    I2CXfer_t xfer = {I2C_XFER_NONE, 1, (I2C_OFFSET + mc_id)};

    SMBus_Rd_Block(&xfer, MC_CMD_ID, MC_CMD_ID_NB, reply);
    if (xfer.sts == I2C_XFER_OK) {
        return MC_CMD_OK;
    } else {
        return MC_CMD_FAIL;
    }
}
