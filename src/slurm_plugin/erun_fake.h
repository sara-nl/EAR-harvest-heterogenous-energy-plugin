/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

spank_err_t spank_getenv (spank_t spank, const char *var, char *buf, int len)

spank_err_t spank_setenv (spank_t spank, const char *var, const char *val, int overwrite)

spank_err_t spank_unsetenv (spank_t spank, const char *var)

spank_context_t spank_context (void)

spank_err_t spank_get_item (spank_t spank, spank_item_t item, int *p)

char *slurm_hostlist_shift (hostlist_t host_list)

hostlist_t slurm_hostlist_create (char *node_list)

spank_err_t spank_option_register_print(spank_t sp, struct spank_option *opt)

spank_err_t spank_option_register_call(int argc, char *argv[], spank_t sp, struct spank_option *opt)

spank_err_t spank_option_register(spank_t sp, struct spank_option *opt)
