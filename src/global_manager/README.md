Energy Aware Runtime: Global Manager Daemon
-------------------------------------------
The EAR Global Manager Daemon (EARGMD) is a cluster wide component that controls the percetage of the maximum energy consumed. It can be configured to take actions automatically like warning sysadmins to take actions or limiting the nodes policy.

Requirements
------------
EARGMD uses periodic power metrics reported by [EARD](../daemon/README.md), the per-node daemon, including job identification details (job id and step id if you are using the SLURM plugin). These metrics are stored and aggregated in a MariaDB (MySQL) database through the [EARDBD](../database_cache/REAME.md).

Configuration
-------------
The EAR Global Manager Daemon uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.

Please visit the [EAR configuration file page](../../etc/conf/README.md) for more information about the options of EARGMD and other components.

API
---
The global manager offers a simple API `eargm_api.c` to notify the execution and finalization of the jobs. The API just notifies the EARGMD about the number of nodes to be used and released after the execution of the job. The SLURM plugin automatically does this task.

Execution
---------
To execute this component, this `systemctl` command examples are provided:
- `sudo systemctl start eargmd` to start the EARGMD service.
- `sudo systemctl stop eargmd` to stop the EARGMD service.
- `sudo systemctl reload eargmd` to force to reload the configuration of the EARGMD service.

Commands
--------
This is a list of the available commands:

| Command           | Description                                          |
| ----------------- | --------------------------------------------------- |
| eargm_new_job     | Informs the EARGMD that a job is about to start.    |
| eargm_end_job     | Informs the EARGMD that a job has finished.    |

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.