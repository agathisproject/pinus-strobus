// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLATFORM_2XL5RPCV8P7A7PGJ
#define	PLATFORM_2XL5RPCV8P7A7PGJ
/** @file */

#include <stdio.h>

#include "hw/gpio.h"
#include "hw/i2c.h"

#define PIN_LED_R PIN_RA0
#define PIN_LED_G PIN_RA1
#define PIN_LED_B PIN_RC7
#define PIN_D_ADDR0 PIN_RC3
#define PIN_D_ADDR1 PIN_RC4
#define PIN_D_ADDR2 PIN_RC5
#define PIN_U_ADDR0 PIN_NONE
#define PIN_U_ADDR1 PIN_NONE
#define PIN_U_ADDR2 PIN_NONE

void cpu_init(void);
void gpio_init(void);
void i2c_init(void);

#endif /* PLATFORM_2XL5RPCV8P7A7PGJ */
