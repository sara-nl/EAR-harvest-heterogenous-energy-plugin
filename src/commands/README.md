# EAR commands


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

**ereport**  
Summarizes the information of a job given by argument, specifying information of each node as well as an average.
Usage is:
- ereport [job_id] [options]

If EAR is compiled without database support, usage is:
- ereport [job_id] [db files path] [options]

Options are:
- -v Makes the command run in verbose mode. This is the only option available if EAR is compiled without database support.
- -u Specifies the MySQL user that will execute the query to report jobs. [default: root]
- -db Specifies the MySQL database from which data will be queried. [default: Report]
- -ip Specifies where the MySQL server can be found. [default: 127.0.0.1]

If EAR is compiled with both database support and file saving support, the default behaviour of **ereport** will be the one with MySQL querying. If a file wants to be used as input a new option can be used:
- -f specifies the file to be used as input to ereport.

