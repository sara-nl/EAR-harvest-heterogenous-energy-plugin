#define MAX_APP_NAME    1024

int report = 0;
int ear_my_rank;
unsigned long ear_frequency;
unsigned long EAR_default_frequency;
unsigned int EAR_default_pstate;
unsigned int ear_resources;
int ear_whole_app = 0;
int ear_use_turbo = USE_TURBO;
char ear_app_name[MAX_APP_NAME];
char ear_node_name[MAX_APP_NAME];
int EAR_VERBOSE_LEVEL = 0;
int ear_papi_init;
char ear_policy_name[MAX_APP_NAME];
