// ear_db
extern char ear_node_name[MAX_APP_NAME]; //ear_states
extern unsigned long EAR_default_frequency; // ear_models
extern int ear_my_rank; // ear_models ear_states ear_cpufreq ear_papi

// ear_models
extern char ear_app_name[MAX_APP_NAME]; // ear_states ear_papi
extern unsigned long ear_frequency; // ear_states ear_cpufreq ear_papi
extern unsigned int EAR_default_pstate;
extern int ear_use_turbo; // ear_papi
extern int ear_whole_app; // ear_papi

// ear_states
extern int report;

// ear_cpufreq
extern int ear_papi_init; // ear_flops_metrics

// ear_gui
extern char *ear_policy_name; // ear_papi

// ear_papi
extern unsigned int ear_resources;

// ear_papi
extern int power_model_policy;
