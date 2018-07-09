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
 gcc -I/home/xgomez/Desktop/git/EAR/src \
 -DUSE_TURBO=1 -DPERFORMANCE_GAIN=1 -DEAR_MIN_P_STATE=1 -DPOWERMON_FREQ=1 -DDAEMON_PORT_NUMBER=1 \
 -o client_api_test client_api_test.c ../eardbd_api.o ../sockets.o
 */

int main(int argc, char **argv)
{
	periodic_metric_t met;
	application_t app;
	state_t s;

	usage(argc, argv);

	// Dummy objects
	memset(&app, 0, sizeof (application_t));
	memset(&met, 0, sizeof (periodic_metric_t));

	app.is_mpi = 1;
	sprintf(app.node_id, "cae2306");
	sprintf(met.node_id, "cae2306");

	// Testing API
	s = eardbd_connect(argv[1], argv[1], 4711, UDP);
	printf("con = %d (%s)\n", s, state_error);
	s = eardbd_send_application(&app);
	printf("sen = %d (%s)\n", s, state_error);
	s = eardbd_send_periodic_metric(&met);
	printf("sen = %d (%s)\n", s, state_error);
	//s = eardbd_disconnect();
	//printf("dis = %d (%s)\n", s, state_error);

	return 0;
}