#ifndef EAR_SLURM_PLUGIN_USER_SYSTEM_H
#define EAR_SLURM_PLUGIN_USER_SYSTEM_H

#include <slurm/spank.h>
#include <common/types/cluster_conf.h>

int local_post_job_configuration(spank_t sp);
int local_pre_job_configuration(spank_t sp, int ac, char **av);

int remote_configuration(spank_t sp);

#endif //EAR_SLURM_PLUGIN_USER_SYSTEM_H
