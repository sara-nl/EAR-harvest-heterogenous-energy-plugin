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

static int local_configuration_user_unprivileged(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_configuration_user_unprivileged");
	
	my_node_conf_t *conf_node;
	policy_conf_t  *conf_plcy;

	if ((conf_node = get_my_node_conf(conf_clus, eard_host)) == NULL) {
		return (ESPANK_ERROR);
	}

	if ((conf_plcy = get_my_policy_conf(conf_clus, conf_node, conf_clus->default_policy)) == NULL) {
		return (ESPANK_ERROR);
	}

	// Forcing EAR_POWER_POLICY (TODO: warning system)
	policy_id_to_name(conf_clus->default_policy, buffer1);
	setenv_local("EAR_POWER_POLICY", buffer1, 1);

	// Forcing EAR_P_STATE
	sprintf(buffer1, "%u", conf_plcy->p_state);
	setenv_local("EAR_P_STATE", buffer1, 1);

	// Forcing EAR_THRESHOLD
	sprintf(buffer1, "%d", conf_plcy->th);
	setenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer1, 1);
	setenv_local("EAR_PERFORMANCE_PENALTY", buffer1, 1);

	// Forcing EAR_TRACES
	setenv_local("EAR_TRACES", "0", 1);

	// Forcing EAR_LEARNING_PHASE
	setenv_local("EAR_LEARNING_PHASE", "0", 1);

	return (ESPANK_SUCCESS);
}

static int local_configuration_user_privileged(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_configuration_user_privileged");
	
	my_node_conf_t *conf_node;
	policy_conf_t  *conf_plcy;
	unsigned int id_plcy;
	char *c_plcy;

	// Getting node configuration
	if ((conf_node = get_my_node_conf(conf_clus, eard_host)) == NULL) {
		plug_error("while getting node configuration");
		return (ESPANK_ERROR);
	}

	// Testing EAR_POWER_POLICY
	if (existenv_local("EAR_POWER_POLICY")) {
		plug_verbose(sp, 2, "Setting environment policy");
		getenv_local("EAR_POWER_POLICY", &c_plcy);
		id_plcy = policy_name_to_id(c_plcy);
	} else {
		id_plcy = conf_clus->default_policy;
		plug_verbose(sp, 2, "Setting default policy %u", id_plcy);
	}

	// Getting policy configuration based on the policy selected of the user
	if ((conf_plcy = get_my_policy_conf(conf_clus, conf_node, id_plcy)) == NULL) {
		plug_error("while getting policy configuration");
		return (ESPANK_ERROR);
	}

	policy_id_to_name(id_plcy, buffer1);
	setenv_local("EAR_POWER_POLICY", buffer1, 1);

	// Testing EAR_P_STATE
	if (!existenv_local("EAR_P_STATE")) {
		sprintf(buffer1, "%u", conf_plcy->p_state);
		setenv_local("EAR_P_STATE", buffer1, 1);
	} else {
		//TODO: is valid
	}

	// Testing EAR_THRESHOLD
	if (!existenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN")) {
		sprintf(buffer1, "%lf", conf_plcy->th);
		setenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer1, 1);
	}

	if (!existenv_local("EAR_PERFORMANCE_PENALTY")) {
		sprintf(buffer1, "%lf", conf_plcy->th);
		setenv_local("EAR_PERFORMANCE_PENALTY", buffer1, 1);
	} else {
		//TODO: is valid
	}

	return (ESPANK_SUCCESS);
}

int local_user_system_configuration(spank_t sp, cluster_conf_t *conf_clus)
{
	plug_verbose(sp, 2, "function local_user_system_configuration");

	#if 1
	return local_configuration_user_privileged(sp, conf_clus);
	#endif

	if (is_user_privileged(sp, conf_clus)) {
		return local_configuration_user_privileged(sp, conf_clus);
	}

	return local_configuration_user_unprivileged(sp, conf_clus);
}
