// SPDX-License-Identifier: GPL-3.0-or-later

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

//static void (*i2c_onTX)(I2CXfer_t *) = NULL;
//static void (*i2c_onRX)(I2CXfer_t *) = NULL;

#if defined(__linux__)
static uint8_t i2c_tx_buff[I2C_BUFFER_SIZE];
static volatile uint16_t i2c_tx_buff_idx = 0;
static uint8_t i2c_rx_buff[I2C_BUFFER_SIZE];
static volatile uint16_t i2c_rx_buff_idx = 0;

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
static I2CCtrl_t s_I2CCtrl = {0, I2C_ST_INIT, 0, 0, NULL, NULL,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0, 0
};
static I2CXfer_t s_I2CXfer_cur = {I2C_RD, I2C_XFER_SL_RX, I2C_OPT_NONE, 0, 0, 0};

I2CCtrl_t *p_avr_getCtrl(uint8_t id) {
    if (id != 0) {
        return NULL;
    }
    return &s_I2CCtrl;
}
#elif defined(__XC16__)
static I2CCtrl_t lst_I2CCtrl[2] = {
    {
        0, I2C_ST_INIT, 0, 0, NULL, NULL,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        0, 0
    },
    {
        1, I2C_ST_INIT, 0, 0, NULL, NULL,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        0, 0
    },
};
static uint8_t i2c1_data = 0;
static uint8_t i2c2_data = 0;
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
//------------------------------------------------------------------------------
void i2c_setTXCallback(uint8_t id, void (*function)(I2CXfer_t *)) {
    i2c_getCtrl(id)->tx_cback = function;
}

void i2c_setRXCallback(uint8_t id, void (*function)(I2CXfer_t *)) {
    i2c_getCtrl(id)->rx_cback = function;
}

int i2c_setTXData(uint8_t id, uint16_t nb, const uint8_t *data) {
    if (nb > I2C_BUFFER_SIZE) {
        return -1;
    }

    for (uint16_t i = 0; i < nb; i++) {
        i2c_getCtrl(id)->tx_buff[i] = data[i];
    }
    return 0;
}

void i2c_setTXByte(uint8_t id, uint16_t nb, uint8_t data) {
    if (nb < I2C_BUFFER_SIZE) {
        i2c_getCtrl(id)->tx_buff[nb] = data;
    }
}

int i2c_getRXData(uint8_t id, uint16_t nb, uint8_t *data) {
    if (nb > I2C_BUFFER_SIZE) {
        return -1;
    }

    for (uint16_t i = 0; i < nb; i++) {
        data[i] = i2c_getCtrl(id)->rx_buff[i];
    }
    return 0;
}

uint8_t i2c_getRXByte(uint8_t id, uint16_t nb) {
    if (nb > I2C_BUFFER_SIZE) {
        return -1;
    } else {
        return i2c_getCtrl(id)->rx_buff[nb];
    }
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

int p_avr_initMaster(uint8_t id, uint16_t freq) {
    I2CCtrl_t *ctrl = i2c_getCtrl(id);
    if (ctrl == NULL) {
        return 1;
    }

    ctrl->freq = p_avr_setRate(freq);

    ctrl->state = I2C_ST_MA_IDLE;
    // enable TWI interface and TWI interrupt
    TWCR = (_BV(TWEN) | _BV(TWIE));
    return 0;
}

int p_avr_initSlave(uint8_t id, uint16_t freq, uint8_t addr) {
    I2CCtrl_t *ctrl = i2c_getCtrl(id);
    if (ctrl == NULL) {
        return 1;
    }

    ctrl->freq = p_avr_setRate(freq);

    ctrl->addr = addr;
    TWAMR = 0x00;
    TWAR = addr << 1;

    ctrl->state = I2C_ST_SL_IDLE;
    // enable TWI interface, ACK and TWI interrupt
    TWCR = (_BV(TWEA) | _BV(TWEN) | _BV(TWIE));
    return 0;
}
#elif defined(__XC16__)
int p_pic_initMaster(uint8_t id, uint16_t freq) {
    I2CCtrl_t *ctrl = i2c_getCtrl(id);
    if (ctrl == NULL) {
        return 1;
    }

    ctrl->freq = 100;

    ctrl->state = I2C_ST_MA_IDLE;
    return 0;
}

int p_pic_initSlave(uint8_t id, uint16_t freq, uint8_t addr) {
    I2CCtrl_t *ctrl = i2c_getCtrl(id);
    if (ctrl == NULL) {
        return 1;
    }

    ctrl->freq = 100;

    ctrl->addr = addr;
    I2C2_SlaveAddressSet(ctrl->addr);

    ctrl->state = I2C_ST_SL_IDLE;
    return 0;
}
#endif

int i2c_initMaster(uint8_t id, uint16_t freq) {
#if defined(__linux__)
    return p_linux_initMaster(id, freq);
#elif defined(__AVR__)
    return p_avr_initMaster(id, freq);
#elif defined(__XC16__)
    return p_pic_initMaster(id, freq);
#endif
}

int i2c_initSlave(uint8_t id, uint16_t freq, uint8_t addr) {
#if defined(__linux__)
    return p_linux_initSlave(id, freq, addr);
#elif defined(__AVR__)
    return p_avr_initSlave(id, freq, addr);
#elif defined(__XC16__)
    return p_pic_initSlave(id, freq, addr);
#endif
}
//------------------------------------------------------------------------------
void i2c_showCtrl(uint8_t id) {
    I2CCtrl_t *ctrl = i2c_getCtrl(id);
    if (ctrl == NULL) {
        return;
    }

    printf("[I2C] %d: state %d (0x%02X, %d kHz)\n", ctrl->id,
           ctrl->state, ctrl-> addr, ctrl->freq);
}
//------------------------------------------------------------------------------
#if defined(__AVR__)
/** sets the Start bit and clears the INT bit in the TWI control register. */
inline void i2c_setSTA(void) {
    TWCR = (_BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE));
}

/** sets the Stop bit and clears the INT bit in the TWI control register. */
inline void i2c_setSTO(void) {
    TWCR = (_BV(TWINT) | _BV(TWSTO) | _BV(TWEN) | _BV(TWIE));
}

/** clears the INT bit in the TWI control register. */
inline void i2c_setNACK(void) {
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWIE));
}

/** sets the Acknowledge bit and clears the INT bit in the TWI control register. */
inline void i2c_setACK(void) {
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
#if defined(__AVR__)
ISR(TWI_vect) {
    switch (TW_STATUS) {
        case TW_REP_START: // sent repeated start condition
        case TW_START:     // sent start condition
            s_I2CCtrl.tx_buff_idx = 0;
            TWDR = s_I2CXfer_cur.addr;
            i2c_setACK();
            break;
        case TW_NO_INFO:   // no state information
        case TW_BUS_ERROR: // bus error, illegal stop/start
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_ERR;
            s_I2CCtrl.tx_cback(&s_I2CXfer_cur);
            break;

        // master transmit
        case TW_MT_SLA_ACK:  // slave receiver acked address
        case TW_MT_DATA_ACK: // slave receiver acked data
            if (s_I2CCtrl.tx_buff_idx < I2C_BUFFER_SIZE) {
                TWDR = s_I2CCtrl.tx_buff[s_I2CCtrl.tx_buff_idx++];
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
                s_I2CXfer_cur.nb = s_I2CCtrl.tx_buff_idx;
                s_I2CCtrl.tx_cback(&s_I2CXfer_cur);
            }
            break;
        case TW_MT_SLA_NACK:  // address sent, nack received
        case TW_MT_DATA_NACK: // data sent, nack received
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_MA_TX;
            s_I2CXfer_cur.nb = s_I2CCtrl.tx_buff_idx;
            s_I2CCtrl.tx_cback(&s_I2CXfer_cur);
            break;
        case TW_MT_ARB_LOST: // lost bus arbitration
            i2c_setNACK();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_ERR;
            s_I2CCtrl.tx_cback(&s_I2CXfer_cur);
            break;

        // master receive
        case TW_MR_DATA_ACK: // data received, ack sent
            s_I2CCtrl.rx_buff[s_I2CCtrl.rx_buff_idx++] = TWDR;
        case TW_MR_SLA_ACK:  // address sent, ack received
            if (s_I2CCtrl.rx_buff_idx < I2C_BUFFER_SIZE) {
                i2c_setNACK();
            }
            //else if(i2c_rx_buff_idx == i2c_rxBufferSize) { //no data in buffer
            //  i2c_setSTO();
            //  s_I2CCtrl->state = I2C_ST_MA_IDLE;
            //  s_I2CCtrl.rx_cback(i2c_rx_buff_idx, i2c_rx_buff);
            //}
            else {
                i2c_setACK();
            }
            break;
        case TW_MR_DATA_NACK: // data received, nack sent
            s_I2CCtrl.rx_buff[s_I2CCtrl.rx_buff_idx++] = TWDR;
        case TW_MR_SLA_NACK: // address sent, nack received
            i2c_setSTO();
            s_I2CCtrl.state = I2C_ST_MA_IDLE;
            s_I2CXfer_cur.type = I2C_RD;
            s_I2CXfer_cur.status = I2C_XFER_MA_RX;
            s_I2CXfer_cur.nb = s_I2CCtrl.rx_buff_idx;
            s_I2CCtrl.rx_cback(&s_I2CXfer_cur);
            break;
        // TW_MR_ARB_LOST = TW_MT_ARB_LOST (http://www.nongnu.org/avr-libc/user-manual/group__util__twi.html)

        // slave receive
        case TW_SR_SLA_ACK:   // addressed, ACK returned
        case TW_SR_GCALL_ACK: // addressed generally, ACK returned
        case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, ACK returned
        case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, ACK returned
            s_I2CCtrl.state = I2C_ST_SL_RX;
            //i2c_rx_buff[0] = TWDR;
            s_I2CCtrl.rx_buff_idx = 0;
            i2c_setACK();
            break;
        case TW_SR_DATA_ACK:       // data received, ACK returned
        case TW_SR_GCALL_DATA_ACK: // data received generally, ACK returned
            if (s_I2CCtrl.rx_buff_idx < I2C_BUFFER_SIZE) {
                s_I2CCtrl.rx_buff[s_I2CCtrl.rx_buff_idx++] = TWDR;
                i2c_setACK();
            } else {
                i2c_setNACK();
            }
            break;
        case TW_SR_DATA_NACK:       // data received, NACK returned
        case TW_SR_GCALL_DATA_NACK: // data received generally, NACK returned
            if (s_I2CCtrl.rx_buff_idx < I2C_BUFFER_SIZE) {
                s_I2CCtrl.rx_buff[s_I2CCtrl.rx_buff_idx++] = TWDR;
            }
            i2c_setNACK();
            break;
        case TW_SR_STOP: // stop or repeated start condition received
            s_I2CCtrl.state = I2C_ST_SL_IDLE;
            s_I2CXfer_cur.type = I2C_RD;
            s_I2CXfer_cur.status = I2C_XFER_SL_RX;
            s_I2CXfer_cur.nb = s_I2CCtrl.rx_buff_idx;
            s_I2CCtrl.rx_cback(&s_I2CXfer_cur);
            i2c_setACK();
            break;

        // slave transmit
        case TW_ST_SLA_ACK:          // addressed, ACK returned
        case TW_ST_ARB_LOST_SLA_ACK: // lost arbitration, ACK returned
            s_I2CCtrl.state = I2C_ST_SL_TX;
            s_I2CCtrl.tx_buff_idx = 0;
            TWDR = s_I2CCtrl.tx_buff[s_I2CCtrl.tx_buff_idx++];
            i2c_setACK();
            break;
        case TW_ST_DATA_ACK: // data transmitted, ACK received
            if (s_I2CCtrl.tx_buff_idx < I2C_BUFFER_SIZE) {
                TWDR = s_I2CCtrl.tx_buff[s_I2CCtrl.tx_buff_idx++];
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
            s_I2CXfer_cur.nb = s_I2CCtrl.rx_buff_idx;
            s_I2CCtrl.tx_cback(&s_I2CXfer_cur);
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
            if (lst_I2CCtrl[1].tx_buff_idx < I2C_BUFFER_SIZE) {
                I2C2_ReadPointerSet(&lst_I2CCtrl[1].tx_buff[lst_I2CCtrl[1].tx_buff_idx++]);
                s_I2CXfer_cur.nb = lst_I2CCtrl[1].tx_buff_idx;
                lst_I2CCtrl[1].tx_cback(&s_I2CXfer_cur);
            } else {
                i2c2_data = 0;
                I2C2_ReadPointerSet(&i2c2_data);
            }
            break;
        case I2C2_SLAVE_RECEIVE_REQUEST_DETECTED:
            //printf("r\n");
            lst_I2CCtrl[1].tx_buff_idx = 0;
            lst_I2CCtrl[1].rx_buff_idx = 0;
            I2C2_WritePointerSet(&i2c2_data);
            break;
        case I2C2_SLAVE_RECEIVED_DATA_DETECTED:
            //printf("d\n");
            s_I2CXfer_cur.type = I2C_WR;
            s_I2CXfer_cur.status = I2C_XFER_SL_RX;
            if (lst_I2CCtrl[1].rx_buff_idx < I2C_BUFFER_SIZE) {
                lst_I2CCtrl[1].rx_buff[lst_I2CCtrl[1].rx_buff_idx++] = i2c2_data;
                s_I2CXfer_cur.nb = lst_I2CCtrl[1].rx_buff_idx;
                lst_I2CCtrl[1].rx_cback(&s_I2CXfer_cur);
            }
            break;
        case I2C2_SLAVE_10BIT_RECEIVE_REQUEST_DETECTED:
            break;
        default:
            break;
    }
    return true;
}
#endif
