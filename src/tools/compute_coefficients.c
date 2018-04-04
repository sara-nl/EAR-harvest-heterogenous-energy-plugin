/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <cpufreq.h>

#include <common/types/application.h>
#include <common/types/coefficient.h>
#include <common/types/projection.h>
#include <common/config.h>

#define CREATE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

application_t *app_list;
application_t **sorted_app_list;
coefficient_t **coeffs_list;

uint *samples_per_app;
uint num_diff_apps;

char *nodename;
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
        if ((strcmp(Applist[pos].app_id, newApp->app_id) == 0) &&
            (Applist[pos].def_f == newApp->def_f)) {
            found = 1;
        } else {
            pos++;
        }
    }
    if (found == 0) return -1;
    else return pos;
}

void average_list_samples(application_t *current, uint samples)
{
    double foravg = (double) samples;
    current->time = current->time / foravg;
    current->GBS = current->GBS / foravg;
    current->DC_power = current->DC_power / foravg;
    current->CPI = current->CPI / foravg;
    current->TPI = current->TPI / foravg;
}

// A=A+B metrics
void accum_app(application_t *A, application_t *B)
{
    A->time += B->time;
    A->GBS += B->GBS;
    A->DC_power += B->DC_power;
    A->TPI += B->TPI;
    A->CPI += B->CPI;
}

void write_app(application_t *A, application_t *B)
{
    strcpy(A->app_id, B->app_id);
    A->def_f = B->def_f;
    A->procs = B->procs;
    A->time = B->time;
    A->GBS = B->GBS;
    A->DC_power = B->DC_power;
    A->TPI = B->TPI;
    A->CPI = B->CPI;
}

void nominal_for_power(uint ref, char *app_name, double *power, double *tpi)
{
    int i = 0, found = 0;
    *power = 0;
    *tpi = 0;

    while ((i < samples_f[ref]) && (found == 0))
    {
        if (strcmp(sorted_app_list[ref][i].app_id, app_name) == 0) {
            *power = sorted_app_list[ref][i].DC_power;
            *tpi = sorted_app_list[ref][i].TPI;
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
        if (strcmp(sorted_app_list[ref][i].app_id, app_name) == 0)
        {
            *cpi = sorted_app_list[ref][i].CPI;
            *tpi = sorted_app_list[ref][i].TPI;
            found = 1;
        } else i++;
    }
}

void init_list_coeffs(uint ref, uint i, uint f, double A, double B, double C, double D, double E, double F)
{
    coeffs_list[ref][i].pstate = f;
    coeffs_list[ref][i].available = 1;
    coeffs_list[ref][i].A = A;
    coeffs_list[ref][i].B = B;
    coeffs_list[ref][i].C = C;
    coeffs_list[ref][i].D = D;
    coeffs_list[ref][i].E = E;
    coeffs_list[ref][i].F = F;
}

void usage(char *app)
{
    fprintf(stdout, "Usage: %s db_name coefficients_db min_freq nodename\n", app);
    exit(1);
}

int main(int argc, char *argv[])
{
    application_t read_app;
    double power, cpi, tpi;
    uint filtered_apps = 0;
    uint f, pos, ref, i;
    char coef_file[256];
    int fd, index;

    if (argc < 5) {
        usage(argv[0]);
    }

    //
    min_freq = (uint) atoi(argv[3]);
    nodename = argv[4];

    // We get how many samples per frequency we have
    num_node_p_states = fill_list_p_states();

    // samples_f is to compute how many samples per frequency we have
    MALLOC(samples_f, uint, num_node_p_states);
    MALLOC(current, uint, num_node_p_states);

    for (i = 0; i < num_node_p_states; i++) {
        samples_f[i] = 0;
        current[i] = 0;
    }

    //TODO: NEW, using CSVS
    application_t *apps;

    num_apps = read_application_text_file(argv[1], &apps);
    MALLOC(app_list, application_t, num_apps);
    MALLOC(samples_per_app, uint, num_apps);
   
    for (i = 0; i < num_apps; i++) {
	print_application(&apps[i]);
        samples_per_app[i] = 0;
    }
 
    for (i = 0; i < num_apps; i++)
    {
        if (apps[i].def_f >= min_freq) {

            if ((index = app_exists(app_list, filtered_apps, &apps[i])) >= 0) {
                // If APP exists, then accumulate its values in
                accum_app(&app_list[index], &apps[i]);
                samples_per_app[index]++;
            } else {
                write_app(&app_list[filtered_apps], &apps[i]);
                samples_per_app[filtered_apps] = 1;
                filtered_apps++;
            }
        }
    }
    //TODO: END NEW

    // We will consider only applictions with f >= min_freq
    num_apps = filtered_apps;

    // We must compute the average per (app,f)
    for (i = 0; i < num_apps; i++) {
        average_list_samples(&app_list[i], samples_per_app[i]);
    }

    fprintf(stdout, "%s: %u total P_STATES (1: %u KHz), readed %d applications with f >= %u\n",
            nodename, num_node_p_states, nom_freq, num_apps, min_freq);

    // We maintain the name's of applications to generate graphs
    for (current_app = 0; current_app < num_apps; current_app++) {
        if (app_list[current_app].def_f >= min_freq) {
            index = freq_to_p_state(app_list[current_app].def_f);
            samples_f[index]++;
        }
    }

    // We group applications with frequencies
    MALLOC(sorted_app_list, application_t *, num_node_p_states);

    for (i = 0; i < num_node_p_states; i++) {
        MALLOC(sorted_app_list[i], application_t, samples_f[i]);
    }

    // Sorting applications by frequency
    for (current_app = 0; current_app < num_apps; current_app++) {
        f = app_list[current_app].def_f;

        if (f >= min_freq) {
            i = freq_to_p_state(f);
            pos = current[i];

            write_app(&sorted_app_list[i][pos], &app_list[current_app]);
            current[i]++;
        }
    }

    // Computing coefficients
    MALLOC(coeffs_list, coefficient_t *, num_node_p_states);

    for (f = 0; f < num_node_p_states; f++) {
        MALLOC(coeffs_list[f], coefficient_t, num_node_p_states);

        for (i = 0; i < num_node_p_states; i++) {
            coeffs_list[f][i].available = 0;
        }
    }

    init_list_coeffs(0, 0, nom_freq, 1, 0, 0, 1, 0, 0);
    double A, B, C, D, E, F;

    // We compute regression
    for (ref = 0; ref < num_node_p_states; ref++)
    {
        if (samples_f[ref])
        {
            sprintf(coef_file, "%s.%u", argv[2], p_state_to_freq(ref));
            OPEN(fd, coef_file, O_WRONLY | O_CREAT | O_TRUNC, CREATE_FLAGS);

            for (f = 0; f < num_node_p_states; f++) // Coefficients per frequency
            {
                if (ref == f)
                {
                    init_list_coeffs(ref, f, p_state_to_freq(f), 1, 0, 0, 1, 0, 0);
                }
                else
                {
                    int n = samples_f[f];

                    if (n > 0)
                    {
                        #ifdef DEBUG
                        fprintf(stdout, "Computing POWER regression for freq %u with %u samples (REF=%u)\n",
                                p_state_to_freq(f), samples_f[f], p_state_to_freq(ref));
                        #endif
                        gsl_matrix *SIGNATURE_POWER = gsl_matrix_calloc(n, 3);
                        gsl_vector *POWER = gsl_vector_alloc(n);
                        gsl_vector *COEFFS = gsl_vector_alloc(3);

                        for (i = 0; i < n; i++) {
                            // POWER
                            gsl_vector_set(POWER, i, sorted_app_list[f][i].DC_power);

                            nominal_for_power(ref, sorted_app_list[f][i].app_id, &power, &tpi);

                            // SIGNATURE VALUES
                            gsl_matrix_set(SIGNATURE_POWER, i, 0, 1);
                            gsl_matrix_set(SIGNATURE_POWER, i, 1, power);
                            gsl_matrix_set(SIGNATURE_POWER, i, 2, tpi);
                        }

                        double chisq;
                        gsl_matrix *cov = gsl_matrix_alloc(3, 3);
                        gsl_multifit_linear_workspace *wspc = gsl_multifit_linear_alloc(n, 3);
                        gsl_multifit_linear(SIGNATURE_POWER, POWER, COEFFS, cov, &chisq, wspc);

                        #ifdef DEBUG
                        fprintf(stdout, "Coefficient for power: %g*DC_power + %g*TPI_f0 + %g\n",
                                gsl_vector_get(COEFFS, 1), gsl_vector_get(COEFFS, 2), gsl_vector_get(COEFFS, 0));
                        #endif
                        A = gsl_vector_get(COEFFS, 1);
                        B = gsl_vector_get(COEFFS, 2);
                        C = gsl_vector_get(COEFFS, 0);
                        gsl_matrix_free(SIGNATURE_POWER);
                        gsl_matrix_free(cov);
                        gsl_vector_free(POWER);
                        gsl_vector_free(COEFFS);
                        gsl_multifit_linear_free(wspc);
                    }
                    if (n > 0)
                    {
                        #ifdef DEBUG
                        fprintf(stdout, "Computing CPI regression for freq %u with %u samples (REF=%u)\n",
                                p_state_to_freq(f), samples_f[f], p_state_to_freq(ref));
                        #endif
                        gsl_matrix *SIGNATURE_CPI = gsl_matrix_calloc(n, 3);
                        gsl_vector *CPI = gsl_vector_alloc(n);
                        gsl_vector *COEFFS = gsl_vector_alloc(3);

                        for (i = 0; i < n; i++)
                        {
                            // CPI
                            gsl_vector_set(CPI, i, sorted_app_list[f][i].CPI);

                            nominal_for_cpi(ref, sorted_app_list[f][i].app_id, &cpi, &tpi);

                            // SIGNATURE VALUES
                            gsl_matrix_set(SIGNATURE_CPI, i, 0, 1);
                            gsl_matrix_set(SIGNATURE_CPI, i, 1, cpi);
                            gsl_matrix_set(SIGNATURE_CPI, i, 2, tpi);
                        }

                        double chisq;
                        gsl_matrix *cov = gsl_matrix_alloc(3, 3);
                        gsl_multifit_linear_workspace *wspc = gsl_multifit_linear_alloc(n, 3);
                        gsl_multifit_linear(SIGNATURE_CPI, CPI, COEFFS, cov, &chisq, wspc);

                        #ifdef DEBUG
                        fprintf(stdout, "Coefficient for cpi: %g*CPI_f0 + %g*TPI_f0 + %g\n",
                                gsl_vector_get(COEFFS, 1), gsl_vector_get(COEFFS, 2), gsl_vector_get(COEFFS, 0));
                        #endif
                        D = gsl_vector_get(COEFFS, 1);
                        E = gsl_vector_get(COEFFS, 2);
                        F = gsl_vector_get(COEFFS, 0);

                        gsl_matrix_free(SIGNATURE_CPI);
                        gsl_matrix_free(cov);
                        gsl_vector_free(CPI);
                        gsl_vector_free(COEFFS);
                        gsl_multifit_linear_free(wspc);

                        init_list_coeffs(ref, f, p_state_to_freq(f), A, B, C, D, E, F);
                    }
                }
            }

            if (write(fd, coeffs_list[ref], sizeof(coefficient_t) * num_node_p_states) !=
                (sizeof(coefficient_t) * num_node_p_states)) {
                perror("Error writting coefficients file\n");
                exit(1);
            }

            close(fd);
        }
    }

    fprintf(stdout, "%s: computed coefficients\n", nodename);
    return 0;
}
