# Energy Aware Runtime: Global Manager
Description
-----------

EAR is a framework to control the energy at different contexts. The EAR Global Manager is a cluster wide components controlling the energy consumed during a given period T2 (configurable) every T1 seconds (also configurable). The EAR Global Manager is controlling the percetage of the maximum energy consumed and it can be configured to take actions automatically or just report warning levels and let the actions to be taken by sysadmins. 

Requirements
------------

EAR Global Manager uses periodic power metrics reported by [EARD](../daemon/README.md). EARD is a per-node daemon and it periodically reports the dc energy consumed in the node, including job identification details (job id, step id when executed in a slurm system). These metrics are stored and aggregated in a MariaDB DB through the [EAR DB Manager](../database_cache/REAME.md).

Brief installation guide
------------------------

EAR includes service files to be used to start/stop the components. Global Manager can be started with the $ETCDIR/systemd/eargmd.service file. By executing the traditional "make install" command, EAR service files are copied in the /etc/systemd/system folder.

sudo systemctl start eargmd --> will start the service

sudo systemctl stop eargmd --> will stop the service

sudo systemctl reload eargmd --> will force to reload the configuration 

Customize installation
----------------------

The Global Manager uses the $ETCDIR/ear.conf file to be configured. The Global Manager can be dynamically configured by reloading the ear.conf file:

##Fields in the ear.conf related to the Global Manager
#verbose level
GlobalManagerVerbose=1

#Period T1 in seconds=10min
GlobalManagerPeriodT1=600

#Period T2 in seconds=30 days
GlobalManagerPeriodT2=2592000

#Abosolut value , in Joules
GlobalManagerEnergyLimit=756000
GlobalManagerPort=6000

#Two modes are supported 0=pasive 1= means active (automatic)
GlobalManagerMode=

#Anyway, a mail can be sent reporting the warning level (and the action taken in automatic mode)
#GlobalManagerMail=



Global Manager commands
----------------------

The Global Manager  offers a simple API (to be used by the SLURM plugin) to be notified about the execution/finalization of jobs. The APi simple notifies the Global Manager about the number of nodes to be used/released by the node. The API is implemented in the eargm_api.c file.

commands/eargm_new_job and commands/eargm_end_job are very simple commands that implements the two functions offered by the Global Manager API.


License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.  
