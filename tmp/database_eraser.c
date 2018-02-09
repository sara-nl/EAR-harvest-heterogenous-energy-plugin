#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string_enhanced.h>
#include <types/application.h>

// gcc -I ../common/ -o coeffs_quality_control coeffs_quality_control.c string_enhanced.o types.o
// gcc -I ../common/ -c ../common/types.c

void show_apps_num(application_t *apps, int n_apps)
{
    application_t *app;
    int i;

    for (i = 0; i < n_apps; ++i)
    {
        app = &apps[i];
        printf("%d %s %lf\n", i, app->app_id, app->time);
    }
}

void save_except_app_num(application_t *apps, int n_apps, int except, char *path)
{
    application_t *app;
    int i;

    for (i = 0; i < n_apps; ++i)
    {
        app = &apps[i];

        if (i != except) {
            append_application_binary_file(path, app);
        }
    }
}

void usage(char *app)
{
    printf("Usage: %s <database file>\n", app);
    printf("Usage: %s <database file> [number] <output file>\n", app);
    exit(1);
}

int main(int argc, char *argv[])
{
    application_t *apps;
    int n_apps;

    n_apps = read_application_binary_file(argv[1], &apps);

    if (argc == 2) {
        show_apps_num(apps, n_apps);
    } else if (argc == 4) {
        save_except_app_num(apps, n_apps, atoi(argv[2]), argv[3]);
    }

    free(apps);

    return 0;
}