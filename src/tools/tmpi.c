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
	state_t s1;
	state_t s2;
	int nodes;
	int rank;
	int n;
	int i;

	if (argc < 2) {
		printf("No has dicho la palabra mágica, a a a\n");
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	n = atoi(argv[2]);

	//
	app = calloc(1, sizeof(application_t));
	app->job.id = (unsigned long) strtoul(argv[1], 0, 10);
	app->job.step_id = rank;
	sprintf(app->job.app_id, "caca");
	app->is_mpi = 1;
	gethostname(app->node_id, 128);	
	app->is_learning = 0;

	//
	get_ear_conf_path(buffer);

	if (read_cluster_conf(buffer, &conf_clus) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		exit(1);
	}

	//
	conf_node = get_my_node_conf(&conf_clus, "cmp2772");
    conf_clus.db_manager.tcp_port      = 4811;
    conf_clus.db_manager.sec_tcp_port  = 4812;
    conf_clus.db_manager.sync_tcp_port = 4813;
	sprintf(conf_node->db_ip, "login2301");
	sprintf(conf_node->db_sec_ip, "");

	//int slp = rank % 10;
	//sleep(slp);
	
	//
	s1 = eardbd_connect(&conf_clus, conf_node);

	// 
	MPI_Barrier(MPI_COMM_WORLD);

	for (i = 0; i < n; ++i) {
		s2 = eardbd_send_application(app);
	}
	if (s1 != 0 || s2 != 0) fprintf(stderr, "[%d]: %d %d\n", rank, s1, s2);	

	//
	eardbd_disconnect();
	MPI_Finalize();
	free(app);

	return 0;
}
