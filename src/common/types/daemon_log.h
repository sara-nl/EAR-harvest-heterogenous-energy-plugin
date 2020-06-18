/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _EAR_DAEMON_LOG_H
#define _EAR_DAEMON_LOG_H

#include <time.h>
#include <common/config.h>
#include <common/types/generic.h>



/** Creates a log file targeted to be used as verbose and/or error channel, returns the fd */
int create_log(char *path,char *service_name);

#endif
