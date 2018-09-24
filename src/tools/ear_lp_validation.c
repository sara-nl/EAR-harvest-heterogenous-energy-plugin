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



#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <cpufreq.h>

#include <common/config.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/types/signature.h>
#include <common/types/coefficient.h>
#include <common/types/projection.h>
#include <common/states.h>
#include <common/database/db_helper.h>

#define CREATE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

int EAR_VERBOSE_LEVEL=1;
application_t *app_list;
application_t **sorted_app_list;
coefficient_v3_t **coeffs_list;

uint *samples_per_app;
uint num_diff_apps;

char nodename[256],*coeff_root;
uint *node_freq_list;
uint num_node_p_states;
uint min_freq;
uint nom_freq;

uint num_apps, current_app = 0;
uint *samples_f, i, *current;

#define MALLOC(var, type, mult)                       \
    var = (type *) malloc(sizeof(type) * mult);       \
    if (var == NULL) {                                \
        fprintf(stderr, "Error allocating memory\n"); \
        exit(1);                                      \
    }
#define OPEN(desc, file_name, permissions, flags)     \
    desc = open(file_name, permissions, flags);       \
    if (desc < 0) {                                   \
        perror("Error opening/creating file.");       \
        exit(1);                                      \
    }


uint p_state_to_freq(int i) {
    return node_freq_list[i];
}

uint freq_to_p_state(uint freq)
{
    int is_greater;
    int is_equal;
    int has_next;
    int found = 0;
    int i = 0;

    while (i < num_node_p_states && !found) {
        is_equal = freq == node_freq_list[i];
        has_next = i < num_node_p_states - 1;
        is_greater = has_next && freq > node_freq_list[i + 1];
        found = is_equal || is_greater || !has_next;
        i = i + !found;
    }

    return i;
}

uint fill_list_p_states()
{
    struct cpufreq_available_frequencies *list_freqs, *first_freq;
    int num_pstates = 0;

    list_freqs = cpufreq_get_available_frequencies(0);
    first_freq = list_freqs;

    while (list_freqs != NULL) {
        list_freqs = list_freqs->next;
        num_pstates++;
    }

    MALLOC(node_freq_list, uint, num_pstates);
    list_freqs = first_freq;

    for (i = 0; i < num_pstates; i++)
    {
        if (i == 1) nom_freq = list_freqs->frequency;
        node_freq_list[i] = list_freqs->frequency;
        list_freqs = list_freqs->next;
    }

    cpufreq_put_available_frequencies(first_freq);
    return num_pstates;
}

int app_exists(application_t *Applist, uint total_apps, application_t *newApp) {
    uint pos = 0, found = 0;

    while ((pos < total_apps) && (found == 0)) {
        if ((strcmp(Applist[pos].job.app_id, newApp->job.app_id) == 0) &&
            (Applist[pos].signature.def_f == newApp->signature.def_f)) {
            found = 1;
        } else {
            pos++;
        }
    }
    if (found == 0) return -1;
    else return pos;
}

void average_list_samples(signature_t *current, uint samples)
{
    double foravg = (double) samples;
    current->time = current->time / foravg;
    current->GBS = current->GBS / foravg;
    current->DC_power = current->DC_power / foravg;
    current->CPI = current->CPI / foravg;
    current->TPI = current->TPI / foravg;
}

int invalid_signature(signature_t *S)
{
	if ((S->time<=MIN_TIME) || (S->time>MAX_TIME))  return 1;
	if ((S->GBS<=MIN_GBS) || (S->GBS > MAX_GBS)) return 1;
	if ((S->DC_power<=MIN_POWER) || (S->DC_power>MAX_POWER)) return 1;
	if ((S->TPI<=MIN_TPI) || (S->TPI>MAX_TPI))	return 1;
	if ((S->CPI<=MIN_CPI) || (S->CPI>MAX_CPI)) return 1;
	return 0;
}

// A=A+B metrics
void accum_app(char *app,char *node,signature_t *A, signature_t *B)
{
	/* Warning 2*/
	if (invalid_signature(B)){
		printf("Invalid signature %s at node %s\n",app,node);
	}
    A->time += B->time;
    A->GBS += B->GBS;
    A->DC_power += B->DC_power;
    A->TPI += B->TPI;
    A->CPI += B->CPI;
}

void write_app(application_t *A, application_t *B)
{
    strcpy(A->job.app_id, B->job.app_id);
    A->signature.def_f = B->signature.def_f;
    A->job.procs = B->job.procs;
    signature_t *sig_a = &A->signature;
    signature_t *sig_b = &B->signature;
    sig_a->time = sig_b->time;
    sig_a->GBS = sig_b->GBS;
    sig_a->DC_power = sig_b->DC_power;
    sig_a->TPI = sig_b->TPI;
    sig_a->CPI = sig_b->CPI;
}

void nominal_for_power(uint ref, char *app_name, double *power, double *tpi)
{
    int i = 0, found = 0;
    *power = 0;
    *tpi = 0;

    while ((i < samples_f[ref]) && (found == 0))
    {
        if (strcmp(sorted_app_list[ref][i].job.app_id, app_name) == 0) {
            *power = sorted_app_list[ref][i].signature.DC_power;
            *tpi = sorted_app_list[ref][i].signature.TPI;
            found = 1;
        } else i++;
    }
}

void nominal_for_cpi(uint ref, char *app_name, double *cpi, double *tpi)
{
    int i = 0, found = 0;
    *cpi = 0;
    *tpi = 0;

    while ((i < samples_f[ref]) && (found == 0))
    {
        if (strcmp(sorted_app_list[ref][i].job.app_id, app_name) == 0)
        {
            *cpi = sorted_app_list[ref][i].signature.CPI;
            *tpi = sorted_app_list[ref][i].signature.TPI;
            found = 1;
        } else i++;
    }
}


void usage(char *app)
{
    fprintf(stdout, "Usage: %s nodename num_apps num_runs num_freqs \n", app);
    exit(1);
}

int main(int argc, char *argv[])
{
    application_t read_app;
    double power, cpi, tpi;
    uint filtered_apps = 0, ret,is_learning=1;
    uint f, pos, ref, i;
	ulong p_state_max;
    int fd, index;
	int expected_num_apps,expected_num_runs, expected_num_freqs;
	

    if (argc != 5){
        usage(argv[0]);
    }
	strcpy(nodename,argv[1]);
	expected_num_apps=atoi(argv[2]);
	expected_num_runs=atoi(argv[3]);
	expected_num_freqs=atoi(argv[4]);
	


    // We get how many samples per frequency we have
    num_node_p_states = fill_list_p_states();

    // samples_f is to compute how many samples per frequency we have
    MALLOC(samples_f, uint, num_node_p_states);
    MALLOC(current, uint, num_node_p_states);

    for (i = 0; i < num_node_p_states; i++) {
        samples_f[i] = 0;
        current[i] = 0;
    }

    application_t *apps,*tmp_apps;
	uint total_apps=0;
	
  	char ear_path[256];
    cluster_conf_t my_conf;
	my_node_conf_t * my_node;
    if (get_ear_conf_path(ear_path)==EAR_ERROR){
            printf("Error getting ear.conf path\n");
            exit(0);
    }
    read_cluster_conf(ear_path,&my_conf);
	my_node=get_my_node_conf(&my_conf,nodename);
    if (my_node==NULL){
    	fprintf(stderr," Error in cluster configuration, node %s not found\n",nodename);
    }
	fprintf(stdout,"ear.conf ready\n");
    init_db_helper(&my_conf.database);
	num_apps =get_num_applications(is_learning, nodename);
 	MALLOC(app_list, application_t, num_apps);
 	MALLOC(apps, application_t, num_apps);
	fprintf(stdout,"%d applications in DB for learning phase\n",num_apps);
    ret=db_read_applications(&tmp_apps,is_learning, 50,NULL);
    while (ret > 0)
    {
		fprintf(stdout,"%d applications retrieved from DB\n",ret);
        for (i=0;i<ret;i++){
            if (strcmp(tmp_apps[i].node_id,nodename)==0){
				copy_application(&apps[total_apps],&tmp_apps[i]);
				total_apps++;
			}
        }
        free(tmp_apps);
        ret=db_read_applications(&tmp_apps,is_learning, 50,NULL);
    }
	/* Warning */
	if (total_apps!=num_apps){
		printf("Warning, DB has reported %d applications and only %d has been readed for node %s\n",num_apps,total_apps,nodename);
	}
	if (total_apps!=(expected_num_apps*expected_num_runs*expected_num_freqs)){
		printf("Warning, DB has reported %d applications and we were expecting %d for node %s\n",total_apps,expected_num_apps*expected_num_runs*expected_num_freqs);
	}
    MALLOC(samples_per_app, uint, num_apps);
   
    for (i = 0; i < num_apps; i++) {
		print_application(&apps[i]);
        samples_per_app[i] = 0;
    }
	/* Min freq set by default to 1 GHz */ 
	min_freq=p_state_to_freq(expected_num_freqs);
    for (i = 0; i < num_apps; i++)
    {
		printf("APP SIGNATURE %lu\n", apps[i].signature.def_f);
        if (apps[i].signature.def_f >= min_freq) {

            if ((index = app_exists(app_list, filtered_apps, &apps[i])) >= 0) {
                // If APP exists, then accumulate its values in
                accum_app(app_list[index].job.app_id,,nodename,&app_list[index].signature, &apps[i].signature);
                samples_per_app[index]++;
            } else {
                write_app(&app_list[filtered_apps], &apps[i]);
                samples_per_app[filtered_apps] = 1;
                filtered_apps++;
            }
        }
    }

    // We will consider only applictions with f >= min_freq
    num_apps = filtered_apps;

    // We must compute the average per (app,f)
    for (i = 0; i < num_apps; i++) {
        average_list_samples(&app_list[i].signature, samples_per_app[i]);
		if (samples_per_app[i]!=expected_runs){
			printf("Warning, app %s freq %ul nodename %s with %d runs, expected %d\n",
			app_list[i].job.app_id,app_list[i].signature.def_f,nodename,samples_per_app[i],expected_runs);
		}
    }
	

    // We maintain the name's of applications to generate graphs
    for (current_app = 0; current_app < num_apps; current_app++) {
        if (app_list[current_app].signature.def_f >= min_freq) {
            index = freq_to_p_state(app_list[current_app].signature.def_f);
            samples_f[index]++;
        }
    }
	for (i=1;i<=expected_pstates;i++) {
		if (samples_f[i]!=expected_num_apps*expect_num_runs){
			/* Warning */
			printf("Warning, pstate %d with %d samples, expected %d\n",i,samples_f[i],expected_num_apps*expect_num_runs);
		}
	}

    return 0;
}
