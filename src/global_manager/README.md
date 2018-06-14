Energy Aware Runtime: Global Manager Daemon
-------------------------------------------
The EAR Global Manager Daemon (EARGMD) is a cluster wide component that controls the percetage of the maximum energy consumed, and it can be configured to take actions automatically like reporting warning levels and let the actions to be taken by sysadmins or limiting the nodes policy.

Requirements
------------
EARGMD uses periodic power metrics reported by [EARD](../daemon/README.md), the per-node daemon, including job identification details (job id, step id when executed in a slurm system). These metrics are stored and aggregated in a MariaDB (MySQL) database through the [EARDBD](../database_cache/REAME.md).

Configuration
-------------
The EAR Global Manager Daemon uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.

```
# Fields related to the Global Manager Daemon

# Verbose level
GlobalManagerVerbose=1   
# Period T1 in seconds (10 min)
GlobalManagerPeriodT1=600
# Period T2 in seconds (30 days)
GlobalManagerPeriodT2=2592000
# Abosolute value (joules)
GlobalManagerEnergyLimit=756000
GlobalManagerPort=6000
# Two modes are supported (0 pasive, 1 active or automatic)
GlobalManagerMode=  
# Anyway, a mail can be sent reporting the warning level (and the action taken in automatic mode)
#GlobalManagerMail=  
```

API
---
The Global Manager offers a simple API (`eargm_api.c`, also used by the SLURM plugin) to be notified about the execution/finalization of the jobs. The API just notifies the EARGMD about the number of nodes to be used/released by the job.

Execution
---------
To execute this component, this `systemctl` command examples are provided:
- `sudo systemctl start eard` to start the EARGMD service.
- `sudo systemctl stop eard` to stop the EARGMD service.
- `sudo systemctl reload eard` to force to reload the configuration of the EARGMD service.

Commands
--------
This is a list of the available commands:

| Command           | Description                                         |
| ----------------- | --------------------------------------------------- |
| eargm_new_job     | Informs the EARGMD that a job is about to start.    |
| eargm_end_job     | Informs the EARGMD that a job has finished.         |

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.
