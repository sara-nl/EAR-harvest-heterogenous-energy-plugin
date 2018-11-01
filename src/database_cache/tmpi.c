#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/sockets.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <database_cache/eardbd.h>


char buffer[4096];
	
int main(int argc, char *argv[])
{
	MPI_Comm comm;
	cluster_conf_t conf_clus;
	my_node_conf_t *conf_node;
	application_t *app;
	state_t s;
	int nodes;
	int rank;
	int i;

	if (argc < 2) {
		printf("No has dicho la palabra mágica, a a a\n");
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//
	app = calloc(1, sizeof(application_t));
	app->job.id = (unsigned long) strtoul(argv[1], 0, 10);
	app->job.step_id = rank;
	sprintf(app->job.app_id, "caca");
	app->is_mpi = 1;
	gethostname(app->node_id, 128);	

	//
	get_ear_conf_path(buffer);

	if (read_cluster_conf(buffer, &conf_clus) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		exit(1);
	}

	//
	conf_node = get_my_node_conf(&conf_clus, "cmp2772");

	//
	s = eardbd_connect(&conf_clus, conf_node);
	if (s != 0) fprintf(stderr, "[%d] eardbd_connect: %d\n", rank, s);

	// 
	MPI_Barrier(MPI_COMM_WORLD);
	s = eardbd_send_application(&app);
	if (s != 0) fprintf(stderr, "[%d] eardbd_send_application: %d\n", rank, s);

	//
	eardbd_disconnect();
	MPI_Finalize();
	free(app);

	return 0;
}
