# EAR commands

EAR offers the following commands: 
* Commands to analyze data stored in the DB: eacct and ereport
* Commands to control and temporally modify cluster settings: econtrol
* Commands to create/update the DB: ear_create_database, ear_store_database

All these commands read the EAR configurarion file (ear.conf) to determine if the user is an authorized (or not user). Root is a special case, it doesn't need to be included in the list of authorized users. Some options are disables when the user is not authorized. 

# Energy Account (eacct)

The eacct command shows accounting information stored in the EAR DB for jobs (and step) IDs. It provides the following options. 

Usage: eacct [Optional parameters]
	Optional parameters: 
		-h	displays this message
		-v	verbose mode for debugging purposes
		-u	specifies the user whose applications will be retrieved. Only available to privileged users. [default for authorized users: all users]
		-j	specifies the job id and step id to retrieve with the format [jobid.stepid]. A user can only retrieve its own jobs unless said user is privileged. [default: all jobs]
		-l	shows the information for each node for each job instead of the global statistics for said job.
		-n	specifies the number of jobs to be shown, starting from the most recent one. [default: all jobs]


# Energy report (ereport)

The ereport command  create reports from the information stored in the EAR DB concerning energy.

# Energy control (econtrol)

The econtrol command modifies cluster settings (temporally) related to power policy settings. These options are sent to all the nodes in the cluster.


Usage: econtrol [options]
	--set-freq 	newfreq		->sets the frequency of all nodes to the requested one
	--set-def-freq 	newfreq		->sets the default frequency
	--set-max-freq 	newfreq		->sets the maximum frequency
	--inc-th 	new_th		->increases the threshold for all nodes
	--red-def-freq 	reduction	->reduces the default frequency
	--restore-conf 			->restores the configuration to all nodes

# Database commands

- ear_create_database: Creates the EAR DB used for accounting and for the global energy control. Requires root access to the MySQL server. It reads the ear.conf to get connection details and DB name.

- ear_store_database: ??

