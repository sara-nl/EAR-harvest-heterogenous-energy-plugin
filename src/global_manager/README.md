# Energy Aware Runtime: Global Manager
Description
-----------

EAR a framework to control the energy at different contexts. The EAR Global Manager is a cluster wide components controlling the energy consumed during a given perid T2 (configurable) every T1 seconds (also configurable). The EAR Global Manager is controlling the percetage of the maximum energy consumed and it can be configured to take actions automatically or just report warning levels and let the actions to be taken by sysadmins. 

Requirements
------------

EAR Global Manager uses periodic power metrics reported by [EARD](../daemon/README.md). EARD is a per-node daemon and it periodically reports the dc energy consumed in the node, including job identification details (job id, step id when executed in a slurm system). These metrics are stored and aggregated in a MariaDB DB through the [EAR DB Manager](../database_daemon/REAME.md).

Brief installation guide
------------------------

EAR includes service files to be used to start/stop the components. Global Manager can be started with the $ETCDIR/systemd/eargmd.service file. By executing the traditional "make install" command, EAR service files are copied in the /etc/systemd/system folder.

sudo systemctl start eargmd --> will start the service
sudo systemctl stop eargmd --> will stop the service
sudo systemctl reload eargmd --> will force to reload the configuration 

Customize installation
----------------------

The Global Manager uses the $ETCDIR/ear.conf file to specify T1, T2 periods as weel as the Energy limit. The Global Manager can be dynamically configured by reloading the ear.conf file:

sudo systemctl reload eargmd

Changelog
---------

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.  
