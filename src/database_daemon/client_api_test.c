#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <database_daemon/eardbd.h>
#include <database_daemon/client_api/eardbd_api.h>
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

	usage(argc, argv);

	// Dummy objects
	memset(&app, 0, sizeof (application_t));
	memset(&met, 0, sizeof (periodic_metric_t));

	app.is_mpi = 1;
	sprintf(app.node_id, "cmp000.lenovo.hpc");

	// Testing API
	eardbd_connect(argv[1], TCP);
	eardbd_send_application(&app);
	eardbd_disconnect();

	return 0;
}