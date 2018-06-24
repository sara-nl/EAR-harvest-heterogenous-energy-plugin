#ifndef EAR_SLURM_PLUGIN_USER_SYSTEM_H
#define EAR_SLURM_PLUGIN_USER_SYSTEM_H

#include <slurm/spank.h>
#include <common/types/cluster_conf.h>

int local_configuration(spank_t sp, int ac, char **av);
int local_user_system_configuration(spank_t sp, cluster_conf_t *conf_clus);
int local_update_ld_preload(spank_t sp);

void remote_update_slurm_vars(spank_t sp);

#endif //EAR_SLURM_PLUGIN_USER_SYSTEM_H
