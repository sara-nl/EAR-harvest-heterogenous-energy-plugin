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

#ifndef _DYN_POLICY_OPERATIONS_H
#define _DYN_POLICY_OPERATIONS_H
#include <common/config.h>
#include <common/types/projection.h>			/* defines projections */
#include <common/types/application.h>			/* defines application */
#include <common/types/signature.h>				/* defines signature */
#include <daemon/shared_configuration.h> 	/* defines settings_conf */

typedef struct policy_dyn
{
  int (*init)(application_t *app,settings_conf_t *conf,uint pstates);
  int (*end)();
  int (*new_loop)(uint loop_id);
  int (*end_loop)(uint loop_id);
  int (*apply)(settings_conf_t *conf,uint loop_id,signature_t *sig);
  int (*is_ok)(uint loop_id, signature_t *curr_sig, signature_t *last_sig);
  int (*default_conf)(ulong *f);
  int (*new_mpi_call)(mpi_call call_type, p2i buf, p2i dest);
  int (*end_mpi_call)();
  int (*reconfigure)(settings_conf_t *conf);
}policy_dyn_t;




#else
#endif
