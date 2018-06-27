#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <linux/limits.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_user_system.h>
#include <common/types/application.h>

extern char buffer1[PATH_MAX];
extern char buffer2[PATH_MAX];
extern int verbosity;

// EARD variables
extern unsigned char eard_host[NAME_MAX+1];
extern unsigned int  eard_port;
extern application_t eard_appl;

// EARGMD variables
extern unsigned char eargmd_host[NAME_MAX+1];
extern unsigned int  eargmd_port;
extern unsigned int  eargmd_nods;

//
char *etc_dir;
char *pre_dir;
char *tmp_dir;

static int is_user_privileged(spank_t sp, cluster_conf_t *conf_clus)
{
	char *aid;
	char *uid;
	int i;

	if (getenv_local("SLURM_JOB_USER", &uid))
	{
		plug_verbose(sp, 2, "looking for privileges for user '%s'", uid);

		for (i = 0; i < conf_clus->num_priv_users; ++i) {
			if (strcmp(uid, conf_clus->priv_users[i]) == 0) {
				return 1;
			}
		}
	}

	if (getenv_local("SLURM_JOB_ACCOUNT", &aid))
	{
		plug_verbose(sp, 2, "looking for privileges for account '%s'", aid);
		
		for (i = 0; i < conf_clus->num_acc; ++i) {
			if (strcmp(aid, conf_clus->priv_acc[i]) == 0) {
				return 1;
			}
		}
	}

	return 0;
}

/*
 * Plugin intern environment variables:
 *
 *   environment variable					user			testing
 * ----------------------------------------------------------------------
 * - EAR									privileged		no controlled
 * - EAR_LEARNING_PHASE						privileged		no controlled
 * - EAR_VERBOSE							no privileged	no controlled
 * - EAR_POWER_POLICY						privileged		controlled
 * - EAR_P_STATE							privileged		controlled
 * - EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN	privileged		controlled
 * - EAR_PERFORMANCE_PENALTY				privileged		controlled
 * - EAR_TRACES								privileged		no controlled
 * - EAR_MPI_DIST							no privileged	no controlled
 * - EAR_USER_DB_PATHNAME					no privileged	no controlled
 */

/*
 *
 * Local user system configuration
 *
 */

static int local_user_configuration_unprivileged(cluster_conf_t *conf_clus)
{
	my_node_conf_t *conf_node;
	policy_conf_t  *conf_plcy;

	// Getting node configuration
	if ((conf_node = get_my_node_conf(conf_clus, eard_host)) == NULL) {
		plug_error("while getting node configuration");
		return (ESPANK_ERROR);
	}

	// Getting node policy
	if ((conf_plcy = get_my_policy_conf(conf_clus, conf_node, conf_clus->default_policy)) == NULL) {
		plug_error("while getting policy configuration");
		return (ESPANK_ERROR);
	}

	// Forcing EAR_POWER_POLICY
	policy_id_to_name(conf_clus->default_policy, buffer1);
	SETENV_LOCAL_RET_ERR("EAR_POWER_POLICY", buffer1, 1);

	//TODO: Depends on the node, future placement in remote_configuration_user_unprivileged
	#if 1
	// Forcing EAR_P_STATE
	sprintf(buffer1, "%u", conf_plcy->p_state);
	setenv_local("EAR_P_STATE", buffer1, 1);

	// Forcing EAR_THRESHOLD
	sprintf(buffer1, "%d", conf_plcy->th);
	setenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer1, 1);
	setenv_local("EAR_PERFORMANCE_PENALTY", buffer1, 1);
	#endif

	// Forcing EAR_TRACES
	SETENV_LOCAL_RET_ERR("EAR_TRACES", "0", 1);

	// Forcing EAR_LEARNING_PHASE
	SETENV_LOCAL_RET_ERR("EAR_LEARNING_PHASE", "0", 1);

	return (ESPANK_SUCCESS);
}

static int local_user_configuration_privileged(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_user_configuration_privileged");

	my_node_conf_t *conf_node;
	policy_conf_t  *conf_plcy;
	unsigned int id_plcy_aux;
	unsigned int id_plcy;
	char *c_plcy;
	int i;

	// Getting node configuration
	if ((conf_node = get_my_node_conf(conf_clus, eard_host)) == NULL) {
		plug_error("while getting node configuration");
		return (ESPANK_ERROR);
	}

	// Selecting default policy by default
	id_plcy = conf_clus->default_policy;

	// Testing EAR_POWER_POLICY
	if (existenv_local("EAR_POWER_POLICY") &&
		getenv_local("EAR_POWER_POLICY", &c_plcy))
	{
		id_plcy_aux = policy_name_to_id(c_plcy);
		
		for(i = 0; i < conf_clus->num_policies; ++i)
		{
			if (conf_clus->power_policies[i].policy == id_plcy_aux)
			{
				id_plcy = id_plcy_aux;
			}	
		} 
	}

	// Getting policy configuration based on the policy selected of the user
	if ((conf_plcy = get_my_policy_conf(conf_clus, conf_node, id_plcy)) == NULL) {
		plug_error("while getting policy configuration");
		return (ESPANK_ERROR);
	}

	//TODO: Depends on the node, future placement in remote_configuration_user_privileged
	#if 1
	// Testing EAR_P_STATE
	if (!existenv_local("EAR_P_STATE")) {
		sprintf(buffer1, "%u", conf_plcy->p_state);
		setenv_local("EAR_P_STATE", buffer1, 1);
	} else {
		//TODO: is valid?
	}

	// Testing EAR_THRESHOLD
	if (!existenv_local("EAR_POWER_POLICY_TH")) {
		sprintf(buffer1, "%lf", conf_plcy->th);
		setenv_local("EAR_POWER_POLICY_TH", buffer1, 1);
	} else {
		//TODO: is valid?
	}
	#endif

	policy_id_to_name(id_plcy, buffer1);
	SETENV_LOCAL_RET_ERR("EAR_POWER_POLICY", buffer1, 1);

	return (ESPANK_SUCCESS);
}

static int local_configuration_user(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_configuration_user");

	if (is_user_privileged(conf_clus)) {
		return local_user_configuration_privileged();
	}

	return local_user_configuration_unprivileged();}

/*
 *
 * Local configuration (all nodes)
 *
 */

int local_post_job_configuration(spank_t sp)
{
	plug_verbose(sp, 2, "function local_update_ld_preload");

	char *ear_root_dir = NULL;
	char *ld_preload = NULL;

	buffer1[0] = '\0';
	buffer2[0] = '\0';

	/*if (getenv_local("LD_PRELOAD", &ld_preload) == 1) {
		strcpy(buffer1, ld_preload);
	}*/

	if (getenv_local("EAR_PREDIR", &ear_root_dir) == 0)
	{
		plug_error("Error, wrong environment for setting LD_PRELOAD");
		return ESPANK_ERROR;
	}
	appendenv(buffer1, ear_root_dir, PATH_MAX);

	// Appending libraries to LD_PRELOAD
	if (isenv_local("EAR_TRACES", "1") == 1)
	{
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
			sprintf(buffer2, "%s/%s", buffer1, OMPI_TRACE_LIB_PATH);
		} else {
			sprintf(buffer2, "%s/%s", buffer1, IMPI_TRACE_LIB_PATH);
		}
	}
	else
	{
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
			sprintf(buffer2, "%s/%s", buffer1, OMPI_LIB_PATH);
		} else {
			sprintf(buffer2, "%s/%s", buffer1, IMPI_LIB_PATH);
		}
	}

	//
	if(SETENV_LOCAL_RET_ERR("LD_PRELOAD", buffer2, 1) != 1) {
		return ESPANK_ERROR;
	}

	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	return ESPANK_SUCCESS;
}

static int plugstack_process(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function plugstack_process");

	int found_predir = 0;
	int found_etcdir = 0;
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp ("default=on", av[i], 10) == 0))
		{
			SETENV_LOCAL_RET_ERR("EAR", "1", 0);
		}
		if ((strlen(av[i]) > 11) && (strncmp ("sysconfdir=", av[i], 11) == 0))
		{
			etc_dir = &av[i][11];
			found_etcdir = 1;

			plug_verbose(sp, 3, "looking for configuration files in path '%s'", etc_dir);
			SETENV_LOCAL_RET_ERR("EAR_ETCDIR", etc_dir, 1);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			pre_dir = &av[i][7];
			found_predir = 1;

			plug_verbose(sp, 3, "looking for library files in path '%s'", pre_dir);
			SETENV_LOCAL_RET_ERR("EAR_PREDIR", pre_dir, 1);
		}
	}

	if (!found_etcdir || !found_predir) {
		plug_error("missing plugstack parameters");
		return (ESPANK_ERROR);
	}

	return (ESPANK_SUCCESS);
}

static int local_pre_job_configuration_general(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_pre_job_configuration_general");

	// Setting variables for EARGMD connection
	sprintf(eargmd_host, "%s", conf_clus.eargm.host);
	eargmd_port = conf_clus.eargm.port;

	// Setting variables for EARD connection
	sprintf(buffer1, "%u", conf_clus.eard.port);
	SETENV_LOCAL_RET_ERR("EARD_PORT", buffer1, 1);

	// Setting LIBEAR variables
	SETENV_LOCAL_RET_ERR("EAR_TMPDIR", conf_clus.tmp_dir, 1);
	SETENV_LOCAL_RET_ERR("EAR_COEFF_DB_PATHNAME", conf_clus.Coefficients_pathname, 1);

	SETENV_LOCAL_RET_ERR("EAR_DYNAIS_WINDOW_SIZE", "500", 1);
	SETENV_LOCAL_RET_ERR("EAR_DYNAIS_LEVELS", "4", 1);

	sprintf(buffer1, "%u", conf_clus.min_time_perf_acc);
	SETENV_LOCAL_RET_ERR("EAR_PERFORMANCE_ACCURACY", buffer1, 1);

	return (ESPANK_SUCCESS);
}

int local_pre_job_configuration(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function local_configuration");

	static cluster_conf_t conf_clus;
	char *conf_path = buffer1;
	int r;

	//
	if ((r = plugstack_process(sp, ac, av)) != ESPANK_SUCCESS) {
		return r;
	}

	// Getting variables
	gethostname(eard_host, NAME_MAX);

	// Passing parameters to get_ear_conf_path (LOL)
	plug_verbose(0, 2, "trying to read config file in '%s' for node '%s'", etc_dir, eard_host);

	SETENV_LOCAL_RET_ERR("ETC", etc_dir, 1);
	conf_path[0] = '\0';

	if (get_ear_conf_path(conf_path) == EAR_ERROR) {
		plug_error("while looking for configuration file");
		return (ESPANK_ERROR);
	}

	// Reading the cluster configuration file
	plug_verbose(0, 2, "found file in '%s'", conf_path);

	if (read_cluster_conf(conf_path, &conf_clus) != EAR_SUCCESS) {
		plug_error("while reading configuration file");
		return (ESPANK_ERROR);
	}


	// General environment
	IF_RET_ERR(local_pre_job_configuration_general(sp, &conf_clus));

	// User specific environment
	IF_RET_ERR(local_configuration_user(sp, &conf_clus));

	// Freeing cluster configuration resources
	free_cluster_conf(&conf_clus);

	return (ESPANK_SUCCESS);
}

/*
 *
 * Remote user system
 *
 */

static int remote_configuration_user_unprivileged(cluster_conf_t *conf_clus)
{
	return ESPANK_SUCCESS;
}

static int remote_configuration_user_privileged(spank_t sp, cluster_conf_t *conf_clus)
{
	return ESPANK_SUCCESS;
}

static int remote_configuration_user(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function remote_configuration_user");

	return ESPANK_SUCCESS;
}

/*
 *
 * Remote configuration (single node)
 *
 */

int remote_configuration(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_configuration");

	char p_state[64];
	int p_freq = 1;

	// If policy is monitoring
	if (isenv_remote(sp, "EAR_POWER_POLICY", "MONITORING_ONLY") == 1)
	{
		// If learning phase is not enabled
		if ((existenv_remote(sp, "EAR_LEARNING_PHASE") == 0) ||
			(isenv_remote(sp, "EAR_LEARNING_PHASE", "0") == 1))
		{
			// If this is passed SLURM's --cpu-freq argument
			if (getenv_remote(sp, "SLURM_CPU_FREQ_REQ", p_state, 64) == 1)
			{
				p_freq = atoi(p_state);
				p_freq = freq_to_p_state(p_freq);

				sprintf(p_state, "%d", p_freq);
				SETENV_REMOTE_RET_ERR(sp, "EAR_P_STATE", p_state, 0);

				plug_verbose(sp, 2, "Updating to P_STATE '%s' by '--cpu-freq=%d' command",
							 p_state, p_freq);
			}
		}
	}

	// User system for LIBEAR
	IF_RET_ERR(remote_configuration_user(sp, &conf_clus));

	// Remote switch
	if (getenv_remote(sp, "SLURM_JOB_NAME", buffer2, PATH_MAX) == 1) {
		SETENV_REMOTE_RET_ERR(sp, "EAR_APP_NAME", buffer2, 1);
	}
	if (getenv_remote(sp, "EAR_TMPDIR", buffer2, PATH_MAX) == 1) {
		SETENV_REMOTE_RET_ERR(sp, "EAR_TMP", buffer2, 1);
	}
	if (getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer2, PATH_MAX) == 1) {
		SETENV_REMOTE_RET_ERR(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer2, 1);
	}
	if (getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer2, PATH_MAX) == 1) {
		SETENV_REMOTE_RET_ERR(sp, "EAR_PERFORMANCE_PENALTY", buffer2, 1);
	}

	return (ESPANK_SUCCESS);
}
