/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef CMDS_98RXG9U8BUUHY401
#define CMDS_98RXG9U8BUUHY401

typedef enum {
    MC_CMD_FAIL,
    MC_CMD_OK,
    MC_CMD_PASS,
} AG_MC_CMD_STATUS_t;

#define AG_PROTO_VER       1

#define AG_CMD_SUMMARY     0x01
#define AG_CMD_SUMMARY_NB  4
#define AG_TMC_FLAG        0x80
#define AG_EEPROM_FLAG     0x40
#define AG_OPER_FLAG       0x01
#define AG_RESET_FLAG      0x01

#define AG_CMD_TYPE        0x02
#define AG_CMD_TYPE_NB     2

#define AG_CMD_CAP         0x03
#define AG_CMD_CAP_NB      2
#define AG_CAP_PCIE_FLAG   0x10
#define AG_CAP_USB_FLAG    0x08
#define AG_CAP_JTAG_FLAG   0x04
#define AG_CAP_1PPS_FLAG   0x02
#define AG_CAP_CLK100_FLAG 0x01

#define AG_CMD_MFR         0xFE
#define AG_CMD_MFR_NB      4

#endif /* CMDS_98RXG9U8BUUHY401 */
