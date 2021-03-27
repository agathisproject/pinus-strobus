/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef MISC_J39EZD4F5QGUGQMU
#define	MISC_J39EZD4F5QGUGQMU
/** @file */

#include <stdio.h>

#include "hw/i2c.h"

void i2c2_tx(I2CXfer_t *i2c_xfer);
void i2c2_rx(I2CXfer_t *i2c_xfer);

uint8_t isRcv(void);
void clrRcv(void);

#endif	/* MISC_J39EZD4F5QGUGQMU */
