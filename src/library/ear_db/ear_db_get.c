/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  Authors: Julita Corbalan (julita.corbalan@bsc.es) 
    and Luigi Brochard (lbrochard@lenovo.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "ear_db_type.h"




char * DPD_Get_Name(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
        fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_Name\n");
#endif
        return MY_APP->app_id;

}
unsigned long DPD_Get_frequency(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
        fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_frequency\n");
#endif
        return MY_APP->f;
}

unsigned int  DPD_Get_procs(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
        fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_procs\n");
#endif
        return MY_APP->procs;
}


double DPD_Get_GBS(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
	fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_GBS\n");
#endif
	return MY_APP->GBS_f0;
}
double DPD_Get_GIPS(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
	fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_GIPS\n");
#endif
	return MY_APP->GIPS_f0;
}
double DPD_Get_POWER(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
	fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_POWER\n");
#endif
	return MY_APP->POWER_f0;
}
double DPD_Get_TPI(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
	fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_TPI\n");
#endif
	return MY_APP->TPI_f0;
}

double DPD_Get_seconds(struct App_info *MY_APP)
{
#ifdef EAR_DB_DEBUG
	fprintf(stderr,"EAR(DB_DEBUG): DPD_Get_seconds\n");
#endif
	return MY_APP->seconds;
}
