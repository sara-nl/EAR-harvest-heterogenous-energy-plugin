Energy Aware Runtime: (Node) Daemon
-----------------------------------
The node daemon is the component in charge of providing any kind of service that requires privileged capabilities. Current version is conceived as an external process executed with root privileges. The EARD provides two basic services, each one covered by one thread:
- Provides privileged metrics such as average frequency, uncore integrated memory controller counters to compute the memory bandwidth, and also energy metrics (DC node, DRAM and package energy).
- Implements a periodic power monitoring service. This service allows EAR package to control the total energy consumed in the system.

Requirements
------------
EARD uses CPUPower, FreeIPMI and PAPI (with RAPL component enabled). Paths to these libraries must be specified during the configure process when they are not installed in defaults paths.

When executed in production, EARD connects with [EARDBD](../database_cache/REAME.md), service that have to be up before starting the node daemon, otherwise values reported by EARD to the EAR database will be lost.

Configuration
-------------
The EAR Daemon uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.

```
# Fields related to the Daemon
# Lines starting with # are comments

# Verbose level
NodeDaemonVerbose=1

# Frequency used by power monitoing service (in seconds)
NodeDaemonPowermonFreq=10

# Maximum p_state supported (1 nominal)
NodeDaemonMaxPstate=1

# Turbo frequencies supported (0 no)
NodeDaemonTurbo=0

# Port to be used by the remote API
NodeDaemonPort=5000
```

Execution
---------
To execute this component, this `systemctl` command examples are provided:
- `sudo systemctl start eard` to start the EARD service.
- `sudo systemctl stop eard` to stop the EARD service.
- `sudo systemctl reload eard` to force to reload the configuration of the EARD service.

Reconfiguration
---------------

After executing a "systemctl reload eard" command, not all the EARD options are dynamically updated. The list of updated variables are:

- NodeDaemonMaxPstate
- NodeDaemonVerbose
- Default policy and default policy settings
- NodeDaemonPowermonFreq

To reconfigure other detauils such as EARD port, coefficients, etc, EARD must be stopped and restarted again.

API
---
The (node) Daemon offers a simple API to request changes on the frequency, modify the current node settings, and reload the system configuration by reading `$(ETC)/ear.conf`

Three APIs are provided:
- Local API, to be used by [EARL](../library/README.md) (or any other runtime). It can be found in [eard_api.h](eard_api.h). This API involves complex data types and is not public.

- Local API, to be used by applications. It is a subset of the EARD api and designed to be used by any applications to contact the privileged metric service offered by EARD. This API is public and can be used without restrictinos, so doesn't include functions to change the frequency. It can be found at [TBD](.).

- Remote API, to be used by the [EARGMD](../global_manager/README.md) or system commands and tools such as the `econtrol`. Can be found at [eard_rapi.h](eard_rapi.h) and is not public.


Basic commands
--------------
EARD includes some basic commands to be used in the same node eard is running. These commands change/get specific settings in the current node. They are not designaed to be used as system tools, just under development systems. We are offering:

1. Commands using the eard local api:
   - commands/ear_get_node_freq: Usage: ./ear_get_node_freq num_cpus
   - commands/ear_set_ondemand: Usage:ear_set_ondemand cpus|[-h]. Changes the current governor to ondemand
   - commands/ear_set_node_freq: Usage: ear_set_node_freq node_freq (in MHz) num_cpus 

2. Commands using the remote api (can be connected from a different node) to notify new job/end job. They do the same functionality than EAR slurm plugin does
   - commands/ear_new_job:Connects with eard and sends a new_job requests. It automatically reads ear.conf file and sets application details based on environment variables.Only needed when slurm is not installed. 
   - commands/ear_end_job: Connects with eard and sends an end_job request. It automatically reads ear.conf file and sets application details based on environmen t variables.  Only needed when slurm is not installed. 

3. Commands using the remote api (can be connected from a different node) to change node settings (defaul/max frequencies and th (for min_time))
   - commands/ear_set_freq: usage:ear_set_freq new_freq (in KHz)
   - commands/ear_inc_th: usage:./ear_inc_th perc to be added to current PerformanceEfficiencyGain (5 means 5%)        
   - commands/ear_red_freq: usage:./ear_red_freq p_states 
   - commands/ear_set_th: usage:./ear_set_th MinPerformanceEfficiencyGain (75 means 0.75%)

Daemon testing
--------------
Look into the [EAR test folder](../tests/README.md)

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.

