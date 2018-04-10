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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX(x,y) ((x) >= (y)) ? (x) : (y)
#define ABS(x) ( x < 0) ? -x : x

#define MAX_NODE_FILES 4
#define MAX_SMALL_NAME_LENGTH 16
#define MAX_BIG_NAME_LENGTH 256
#define MAX_NAME_LENGTH 64
#define MAX_SAMPLES 8192
#define MIN_SAMPLES 2048
#define BOT_SAMPLES 100

#define MILLION 1000000.0f
#define MIN_FREQ 1.7f

#define EVENTS 14
#define EV_IDP 1
#define EV_LEN 2
#define EV_ITE 3
#define EV_TIM 4
#define EV_CPI 5
#define EV_TPI 6
#define EV_GBS 7
#define EV_POW 8
#define EV_TIMP 9
#define EV_CPIP 10
#define EV_POWP 11
#define EV_FREQ 12
#define EV_ENE 13

typedef unsigned int uint;
const int event_add = 60000;
const float event_mul = 1000.0f;
int total_node_files;

struct NodeFile {
    unsigned int file_size;
    char path[MAX_NAME_LENGTH];
    char node[MAX_NAME_LENGTH];
    float max_value[EVENTS];
    float data[EVENTS][MAX_SAMPLES];
    unsigned int timestamp[EVENTS][MAX_SAMPLES];
    unsigned int lengths[EVENTS];
    unsigned int indexes[EVENTS];
    unsigned int last_timestamp;
    unsigned int rank;
} node_files[MAX_NODE_FILES];

struct Appinfo {
    unsigned int file_found;
    char copy_command[MAX_BIG_NAME_LENGTH];
    char server_path[MAX_BIG_NAME_LENGTH];
    char local_path[MAX_BIG_NAME_LENGTH];
    char file_path[MAX_BIG_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];
    char policy[MAX_NAME_LENGTH];
    unsigned int total_timestamp;
    unsigned int mpis_per_node;
    unsigned int nodes;
    unsigned int mpis;
} appinfo;

struct Archinfo {
    unsigned int file_found;
    char file_path[MAX_BIG_NAME_LENGTH];
    char cpu_model[MAX_BIG_NAME_LENGTH];
    char cpu_name[MAX_BIG_NAME_LENGTH];
    float fmax_freq;
    float fmin_freq;
    float fnom_freq;
    unsigned int max_freq;
    unsigned int min_freq;
    unsigned int nom_freq;
    unsigned int sockets;
    unsigned int cores_per_socket;
} archinfo;

void store_event(uint node_file_index, uint event, float value, unsigned int timestamp)
{
    NodeFile *node_file = &node_files[node_file_index];

    uint data_index = node_file->lengths[event];
    uint max_value = node_file->max_value[event];

    node_file->data[event][data_index] = value;
    node_file->max_value[event] = MAX(max_value, value);
    node_file->timestamp[event][data_index] = timestamp;
    node_file->last_timestamp = timestamp;

    node_file->lengths[event] = node_file->lengths[event] + 1;
    node_file->indexes[event] = node_file->lengths[event];

    if (timestamp > appinfo.total_timestamp) {
        appinfo.total_timestamp = timestamp;
    }
}

void read_node_files()
{
    FILE *fd;
    int result;
    unsigned int event;
    unsigned int timestamp;
    float value;

    struct stat st;
    char buffer[MAX_BIG_NAME_LENGTH];
    char *timestamp_ptr;
    char *event_ptr;
    char *value_ptr;
    char *line, *tmp1, *tmp2;

    for (int i = 0; i < total_node_files; ++i)
    {
        result = stat(node_files[i].path, &st);

        if (result != -1 && st.st_size > node_files[i].file_size)
        {
            fd = fopen(node_files[i].path, "r");

            fseek(fd, node_files[i].file_size, SEEK_SET);
            node_files[i].file_size = st.st_size;

            if (fd != NULL)
            {
                while ((line = fgets(buffer, MAX_BIG_NAME_LENGTH, (FILE *) fd)) != NULL)
                {
                    timestamp_ptr = strsep(&line, ";");
                    tmp1 = line;
                    event_ptr = strsep(&tmp1, ";");
                    tmp2 = tmp1;
                    value_ptr = strsep(&tmp2, "\n");

                    if (line != NULL && strlen(timestamp_ptr) &&
                        tmp1 != NULL && strlen(event_ptr) &&
                        tmp2 != NULL && strlen(value_ptr))
                    {
                        event = atoi(event_ptr);
                        value = atof(value_ptr);
                        timestamp = atoi(timestamp_ptr);
                        store_event(i, event, value, timestamp);
                    }
                    else
                    {
                        printf("Parsing error, avoiding value of file %u.%s.\n",
                               node_files[i].rank, node_files[i].node);
                    }
                }

                //node_files[i].file_size = (int) ftell(fd);
                fclose(fd);
            }
        }
    }
}

void read_appinfo_file()
{
    char buffer[MAX_BIG_NAME_LENGTH];
    FILE *fd;

    if (!appinfo.file_found)
        return;

    fd = fopen(appinfo.file_path, "r");

    if (fd != NULL)
    {
        #define GETBUF if (fgets(buffer, MAX_BIG_NAME_LENGTH, fd) != NULL)
        GETBUF strcpy(appinfo.name, buffer);
        GETBUF appinfo.nodes = atoi(buffer);
        GETBUF appinfo.mpis = atoi(buffer);
        GETBUF appinfo.mpis_per_node = atoi(buffer);
        GETBUF strcpy(appinfo.policy, buffer);

        appinfo.name[strlen(appinfo.name) - 1] = '\0';
        appinfo.policy[strlen(appinfo.policy) - 1] = '\0';

        fclose(fd);
    }
    else {
        printf("Appinfo file error: imposible to read %s.\n", appinfo.local_path);
    }
}

void read_archinfo_file()
{
    char buffer[MAX_BIG_NAME_LENGTH];
    FILE *fd;

    if (!archinfo.file_found)
        return;

    fd = fopen(archinfo.file_path, "r");

    if (fd != NULL)
    {
        #define GETBUF if (fgets(buffer, MAX_BIG_NAME_LENGTH, fd) != NULL)
        GETBUF archinfo.sockets = atoi(buffer);
        GETBUF archinfo.cores_per_socket = atoi(buffer);
        GETBUF archinfo.max_freq = atoi(buffer);
        GETBUF archinfo.min_freq = atoi(buffer);
        GETBUF archinfo.nom_freq = atoi(buffer);
        GETBUF strcpy(archinfo.cpu_model, buffer);
        GETBUF strcpy(archinfo.cpu_name, buffer);

        archinfo.fmin_freq = (float) archinfo.min_freq / 1000000.0f;
        archinfo.fnom_freq = (float) archinfo.nom_freq / 1000000.0f;
        archinfo.fmax_freq = (float) archinfo.max_freq / 1000000.0f;

        fclose(fd);

        // Setting first frequency
        for (int i = 0; i < MAX_NODE_FILES; ++i) {
            store_event(i, EV_FREQ, (float) archinfo.nom_freq, 0);
        }
    }
    else {
        printf("Archinfo file error: imposible to read %s.\n", appinfo.local_path);
    }
}

void scan_file(const char* path, const char* file_name)
{
    #define EXTENSION "txt"
    #define EXTENSION_LENGTH 3

    char file_name_copy[MAX_NAME_LENGTH];
    char *rank_ptr, *node_ptr, *ext_ptr;
    NodeFile *node_file;
    int length = 0;
    int rank = 0;

    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        return;

    printf("- %s ... ", file_name);

    if (strcmp(file_name, "Appinfo." EXTENSION) == 0)
    {
        strcpy(appinfo.file_path, path);
        strcat(appinfo.file_path, file_name);
        appinfo.file_found = 1;
    }
    else if (strcmp(file_name,  "Archinfo." EXTENSION) == 0)
    {
        strcpy(archinfo.file_path, path);
        strcat(archinfo.file_path, file_name);
        archinfo.file_found = 1;
    }
    else
    {
        strcpy(file_name_copy, file_name);
        length = strlen(file_name);

        if (length < 7 || total_node_files == MAX_NODE_FILES) {
            printf("denied.\n");
            return;
        }

        rank_ptr = file_name_copy;
        node_ptr = file_name_copy;
        ext_ptr = &file_name_copy[length - EXTENSION_LENGTH];
        file_name_copy[length - 4] = '\0';
        strsep(&node_ptr, ".");

        bool denied = rank_ptr == NULL;
        denied |= node_ptr == NULL;
        denied |= ext_ptr == NULL;

        if (denied || strcmp(ext_ptr, EXTENSION) != 0) {
            printf("denied.\n");
            return;
        }

        rank = atoi(rank_ptr);

        if (rank == 0 && rank_ptr[0] != '0') {
            printf("denied.\n");
            return;
        }

        node_file = &node_files[total_node_files];
        total_node_files += 1;

        node_file->rank = rank;
        node_file->file_size = 0;
        strcpy(node_file->node, node_ptr);
        strcpy(node_file->path, path);
        strcat(node_file->path, file_name);
    }
    printf("accepted.\n");
}

void scan_folder(char *local_path)
{
    struct dirent *directory = NULL;
    char path[MAX_BIG_NAME_LENGTH];
    DIR *dd;

    sprintf(appinfo.local_path, "%s", local_path);
    sprintf(path, "%s/", appinfo.local_path);
    dd = opendir(path);

    if (dd != NULL)
    {
        printf("Reading files in %s:\n", path);
        while ((directory = readdir(dd)) != NULL)
        {
            scan_file(path, directory->d_name);
        }
        closedir(dd);
        printf("Reading completed.\n");
    }
    else
    {
        printf("Error: the path doesn't exists.\n");
        exit(1);
    }

    printf("%s\n%s\n", archinfo.file_path, appinfo.file_path);
}

void save_paraver()
{
    NodeFile *node_file;
    char full_path[256];
    char buffer[256];
    char path[256];
    FILE *fd;
    int i;

    sprintf(path, "%s/paraver", appinfo.local_path);
    mkdir(path, 0700);

    // Config
    sprintf(full_path, "%s/%s.%s.pcf", path, appinfo.name, appinfo.policy);
    remove(full_path);
    fd = fopen(full_path, "w");

    fputs("GRADIENT_COLOR\n", fd);
    fputs("0\t{255,255,255}\n\n", fd);
    fputs("GRADIENT_NAMES\n", fd);
    fputs("0\twhite\n\n", fd);
    fputs("EVENT_TYPE\n0\t60001\tPERIOD_ID\n\n", fd);
    fputs("EVENT_TYPE\n0\t60002\tPERIOD_LENGTH\n\n", fd);
    fputs("EVENT_TYPE\n0\t60003\tPERIOD_ITERATIONS\n\n", fd);
    fputs("EVENT_TYPE\n0\t60004\tPERIOD_TIME\n\n", fd);
    fputs("EVENT_TYPE\n0\t60005\tPERIOD_CPI\n\n", fd);
    fputs("EVENT_TYPE\n0\t60006\tPERIOD_TPI\n\n", fd);
    fputs("EVENT_TYPE\n0\t60007\tPERIOD_GBS\n\n", fd);
    fputs("EVENT_TYPE\n0\t60008\tPERIOD_POWER\n\n", fd);
    fputs("EVENT_TYPE\n0\t60009\tPERIOD_TIME_PROJECTION\n\n", fd);
    fputs("EVENT_TYPE\n0\t60010\tPERIOD_CPI_PROJECTION\n\n", fd);
    fputs("EVENT_TYPE\n0\t60011\tPERIOD_POWER_PROJECTION\n\n", fd);
    fputs("EVENT_TYPE\n0\t60012\tFREQUENCY\n\n", fd);
    fputs("EVENT_TYPE\n0\t60013\tENERGY\n\n", fd);

    fclose(fd);

    // Trace.prv
    sprintf(full_path, "%s/%s.%s.prv", path, appinfo.name, appinfo.policy);
    remove(full_path);
    fd = fopen(full_path, "w");

    fprintf(fd, "#Paraver (01/01/1970 at 00:00):%u:", appinfo.total_timestamp);

    // Nodes
    for (i = 0; i < total_node_files * 2; i += 2) {
        buffer[i] = '1';
        buffer[i+1] = ',';
    }
    buffer[i-1] = '\0';
    fprintf(fd, "%u(%s):", total_node_files, buffer);

    // Tasks
    fprintf(fd, "1:%u(1:1", total_node_files);

    for (i = 1; i < total_node_files; ++i) {
        fprintf(fd, ",1:%u", i+1);
    }
    fputs(")\n", fd);

    // States
    for (i = 0; i < total_node_files; ++i)
    {
        node_file = &node_files[i];
        fprintf(fd, "1:%u:1:%u:1:%u:%u:%u\n", i + 1, i + 1, 0, appinfo.total_timestamp, 1);
    }

    // Events
    for (i = 0; i < total_node_files; ++i) {
        node_file = &node_files[i];

        for (int j = 1; j < EVENTS; ++j) {
            int event_length = node_file->lengths[j];

            for (int k = 0; k < event_length; ++k)
            {
                float data = node_file->data[j][k];

                if (j == 5 || j == 6 || j == 7 || j == 10) {
                    data = data * event_mul;
                }

                fprintf(fd, "2:%u:1:%u:1:%u:%u:%0.0f\n",
                        i + 1, i + 1, node_file->timestamp[j][k], j + event_add, data);
            }
        }
    }

    fclose(fd);

    // Trace.row
    sprintf(full_path, "%s/%s.%s.row", path, appinfo.name, appinfo.policy);
    remove("%s/trace.row");
    fd = fopen(full_path, "w");

    fprintf(fd, "LEVEL CPU SIZE %u\n", total_node_files);
    for (i = 0; i < total_node_files; ++i) {
        node_file = &node_files[i];
        fprintf(fd, "%u.%s\n", node_file->rank, node_file->node);
    }
    fprintf(fd, "\nLEVEL NODE SIZE %u\n", total_node_files);
    for (i = 0; i < total_node_files; ++i) {
        node_file = &node_files[i];
        fprintf(fd, "%s\n", node_file->node);
    }
    fprintf(fd, "\nLEVEL THREAD SIZE %u\n", total_node_files);
    for (i = 0; i < total_node_files; ++i) {
        node_file = &node_files[i];
        fprintf(fd, "THREAD 1.%u.1\n", i + 1);
    }

    fclose(fd);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: gui_to_paraver <gui.download.folder>\n");
        exit(1);
    }
    scan_folder(argv[1]);
    read_archinfo_file();
    read_appinfo_file();
    read_node_files();
    save_paraver();
}