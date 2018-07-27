# EAR commands

EAR offers the following commands: 
* Commands to analyze data stored in the DB: eacct and ereport
* Commands to control and temporally modify cluster settings: econtrol
* Commands to create/update the DB: ear_create_database, ear_store_database

# Energy Account (eacct)

# Energy report (ereport)

**econtrol**  
Command to send control instructions to all nodes. 
Usage is:    
- econtrol [options]

The options are the following:
- --set-freq sets the frequency of all nodes to the specified one
- --red-max-freq reduces the maximum frequency
- --red-def-freq reduces the default frequency
- --inc-th increases the threshold for all nodes

At least one option must be specified.

**ear_create_database**  
Creates the database used by EAR's daemon to send metrics and internal data, as well as the daemon's user to use said database. Requires root access to the MySQL server specified via arguments from the location the command is executed. Usage is:  
- ear_create_database [MySQL ip]
