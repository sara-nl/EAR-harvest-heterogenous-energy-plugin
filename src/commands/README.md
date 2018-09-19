# EAR commands

EAR offers the following commands: 
* Commands to analyze data stored in the DB: eacct and ereport
* Commands to control and temporally modify cluster settings: econtrol
* Commands to create/update the DB: ear_create_database

All these commands read the EAR configurarion file (ear.conf) to determine if the user is an authorized (or not user). Root is a special case, it doesn't need to be included in the list of authorized users. Some options are disables when the user is not authorized. 

# Energy Account (eacct)

The eacct command shows accounting information stored in the EAR DB for jobs (and step) IDs. It provides the following options. 
```
Usage: eacct [Optional parameters]
	Optional parameters: 
		-h	displays this information
		-u	specifies the user whose applications will be retrieved. Only available to privileged users. [default for authorized users: all users]
		-j	specifies the job id and step id to retrieve with the format [jobid.stepid]. A user can only retrieve its own jobs unless said user is privileged. [default: all jobs]
		-l	shows the information for each node for each job instead of the global statistics for said job.
		-n	specifies the number of jobs to be shown, starting from the most recent one. [default: all jobs]
        -t  specifies the energy tag to filter the retrieved jobs. [default: all tags]

```


# Energy report (ereport)

The ereport command creates reports from the information stored in the EAR DB concerning energy.
...
Usage: ereport [Optional parameters]
    Optional parameters:
        -s  start_time  indicates the starting period from which the energy information will be computed. Format: YYYY-MM-DD. Default: 1970-01-01.
        -e  end_time    indicates the end of the period from which the energy information will be computed. Format: YYYY-MM-DD. Default: current time.
        -n  node_name   indicates from which node the energy information will be computed. Default: none (all nodes computed together). 'all' option will show all nodes computed individually.
        -u  user_name   requests the energy consumed by a user in the selected period of time. Default: none (not filtering by user). 'all' option will show all users individually.
        -t  energy_tag  requests the energy consumed filtering by the selected energy tag. Default: none (not filtering by energy_tag). 'all' option will show all tags individually.
        -h              displays this information

# Energy control (econtrol)

The econtrol command modifies cluster settings (temporally) related to power policy settings. These options are sent to all the nodes in the cluster.

```
Usage: econtrol [options]
	--set-freq 	newfreq		    ->sets the frequency of all nodes to the requested one
	--set-def-freq 	newfreq		->sets the default frequency
	--set-max-freq 	newfreq		->sets the maximum frequency
	--inc-th 	new_th		    ->increases the threshold for all nodes
	--red-def-freq 	reduction	->reduces the default frequency
	--restore-conf 			    ->restores the configuration to all nodes
```

# Database commands

- ear_create_database: Creates the EAR DB used for accounting and for the global energy control. Requires root access to the MySQL server. It reads the ear.conf to get connection details and DB name.

