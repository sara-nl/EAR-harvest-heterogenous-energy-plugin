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



#ifndef _EAR_TRACES_H_
#define _EAR_TRACES_H_

/*
        timestamp:1:            Period id
        timestamp:2:            Period length
        timestamp:3:            Period iterations
        timestamp:4:            Period time
        timestamp:5:            Period CPI
        timestamp:6:            Period TPI
        timestamp:7:            Period GBs
        timestamp:8:            Period power
        timestamp:9:            Period time projection
        timestamp:10:           Period CPI projection
        timestamp:11:           Period power projection
        timestamp:12:           Frequency
*/

#include <common/config.h>
#include <common/types/generic.h>

#define MIN_FREQ_FOR_SAMPLING 500000
#define PERIOD_ID			1
#define PERIOD_LENGTH		2
#define PERIOD_ITERATIONS	3
#define PERIOD_TIME			4
#define PERIOD_CPI			5
#define PERIOD_TPI			6
#define PERIOD_GBS			7
#define PERIOD_POWER		8
#define PERIOD_TIME_PROJ	9
#define PERIOD_CPI_PROJ		10
#define PERIOD_POWER_PROJ	11
#define PERIOD_FREQ			12
#define APP_ENERGY			13

#ifdef EAR_GUI
	/** Executed at application start */
 	void traces_init(int global_rank, int local_rank, int nodes, int mpis, int ppn);
	/** Executed at application end */
	void traces_end(int global_rank,int local_rank, unsigned long int total_ener);

	/**@{ Executed when application signature is computed at EVALUATING_SIGNATURE and SIGANTURE_STABLE states */
	void traces_frequency(int global_rank, int local_rank, unsigned long f);
	void traces_new_signature(int global_rank, int local_rank, double seconds, double cpi, double tpi, double gbs, double power);
	void traces_PP(int global_rank, int local_rank, double seconds, double cpi, double power); /**@}*/ 

	/**@{ Executed when each time a new loop is detected, the loop ends, or a new iteration are reported */
	void traces_new_n_iter(int global_rank, int local_rank, int period_id, int loop_size, int iterations);
	void traces_new_period(int global_rank, int local_rank, int period_id);
	void traces_end_period(int global_rank, int local_rank); /**@}*/ 

	/** Executed at each mpi_call */
	void traces_mpi_call(int global_rank, int local_rank, ulong time, ulong ev, ulong a1, ulong a2, ulong a3);
#else
	#define traces_init(g,l,n,m,p)
	#define traces_new_n_iter(g,l,p,lo,i)
	#define traces_end(g,l,e)
	#define traces_new_signature(g,l,s,c,t,gb,p)
	#define traces_frequency(g,l,f)
	#define traces_PP(g,l,s,c,p)
	#define traces_new_period(g,l,p)
	#define traces_end_period(g,l)
	#define traces_mpi_call(g,l,t,e,a1,a2,a3);
#endif

#endif
