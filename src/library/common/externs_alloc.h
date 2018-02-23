#include <common/types/application.h>
#include <linux/limits.h>

#define MAX_APP_NAME 1024

int EAR_VERBOSE_LEVEL;

application_t loop_signature;
application_t application;

char loop_summary_path[PATH_MAX];
char app_summary_path[PATH_MAX];

unsigned long ear_frequency;
unsigned int ear_resources;
int power_model_policy;
int ear_use_turbo = USE_TURBO;
int ear_whole_app;
int ear_papi_init;
int ear_my_rank;
int report;

//TODO: clean
char ear_node_name[MAX_APP_NAME];
char ear_app_name[MAX_APP_NAME];
char ear_policy_name[MAX_APP_NAME];
unsigned long EAR_default_frequency;
unsigned int EAR_default_pstate;
