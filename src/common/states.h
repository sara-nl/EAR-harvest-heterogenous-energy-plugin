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

#ifndef STATES_H
#define STATES_H

/* error definitions */
#define EAR_SUCCESS              0
#define EAR_ERROR               -1
#define EAR_WARNING             -2
#define EAR_ALLOC_ERROR         -3
#define EAR_READ_ERROR          -4
#define EAR_OPEN_ERROR			-5
#define EAR_WRITE_ERROR			-6
#define EAR_FILE_NOT_FOUND      -7
#define EAR_NOT_INITIALIZED		-8
#define EAR_NOT_READY		    -9
#define EAR_BUSY				-10
#define EAR_ALREADY_CLOSED		-11
#define EAR_ARCH_NOT_SUPPORTED	-12
#define EAR_BAD_ARGUMENT		-13
#define EAR_MYSQL_ERROR         -14
#define EAR_MYSQL_STMT_ERROR    -15
#define EAR_ADDR_NOT_FOUND		-19
#define EAR_SOCK_OP_ERROR		-20
#define EAR_SOCK_BAD_PROTOCOL	-21
#define EAR_SOCK_DISCONNECTED	-22
#define EAR_DBD_ERROR_MAIN		-23
#define EAR_DBD_ERROR_MIRR		-24
#define EAR_DBD_ERROR_BOTH		-25

// TODO: this is a config not a state
#define DYNAIS_ENABLED      1
#define DYNAIS_DISABLED     0
#define PERIODIC_MODE_ON    1
#define PERIODIC_MODE_OFF   0

/* type & functions */
typedef int state_t;
char *state_error_str;

/* global data */
char *intern_error_str;
int intern_error_num;

#define state_ok(state) \
	state == EAR_SUCCESS

#define state_fail(state) \
	state != EAR_SUCCESS

#define state_is(state1, state2) \
	state1 == state2

#define state_return(state) \
	return state;

#define state_return_msg(state, error_num, error_str) \
	intern_error_num = error_num; \
	intern_error_str = error_str; \
	return state;
#else
#endif //STATES_H

