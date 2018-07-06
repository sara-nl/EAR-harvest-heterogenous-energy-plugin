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

#ifndef EAR_STATES_H
#define EAR_STATES_H

#define EAR_SUCCESS              0
#define EAR_ERROR               -1
#define EAR_WARNING             -2
#define EAR_ALLOC_ERROR         -3
#define EAR_READ_ERROR          -4
#define EAR_FILE_NOT_FOUND      -5
#define EAR_NOT_READY		    -6
#define EAR_MYSQL_ERROR         -7
#define EAR_MYSQL_STMT_ERROR    -8

#define EAR_ADDR_NOT_FOUND		-19
#define EAR_SOCK_CREAT_ERROR	-20
#define EAR_SOCK_BIND_ERROR		-21
#define EAR_SOCK_LISTEN_ERROR	-22
#define EAR_SOCK_RECV_ERROR		-23
#define EAR_SOCK_ACCEPT_ERROR	-24

#define DYNAIS_ENABLED      1
#define DYNAIS_DISABLED     0
#define PERIODIC_MODE_ON    1
#define PERIODIC_MODE_OFF   0

typedef int state_t;

/*typedef struct state {
	char *error;
	void *data;
	int no;
} state_t;*/

#endif //EAR_STATES_H
