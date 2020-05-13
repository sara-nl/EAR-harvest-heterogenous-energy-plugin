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

#ifndef METRICS_FREQUENCY_IMC_INTEL63_H
#define METRICS_FREQUENCY_IMC_INTEL63_H

#include <metrics/frequency/imc.h>

state_t ifreq_intel63_init(topology_t *tp);

state_t ifreq_intel63_dispose();

state_t ifreq_intel63_read(freq_imc_t *ef);

state_t ifreq_intel63_read_diff(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);

state_t ifreq_intel63_read_copy(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);

state_t ifreq_intel63_data_alloc(freq_imc_t *ef, ulong *freqs[], ulong *freqs_count);

state_t ifreq_intel63_data_count(uint *count);

state_t ifreq_intel63_data_copy(freq_imc_t *ef_dst, freq_imc_t *ef_src);

state_t ifreq_intel63_data_free(freq_imc_t *ef, ulong *freqs[]);

state_t ifreq_intel63_data_diff(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);

state_t ifreq_intel63_data_print(ulong *freqs, ulong *average);

#endif //EAR_FREQUENCY_UNCORE_H
