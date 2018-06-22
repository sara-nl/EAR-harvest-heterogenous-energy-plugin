#ifndef EAR_SLURM_PLUGIN_USER_SYSTEM_H
#define EAR_SLURM_PLUGIN_USER_SYSTEM_H

#include <slurm/spank.h>
#include <common/types/cluster_conf.h>

int local_user_system_configuration(spank_t sp, cluster_conf_t *conf_clus);

#endif //EAR_SLURM_PLUGIN_USER_SYSTEM_H
