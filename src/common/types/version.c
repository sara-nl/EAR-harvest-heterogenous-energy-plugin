/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <common/config.h>
#include <stdlib.h>
#include <stdio.h>

void version_to_str(char *buff)
{
	sprintf(buff,"%s", RELEASE);
}

void print_version()
{
    char msg[256];
    sprintf(msg, "EAR version %s\n", RELEASE);
    printf(msg);
}

