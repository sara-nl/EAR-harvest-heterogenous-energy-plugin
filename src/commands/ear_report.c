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

#define STANDARD_NODENAME_LENGTH 25
#define APP_TEXT_FILE_FIELDS 29

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/types/application.h>
#if DB_MYSQL
#include <mysql/mysql.h>
#include <common/states.h>
#include <common/types/cluster_conf.h>
#include <common/database/mysql_io_functions.h>
cluster_conf_t my_conf;
int EAR_VERBOSE_LEVEL=0;
#endif


void usage(char *app)
{
    #if DB_FILES && !DB_MYSQL
	printf("Usage: %s job_id user_db_pathname [-v]\n",app);
    #endif
    #if DB_MYSQL
    printf("Usage: %s job_id [Optional parameters]\n"\
"\tOptional parameters: \n" \
"\t\t-v\tverbose mode for debugging purposes\n" \
"\t\t-u\tspecifies the user that executes the query [default: root]\n" \
"\t\t-db\tspecifies the database on which the query is executed [default: Report]\n" \
"\t\t-ip\tspecifies the ip where the MySQL server can be found [default: 127.0.0.1]\n" \
"", app);

#if DB_FILES
    printf("\t\t-f\tspecifies the file where the user-database can be found. If this option is used, the information will be read from the file and not the database.\n");
#endif
    #endif
	exit(1);
}

void read_from_files(int argc, char *argv[], char verbose, char file_location)
{
	int job_id, num_nodes, i, step_id=0;
	char **nodes;

    char nodelist_file_path[256], *nodelog_file_path, *env;
    char line_buf[256];
    FILE *nodelist_file, *node_file;
    
    
    char *token;
    job_id = atoi(strtok(argv[1], "."));
    token = strtok(NULL, ".");
    if (token != NULL) step_id = atoi(token);
    //job_id = job_id * 100 + step_id;
    
    
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
    
    if (num_nodes == 0)
    {
        fprintf(stderr, "No nodes found.\n");
        return;
    }
    
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
    char *nodename_prepend = (file_location > 0) ? argv[file_location] : argv[2];
    nodelog_file_path = malloc(strlen(nodename_extension)+ strlen(nodename_prepend) + STANDARD_NODENAME_LENGTH + 1);


    //allocate memory to hold all possible found jobs
    application_t **apps = (application_t**) malloc(sizeof(application_t*)*num_nodes);
    int jobs_counter = 0;

    //parse and filter job executions
    for (i = 0; i < num_nodes; i++)
    {
        sprintf(nodelog_file_path, "%s%s%s", nodename_prepend, nodes[i], nodename_extension);
        if (verbose) printf("Opening file for node %s (%s).\n", nodes[i], nodelog_file_path);
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
            if (apps[jobs_counter]->job.id == job_id && apps[jobs_counter]->job.step_id == step_id)
            {
                if (verbose) printf("Found job_id %d in file %s\n", apps[jobs_counter]->job.id, nodelog_file_path);
                jobs_counter++;
                break;
            }
        }

        fclose(node_file);
    }

    if (i == 1)
    {
        fprintf(stderr, "No jobs were found with id: %u.\n", job_id);
        free(nodelog_file_path);
        for (i = 0; i < num_nodes; i++)
            free(nodes[i]);
        free(nodes);
        return;
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
void read_from_database(int argc, char *argv[], int db, int usr, int host, char verbose)
{
    int num_apps = 0;
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }

    char *database = db > 0 ? argv[db] : my_conf.database.database;
    char *user = usr > 0 ? argv[usr] : my_conf.database.user;
    char *ip = host > 0 ? argv[host] : my_conf.database.ip;

    if(!mysql_real_connect(connection, ip, user,"", database, 0, NULL, 0))
    {
        fprintf(stderr, "Error connecting to the database(%d):%s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(1);
    }

    char query[256];
    
    int job_id, step_id = 0;
    char *token;
    job_id = atoi(strtok(argv[1], "."));
    token = strtok(NULL, ".");
    if (token != NULL) step_id = atoi(token);
    //job_id = job_id * 100 + step_id;
 
    if (verbose) fprintf(stderr, "Preparing query statement\n");
    sprintf(query, "SELECT * FROM Applications WHERE job_id=%u and step_id=%u", job_id, step_id);
    application_t *apps;

    if (verbose) fprintf(stderr, "Retrieving applications\n");
    num_apps = mysql_retrieve_applications(connection, query, &apps);
    if (verbose) fprintf(stderr, "Finalized retrieving applications\n");

    if (num_apps == EAR_MYSQL_ERROR)
    {
        fprintf(stderr, "Error retrieving information from database (%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(1);
    }

    else if (num_apps < 1)
    {
        printf("No jobs with %u job_id and %u step_id found. \n", job_id, step_id);
        mysql_close(connection);
        exit(1);
    }


    double avg_time, avg_power, total_energy, avg_f, avg_frequency, avg_GBS, avg_CPI, curr_energy;
    avg_frequency = 0;
    avg_time = 0;
    avg_power = 0;
    total_energy = 0;
    avg_CPI = 0;
    avg_GBS = 0;

    int i = 0;    
    if (apps[0].is_mpi)
    {
        printf("Node information:\n\tNodename\tTime (secs)\tDC Power (Watts)\tEnergy (Joules)\tAvg_freq (GHz)\tCPI\tGBS\n\t");
    
        for (i = 0; i < num_apps; i++)
        {
            avg_f = (double) apps[i].signature.avg_f/1000000;
            printf("%s \t\t%.2lf \t\t%.2lf \t\t\t%.2lf \t%.2lf\t\t%.2lf\t%.2lf\n\t", 
                    strtok(apps[i].node_id, "."), apps[i].signature.time, apps[i].signature.DC_power, 
		    apps[i].signature.DC_power * apps[i].signature.time, avg_f, apps[i].signature.CPI, apps[i].signature.GBS);
            avg_frequency += avg_f;
            avg_time += apps[i].signature.time;
            avg_power += apps[i].signature.DC_power;
            avg_GBS += apps[i].signature.GBS;
            avg_CPI += apps[i].signature.CPI;
            total_energy += apps[i].signature.time * apps[i].signature.DC_power;

        }
    }
    //case mpi without ear
    else if (num_apps > 1)
    {
        printf("Node information:\n\tNodename\tTime (secs)\tDC Power (Watts)\tEnergy (Joules)\tAvg_freq (GHz)\n\t");

        for (i = 0; i < num_apps; i++)
        {
            avg_f = (double) apps[i].power_sig.avg_f/1000000;
            printf("%s \t\t%.2lf \t\t%.2lf \t\t\t%.2lf \t%.2lf\t\t\n\t", 
                    strtok(apps[i].node_id, "."), apps[i].power_sig.time, apps[i].power_sig.DC_power, 
                    apps[i].power_sig.DC_power * apps[i].power_sig.time, avg_f);
            avg_frequency += avg_f;
            avg_time += apps[i].power_sig.time;
            avg_power += apps[i].power_sig.DC_power;
            total_energy += apps[i].power_sig.time * apps[i].power_sig.DC_power;
        }
    }
    avg_frequency /= num_apps;
    avg_time /= num_apps;
    avg_power /= num_apps;
    avg_CPI /= num_apps;
    avg_GBS /= num_apps;

    i=0;
    printf("\nApplication summary\n\tApp_id: %s\n\tJob_id: %lu\n\tStep_id: %lu\n\tPolicy: %s\n\tPolicy threshold: %.2lf\n",
            apps[0].job.app_id, apps[0].job.id, apps[0].job.step_id, apps[0].job.policy, apps[0].job.th);

    if (apps[0].is_mpi)
    {
        printf("\nApplication average:\n\tTime (secs.) \tDC Power (Watts) \tAcc. Energy (Joules) \tAvg_freq (GHz)\tCPI\tGBS\n\t");

        printf("%.2lf \t\t%.2lf \t\t\t%.2lf \t\t%.2lf\t\t%.2lf\t%.2lf\n", 
                avg_time, avg_power, total_energy, avg_frequency, avg_CPI, avg_GBS);
    }
    else if (num_apps > 1)
    {
        printf("\nApplication average:\n\tTime (secs.) \tDC Power (Watts) \tAcc. Energy (Joules) \tAvg_freq (GHz)\n\t");

        printf("%.2lf \t\t%.2lf \t\t\t%.2lf \t\t%.2lf\t\t\n", 
                avg_time, avg_power, total_energy, avg_frequency);
    }
    else
    {
        printf("\nApplication information:\n\tNodename\tTime (secs)\tDC Power (Watts)\tEnergy (Joules)\t Avg_freq (GHz)\n\t");
        avg_f = (double) apps[0].power_sig.avg_f/1000000;
        printf("%s \t%.2lf \t\t%.2lf \t\t\t%.2lf \t\t%.2lf\n",
                strtok(apps[0].node_id, "."), apps[0].power_sig.time, apps[0].power_sig.DC_power,
                apps[0].power_sig.DC_power * apps[0].power_sig.time, avg_f);
    }

    free(apps);

    mysql_close(connection);
}
#endif

void main(int argc, char *argv[])
{

#if DB_MYSQL
    if (argc < 2) usage(argv[0]);
#else
    if (argc < 3) usage(argv[0]);
#endif
    char verbose = 0;

#if DB_FILES && !DB_MYSQL
    if (argc > 3 && !strcmp("-v", argv[3])) verbose = 1;
    else verbose = 0;
    read_from_files(argc, argv, verbose, 0);
    exit(1);
#endif

#if DB_MYSQL
    int database = 0, user = 0, ip = 0, files = 0;
    int i = 0;
    if (argc > 2)
    {
        for (i = 2; i < argc; i++)
        {
            //fprintf(stderr, "Processing arg %d: %s\n", i, argv[i]);
            if (!strcmp("-db", argv[i])){
                database = ++i;
            }
            else if (!strcmp("-u", argv[i])){
                user = ++i;
            }
            else if (!strcmp("-ip", argv[i])){
                ip = ++i;
            }
            else if (!strcmp("-v", argv[i])){
                verbose = 1;
            }
#if DB_FILES
            else if (!strcmp("-f", argv[i])){
                files = ++i;
            }
#endif
        }
    }
    if (!files)
    {
        char ear_path[256];
        if (get_ear_conf_path(ear_path)==EAR_ERROR){
            printf("Error getting ear.conf path\n");
            exit(0);
        }
        read_cluster_conf(ear_path,&my_conf);  
        if (verbose) fprintf(stderr, "Initializing database reading.\n");
        read_from_database(argc, argv, database, user, ip, verbose);
    }
    else 
    {
        if (verbose) fprintf(stderr, "Reading from file.\n");
        read_from_files(argc, argv, verbose, files);
    }
#endif
    exit(1);
}
