#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <database_cache/sockets.h>
#include <database_cache/eardbd_api.h>
#include <common/types/application.h>
#include <common/types/periodic_metric.h>

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
	sprintf(app.node_id, "cae2306");

	// Testing API
	eardbd_connect(argv[1], 4712, UDP);
	eardbd_send_application(&app);
	eardbd_disconnect();

	printf("void %lu", sizeof(void));

	return 0;
}
