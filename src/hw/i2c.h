// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef I2C_J32E49JL5HYBV8GS
#define I2C_J32E49JL5HYBV8GS
/** @file */

#include <stdint.h>

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
    volatile I2C_XFER_STATUS_t status;
    I2C_XFER_OPTS_t opts;
    uint8_t addr;
    uint8_t cmd;
    volatile uint16_t nb;
} I2CXfer_t;

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

#if defined(__AVR__)
#define I2C_CTRL_CNT 1
#define I2C_BUFFER_SIZE 16
#elif defined(__XC16__)
#define I2C_CTRL_CNT 2
#define I2C_BUFFER_SIZE 16
#endif
typedef struct {
    uint8_t id;
    volatile I2C_STATE_t state;
    uint8_t addr;                   /**< 0x00 if master, 7bit address if slave */
    uint16_t freq;                  /**< frequency (in kHz) */
    void (*tx_cback)(I2CXfer_t *);  /**< TX callback */
    void (*rx_cback)(I2CXfer_t *);  /**< RX callback */
    uint8_t tx_buff[I2C_BUFFER_SIZE];
    uint8_t rx_buff[I2C_BUFFER_SIZE];
    volatile uint16_t tx_buff_idx;
    volatile uint16_t rx_buff_idx;
} I2CCtrl_t;

/**
 * @param id I2C Controller id
 * @return I2CCtrl_t
 */
I2CCtrl_t *i2c_getCtrl(uint8_t id);

/**
 * @brief TX callback function
 * Called on:
 *   - bus error, illegal stop/start
 *   - master TX, after last data ACK
 *   - master TX, if data is NACKd
 *   - master TX, lost bus arbitration
 *   - slave TX, after last data ACK
 *
 * @param id I2C Controller id
 */
void i2c_setTXCallback(uint8_t id, void (*function)(I2CXfer_t *));

/**
 * @brief RX callback function
 * Called on:
 *   - master RX, after the slave NACKs the last byte
 *   - slave RX, after STOP or Repeated START
 *
 * @param id I2C Controller id
 */
void i2c_setRXCallback(uint8_t id, void (*function)(I2CXfer_t *));

/**
 * @brief copy *nb* bytes from *data* to TX buffer
 *
 * @param id I2C Controller id
 * @param nb
 * @param data
 * @return 0 if no error
 */
int i2c_setTXData(uint8_t id, uint16_t nb, const uint8_t *data);

/**
 * @brief set the *nb* byte of TX buffer to *data*
 *
 * @param id I2C Controller id
 * @param nb
 * @param data
 */
void i2c_setTXByte(uint8_t id, uint16_t nb, uint8_t data);

/**
 * @brief copy *nb* bytes from RX buffer to *data*
 *
 * @param id I2C Controller id
 * @param nb
 * @param data
 * @return 0 if no error
 */
int i2c_getRXData(uint8_t id, uint16_t nb, uint8_t *data);

/**
 * @brief get the *nb* byte of RX buffer
 *
 * @param id I2C Controller id
 * @param nb
 * @return uint8_t
 */
uint8_t i2c_getRXByte(uint8_t id, uint16_t nb);

/**
 * @brief init I2C controller as master
 *
 * @param id I2C Controller id
 * @param freq in kHz
 * @return 0 if no error
 */
int i2c_initMaster(uint8_t id, uint16_t freq);

/**
 * @brief init I2C controller as slave
 *
 * @param id I2C Controller id
 * @param freq in kHz
 * @param addr
 * @return 0 if no error
 */
int i2c_initSlave(uint8_t id, uint16_t freq, uint8_t addr);

/**
 * @brief show I2C Controller info
 *
 * @param id I2C Controller id
 */
void i2c_showCtrl(uint8_t id);

#endif /* I2C_J32E49JL5HYBV8GS */
