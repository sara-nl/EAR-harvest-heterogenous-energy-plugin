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



/**
*    \file ear_power_monitor.h
*    \brief This file include functions to simplify the work of power monitoring It can be used by privileged applications (such as eard)
*	 \brief	or not-privileged applications such as the EARLib (or external commands)
*
*/
#ifndef _POWER_MON_H_
#define _POWER_MON_H_

#include <common/states.h>
#include <metrics/ipmi/energy_node.h>
#include <metrics/papi/energy_cpu.h>

#define NUM_SOCKETS 2
#define POWER_MON_OK	EAR_SUCCESS
#define POWER_MON_ERROR	EAR_ERROR

typedef long long rapl_data_t;
typedef unsigned long node_data_t;

typedef struct energy_mon_data{
	time_t 		sample_time;
	node_data_t AC_node_energy;
	node_data_t DC_node_energy;
	rapl_data_t DRAM_energy[NUM_SOCKETS];
	rapl_data_t CPU_energy[NUM_SOCKETS];
}energy_data_t;

typedef struct power_data{
    time_t begin,end;
    double avg_dc,avg_ac;
    double avg_dram[NUM_SOCKETS];
    double avg_cpu[NUM_SOCKETS];
}power_data_t;


/**  Starts power monitoring */
int init_power_ponitoring();
/** Ends power monitoring */
void end_power_monitoring();

/** Energy is returned in mili Joules */
int read_enegy_data(energy_data_t *acc_energy);
/** Computes the difference between two energy measurements */
int diff_energy_data(energy_data_t *end,energy_data_t *init,energy_data_t *diff);


/** Copies the energy measurement in *src to *dest */
void copy_energy_data(energy_data_t *dest,energy_data_t *src);
/** Prints the data from an energy measurement to stdout */
void print_energy_data(energy_data_t *e);

/** Computes the power between two energy measurements */
void compute_power(energy_data_t *e_begin,energy_data_t *e_end,power_data_t *my_power);
/** Prints power information to the stdout */
void print_power(power_data_t *my_power);
/** Write (text mode) the power information in the provided file descriptor */
void report_periodic_power(int fd,power_data_t *my_power);


/** Computes the difference betwen two node energy measurements */
node_data_t diff_node_energy(node_data_t end,node_data_t init);
/** Computes the difference betwen two RAPL energy measurements */
rapl_data_t diff_RAPL_energy(rapl_data_t end,rapl_data_t init);



#else
#endif
