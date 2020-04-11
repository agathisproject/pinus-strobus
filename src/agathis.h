// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AGATHIS_6PLS6RVRFVYEP7NX
#define AGATHIS_6PLS6RVRFVYEP7NX
/** @file */

#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#define I2C_OFFSET 0x20

/**
 * @brief state of the local MC (Management Controller)
 */
typedef struct {
    uint8_t TMC;      /**< Tree Management Controller enabled */
    uint8_t addr_d;   /**< down-trunk management address */
    uint8_t addr_u;   /**< up-trunk management address */
    uint8_t addr_i2c; /**< MC I2C address */
} MCState_t;

extern MCState_t MC;

typedef enum {
    MC_NOT_PRESENT,
    MC_INVALID,
    MC_PRESENT,
} RmtMCState_t;
/**
 * @brief info about the other MCs (Management Controllers)
 */
typedef struct {
    RmtMCState_t state;
} MCInfo_t;

#define MC_MAX_CNT 15 /** max number of MCs in the tree */
extern MCInfo_t RmtMC[MC_MAX_CNT];
extern SemaphoreHandle_t xSemaphore_MMC;

void MC_Initialize();

void MC_Show();

#endif /* AGATHIS_6PLS6RVRFVYEP7NX */
