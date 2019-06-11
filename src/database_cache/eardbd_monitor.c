#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <common/sizes.h>
#include <common/output/verbose.h>
#include <database_cache/eardbd_api.h>

char cmnd[SZ_PATH];
char opt1[SZ_PATH];
char opt2[SZ_PATH];
cluster_conf_t conf_clus;
my_node_conf_t conf_node;
application_t app;

void *send_applications()
{
	application_t app;
	edb_state_t s;
	int i = 100;

	sprintf(app.job.app_id, "monitor-multi");
	gethostname(app.node_id, 128);
	app.is_learning = 0;
	app.job.id = 1000000;
	app.is_mpi = 1;

	for (; i > 0; --i) {
		app.job.step_id += 1;
		s = eardbd_send_application(&app);
		verbose(0, "send app result (%d, %d)", s.server, s.mirror);
	}

	verbose(0, "exitting send_applications()");
	pthread_exit(NULL);
}

void *send_periodics()
{
	periodic_metric_t per;
	edb_state_t s;
	int i = 100;

	gethostname(per.node_id, 128);
	per.job_id = 1000000;

	for (; i > 0; --i) {
		per.step_id += 1;
		s = eardbd_send_periodic_metric(&per);
		verbose(0, "send periodic result (%d, %d)", s.server, s.mirror);
	}

	verbose(0, "exitting send_periodics()");
	pthread_exit(NULL);
}

void *send_events()
{
	ear_event_t eve;
	edb_state_t s;
	int i = 100;

	gethostname(eve.node_id, 128);
	eve.jid = 1000000;

	for (; i > 0; --i)
	{
		eve.jid = 10000;
		eve.step_id += 1;
		eve.event = 0;
		eve.freq = 0;
		eve.timestamp = 0;

		s = eardbd_send_event(&eve);
		verbose(0, "send event result (%d, %d)", s.server, s.mirror);
	}

	verbose(0, "exitting send_events()");
	pthread_exit(NULL);
}

int multithread_test()
{
	pthread_t *ths;
	int num_th;
	int i, j;
	int ret;

	num_th = 1;
	ths = malloc(num_th * sizeof(pthread_t));

	for (i = 0; i < num_th; ++i) {
		ret = pthread_create(&ths[i], NULL, send_periodics, NULL);
		if (ret) error("pthread_create");
		ret = pthread_create(&ths[i + 1], NULL, send_applications, NULL);
		if (ret) error("pthread_create");
		ret = pthread_create(&ths[i + 2], NULL, send_events, NULL);
		if (ret) error("pthread_create");
	}

	verbose(0, "waiting");
	for (i = 0; i < (num_th * 3); i++) {
		pthread_join(ths[i], NULL);
	}
	edb_state_t s;
	verbose(0, "ending");
	return 0;
}

int is(char *buf, char *string)
{
	return strcmp(buf, string) == 0;
}

int main(int argc, char *argv)
{
	char *col = "        ";
	edb_state_t s;
	int i;

	if (get_ear_conf_path(opt1) == EAR_ERROR) {
		error("while getting ear.conf path '%s' '%s' (%d, '%s')", opt1, getenv("EAR_ETC"), errno, strerror(errno));
		return 0;
	}

	read_cluster_conf(opt1, &conf_clus);

	while (1)
	{
		dprintf(verb_channel, "command: ");
		scanf("%s", cmnd);
		//scanf("%s %s %s", cmnd, opt1, opt2);

		if (is(cmnd, "connect")) {
			scanf("%s %s", opt1, opt2);
			if (!is(opt1, "-")) strcpy(conf_node.db_ip, opt1);
			if (!is(opt2, "-")) strcpy(conf_node.db_sec_ip, opt2);
			s = eardbd_connect(&conf_clus, &conf_node);
			verbose(0, "%s connection result (%d, %d)", col, s.server, s.mirror);
		} else if (is(cmnd, "disconnect")) {
			eardbd_disconnect();
		} else if (is(cmnd, "reconnect")) {
			s = eardbd_reconnect(&conf_clus, &conf_node, s);
			verbose(0, "%s reconnection result (%d, %d)", col, s.server, s.mirror);
		} else if (is(cmnd, "ping")) {
			s = eardbd_send_ping();
			verbose(0, "%s ping result (%d, %d)", col, s.server, s.mirror);
		} else if (is(cmnd, "send")) {
			scanf("%d", &i);

			sprintf(app.job.app_id, "monitor");
			gethostname(app.node_id, 128);
			app.is_learning = 0;
			app.job.id = 1000000;
			app.is_mpi = 1;

			for (; i > 0; --i) {
				app.job.step_id += 1;
				s = eardbd_send_application(&app);
				verbose(0, "%s send app result (%d, %d)", col, s.server, s.mirror);
			}

		} else if (is(cmnd, "multitest")) {
			multithread_test();
		} else if (is(cmnd, "exit")) {
			return 0;
		}
	}

	return 0;
}
