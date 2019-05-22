/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

#ifndef _DYN_POLICY_H
#define _DYN_POLICY_H
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/config.h>
#include <common/types/projection.h>
#include <common/types/application.h>


/*
* Uses: 
* coefficients
* projections
* policy_configuration (th)
* application
*/

/** Application init
*  Input: num of pstates 
* current application and current configuration
*/
int  policy_init(application_t *app,settings_conf_t *conf,uint pstates);

/** Called at application end
*/
int  policy_end();

/** 
* Called when new_loop is detected 
*/
int  policy_new_loop(uint loop_id);

/**
* Called when loop ends
*/
int policy_end_loop(uint loop_id);


/** It is called when signature is ready 
* It uses loop signature , coefficients+projections
* returns new frequency and sets ready to 1 when signature is ok or 0 when policy must be called again
* Should also receive: policy_global configuration, list of previously computed signatures for same loop and different frequencies
*/
int policy(settings_conf_t *conf,uint loop_id,signature_t *sig);

/** It is called after applying policy decisions. 
* input: current signature and previous signature
* returns 1 when policy is ok or 0 otherwise
* */
int policy_ok(uint loop_id, signature_t *curr_sig, signature_t *last_sig);

/** It is called after a maximum number of policy tries for a given loop
* input current frequency
* should also receive system/policy configuration
* return new frequency
*/
int  policy_get_default_conf(ulong *f);


/******* Extensions proposed */

int policy_new_mpi_call(mpi_call call_type, p2i buf, p2i dest);
int polic_end_mpi_call(mpi_call call_type);
int policy_reconfigure(settings_conf_t *conf);

#else
#endif
