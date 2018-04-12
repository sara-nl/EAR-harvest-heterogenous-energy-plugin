/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <common/environment.h>

extern char *conf_ear_tmp=NULL;
extern char *conf_ear_db_pathname=NULL;
extern int conf_ear_verbose=DEFAULT_VERBOSE;



char * getenv_ear_tmp();
char *getenv_ear_db_pathname();
int getenv_ear_verbose();

// get_ functions must be used after getenv_
char * get_ear_tmp();
void set_ear_tmp(char *new_tmp);
char *get_ear_db_pathname();
int get_ear_verbose();
void set_ear_verbose(int verb);

