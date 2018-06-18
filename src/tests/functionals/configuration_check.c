/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/cluster_conf.h>

int EAR_VERBOSE_LEVEL=1;

void check_incomplete_statements(FILE *conf_file)
{
    char line[256];
    char *ltoken, *rtoken;
    int curr_line = 0;
    while (fgets(line, 256, conf_file) != NULL)
    {
        curr_line++;
        if (line[0] == '#') continue;

        ltoken = strtok(line, "=");
        if (!strcmp(ltoken, "\n")) continue;
        rtoken = strtok(NULL, " ");
        
        //printf("curr_tok: %s\t%s\n", ltoken, rtoken);
        if (rtoken == NULL) continue;

        if (strlen(rtoken) < 1 || !strcmp(rtoken, "\n"))
            printf("WARNING: empty statement at line %d beginning with %s\n", curr_line, ltoken);
            
    }
}

void check_invalid_ports(FILE *conf_file)
{
    char line[256];
    char *ltoken, *rtoken;
    int curr_line = 0;
    int port_num = 0;
    while (fgets(line, 256, conf_file) != NULL)
    {
        curr_line++;
        if (line[0] == '#') continue;

        ltoken = strtok(line, "=");

        if (!strcmp(ltoken, "\n")) continue;
        strtoup(ltoken);
        if (strstr(ltoken, "PORT") == NULL) continue;
        rtoken = strtok(NULL, "\n");

        if (rtoken == NULL) continue;
        port_num = atoi(rtoken);
        if (port_num < 0)
            printf("WARNING: negative port number at line %d beginning with %s\n", curr_line, ltoken);
    }
}

void main(int argc,char *argv[])
{
	cluster_conf_t my_cluster;
	char ear_path[256];
	if (get_ear_conf_path(ear_path)==EAR_ERROR){
		printf("Error getting ear.conf path\n");
		exit(0);
	}
    
    printf("Opening file %s\n", ear_path);
    FILE *conf_file = fopen(ear_path, "r");
    if (conf_file == NULL)
    {
        fprintf(stderr, "ERROR opening file: %s\n", ear_path);
        exit(0);
    }
    
    printf("Checking statements\n");
    check_incomplete_statements(conf_file);
    rewind(conf_file);
    check_invalid_ports(conf_file);
    fclose(conf_file);



    /*read_cluster_conf(ear_path,&my_cluster);
	print_cluster_conf(&my_cluster);
    my_node_conf_t *my = get_my_node_conf(&my_cluster, "cae2306");
    
    int i;
    for (i = 0; i < my->num_policies; i++)
    {
        print_policy_conf(&my->policies[i]);
    }*/
}
