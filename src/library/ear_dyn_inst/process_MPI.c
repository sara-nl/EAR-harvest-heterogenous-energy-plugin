/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <ear_dyn_inst/ear_api.h>


void before_init(){
}

void after_init(){
	ear_init();
}
void before_mpi(mpi_call call_type, p2i buf, p2i dest){
	ear_mpi_call(call_type,buf,dest);
}

void after_mpi(mpi_call call_type){

}

void before_finalize(){
	ear_finalize();
}

void after_finalize(){
}


