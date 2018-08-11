#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <common/states.h>
#include <database_cache/eardbd_api.h>

void usage(int argc, char **argv)
{
	if (argc != 2 && argc != 3)
	{
		printf("usage: %s ip.main ip.mirror\n", argv[0]);
		exit(1);
	}
}

/*
 *
 */

int main(int argc, char **argv)
{
	cluster_conf_t conf_clus;
	periodic_metric_t met;
	application_t app;
	int job_id = 0;
	state_t s;

	//usage(argc, argv);

	// Dummy objects
	memset(&app, 0, sizeof (application_t));
	memset(&met, 0, sizeof (periodic_metric_t));

	app.is_mpi = 1;
	app.power_sig.DC_power = 100;
	app.power_sig.DRAM_power = 200;
	app.power_sig.PCK_power = 300;
	app.power_sig.DC_power = 100;
	gethostname(app.node_id, 128);
	report_application_data(&app);

	// Testing API
	s = eardbd_connect(&conf_clus);

	printf("CONNECT RETURNED %d\n", s);

	while(1) {
		s = eardbd_send_application(&app);
		printf("SEND RETURNED %d\n", s);
		sleep(4);
		job_id += 1;
		app.job.id = job_id;
	}

	eardbd_disconnect();

	return 0;
}
