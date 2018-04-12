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

char *conf_ear_tmp=NULL;
char *conf_ear_db_pathname=NULL;
int conf_ear_verbose=DEFAULT_VERBOSE;



char * getenv_ear_tmp()
{
	char *my_ear_tmp;
	my_ear_tmp=getenv("EAR_TMP");
	if (my_ear_tmp==NULL){
		my_ear_tmp=getenv("TMP");
		//??
		if (my_ear_tmp==NULL) my_ear_tmp=getenv("HOME");
	}
	conf_ear_tmp=malloc(strlen(my_ear_tmp)+1);
	strcpy(conf_ear_tmp,my_ear_tmp);
	return conf_ear_tmp;	
}
char *getenv_ear_db_pathname()
{
	char *my_ear_db_pathname = getenv("EAR_DB_PATHNAME");

	if (my_ear_db_pathname != NULL && strcmp(my_ear_db_pathname,"") != 0)
	{
		conf_ear_db_pathname=malloc(strlen(my_ear_db_pathname)+1);
		strcpy(conf_ear_db_pathname,my_ear_db_pathname);
	}

	return conf_ear_db_pathname;
}


int getenv_ear_verbose()
{
	char *my_verbose;
	my_verbose=getenv("EAR_VERBOSE");
	if (my_verbose!=NULL){
		conf_ear_verbose=atoi(my_verbose);
		if ((conf_ear_verbose<0) || (conf_ear_verbose>4)) conf_ear_verbose=DEFAULT_VERBOSE;
	}	
	return conf_ear_verbose;
}


// get_ functions must be used after getenv_
char * get_ear_tmp()
{
	return conf_ear_tmp;
}
void set_ear_tmp(char *new_tmp)
{
	if (conf_ear_tmp!=NULL) free(conf_ear_tmp);
	conf_ear_tmp=malloc(strlen(new_tmp)+1);
	strcpy(conf_ear_tmp,new_tmp);
}

char *get_ear_db_pathname()
{
	return conf_ear_db_pathname;
}

int get_ear_verbose()
{
	return conf_ear_verbose;
}
void set_ear_verbose(int verb)
{
	conf_ear_verbose=verb;
}

