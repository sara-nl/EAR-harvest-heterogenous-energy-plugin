/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_EARDBD_SIGNALS_H
#define EAR_EARDBD_SIGNALS_H

#include <signal.h>

#define _error(...) \
	verbose_line(); \
	verbose(0, "ERROR, " __VA_ARGS__); \
	error_handler();

void signal_handler(int signal, siginfo_t *info, void *context);

void error_handler();

#endif //EAR_EARDBD_SIGNALS_H
