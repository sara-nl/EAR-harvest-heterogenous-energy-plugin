/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#define STANDARD_NODENAME_LENGTH 25
#define APP_TEXT_FILE_FIELDS 29

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/types/application.h>
#if DB_MYSQL
#include <mysql.h>
#include <common/states.h>
#include <common/database/mysql_io_functions.h>
#endif


void usage(char *app)
{
    #if DB_FILES
	printf("Usage: %s job_id user_db_pathname [-v]\n",app);
    #endif
    #if DB_MYSQL
    printf("Usage: %s job_id db_ip [options]\n \
                \tOptions: \
                    \t\t-u\tspecifies the user that executes the query\
                    \t\t-db\tspecifies the database on which the query is executed\n", app);
    #endif
	exit(1);
}

void read_from_files(int argc, char *argv[], char verbose)
{
	int job_id, num_nodes, i;
	char **nodes;

    job_id = atoi(argv[1]);
    char nodelist_file_path[256], *nodelog_file_path, *env;
    char line_buf[256];
    FILE *nodelist_file, *node_file;


    strcpy(nodelist_file_path, EAR_INSTALL_PATH);
    strcat(nodelist_file_path, "/etc/sysconf/nodelist.conf");

    nodelist_file = fopen(nodelist_file_path, "r");
    if (nodelist_file == NULL)
    {
        printf("Error opening nodelist file at \"$EAR_INSTALL_PATH/etc/sysconf/nodelist.conf\".\n");
        printf("Filepath: %s\n", nodelist_file_path);
        exit(1);
    }
    if (verbose) printf("Nodelist found at: %s\n", nodelist_file_path);
    
    //count the total number of nodes
    if (verbose) printf("Counting nodes...\n");
    num_nodes = 0;
    while(fscanf(nodelist_file, "%s\n", line_buf) > 0) {
        num_nodes += 1;
    }
    rewind(nodelist_file);
    if (verbose) printf("Found %d nodes.\n", num_nodes);

    //initialize memory for each 
    nodes = calloc(num_nodes, sizeof(char*));
    for (i = 0; i < num_nodes; i++)
    {
        nodes[i] = malloc(sizeof(char) * STANDARD_NODENAME_LENGTH);
        fscanf(nodelist_file, "%s", nodes[i]);
    }
    //all the information needed from nodelist_file has been read, it can be closed
    fclose(nodelist_file);


    char *nodename_extension = ".hpc.eu.lenovo.com.csv";
    char *nodename_prepend = argv[2];
    nodelog_file_path = malloc(strlen(nodename_extension)+ strlen(nodename_prepend) + STANDARD_NODENAME_LENGTH + 1);


    //allocate memory to hold all possible found jobs
    application_t **apps = (application_t**) malloc(sizeof(application_t*)*num_nodes);
    int jobs_counter = 0;

    //parse and filter job executions
    for (i = 0; i < num_nodes; i++)
    {
        sprintf(nodelog_file_path, "%s%s%s", nodename_prepend, nodes[i], nodename_extension);
        node_file = fopen(nodelog_file_path, "r");

        if (node_file == NULL)
        {
            continue;
        }
        if (verbose) printf("File for node %s (%s) opened.\n", nodes[i], nodelog_file_path);

        //first line of each file is the header
        fscanf(node_file, "%s\n", line_buf);
        
        apps[jobs_counter] = (application_t*) malloc(sizeof(application_t));
        init_application(apps[jobs_counter]);
        if (verbose) printf("Checking node for signatures with %d job id.\n", job_id);
        while (scan_application_fd(node_file, apps[jobs_counter]) == APP_TEXT_FILE_FIELDS)
        {
            if (apps[jobs_counter]->job.id == job_id )
            {
                if (verbose) printf("Found job_id %d in file %s\n", apps[jobs_counter]->job.id, nodelog_file_path);
                jobs_counter++;
                break;
            }
        }

        fclose(node_file);
    }

    printf("Node information:\nJob_id \tNodename \t\t\tTime (secs) \tDC Power (Watts) \tEnergy (Joules) \tAvg_freq (GHz)\n");

    double avg_time, avg_power, total_energy, avg_f, avg_frequency;
    avg_frequency = 0;
    avg_time = 0;
    avg_power = 0;
    total_energy = 0;
    for (i = 0; i < jobs_counter; i++)
    {
        avg_f = (double) apps[i]->signature.avg_f/1000000;
        printf("%s \t%s \t%.2lf \t\t%.2lf \t\t\t%.2lf \t\t%.2lf\n", 
                apps[i]->job.id, apps[i]->node_id, apps[i]->signature.time, apps[i]->signature. DC_power, 
		apps[i]->signature.DC_power * apps[i]->signature.time, avg_f);
        avg_frequency += avg_f;
        avg_time += apps[i]->signature.time;
        avg_power += apps[i]->signature. DC_power;
        total_energy += apps[i]->signature.time * apps[i]->signature.DC_power;

    }

    avg_frequency /= jobs_counter;
    avg_time /= jobs_counter;
    avg_power /= jobs_counter;

    printf("\nApplication average:\nJob_id \tTime (secs.) \tDC Power (Watts) \tAccumulated Energy (Joules) \tAvg_freq (GHz)\n");

    printf("%s \t%.2lf \t\t%.2lf \t\t\t%.2lf \t\t\t%.2lf\n", 
            argv[1], avg_time, avg_power, total_energy, avg_frequency);


    for (i = 0; i < jobs_counter; i++)
        free(apps[i]);
    free(apps);

    free(nodelog_file_path);


    //freeing variables
    for (i = 0; i < num_nodes; i++)
        free(nodes[i]);
    free(nodes);

}

#if DB_MYSQL
void read_from_database(int argc, char *argv[], char db, char user)
{
    int num_apps = 0;
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }
    char *database = db > 0 ? argv[db] : "Report";
    char *user = user > 0 ? argv[user] : "root";

    mysql_real_connect(connection, argv[2], "root", "", "Report", 0, NULL, 0);

    char query[256];

    sprintf(query, "SELECT * FROM Applications WHERE job_id=%s", argv[1]);
    application_t *apps;

    num_apps = mysql_retrieve_applications(connection, query, &apps);

    if (num_apps == EAR_MYSQL_ERROR)
    {
        fprintf(stderr, "Error retrieving information from database (%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(1);
    }

    if (num_apps < 1)
    {
        printf("No jobs with %s job_id found. \n", argv[1]);
        mysql_close(connection);
        exit(1);
    }


    double avg_time, avg_power, total_energy, avg_f, avg_frequency;
    avg_frequency = 0;
    avg_time = 0;
    avg_power = 0;
    total_energy = 0;
    int i = 0;    
    
    printf("Node information:\nJob_id \tNodename \t\t\tTime (secs) \tDC Power (Watts) \tEnergy (Joules) \tAvg_freq (GHz)\n");
    
    for (i = 0; i < num_apps; i++)
    {
        avg_f = (double) apps[i].signature.avg_f/1000000;
        printf("%u \t%s \t%.2lf \t\t%.2lf \t\t\t%.2lf \t\t%.2lf\n", 
                apps[i].job.id, apps[i].node_id, apps[i].signature.time, apps[i].signature.DC_power, 
		apps[i].signature.DC_power * apps[i].signature.time, avg_f);
        avg_frequency += avg_f;
        avg_time += apps[i].signature.time;
        avg_power += apps[i].signature. DC_power;
        total_energy += apps[i].signature.time * apps[i].signature.DC_power;

    }

    avg_frequency /= num_apps;
    avg_time /= num_apps;
    avg_power /= num_apps;

    printf("\nApplication average:\nJob_id \tTime (secs.) \tDC Power (Watts) \tAccumulated Energy (Joules) \tAvg_freq (GHz)\n");

    printf("%s \t%.2lf \t\t%.2lf \t\t\t%.2lf \t\t\t%.2lf\n", 
            argv[1], avg_time, avg_power, total_energy, avg_frequency);


    free(apps);

    mysql_close(connection);
}
#endif

void main(int argc, char *argv[])
{
    if (argc < 3) usage(argv[0]);

    #if DB_FILES
    char verbose = 0;
    if (argc > 3 && !strcmp("-v", argv[3])) verbose = 1;
    else verbose = 0;
    read_from_files(argc, argv, verbose);
    #endif

    #if DB_MYSQL
    char database = 0, user = 0;
    int i = 0;
    if (argc > 3)
    {
        for (i = 3; i < argc - 1; i++)
        {
            if (!strcmp("-db", argv[i]))
                database = i++;
            if (!strcmp("-u", argv[i]))
                user = i++;
        }
    }
    read_from_database(argc, argv, database, user);
    #endif
    exit(1);
}
