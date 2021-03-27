/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef I2C_J32E49JL5HYBV8GS
#define I2C_J32E49JL5HYBV8GS
/** @file */

#include <stdint.h>

typedef enum {
    I2C_ST_INIT = 0,
    I2C_ST_MA_IDLE,
    I2C_ST_MA_TX,
    I2C_ST_MA_TX2,
    I2C_ST_MA_RX,
    I2C_ST_SL_IDLE,
    I2C_ST_SL_TX,
    I2C_ST_SL_RX,
} I2C_STATE_t;

#define I2C_DEV_MAX_LEN 32

typedef struct {
    uint8_t id;
    volatile I2C_STATE_t state;
    char dev[I2C_DEV_MAX_LEN];  /**< device */
    uint8_t addr;               /**< 0x00 if master, 7bit address if slave */
    uint16_t freq;              /**< frequency (in kHz) */
    uint16_t max_bytes;         /**< max number of bytes in a transfer */
} I2CCtrl_t;

/**
 * @param id
 * @return I2CCtrl_t
 */
I2CCtrl_t *i2c_getCtrl(uint8_t id);

void i2c_showCtrl(I2CCtrl_t *I2CCtrl);

/**
 * @brief init I2C controller as master
 *
 * @param freq in kHz
 * @return 0 if no error
 */
int i2c_initMaster(I2CCtrl_t *I2CCtrl, uint16_t freq);

/**
 * @brief init I2C controller as slave
 *
 * @param freq in kHz
 * @param addr
 * @return 0 if no error
 */
int i2c_initSlave(I2CCtrl_t *I2CCtrl, uint16_t freq, uint8_t addr);

typedef enum {
    I2C_RD = 0,
    I2C_WR,
    SMBUS_RD8,
    SMBUS_WR8,
} I2C_XFER_TYPE_t;

typedef enum {
    I2C_XFER_ERR = 0,
    I2C_XFER_MA_RX,
    I2C_XFER_MA_TX,
    I2C_XFER_SL_RX,
    I2C_XFER_SL_TX,
} I2C_XFER_STATUS_t;

typedef enum {
    I2C_OPT_NONE = 0,
    I2C_OPT_RESTART,
} I2C_XFER_OPTS_t;

typedef struct {
    volatile I2C_XFER_TYPE_t type;
    I2C_XFER_OPTS_t opts;
    volatile I2C_XFER_STATUS_t status;
    uint8_t addr;
    uint8_t cmd;
    volatile uint16_t nb;
} I2CXfer_t;

/**
 * @brief TX callback function
 * Called on:
 *   - bus error, illegal stop/start
 *   - master TX, after last data ACK
 *   - master TX, if data is NACKd
 *   - master TX, lost bus arbitration
 *   - slave TX, after last data ACK
 */
void i2c_setTXCallback(void (*function)(I2CXfer_t *));

/**
 * @brief RX callback function
 * Called on:
 *   - master RX, after the slave NACKs the last byte
 *   - slave RX, after STOP or Repeated START
 */
void i2c_setRXCallback(void (*function)(I2CXfer_t *));

/**
 * @brief copy *nb* bytes from *data* to TX buffer
 *
 * @param nb
 * @param data
 * @return 0 if no error
 */
int i2c_setTXData(uint16_t nb, const uint8_t *data);

/**
 * @brief set the *nb* byte of TX buffer to *data*
 *
 * @param nb
 * @param data
 */
void i2c_setTXByte(uint16_t nb, uint8_t data);

/**
 * @brief copy *nb* bytes from RX buffer to *data*
 *
 * @param nb
 * @param data
 * @return 0 if no error
 */
int i2c_getRXData(uint16_t nb, uint8_t *data);

/**
 * @brief get the *nb* byte of RX buffer
 *
 * @param nb
 * @return uint8_t
 */
uint8_t i2c_getRXByte(uint16_t nb);

#endif /* I2C_J32E49JL5HYBV8GS */
