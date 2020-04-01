// SPDX-License-Identifier: GPL-3.0-or-later

#include "cmd.h"

#include <stdio.h>

#include "constants.h"
#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

void ver(ParsedCmd *cmdp) {
    printf("   OS: %s\n", tskKERNEL_VERSION_NUMBER);
    printf("   FW: %d\n", APP_VERSION);
    //printf("build: %s\n", APP_BUILD);
}

void info(ParsedCmd *cmdp) {
    printf("info ...\n");
}
