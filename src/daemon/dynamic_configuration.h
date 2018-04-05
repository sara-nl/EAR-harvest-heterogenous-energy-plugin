/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
#else
#endif
