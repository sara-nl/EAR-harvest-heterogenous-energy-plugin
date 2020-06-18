/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <common/config.h>


int create_log(char *path,char *service_name)
{
        int fd;
        char logfile[256];
        mode_t my_mask;
        my_mask=umask(0);

        sprintf(logfile,"%s/%s.log",path,service_name);
        unlink(logfile);
        //fd=open(logfile,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        fd=open(logfile,O_CREAT|O_WRONLY|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        chmod(logfile,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        
        umask(my_mask);

        return fd;
}

