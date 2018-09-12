/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*   It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*   Copyright (C) 2017
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

#include <common/database/auxiliars.h>

/** Takes the normal signature and converts its double values to float. */
fsignature_t convert_signature_float(signature_t sig)
{
    fsignature_t fsig;
    fsig.DC_power   = (float) sig.DC_power;
    fsig.DRAM_power = (float) sig.DRAM_power;
    fsig.PCK_power  = (float) sig.PCK_power;
    fsig.EDP        = (float) sig.EDP;
    fsig.GBS        = (float) sig.GBS;
    fsig.TPI        = (float) sig.TPI;
    fsig.CPI        = (float) sig.CPI;
    fsig.Gflops     = (float) sig.CPI;
    fsig.time       = (float) sig.time;

    fsig.avg_f = sig.avg_f;
    fsig.def_f = sig.def_f;

    return fsig;
}

/** Takes the float signature and converts its float values to double. */
signature_t convert_signature_double(fsignature_t fsig)
{
    signature_t sig;
    sig.DC_power   = (float) fsig.DC_power;
    sig.DRAM_power = (float) fsig.DRAM_power;
    sig.PCK_power  = (float) fsig.PCK_power;
    sig.EDP        = (float) fsig.EDP;
    sig.GBS        = (float) fsig.GBS;
    sig.TPI        = (float) fsig.TPI;
    sig.CPI        = (float) fsig.CPI;
    sig.Gflops     = (float) fsig.CPI;
    sig.time       = (float) fsig.time;

    sig.avg_f = fsig.avg_f;
    sig.def_f = fsig.def_f;

    return sig;
}

