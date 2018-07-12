#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <database_cache/eardbd.h>
#include <database_cache/eardbd_api.h>
#include <common/types/periodic_metric.h>
#include <common/types/application.h>
#include <common/states.h>

void usage(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: %s IP/Hostname\n", argv[0]);
		exit(1);
	}
}

/*
 *
 */

int main(int argc, char **argv)
{
	periodic_metric_t met;
	application_t app;
	int job_id = 0;
	state_t s;

	usage(argc, argv);

	// Dummy objects
	memset(&app, 0, sizeof (application_t));
	memset(&met, 0, sizeof (periodic_metric_t));

	app.is_mpi = 1;
	gethostname(app.node_id, 128);

	// Testing API
	eardbd_connect(argv[1], 4711, TCP);
	
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
