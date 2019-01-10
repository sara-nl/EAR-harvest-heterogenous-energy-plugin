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

#ifndef _EAR_TYPES_GENERIC
#define _EAR_TYPES_GENERIC

#include <stdint.h>

typedef unsigned char		uchar;
typedef unsigned long long	ull;
typedef unsigned long long	ullong;
typedef unsigned long 		ulong;
typedef unsigned int		uint;
typedef uint8_t				job_type;
typedef ulong				job_id;

// Obsolete
#define GENERIC_NAME 		256
#define	UID_NAME			8
#define POLICY_NAME 		32
#define ENERGY_TAG_SIZE		32
#define MAX_PATH_SIZE		256
#define NODE_SIZE			256
#define NAME_SIZE			128
#define ID_SIZE				64

#endif
