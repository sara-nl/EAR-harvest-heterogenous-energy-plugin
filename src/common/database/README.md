# Database installation

1. Compile and install EAR with the database option enabled.

2. Modify the $ETC_EAR/ear/ear.conf file to specify in the database section the IP of the MySQL server as well as the user, user password (optional) and database port. Make sure that the said user (which may or may not have been created with the ear_create_database command) has the proper permissions towards the database created (only INSERT and SELECT privileges are needed) as well as permission to access the database from foreign IPs (such as the ones from the computation nodes). Said privileges should be granted at database creation, but some MySQL configurations may interfere and privileges will have to be granted manually by the admin.

3. Run the command $EAR_INSTALL_PATH/sbin/ear_create_database, which will create the database and user with the configuration previously specified in the ear.conf file. MySQL's root user needs permission to log from the location in which the command is being executed (if it's being executed locally nothing needs to be done). If the root user has a password, use the _-p_ option which will ask the user to introduce it.

4. Start either the EAR daemon, the slurm controller and daemon with the EAR plugin or the _eard_ service.

5. Run the jobs. The data should be successfully stored when a job is completed.



# Database information

The main tables in the database Report and each of their rows are:

    - Applications:  Contains basic information about where the application was executed (node_id) and the id's of the executed job and the final computated application signature.

    - Jobs:  Contains the information about a job execution [user_id, app_id,start_time, end_time], as well as the EAR parameters defined executon (policy, threshold) and node information (procs, def_f). 

    - Signatures: Contains the information of an application signature computed and used by EAR.

    - Loops: Contains the information of an application loop, its identifying characteristics (type of event, size, level and total iterations), the job it belongs to and the computed signature of
    the loop.

    - Power_signatures: Contains power metrics of an application.
    
    - Periodic_metrics: Contains information about a node's energy consumption with the purpose of monitoring it.

    - Periodic_aggregations: Contains the same information as Periodic_metrics but aggregated by periods of time, not of each node individually.

    - Learning_[tables]: The tables that begin with the "Learning_" prefix contain the same as their counterparts without prefix, but are used exclusively for the learning phase of the EAR library.


-The relationship between the tables can be seen in the adjunct diagram, as 
 well as the general equivalent of their MySQL types.

Other tables are:
    - Events: Contains a registry of EAR events (change in frequency, DynAIS on/off, etc.).
    - Warnings: Contains a registry of EAR's Global Manager warnings.

