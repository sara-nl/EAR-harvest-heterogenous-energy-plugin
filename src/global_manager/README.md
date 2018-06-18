Energy Aware Runtime: Global Manager Daemon
-------------------------------------------
The EAR Global Manager Daemon (EARGMD) is a cluster wide component that controls the percetage of the maximum energy consumed, and it can be configured to take actions automatically like reporting warning levels and let the actions to be taken by sysadmins or limiting the nodes policy.

Requirements
------------
EARGMD uses periodic power metrics reported by [EARD](../daemon/README.md), the per-node daemon, including job identification details (job id, step id when executed in a slurm system). These metrics are stored and aggregated in a MariaDB (MySQL) database through the [EARDBD](../database_cache/REAME.md).

<<<<<<< HEAD
EAR Global Manager uses periodic power metrics reported by [EARD](../daemon/README.md). EARD is a per-node daemon and it periodically reports the dc energy consumed in the node, including job identification details (job id, step id when executed in a slurm system). These metrics are stored and aggregated in a MariaDB DB through the [EAR DB Manager](../database_cache/REAME.md).

Brief installation guide
------------------------

EAR includes service files to be used to start/stop the components. Global Manager can be started with the $ETCDIR/systemd/eargmd.service file. By executing the traditional "make install" command, EAR service files are copied in the /etc/systemd/system folder.

sudo systemctl start eargmd --> will start the service 
</br>sudo systemctl stop eargmd --> will stop the service 
</br>sudo systemctl reload eargmd --> will force to reload the configuration  (ear.conf file)
=======
Configuration
-------------
The EAR Global Manager Daemon uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.
>>>>>>> e000a65a956d63fa6cda14643f21abfc5cd91da9

```
# Fields related to the Global Manager Daemon

# Verbose level
GlobalManagerVerbose=1   
<<<<<<< HEAD
#Period T1 in seconds=10min   
GlobalManagerPeriodT1=600   
#Period T2 in seconds=30 days   
GlobalManagerPeriodT2=2592000   
#Abosolut value , in Joules  
GlobalManagerEnergyLimit=756000   
GlobalManagerPort=60000
#Two modes are supported 0=pasive 1= means active (automatic)   
GlobalManagerMode=0
#Anyway, a mail can be sent reporting the warning level (and the action taken in automatic mode) . When using nomail as email address we can also disable that option
GlobalManagerMail=nomail 
=======
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
>>>>>>> e000a65a956d63fa6cda14643f21abfc5cd91da9

API
---
The Global Manager offers a simple API (`eargm_api.c`, also used by the SLURM plugin) to be notified about the execution/finalization of the jobs. The API just notifies the EARGMD about the number of nodes to be used/released by the job.

Execution
---------
To execute this component, this `systemctl` command examples are provided:
- `sudo systemctl start eard` to start the EARGMD service.
- `sudo systemctl stop eard` to stop the EARGMD service.
- `sudo systemctl reload eard` to force to reload the configuration of the EARGMD service.

<<<<<<< HEAD
Global Manager commands
----------------------

The Global Manager  offers a simple API (to be used by the SLURM plugin) to be notified about the execution/finalization of jobs. The APi simple notifies the Global Manager about the number of nodes to be used/released by the node. The API is implemented in the eargm_api.c file.

 - commands/eargm_new_job 
 - commands/eargm_end_job 

are very simple commands that implements the two functions offered by the Global Manager API.
=======
Commands
--------
This is a list of the available commands:
>>>>>>> e000a65a956d63fa6cda14643f21abfc5cd91da9

| Command           | Description                                         |
| ----------------- | --------------------------------------------------- |
| eargm_new_job     | Informs the EARGMD that a job is about to start.    |
| eargm_end_job     | Informs the EARGMD that a job has finished.         |

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.
