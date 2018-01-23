#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ear_dynais/ear_dynais.h>
#include <string_enhanced.h>
#include <config.h>

// Please, define METRICS in <ear_dynais.h>
int EAR_VERBOSE_LEVEL = 0;
#define N_LOOPS 256

struct loop
{
    ulong value;
    int level;
    int size;
};

struct loop loops[N_LOOPS];
int loops_index = -1;

//
int loop_index(ulong value, int size, int level)
{
    int i;

    for (i = 0; i < N_LOOPS; ++i)
    {
        //
        if (i > loops_index)
        {
            loops_index += 1;
            loops[i].value = value;
            loops[i].level = level;
            loops[i].size = size;
            return i;
        }
        else if (loops[i].value == value &&
                 loops[i].level == level &&
                 loops[i].size == size)
        {
            return i;
        }
    }

    return -1;
}

void usage()
{
    printf("Usage: dynais_input file size_windows n_levels\n");
    printf("- file: path to the input file\n");
    printf("- size_windows: size of each level window\n");
    printf("- n_levels: number of levels\n");
    exit(1);
}

int main(int argc,char *argv[])
{
    unsigned long value[4];
    unsigned int last_loop_level;
    unsigned int level, size;
    int fd, status, index;
    int *iters;

    if (argc != 4) {
        usage();
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("File read failed.");
        exit(1);
    }

    //
    iters = malloc(atoi(argv[3]) * sizeof(int));
    dynais_init(atoi(argv[2]), atoi(argv[3]));
    last_loop_level = 0;

    // Header
    print_spacing_digits(8);
    print_spacing_string("TAG");
    print_spacing_string("LEVEL");
    print_spacing_string("SIZE");
    print_spacing_string("ITERS");
    print_spacing_string("STATUS");
    printf("\n");

    // Reading
    while(read(fd, &value, sizeof(value)) > 0)
    {
        //
        status = dynais(value[3], &size, &level);

        //
        if (status == NEW_ITERATION) {
            iters[level] += 1;
        }

        //
        if (status == END_LOOP)
        {
            print_spacing_string("-");
            print_spacing_int(level);
            print_spacing_string("-");
            print_spacing_int(iters[last_loop_level]);
            printf("END LOOP\n");
        }

        //
        if (status == END_NEW_LOOP)
        {
            index = loop_index(value[3], size, level);
            print_spacing_int(index);
            print_spacing_int(level);
            print_spacing_int(size);
            print_spacing_int(iters[last_loop_level]);
            printf("END NEW LOOP\n");
        }

        //
        if (status == NEW_LOOP)
        {
            index = loop_index(value[3], size, level);
            print_spacing_int(index);
            print_spacing_int(level);
            print_spacing_int(size);
            print_spacing_string("-");
            printf("NEW LOOP\n");
        }

        //
        if (status >= NEW_LOOP)
        {
            last_loop_level = level;
            iters[level] = 3;
        }
    }

    //
    dynais_dispose();

    #if METRICS
    dynais_metrics_print();
    #endif

    close(fd);

    return 0;
}
