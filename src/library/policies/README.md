
# Policies

## List of policies
The list of official supported policies are:

- monitoring: This policy is not a real policy, it only offers application performance and power profiling
- min_time: This policy starts at a default frequency lower than nominal and increases the frequency up to the nominal in case the application presents a good performance scalability. It uses the CPU energy models
- min_energy: This policy starts at nominal frequency and reduces the frequency to minimize the energy with a limit in the performance degradation. It uses the CPU energy models

## Under development policies
	- min_energy_no_models and min_time_no_models: These two policies uses the same criteria than the corresponding official ones but they select the CPU frequency by doing a linear search rather than using the CPU energy models.
	- app_monitoring: Offers additional mpi information to compute the percentage of mpi vs computation time. This feature is activated when using the SLURM_GET_EAR_STATS environment variable
	- app_min_time and app_min_energy: These policies uses the same goal than their corresponding official ones but a global signature is used ratehr than per-node. 
	- load_balance: This policy uses a similar algorithm than min_energy but computes the per-node load balance and potentially selects a different frequency per process. It uses the process affinity mask. 
	- gpu_monitoring, gpu_min_time and gpu_min_energy: Offers the corresponding GPU policy version. They are automatically loaded based on the CPU policy 


## Additional files to support dynamic policy setting management
- When using EARGM, policy settings can be dynamically modified to adapt the system to the energy limits. Files with extension \_eard are loaded by the eard and implements the functions to react to EARGM requests

## Policy selection
	- the CPU policy is selected using the --ear-policy flag when submitting a job. The GPU policy is selected based on the CPU policy
	- To use the "app" version for a policy, the SLURM_APP_MGR_POLICIES environment variable must be set to 1

## Policy API

EAR supports energy policies as plugins. The policy API is defined in policy.c. Till version 3.3 only local (per-node) policies were supported. Version 3.4 includes per-application policies. Policy functions are optional and the generic policy API checks each function before calling it. The list of functions is (check policy.c for updates)

	- policy_init
	- policy_apply
	- policy_app_apply
	- policy_get_default_freq
	- policy_ok
	- policy_max_tries
	- policy_end
	- policy_loop_init
	- policy_loop_end
	- policy_new_iteration
	- policy_mpi_init
	- policy_mpi_end
	- policy_configure

Same API is used for CPU and GPU.


*policy_apply* refers to per-node policy selection and *policy_app_apply* refers to per-application frequency selection. 

## Policy states

Depending on the readiness of the policy, the EAR sates transitions changes. 

	- EAR_POLICY_READY : The policy has finished, a new frequency has been selected and the new state must be applied. Refers to per-node policies
	- EAR_POLICY_CONTINUE: The policy needs more time in the same state. EAR state is not modified. Refers to per-node and per-application policies.
	- EAR_POLICY_GLOBAL_EV: The policy uses a per-application policy and a global signature evaluation or per-application state must be applied
	- EAR_POLICY_GLOBAL_READY: The per-application policy is ready and the new state must be applied. 

## Local (per-node) signatures
Per-node signatures are computed by src/library/metric.c file. A per-node signature computation is started automatically after the last one is computed. It should be re-started manually in case the frequency is modified after it was started to guarantee it is computed with a constant frequency.

## Global (per-application) signatures
Per-application signature or metrics are computed once they are available. src/library/common/global_comm.c file includes functions to check shared signatures readiness and are called in policy_new_iteration functions in the specific part given some policies are not using it. 

## Powercap management

After frequency selection, EAR checks if there is some powercap set and a DVFS approach is used for power management. In that case, EAR cooperates with the powercap module. In case it is set, EAR estimates based on the models, whether the CPU and GPU frequency selecteds are ok according to the limit. Moreover, EAR sets the requested frequency in a shared memory to inform the powercap module about the frequency selected by the policy. 

In case the powercap module uses a direct approach such as the Intel Node Manager, it is not needed to adapt the frequency.
    




