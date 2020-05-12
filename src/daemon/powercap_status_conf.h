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

#ifndef _PC_STATUS_H_
#define _PC_STATUS_H_

#define DOMAIN_NODE 0
#define DOMAIN_CPU 1

#define PC_STATUS_OK      0
#define PC_STATUS_GREEDY  1
#define PC_STATUS_RELEASE 2
#define PC_STATUS_ASK_DEF 3
#define PC_STATUS_IDLE    4
#define PC_STATUS_STOP		5
#define PC_STATUS_START 	6
#define PC_STATUS_RUN			7

#define PC_DVFS						50
#define PC_POWER					51


#define PC_MODE_LIMIT			200
#define PC_MODE_TARGET		201				

#define PC_STATUS_ERROR   100


#endif
