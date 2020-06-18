/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <common/config.h>

#include <common/types/gm_warning.h>
#include <string.h>
#include <common/types/generic.h>
#define NODE_SIZE 256


/** Replicates the periodic_metric in *source to *destiny */
void copy_gm_warning(gm_warning_t *destiny, gm_warning_t *source)
{
    memcpy(destiny, source, sizeof(gm_warning_t));
}

/** Initializes all values of the gm_warning to */
void init_gm_warning(gm_warning_t *gmw)
{
    memset(gmw, 0, sizeof(gm_warning_t));
}


