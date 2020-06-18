/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

/**
*    \file dynamic_configurarion.h
*    \brief exports the API to control the dynamic reconfiguration of EARlib arguments (at this moment frequency and min_tim eth)
*
*/

#ifndef _DYN_CONF_H
#define _DYN_CONF_H

/** Creates a socket to accept remote commands for dynamic EAR configuration. It is dessigned to be executed in the context of a new thread
*/
void *eard_dynamic_configuration(void *no_args);

/** Returns the frequency defined dynamically
*/
ulong max_dyn_freq();

#endif
