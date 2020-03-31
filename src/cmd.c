// SPDX-License-Identifier: GPL-3.0-or-later

#include "cmd.h"

#include <stdio.h>

#include "constants.h"

void ver(ParsedCmd *cmdp) {
    printf("  ver: %d\n", APP_VERSION);
    printf("build: %s\n", APP_BUILD);
}

void info(ParsedCmd *cmdp) {
    printf("info ...\n");
}
