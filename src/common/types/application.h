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



#ifndef _EAR_TYPES_APPLICATION
#define _EAR_TYPES_APPLICATION
#include <stdint.h>
#include <stdio.h>
#include <common/config.h>
#include <common/types/job.h>
#include <common/types/signature.h>
#include <common/types/power_signature.h>

#define GENERIC_NAME 256

typedef struct application
{
	job_t job;	
	uint8_t is_mpi;
	uint8_t is_learning;
	char node_id[GENERIC_NAME];		
	power_signature_t power_sig; // power_sig are power metrics related to the whole job, not only the mpi part
	signature_t	signature; 		 // signature refers to the mpi part, it includes power metrics and performance metrics
} application_t;

// Function declarations

/** Resets the data. */
void init_application(application_t *app);

/** Prints the contents of app to the STDOUT*/
int print_application(application_t *app);

/** Prints a summary of the application to STDOUT */
void report_application_data(application_t *app);
/** Prints a summary of the application (only mpi part,power signature is not reported) to STDOUT */
void report_mpi_application_data(application_t *app);

/** Reads a file of applications saved in binary format. A memory block is
    allocated for the read applications, and is returned by the argument 'apps'.
    The returned integer is the number of applications read. If the integer is
    negative, one of the following errores ocurred: EAR_ALLOC_ERROR,
    EAR_READ_ERROR or EAR_FILE_NOT_FOUND. */
int read_application_binary_file(char *path, application_t **apps);

/** Reads a file of applications saved in CSV format. A block of memory is
    allocated for this read applications, and is returned by the argument 'apps'.
    The returned integer is the number of applications read. If the integer is
    negative, one of the following errors ocurred: EAR_ALLOC_ERROR,
    EAR_READ_ERROR or EAR_FILE_NOT_FOUND. */
int read_application_text_file(char *path, application_t **apps, char is_extended);

/** Appends in a file an application in binary format. The returned integer is
    one of the following states: EAR_SUCCESS or EAR_ERROR. */
int append_application_binary_file(char *path, application_t *app);

/** Appends in a file an application in CSV format. The returned integer is one
*   of the following states: EAR_SUCCESS or EAR_ERROR. */
int append_application_text_file(char *path, application_t *app, char is_extended);

/** Replicates the application in *source to *destiny */
void copy_application(application_t *destiny, application_t *source);

/** PENDING */
int scan_application_fd(FILE *fd, application_t *app, char is_extended);


#define create_ID(id,sid)	(id*100+sid)

#endif
