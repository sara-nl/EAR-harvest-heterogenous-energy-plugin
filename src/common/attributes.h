/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef COMMON_SYSTEM_ATTRIBUTES_H
#define COMMON_SYSTEM_ATTRIBUTES_H

#define attr2(a1, a2)	__attribute__ ((a1, a2))
#define attr(a)			__attribute__ ((a))
#define attr_hidden		visibility("hidden")
#define attr_weak		weak

//#define ATTR_PROTECTED	__attribute__ ((visibility ("protected")))
//#define ATTR_INTERNAL 	__attribute__ ((visibility ("internal")))
//#define ATTR_HIDDEN		__attribute__ ((visibility ("hidden")))
//#define ATTR_WEAK		__attribute__((weak))

#endif //COMMON_SYSTEM_ATTRIBUTES_H
