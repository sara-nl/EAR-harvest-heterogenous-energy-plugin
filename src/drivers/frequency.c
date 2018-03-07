//
// Created by xgomez on 7/03/18.
//
uint p_states;
uint num_cpus;

static int check_all_cpus_presence()
{
	int i;

	for (i = 0; i < num_cpus; i++)
	{
		
		status = cpufreq_cpu_exists(i);
		ear_cpufreq[i] = 0;

		if (status == 0) {
			ear_cpufreq[i] = cpufreq_get(i);
			ear_verbose(4, "EAR: Curent cpu frequency for cpu %u is %u \n",i,ear_cpufreq[i]);
		}
	}
}

int frequency_init()
{
	struct cpufreq_available_frequencies *list_f, *first;
	int status, i;

	// TODO: metrics dependancy, remove and pass the number of cpus
	hwinfo = metrics_get_hw_info();

	if (hwinfo == NULL) {
		VERBOSE_N(0, "PAPI hardware scanning returned NULL, exiting");
		exit(1);
	}

	num_cpus = hwinfo->sockets * hwinfo->cores * hwinfo->threads;
	p_states = (uint *) malloc(sizeof(uint) * num_cpus);

	if (p_states == NULL) {
		VERBOSE_N(0, "ERROR while allocating memory, exiting");
		exit(1);
	}

	// We check all the cpus are online, we should detect cores but
	// we start with this approach

}