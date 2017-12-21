#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <papi.h>
#include <config.h>
#include <hardware.h>
#include <ear_verbose.h>
#include <intel_model_list.h>

#define MACC(core, set, event) flops_accum[core * 8 + set * 4 + event]
#define MOBT(core, set, event) flops_obtnd[core * 8 + set * 4 + event]
#define MOPT(core, set) options_cpu[core * 2 + set]
#define MSET(core, set) events_sets[core * 2 + set]

#define FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N       "FP_ARITH:SCALAR_DOUBLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N  "FP_ARITH:128B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N  "FP_ARITH:256B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N  "FP_ARITH:512B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_PACKED_SINGLE_N       "FP_ARITH:SCALAR_SINGLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N  "FP_ARITH:128B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N  "FP_ARITH:256B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N  "FP_ARITH:512B_PACKED_SINGLE"
#define EVENTS_X_SET                                4
#define EVENTS_SETS                                 2
#define SP_OPS                                      0
#define DP_OPS                                      1

int EAR_VERBOSE_LEVEL = 4;

PAPI_option_t *options_cpu;
int inst_iters[EVENTS_SETS][EVENTS_X_SET] = {{10000000, 1, 1, 1}, {10000000, 1, 1, 1}};
int inst_weights[EVENTS_SETS][EVENTS_X_SET] = {{1, 4, 8, 16}, {1, 2, 4, 8}};
long long *flops_accum;
long long *flops_obtnd;
int *events_sets;
int spinning = 0;
int _n_cores;

void check_added_event(int event_set, char *event)
{
    int ret = PAPI_add_named_event(event_set, event);

    if (ret != PAPI_OK) {
        ear_verbose(0, "Added_PAPI named_event %s (%s)\n", event, PAPI_strerror(ret));
    }
}

void init_flops_metrics(int n_cores)
{
    int core, set, event, retval;
    int events_cid;

    flops_accum = malloc(EVENTS_SETS * EVENTS_X_SET * n_cores * sizeof(long long));
    flops_obtnd = malloc(EVENTS_SETS * EVENTS_X_SET * n_cores * sizeof(long long));
    options_cpu = malloc(EVENTS_SETS * n_cores * sizeof(PAPI_option_t));
    events_sets = malloc(EVENTS_SETS * n_cores * sizeof(int));
    _n_cores = n_cores;

    retval = PAPI_library_init(PAPI_VER_CURRENT);

    if (retval != PAPI_VER_CURRENT)
    {
        ear_verbose(0, "%s ERROR: unable to initialize PAPI (%s)\n",
                    __FILE__, PAPI_strerror(retval));
        exit(1);
    }

    // Here , papi is initialized
    events_cid = PAPI_get_component_index("perf_event");

    if (events_cid < 0)
    {
        ear_verbose(0, "%s ERROR: perf_event component not found (%s)\n",
                    __FILE__, PAPI_strerror(events_cid));
        exit(1);
    }

    for (core = 0; core < n_cores; core++)
    {
        for (set = 0; set < EVENTS_SETS; set++)
        {
            /* Event values set to 0 */
            for (event = 0; event < EVENTS_X_SET; event++) {
                MACC(core, set, event) = 0;
                MOBT(core, set, event) = 0;
            }

            /* Init event sets */
            MSET(core, set) = PAPI_NULL;

            retval = PAPI_create_eventset(&MSET(core, set));

            if (retval != PAPI_OK) {
                ear_verbose(0, "%s ERROR: Creating %d eventset (%s)\n",
                            __FILE__, set, PAPI_strerror(retval));
                exit(1);
            }

            retval = PAPI_assign_eventset_component(MSET(core, set), events_cid);

            if (retval != PAPI_OK) {
                ear_verbose(0, "%s ERROR: PAPI_assign_eventset_component (%s)\n",
                            __FILE__, PAPI_strerror(retval));
                exit(1);
            }

            //MOPT(core, set).cpu.eventset = MSET(core, set);
            //MOPT(core, set).cpu.cpu_num = core;
            MOPT(core, set).cpu.eventset = MSET(core, set);
            MOPT(core, set).attach.tid=getpid();

            retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t *) &MOPT(core, set));

            if (retval != PAPI_OK) {
                ear_verbose(0, "%s ERROR: %s\n", __FILE__, PAPI_strerror(retval));
            }

            retval = PAPI_set_multiplex(MSET(core, set));

            if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(MSET(core, set)) > 0)) {
                ear_verbose(0, "Event set to compute FLOPS already has multiplexing enabled\n");
            } else if (retval != PAPI_OK) {
                ear_verbose(0, "(%s) ERROR: event set can not be multiplexed (%s)\n",
                            __FILE__, PAPI_strerror(retval));
                exit(1);
            }

            switch (set) {
                case SP_OPS:
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_PACKED_SINGLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N);
                    break;
                case DP_OPS:
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N);
                    check_added_event(MSET(core, set), FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N);
                    break;
            }
        }
    }
}

void reset_flops_metrics()
{
    int core, set, event, retval;

    for (core = 0; core < _n_cores; core++)
    {
        for (set = 0; set < EVENTS_SETS; set++)
        {
            if ((retval = PAPI_reset(MSET(core, set))) != PAPI_OK)
            {
                ear_verbose(0, "(%s) ERROR: while resetting (%s)\n", __FILE__, PAPI_strerror(retval));
            }

            for (event = 0; event < EVENTS_X_SET; event++)
            {
                MOBT(core, set, event) = 0;
            }
        }
    }
}

void start_flops_metrics()
{
    int core, set, retval;

    for (core = 0; core < _n_cores; core++)
    {
        for (set = 0; set < EVENTS_SETS; set++)
        {
            if ((retval = PAPI_start(MSET(core, set))) != PAPI_OK)
            {
                ear_verbose(0, "(%s) ERROR: while starting (%s)\n", __FILE__, PAPI_strerror(retval));
            }
        }
    }
}

/* Stops includes accumulate metrics */
void stop_flops_metrics()
{
    int core, set, event, retval;

    for (core = 0; core < _n_cores; core++)
    {
        for (set = 0; set < EVENTS_SETS; set++)
        {
            if ((retval = PAPI_stop(MSET(core, set), (long long *) &MOBT(core, set, 0))) != PAPI_OK)
            {
                ear_verbose(0, "(%s) ERROR: while stopping (%s)\n", __FILE__, PAPI_strerror(retval));
            }
            else
            {
                for (event = 0; event < EVENTS_X_SET; event++)
                {
                    MACC(core, set, event) += MOBT(core, set, event);
                }
            }
        }
    }
}

void print_gflops(unsigned long total_time_us)
{
    long long inst_n, inst_d, inst_w;
    int core, set, event;
    double gflops;
    float inst_e;

    // Header
    ear_verbose(0, "Core   PS inst.    PS err.  SP inst.    SP err.  GFLOPS\n");

    // For every core
    for (core = 0; core < _n_cores; core++)
    {
        print_spacing_digits(7);
        print_spacing_int(core);
        inst_w = 0;

        // PACKED_SINGLE and SCALAR_DOUBLE
        for (set = 0; set < EVENTS_SETS; set++)
        {
            inst_n = MACC(core, set, 0);
            inst_d = llabs(inst_n - inst_iters[set][0]);
            inst_e = (float) inst_d / (float) inst_n;
            inst_w = inst_w + (inst_weights[set][0] * inst_n);

            print_spacing_digits(12);
            print_spacing_ull(inst_n);
            if (isinf(inst_e)) inst_e = 0;
            ear_verbose(0, "%0.3f    ", inst_e);
        }

        // GFLOPS
        gflops = (double) inst_w / (double) (total_time_us * 1000);
        ear_verbose(0, "%0.3lf", gflops);
        ear_verbose(0, "\n");
    }
}

static void set_affinity(int core_bind)
{
    cpu_set_t set;
    CPU_SET(core_bind, &set);
    sched_setaffinity(0, sizeof(cpu_set_t), &set);
}

static void compute()
{
    double double_inst = 0.0;
    float single_inst = 0.0;
    int i;

    // PACKED_SINGLE
    for(i = 0; i < inst_iters[0][0]; i++) {
        single_inst = single_inst + 1.0;
    }
    // SCALAR_DOUBLE
    for(i = 0; i < inst_iters[1][0]; i++) {
        double_inst = double_inst + 1.0;
    }
}

static void *threaded_compute(void *arg)
{
    int core_bind = *((int *) arg);

    set_affinity(core_bind);
    while(spinning == 0);
    compute();

    pthread_exit(0);
}

int main (int argc, char *argv[])
{
    int *tbinds;
    pthread_t *tids;
    long long start_time;
    long long exec_time;
    long long flops = 0;
    int this_core;
    int n_cores;
    int status;
    int i;

    // This thread is set to core 0
    set_affinity(0);
    n_cores = get_nprocs();
    this_core = sched_getcpu();

    // Reserving memory
    tbinds = malloc(sizeof(pid_t) * n_cores);
    tids = malloc(sizeof(pthread_t) * n_cores);

    printf("CPU Model: \t\t%i\n", 85);
    printf("Running in core: \t%d/%d\n", this_core, n_cores);
    printf("-------------------------- Working\n");

    // Creating the threads
    for (i = 1; i < n_cores; ++i)
    {
        tbinds[i] = i;
        pthread_create(&tids[i], NULL, threaded_compute, (void *) &tbinds[i]);
    }

    init_flops_metrics(n_cores);
    reset_flops_metrics();
    start_flops_metrics();
    spinning = 1;

    start_time = PAPI_get_real_usec();
    compute();
    exec_time = (PAPI_get_real_usec() - start_time);

    for (i = 1; i < n_cores; i++) {
        pthread_join(tids[i], NULL);
    }
    stop_flops_metrics(&flops);

    printf("-------------------------- Summary\n");
    print_gflops(exec_time);

    return 0;
}
