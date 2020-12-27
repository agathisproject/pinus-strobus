/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "i2c.h"

#include <stdio.h>
#if defined(__linux__)
#include <stdlib.h>
#include "string.h"
#elif defined(__AVR__)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#elif defined(__XC16__)
#include <i2c1.h>
#include <i2c2.h>
#endif

static void (*i2c_onTX)(I2CXfer_t *) = NULL;
static void (*i2c_onRX)(I2CXfer_t *) = NULL;

#if defined(__linux__)
#define I2C_BUFFER_SIZE 128

static uint8_t i2c_tx_buff[I2C_BUFFER_SIZE];
volatile static uint16_t i2c_tx_buff_idx = 0;
static uint8_t i2c_rx_buff[I2C_BUFFER_SIZE];
volatile static uint16_t i2c_rx_buff_idx = 0;

static I2CCtrl_t *lst_I2CCtrl = NULL;

I2CCtrl_t *p_linux_getCtrl(uint8_t id) {
    if (id != 0) {
        return NULL;
    }

    lst_I2CCtrl = malloc(sizeof(I2CCtrl_t));
    if (lst_I2CCtrl == NULL) {
        printf("[I2C] CANNOT malloc: %s\n", __func__ );
        exit(EXIT_FAILURE);
    }

    lst_I2CCtrl->id = 0;
    lst_I2CCtrl->state = I2C_ST_INIT;
    strncpy(lst_I2CCtrl->dev, "dummy", I2C_DEV_MAX_LEN);
    lst_I2CCtrl->addr = 0;
    lst_I2CCtrl->freq = 0;
    lst_I2CCtrl->max_bytes = I2C_BUFFER_SIZE;

    return lst_I2CCtrl;
}
#elif defined(__AVR__)
#define I2C_BUFFER_SIZE 16

static uint8_t i2c_tx_buff[I2C_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile static uint16_t i2c_tx_buff_idx = 0;
static uint8_t i2c_rx_buff[I2C_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile static uint16_t i2c_rx_buff_idx = 0;

static I2CCtrl_t s_I2CCtrl = {0, I2C_ST_INIT, "avr", 0, 0, I2C_BUFFER_SIZE};
static I2CXfer_t s_I2CXfer_cur = {I2C_RD, I2C_XFER_SL_RX, I2C_OPT_NONE, 0, 0};

I2CCtrl_t *p_avr_getCtrl(uint8_t id) {
    if (id != 0) {
        return NULL;
    }
    return &s_I2CCtrl;
}
#elif defined(__XC16__)
#define I2C_BUFFER_SIZE 16

static uint8_t i2c_tx_buff[I2C_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile static uint16_t i2c_tx_buff_idx = 0;
static uint8_t i2c_rx_buff[I2C_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile static uint16_t i2c_rx_buff_idx = 0;
static uint8_t i2c2_rx = 0;

static I2CCtrl_t lst_I2CCtrl[2] = {
    {0, I2C_ST_MA_IDLE, "pic1", 0, 100, I2C_BUFFER_SIZE},
    {1, I2C_ST_SL_IDLE, "pic2", 0, 100, I2C_BUFFER_SIZE}
};
static I2CXfer_t s_I2CXfer_cur = {I2C_RD, I2C_XFER_SL_RX, I2C_OPT_NONE, 0, 0};

I2CCtrl_t *p_pic_getCtrl(uint8_t id) {
    if (id > 1) {
        return NULL;
    }
    return &lst_I2CCtrl[id];
}
#endif

I2CCtrl_t *i2c_getCtrl(uint8_t id) {
#if defined(__linux__)
    return p_linux_getCtrl(id);
#elif defined(__AVR__)
    return p_avr_getCtrl(id);
#elif defined(__XC16__)
    return p_pic_getCtrl(id);
#endif
}

void i2c_showCtrl(I2CCtrl_t *I2CCtrl) {
    printf("[I2C] %d: %s, state %d (0x%02X, %d kHz)\n", I2CCtrl->id,
           I2CCtrl->dev, I2CCtrl->state, I2CCtrl-> addr, I2CCtrl->freq);
}
//------------------------------------------------------------------------------
#if defined(__linux__)
int p_linux_initMaster(I2CCtrl_t *I2CCtrl, uint16_t freq) {
    I2CCtrl->state = I2C_ST_MA_IDLE;
    I2CCtrl->addr = 0;
    I2CCtrl->freq = freq;
    return 0;
}

int p_linux_initSlave(I2CCtrl_t *I2CCtrl, uint16_t freq, uint8_t addr) {
    I2CCtrl->state = I2C_ST_SL_IDLE;
    I2CCtrl->addr = addr;
    I2CCtrl->freq = freq;
    return 0;
}
#elif defined(__AVR__)
/**
 * set bit rate register.
 *
 * @param f - desired frequency (in kHz)
 *
 * @return actual frequency (in kHz)
 */
uint16_t p_avr_setRate(uint16_t f) {
    uint16_t f_cpu = F_CPU / 1000L;
    uint16_t tmp = 0;
    uint8_t prescaler = 0;

    //printf("DBG freq: %u\n", f);
    for (prescaler = 1; prescaler < 65; prescaler = (prescaler * 4)) {
        // see datasheet - 21.5.2 Bit Rate Generator Unit
        tmp = ((uint16_t) (f_cpu / f) - 16) / (2 * prescaler);
        if (tmp < 255) {
            break;
        }
    }
    //printf("DBG pre: %u, %u\n", prescaler, tmp);
    switch (prescaler) {
        case 4:
            TWSR = _BV(TWPS0);
            break;
        case 16:
            TWSR = _BV(TWPS1);
            break;
        case 64:
            TWSR = (_BV(TWPS1) | _BV(TWPS0));
            break;
        default: // prescaler set to 1
            TWSR = 0x00;
            break;
    }
    TWBR = tmp;
    return (uint16_t) (f_cpu / (16 + 2 * tmp * prescaler));
}

int p_avr_initMaster(I2CCtrl_t *I2CCtrl, uint16_t freq) {
    I2CCtrl->freq = p_avr_setRate(freq);

    I2CCtrl->state = I2C_ST_MA_IDLE;
    // enable TWI interface and TWI interrupt
    TWCR = (_BV(TWEN) | _BV(TWIE));
    return 0;
}

int p_avr_initSlave(I2CCtrl_t *I2CCtrl, uint16_t freq, uint8_t addr) {
    I2CCtrl->freq = p_avr_setRate(freq);
    I2CCtrl->addr = addr;
    TWAMR = 0x00;
    TWAR = addr << 1;

    I2CCtrl->state = I2C_ST_SL_IDLE;
    // enable TWI interface, ACK and TWI interrupt
    TWCR = (_BV(TWEA) | _BV(TWEN) | _BV(TWIE));
    return 0;
}
#elif defined(__XC16__)
int p_pic_initMaster(I2CCtrl_t *I2CCtrl, uint16_t freq) {
    if (I2CCtrl->id == 0) {
        return 0;
    }
    return 1;
}

int p_pic_initSlave(I2CCtrl_t *I2CCtrl, uint16_t freq, uint8_t addr) {
    if (I2CCtrl->id == 1) {
        I2CCtrl->addr = addr;
        I2C2_SlaveAddressSet(I2CCtrl->addr);
        return 0;
    }
    return 1;
}
#endif

int i2c_initMaster(I2CCtrl_t *I2CCtrl, uint16_t freq) {
#if defined(__linux__)
    return p_linux_initMaster(I2CCtrl, freq);
#elif defined(__AVR__)
    return p_avr_initMaster(I2CCtrl, freq);
#elif defined(__XC16__)
    return p_pic_initMaster(I2CCtrl, freq);
#endif
}

int i2c_initSlave(I2CCtrl_t *I2CCtrl, uint16_t freq, uint8_t addr) {
#if defined(__linux__)
    return p_linux_initSlave(I2CCtrl, freq, addr);
#elif defined(__AVR__)
    return p_avr_initSlave(I2CCtrl, freq, addr);
#elif defined(__XC16__)
    return p_pic_initSlave(I2CCtrl, freq, addr);
#endif
}
//------------------------------------------------------------------------------
void i2c_setTXCallback(void (*function)(I2CXfer_t *)) {
    i2c_onTX = function;
}

void i2c_setRXCallback(void (*function)(I2CXfer_t *)) {
    i2c_onRX = function;
}

int i2c_setTXData(uint16_t nb, const uint8_t *data) {
    if (nb > I2C_BUFFER_SIZE) {
        return -1;
    }

    for (uint16_t i = 0; i < nb; i++) {
        i2c_tx_buff[i] = data[i];
    }
    return 0;
}

int i2c_getRXData(uint16_t nb, uint8_t *data) {
    if (nb > I2C_BUFFER_SIZE) {
        return -1;
    }

    for (uint16_t i = 0; i < nb; i++) {
        data[i] = i2c_rx_buff[i];
    }
    return 0;
}
//------------------------------------------------------------------------------
//#if defined(__AVR__)
///** use for debug only */
//inline void ledOn() {
//    DDRB |= (1<<PB5);
//    PORTB |= (1<<PB5);
//}
//
//inline void ledOff() {
//    DDRB |= (1<<PB5);
//    PORTB &= ~(1<<PB5);
//}
//#endif
//------------------------------------------------------------------------------
#if defined(__AVR__)
/** sets the Start bit and clears the INT bit in the TWI control register. */
inline void i2c_setSTA() {
    TWCR = (_BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE));
}

/** sets the Stop bit and clears the INT bit in the TWI control register. */
inline void i2c_setSTO() {
    TWCR = (_BV(TWINT) | _BV(TWSTO) | _BV(TWEN) | _BV(TWIE));
}

/** clears the INT bit in the TWI control register. */
inline void i2c_setNACK() {
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWIE));
}

/** sets the Acknowledge bit and clears the INT bit in the TWI control register. */
inline void i2c_setACK() {
    TWCR = (_BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE));
}
#endif
//------------------------------------------------------------------------------
/**
 * sets the Write bit for the transfer and initiates Start condition on bus.
 */
//void i2c_masterWrite(uint8_t addr) {
//    while((i2c_info.state != MASTER_IDLE) && (i2c_info.state != MASTER_TX2)) {
//        //if there's already a transfer in progress wait ...
//    }
//    if(i2c_info.state == MASTER_IDLE) {
//        i2c_info.addr = (addr & 0xFE); //set write bit
//    }
//    i2c_info.state = MASTER_TX;
//    i2c_setSTA();
//}

/**
 * sets the Read bit for the transfer, writes the slave address as the first byte of the RX buffer and
 * sets the RX buffer index to 1, then initiates Start condition on bus.
 */
//void i2c_masterRead(uint8_t addr, uint8_t len, uint8_t opt) {
//    while((i2c_info.state != MASTER_IDLE) && (i2c_info.state != MASTER_TX2)) {
//        //if there's already a transfer in progress wait ...
//    }
//    i2c_info.addr = (addr | 0x01); //set read bit
//    i2c_rx_buff[0] = addr;
//    i2c_rx_buff_idx = 1;
//    i2c_info.state = MASTER_RX;
//    i2c_setSTA();
//}
//------------------------------------------------------------------------------
//uint8_t pi2c_getStatus(){
//    return i2c_info.status;
//}
//
//uint8_t i2c_getState() {
//    return i2c_info.state;
//}
//
//uint8_t pi2c_debug() {
//    return i2c_info.debug;
//}
//------------------------------------------------------------------------------
#if defined(__AVR__)
ISR(TWI_vect) {
    switch (TW_STATUS) {
        case TW_REP_START: // sent repeated start condition
        case TW_START:     // sent start condition
            i2c_tx_buff_idx = 0;
            TWDR = s_I2CXfer_cur.addr;
            i2c_setACK();
            break;
        case TW_NO_INFO:   // no state information
        case TW_BUS_ERROR: // bus error, illegal stop/start
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_ERR;
            i2c_onTX(&s_I2CXfer_cur);
            break;

        // master transmit
        case TW_MT_SLA_ACK:  // slave receiver acked address
        case TW_MT_DATA_ACK: // slave receiver acked data
            if (i2c_tx_buff_idx < I2C_BUFFER_SIZE) {
                TWDR = i2c_tx_buff[i2c_tx_buff_idx++];
                i2c_setACK();
            } else { //no more data to transmit
                if (s_I2CXfer_cur.opts == I2C_OPT_RESTART) {
                    TWCR = _BV(TWEN); //don't clear interrupt, don't enable interrupts
                    s_I2CCtrl.state = I2C_ST_MA_TX2;
                } else {
                    i2c_setSTO();
                    s_I2CCtrl.state = I2C_ST_MA_IDLE;
                }
                s_I2CXfer_cur.type = I2C_WR;
                s_I2CXfer_cur.status = I2C_XFER_MA_TX;
                s_I2CXfer_cur.nb = i2c_tx_buff_idx;
                i2c_onTX(&s_I2CXfer_cur);
            }
            break;
        case TW_MT_SLA_NACK:  // address sent, nack received
        case TW_MT_DATA_NACK: // data sent, nack received
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_MA_TX;
            s_I2CXfer_cur.nb = i2c_tx_buff_idx;
            i2c_onTX(&s_I2CXfer_cur);
            break;
        case TW_MT_ARB_LOST: // lost bus arbitration
            i2c_setNACK();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_ERR;
            i2c_onTX(&s_I2CXfer_cur);
            break;

        // master receive
        case TW_MR_DATA_ACK: // data received, ack sent
            i2c_rx_buff[i2c_rx_buff_idx++] = TWDR;
        case TW_MR_SLA_ACK:  // address sent, ack received
            if (i2c_rx_buff_idx < I2C_BUFFER_SIZE) {
                i2c_setNACK();
            }
            //else if(i2c_rx_buff_idx == i2c_rxBufferSize) { //no data in buffer
            //  i2c_setSTO();
            //  s_I2CCtrl->state = I2C_ST_MA_IDLE;
            //  i2c_onRX(i2c_rx_buff_idx, i2c_rx_buff);
            //}
            else {
                i2c_setACK();
            }
            break;
        case TW_MR_DATA_NACK: // data received, nack sent
            i2c_rx_buff[i2c_rx_buff_idx++] = TWDR;
        case TW_MR_SLA_NACK: // address sent, nack received
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_RD;
            s_I2CXfer_cur.status = I2C_XFER_MA_RX;
            s_I2CXfer_cur.nb = i2c_rx_buff_idx;
            i2c_onRX(&s_I2CXfer_cur);
            break;
        // TW_MR_ARB_LOST = TW_MT_ARB_LOST (http://www.nongnu.org/avr-libc/user-manual/group__util__twi.html)

        // slave receive
        case TW_SR_SLA_ACK:   // addressed, ACK returned
        case TW_SR_GCALL_ACK: // addressed generally, ACK returned
        case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, ACK returned
        case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, ACK returned
            s_I2CCtrl.state = I2C_ST_SL_RX;
            //i2c_rx_buff[0] = TWDR;
            i2c_rx_buff_idx = 0;
            i2c_setACK();
            break;
        case TW_SR_DATA_ACK:       // data received, ACK returned
        case TW_SR_GCALL_DATA_ACK: // data received generally, ACK returned
            if (i2c_rx_buff_idx < I2C_BUFFER_SIZE) {
                i2c_rx_buff[i2c_rx_buff_idx++] = TWDR;
                i2c_setACK();
            } else {
                i2c_setNACK();
            }
            break;
        case TW_SR_DATA_NACK:       // data received, NACK returned
        case TW_SR_GCALL_DATA_NACK: // data received generally, NACK returned
            if (i2c_rx_buff_idx < I2C_BUFFER_SIZE) {
                i2c_rx_buff[i2c_rx_buff_idx++] = TWDR;
            }
            i2c_setNACK();
            break;
        case TW_SR_STOP: // stop or repeated start condition received
            s_I2CCtrl.state = I2C_ST_SL_IDLE;
            s_I2CXfer_cur.type = I2C_RD;
            s_I2CXfer_cur.status = I2C_XFER_SL_RX;
            s_I2CXfer_cur.nb = i2c_rx_buff_idx;
            i2c_onRX(&s_I2CXfer_cur);
            i2c_setACK();
            break;

        // slave transmit
        case TW_ST_SLA_ACK:          // addressed, ACK returned
        case TW_ST_ARB_LOST_SLA_ACK: // lost arbitration, ACK returned
            s_I2CCtrl.state = I2C_ST_SL_TX;
            i2c_tx_buff_idx = 0;
            TWDR = i2c_tx_buff[i2c_tx_buff_idx++];
            i2c_setACK();
            break;
        case TW_ST_DATA_ACK: // data transmitted, ACK received
            if (i2c_tx_buff_idx < I2C_BUFFER_SIZE) {
                TWDR = i2c_tx_buff[i2c_tx_buff_idx++];
                i2c_setACK();
            } else {
                TWDR = 0xFF;
                i2c_setNACK();
            }
            break;
        case TW_ST_DATA_NACK: // data transmitted, NACK received
        case TW_ST_LAST_DATA: // last data byte transmitted, ACK received
            s_I2CCtrl.state = I2C_ST_SL_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_SL_TX;
            s_I2CXfer_cur.nb = i2c_rx_buff_idx;
            i2c_onTX(&s_I2CXfer_cur);
            i2c_setACK();
            break;
    }
}
#elif defined(__XC16__)
bool I2C2_StatusCallback(I2C2_SLAVE_DRIVER_STATUS status) {
    switch (status) {
        case I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED:
            //printf("t\n");
            s_I2CXfer_cur.type = I2C_RD;
            s_I2CXfer_cur.status = I2C_XFER_SL_TX;
            s_I2CXfer_cur.nb = i2c_rx_buff_idx;
            i2c_onTX(&s_I2CXfer_cur);
            I2C2_ReadPointerSet(&i2c_tx_buff[i2c_tx_buff_idx++]);
            break;
        case I2C2_SLAVE_RECEIVE_REQUEST_DETECTED:
            //printf("r\n");
            i2c_tx_buff_idx = 0;
            i2c_rx_buff_idx = 0;
            I2C2_WritePointerSet(&i2c2_rx);
            break;
        case I2C2_SLAVE_RECEIVED_DATA_DETECTED:
            //printf("d\n");
            i2c_rx_buff[i2c_rx_buff_idx++] = i2c2_rx;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_SL_RX;
            s_I2CXfer_cur.nb = i2c_rx_buff_idx;
            i2c_onRX(&s_I2CXfer_cur);
            break;
        case I2C2_SLAVE_10BIT_RECEIVE_REQUEST_DETECTED:
            break;
        default:
            break;
    }
    return true;
}
#endif
