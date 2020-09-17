/*
 * *
 * * This program is part of the EAR software.
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management. It has been developed in the context of the
 * * Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
 * *
 * * Copyright © 2017-present BSC-Lenovo
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * This file is licensed under both the BSD-3 license for individual/non-commercial
 * * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * * found in COPYING.BSD and COPYING.EPL files.
 * */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <metrics/io/io.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <common/string_enhanced.h>
#include <common/plugins.h>
// #define SHOW_DEBUGS 1
#include <common/output/verbose.h>


state_t io_init(ctx_t *c,pid_t pid)
{
	FILE *f;
	char io_path[MAX_PATH_SIZE];
	sprintf(io_path,IO_PATH,(int)pid);
	f=fopen(io_path,"r");
	if (f==NULL){
		state_return_msg(EAR_OPEN_ERROR,errno,"Opening io file");
	}
	c->context=(void *)f;
	state_return(EAR_SUCCESS);
}
static void assign_token_value(io_data_t *iodata,char *token,char *value)
{

	if (!strcmp(token,"rchar")){
		iodata->rchar=atoi(value);
	}
	if (!strcmp(token,"wchar")){
		iodata->wchar=atoi(value);
	}
	if (!strcmp(token,"syscr")){
		iodata->syscr=atoi(value);
	}
	if (!strcmp(token,"syscw")){
		iodata->syscw=atoi(value);
	}
	if (!strcmp(token,"read_bytes")){
		iodata->read_bytes=atoi(value);
	}
	if (!strcmp(token,"write_bytes")){
		iodata->write_bytes=atoi(value);
	}
	if (!strcmp(token,"cancelled_write_bytes")){
		iodata->cancelled=atoi(value);
	}
}
state_t io_read(ctx_t *c, io_data_t *iodata)
{
	char line[256];	
	char *token,*value;
	fseek((FILE *)c->context,0L, SEEK_SET);
	while (fgets(line, 256, (FILE *)c->context) != NULL)
  {
    token = strtok(line, ":");
		if (token != NULL){
			remove_chars(token,' ');
			debug("token found %s",token);
			value=strtok(NULL,"");
			if (value != NULL){
				remove_chars(value,' ');
				debug("value %s",value);
				assign_token_value(iodata,token,value);
			}
		}
	}
	state_return(EAR_SUCCESS);	
}

state_t io_alloc(io_data_t **iodata)
{
	io_data_t *t;
	t=calloc(1,sizeof(io_data_t));
	*iodata=t;
}

state_t io_free(io_data_t *iodata)
{
	free(iodata);
}

state_t io_print(io_data_t *iodata)
{ 
	char msg[512];
	io_tostr(iodata,msg,sizeof(msg));
	printf(msg);
}

state_t io_tostr(io_data_t *iodata,char *msg,size_t len)
{
	snprintf(msg,len,"rchar %u wchar %u syscr %u syscw %u read_bytes %u write_bytes %u",iodata->rchar,iodata->wchar,iodata->syscr,iodata->syscw,iodata->read_bytes,iodata->write_bytes);
}

state_t io_diff(io_data_t *diff,io_data_t *iodata_init,io_data_t *iodata_end)
{
	diff->rchar = iodata_end->rchar -  iodata_init->rchar;
	diff->wchar = iodata_end->wchar -  iodata_init->wchar;
	diff->syscr = iodata_end->syscr - iodata_init->syscr;
	diff->syscw = iodata_end->syscw - iodata_init->syscw;
	diff->read_bytes = iodata_end->read_bytes - iodata_init->read_bytes;
	diff->write_bytes = iodata_end->write_bytes - iodata_init->write_bytes;
	diff->cancelled = iodata_end->cancelled - iodata_init->cancelled;
}

state_t io_dispose(ctx_t *c)
{
	fclose((FILE *)c->context);
}

