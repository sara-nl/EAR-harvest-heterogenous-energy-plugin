#define MAX_APP_NAME 1024

int EAR_VERBOSE_LEVEL;
char ear_node_name[MAX_APP_NAME];
char ear_app_name[MAX_APP_NAME];
char ear_policy_name[MAX_APP_NAME];
unsigned long EAR_default_frequency;
unsigned long ear_frequency;
unsigned int EAR_default_pstate;
unsigned int ear_resources;
int power_model_policy;
int ear_use_turbo = USE_TURBO;
int ear_whole_app;
int ear_papi_init;
int ear_my_rank;
int report;
