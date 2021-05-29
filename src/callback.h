// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CALLBACK_J39EZD4F5QGUGQMU
#define	CALLBACK_J39EZD4F5QGUGQMU
/** @file */

#include <stdio.h>

#include "hw/i2c.h"

void tx_i2c1(I2CXfer_t *i2c_xfer);
void rx_i2c1(I2CXfer_t *i2c_xfer);

uint8_t getRX1(void);
void clrRX1(void);

#endif /* CALLBACK_J39EZD4F5QGUGQMU */
