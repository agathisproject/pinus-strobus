// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HW_7424EERMK9ER9CW5
#define HW_7424EERMK9ER9CW5
/** @file */

#include <stdint.h>

uint8_t GPIO_Get_Addr_D();
void GPIO_Set_Addr_U(uint8_t addr);

void GPIO_LED_Red(uint8_t state);
void GPIO_LED_Green(uint8_t state);
void GPIO_LED_Blue(uint8_t state);

typedef enum {
    I2C_XFER_NONE,
    I2C_XFER_OK,
    I2C_XFER_FAIL,
} I2CXferStatus;

typedef struct {
    I2CXferStatus sts;
    uint8_t i2c_id;
    uint8_t i2c_addr;
} I2CXfer_t;

void SMBus_Rd_Block (I2CXfer_t *i2c_xfer, uint8_t cmd, uint8_t nb,
                     uint8_t *data);
void SMBus_Wr_Block (I2CXfer_t *i2c_xfer, uint8_t nb, uint8_t *data);

#endif /* HW_7424EERMK9ER9CW5 */
