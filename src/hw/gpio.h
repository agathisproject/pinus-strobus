/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef GPIO_799G4Z42B2HTRMNQ
#define GPIO_799G4Z42B2HTRMNQ
/** @file */

#include <stdint.h>

typedef enum {
    PIN_TYPE_IN = 0,
    PIN_TYPE_OUT,
    PIN_TYPE_TRISTATE,
} PIN_TYPE_t;

void gpio_cfg(uint8_t pinId, PIN_TYPE_t type);

void gpio_set(uint8_t pinId, uint8_t val);

uint8_t gpio_get(uint8_t pinId);

#endif /* GPIO_799G4Z42B2HTRMNQ */
