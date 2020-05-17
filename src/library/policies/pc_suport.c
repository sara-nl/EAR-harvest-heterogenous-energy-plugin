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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/config.h>
#include <common/includes.h>
#include <daemon/powercap_status_conf.h>
#include <common/types/signature.h>
#include <common/hardware/frequency.h>
#include <common/types/projection.h>
#if POWERCAP

ulong pc_support_adapt_freq(node_powercap_opt_t *pc,ulong f,signature_t *s)
{
		ulong req_f,cfreq;
    uint plimit,cpstate,ppstate;
		double ppower;
    req_f=f;
    plimit=c->app->pc_opt.last_t1_allocated; 				/* limit */
		cfreq=frequency_closest_high_freq(s->avg_f,1); 	/* current freq */
		cpstate=frequency_closest_pstate(cfreq);			 	/* current pstate */
		ppstate=frequency_closest_pstate(req_f);				/* pstate for freq selected */
		project_power(s,cpstate,ppstate,&ppower);				/* Power at freq selected */
		debug("checking frequency: cfreq %lu cpstate %u power %lf limit %u",cfreq,cpstate,ppower,plimit);		
		if ((uint)ppower<=plimit){
			return req_f;
		}else{
			numpstates=frequency_get_num_pstates();
			do{
				ppstate++;
				project_power(s,cpstate,ppstate,&ppower);
			}while(((uint)ppower>plimit) && (ppstate<numpstates));
		}
		req_f=frequency_pstate_to_freq(ppstate);
		return req_f;

}
#endif
