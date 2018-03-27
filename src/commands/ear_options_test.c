#include <stdio.h>     
#include <stdlib.h>    
#include <getopt.h>

#define DEF_FREQ 2100000
#define DEF_JOB 0

void usage(char *app)
{
	printf("Usage: %s [--freq=frequency] [--job_id=job_id]\n",app);
	printf("	Sets the CPU frequency of all the nodes where the job_id is running\n");
	printf("	* If frequency is not specified, default frequency is used\n");
	printf("	* If job_id is not specified, frequency will be set in all the nodes\n");
	exit(1);
}
int main(int argc, char **argv)
{
    int c;
    int digit_optind = 0;
	ulong def_f=DEF_FREQ;
	ulong def_j=DEF_JOB;

	if (argc==1) usage(argv[0]);

   while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"freq",    optional_argument, 0,  0 },
            {"job_id",  optional_argument, 0,  1 },
        };

       c = getopt_long(argc, argv, "",
                 long_options, &option_index);
        if (c == -1)
            break;

       switch (c) {
        case 0:
            if (optarg) def_f=(ulong)atol(optarg);
            break;
       case 1:
            if (optarg) def_j=(ulong)atol(optarg);
            break;
       case '?':
            break;

       default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

   if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }
	if (def_j==DEF_JOB)  	printf("Setting CPU frequency of all the nodes at %lu\n",def_f);
	else 			printf("Setting CPU frequency of nodes running job %lu at %lu\n",def_j,def_f);

   exit(EXIT_SUCCESS);
}

