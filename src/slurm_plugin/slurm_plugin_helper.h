#ifndef EAR_SLURM_PLUGIN_HELPER_H
#define EAR_SLURM_PLUGIN_HELPER_H

void appendenv(char *destiny, char *source);
int setenv_local(const char *name, const char *value, int replace);
int setenv_remote(spank_t sp, char *name, char *value, int replace);
int getenv_remote(spank_t sp, char *name, char *value, int length);
int existenv_local(char *name);
int existenv_remote(spank_t sp, char *name);
int isenv_local(char *name, char *value);
int isenv_remote(spank_t sp, char *name, char *value);

int freq_to_p_state(int freq);
int file_to_environment(spank_t sp, const char *path);
int find_ear_conf_file(spank_t sp, int ac, char **av);

#endif //EAR_SLURM_PLUGIN_HELPER_H
