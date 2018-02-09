#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <types/application.h>
#include <types/projection.h>
#include <string_enhanced.h>

typedef struct control
{
    application_t *apps;
    application_t *apps_merged;
    coefficient_t *coeffs;
    projection_t *projs;
    int n_apps;
    int n_apps_merged;
    int n_coeffs;
    uint f0_mhz;
    uint mode;
    char *csv;
} control_t;

// gcc -I ../common/ -o coeffs_quality_control coeffs_quality_control.c string_enhanced.o types.o
// gcc -I ../common/ -c ../common/types.c

double power_proj(double power0, double tpi0, coefficient_t *coeffs)
{
    return coeffs->A * power0 + coeffs->B * tpi0 + coeffs->C;
}

double cpi_proj(double cpi0, double tpi0, coefficient_t *coeffs)
{
    return coeffs->D * cpi0 + coeffs->E * tpi0 + coeffs->F;
}

double time_proj(double time0, double cpip, double cpi0, ulong f0, ulong fn)
{
    return (time0 * cpip / cpi0) * ((double) f0 / (double) fn);
}

int find(application_t *apps, int n_apps, char *app_id, uint f0_mhz)
{
    int equal_id = 0;
    int equal_f = 0;
    int i;

    for(i = 0; i < n_apps; ++i)
    {
        equal_id = strcmp(app_id, apps[i].app_id) == 0;
        equal_f = f0_mhz == apps[i].def_f;

        if (equal_id && equal_f) {
            return i;
        }
    }

    return -1;
}

application_t *merge(control_t *control)
{
    application_t *apps, *apps_merged;
    int n_apps, equal_id, equal_f, i, j, k;
    double cpi, tpi, power, time, counter;
    char *app_id;
    uint f0_mhz;

    // Allocation
    apps = control->apps;
    n_apps = control->n_apps;

    // Allocation
    apps_merged = (application_t *) malloc(n_apps * sizeof(application_t));
    memset(apps_merged, 0, sizeof(application_t));

    // Initialization
    for(i = 0; i < n_apps; ++i)
    {
        apps_merged[i].app_id[0] = '\0';
    }

    //
    for(i = 0, j = 0; i < n_apps; ++i)
    {
        f0_mhz = apps[i].def_f;
        app_id = apps[i].app_id;

        if (find(apps_merged, j, app_id, f0_mhz) == -1)
        {
            memcpy(&apps_merged[j], &apps[i], sizeof(application_t));

            power = apps[i].DC_power;
            time = apps[i].time;
            tpi = apps[i].TPI;
            cpi = apps[i].CPI;
            counter = 1.0;

            for(k = i + 1; k < n_apps; ++k)
            {
                equal_id = strcmp(app_id, apps[k].app_id) == 0;
                equal_f = f0_mhz == apps[k].def_f;

                if (equal_id && equal_f)
                {
                    power += apps[k].DC_power;
                    time += apps[k].time;
                    tpi += apps[k].TPI;
                    cpi += apps[k].CPI;
                    counter += 1.0;
                }
            }

            apps_merged[j].DC_power = power / counter;
            apps_merged[j].time = time / counter;
            apps_merged[j].TPI = tpi / counter;
            apps_merged[j].CPI = cpi / counter;

            j += 1;
        }
    }

    control->apps_merged = apps_merged;
    control->n_apps_merged = j;

    return apps_merged;
}

void evaluate(control_t *control)
{
    static char *HEADER = "application;time_real;time_proj;time_error;" \
                          "power_real;power_proj;power_error;" \
                          "cpi_real:cpi_proj:cpi_error";
    char buffer[32];
    double cpi0, tpi0, time0, power0;
    double cpip, tpip, timep, powerp;
    double cpie, tpie, timee, powere;
    application_t *apps, *m;
    coefficient_t *coeffs;
    int n_apps, n_coeffs;
    int i, j, k;
    uint f0_mhz;
    int fd = -1;

    // Output
    if (control->csv != NULL)
    {
        #define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
        #define OPTIONS O_WRONLY | O_CREAT | O_TRUNC

        fd = open(control->csv, OPTIONS, PERMISSION);

        if (fd >= 0) {
            dprintf(fd, "%s\n", HEADER);
        }
    }

    // Initializations
    buffer[0] = '\0';
    f0_mhz = control->f0_mhz;
    coeffs = control->coeffs;
    apps = control->apps_merged;
    n_coeffs = control->n_coeffs;
    n_apps = control->n_apps_merged;

    // Apps iteration
    for (j = 0; j < n_apps; ++j)
    {
        if (apps[j].def_f == f0_mhz)
        {
            // Print content
            set_spacing_digits(18);
            print_spacing_string(apps[j].app_id);

            set_spacing_digits(11);
            printf("@");
            print_spacing_uint(apps[j].def_f);

            set_spacing_digits(12);
            printf("| ");
            print_spacing_string("T. Real");
            print_spacing_string("T. Proj");
            print_spacing_string("T. Err");
            printf("| ");
            print_spacing_string("P. Real");
            print_spacing_string("P. Proj");
            print_spacing_string("P. Err");

            printf("\n");

            cpi0 = apps[j].CPI;
            tpi0 = apps[j].TPI;
            time0 = apps[j].time;
            power0 = apps[j].DC_power;

            for (i = 0; i < n_coeffs; i++)
            {
                if (coeffs[i].available)
                {
                    cpip = cpi_proj(cpi0, tpi0, &coeffs[i]);
                    timep = time_proj(time0, cpip, cpi0, f0_mhz, coeffs[i].pstate);
                    powerp = power_proj(power0, tpi0, &coeffs[i]);

                    //
                    k = find(apps, n_apps, apps[j].app_id, coeffs[i].pstate);

                    // Print content
                    set_spacing_digits(18);
                    print_spacing_string("->");

                    set_spacing_digits(12);
                    print_spacing_ulong(coeffs[i].pstate);

                    if (k != -1)
                    {
                        m = &apps[k];

                        timee = fabs((1.0 - (m->time / timep)) * 100.0);
                        powere = fabs((1.0 - (m->DC_power / powerp)) * 100.0);
                        cpie = fabs((1.0 - (m->CPI / cpip)) * 100.0);

                        printf("| ");
                        sprintf(buffer, "%0.2lf", m->time);
                        print_spacing_string(buffer);

                        sprintf(buffer, "%0.2lf", timep);
                        print_spacing_string(buffer);

                        sprintf(buffer, "%0.2lf", timee);
                        print_spacing_string(buffer);

                        printf("| ");
                        sprintf(buffer, "%0.2lf", m->DC_power);
                        print_spacing_string(buffer);

                        sprintf(buffer, "%0.2lf", powerp);
                        print_spacing_string(buffer);

                        sprintf(buffer, "%0.2lf", powere);
                        print_spacing_string(buffer);
                    } else {
                        printf("| ");
                        print_spacing_string("-");
                        print_spacing_string("-");
                        print_spacing_string("-");

                        printf("| ");
                        print_spacing_string("-");
                        print_spacing_string("-");
                        print_spacing_string("-");
                    }

                    if (fd >= 0) {
                        dprintf(fd, "%s;%u;%u;", apps[j].app_id, apps[j].def_f, m->def_f);
                        dprintf(fd, "%lf;%lf;%lf;", m->time, timep, timee);
                        dprintf(fd, "%lf;%lf;%lf;", m->DC_power, powerp, powere);
                        dprintf(fd, "%lf;%lf;%lf\n", m->CPI, cpip, cpie);
                    }

                    printf("\n");
                }
            }
        }
    }

    if (fd >= 0) {
        close(fd);
    }
}

void save_applications_merged(control_t *control)
{
    application_t *apps;
    int i;

    if (control->csv == NULL) {
        return;
    }

    for (i = 0; i < control->n_apps_merged; ++i)
    {
        append_application_text_file(control->csv, &control->apps_merged[i]);
    }
}

int append_coefficient_text_file(char *path, coefficient_t *coeff)
{

}

void save_coefficients(control_t *control)
{
    static char *HEADER = "nodename;F_ref;F_n;AVAIL;A;B;C;D;E;F";
    coefficient_t *coeff;
    int fd, ret, i;

    if (control->csv == NULL) {
        return;
    }

    if ((fd = open(control->csv, OPTIONS, PERMISSION)) < 0) {
        return;
    }

    ret = dprintf(fd, "%s\n", HEADER);

    for (i = 0; i < control->n_coeffs; ++i)
    {
        if (control->coeffs[i].available)
        {
            coeff = &control->coeffs[i];

            dprintf(fd, "%lu;%u;%lf;%lf;%lf;%lf;%lf;%lf\n",
                    coeff->pstate, coeff->available,
                    coeff->A, coeff->B, coeff->C,
                    coeff->D, coeff->E, coeff->F);
        }
    }

    close(fd);
}

void usage(char *app)
{
    printf("Usage: %s [evaluate_coefficients] <coefficients file> <summary file> [frequency] <output file>\n", app);
    printf("Usage: %s [read_coefficients] <coefficients file> <output file>\n", app);
    printf("Usage: %s [merge_summary] <summary file> <output file>\n", app);
    exit(1);
}

int main(int argc, char *argv[])
{
    control_t control;

    // Mode
    control.mode  = 1 * (strcmp(argv[1], "evaluate_coefficients") == 0);
    control.mode += 2 * (strcmp(argv[1], "read_coefficients") == 0);
    control.mode += 3 * (strcmp(argv[1], "merge_summary") == 0);

    //
    if (control.mode == 1 && (argc == 5 || argc == 6))
    {
        //
        control.n_apps = read_application_text_file(argv[3], &control.apps);
        control.n_coeffs = read_coefficients_file(argv[2], &control.coeffs, 0);
        control.f0_mhz = (unsigned long) atoi(argv[4]);
        control.csv = argv[5];

        //
        control.apps_merged = merge(&control);

        //
        evaluate(&control);

        //
        free(control.coeffs);
        free(control.apps_merged);
        free(control.apps);
    }
    //
    else if (control.mode == 2 && (argc == 3 || argc == 4))
    {
        //
        control.n_coeffs = read_coefficients_file(argv[2], &control.coeffs, 0);
        control.csv = argv[3];

        save_coefficients(&control);

        //
        free(control.coeffs);
    }
    //
    else if (control.mode == 3 && (argc == 3 || argc == 4))
    {
        //
        control.n_apps = read_application_text_file(argv[2], &control.apps);
        control.csv = argv[3];

        //
        control.apps_merged = merge(&control);

        //
        save_applications_merged(&control);

        //
        free(control.apps_merged);
        free(control.apps);
    }
    else
    {
        usage(argv[0]);
    }

    if (control.csv != NULL) {
        printf("···\n");
        printf("*Output: %s\n", control.csv);
    }

    return 0;
}
