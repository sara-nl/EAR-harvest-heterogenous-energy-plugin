/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_CHECK_H
#define EAR_CHECK_H

// Searchs the module name in /proc/modules file.
// Returns 1 if found or 0 if doesn't.
int find_module(char *module);

#endif //EAR_CHECK_H
