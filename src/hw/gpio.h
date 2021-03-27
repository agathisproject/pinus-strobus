/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef GPIO_799G4Z42B2HTRMNQ
#define GPIO_799G4Z42B2HTRMNQ
/** @file */

#include <stdint.h>

typedef enum {
    PIN0 = 0,
    PIN1,
    PIN2,
    PIN3,
    PIN4,
    PIN5,
    PIN6,
    PIN7,
    PIN8,
    PIN9,
    PIN_RA0,
    PIN_RA1,
    PIN_RC3,
    PIN_RC4,
    PIN_RC5,
    PIN_RC7,
    PIN_NONE,
} PIN_ID_t;

typedef enum {
    PIN_TYPE_IN = 0,
    PIN_TYPE_OUT,
    PIN_TYPE_TRISTATE,
} PIN_TYPE_t;

void gpio_cfg(PIN_ID_t pinId, PIN_TYPE_t type);

void gpio_set(PIN_ID_t pinId, uint8_t val);

uint8_t gpio_get(PIN_ID_t pinId);

#endif /* GPIO_799G4Z42B2HTRMNQ */
