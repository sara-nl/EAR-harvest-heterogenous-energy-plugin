EAR configuration file
----------------------
`ear.conf` is a text file describing the EAR package behavior in the cluster. It must be readable by all compute nodes and by nodes where commands are executed.

Usually the first word in the configuration file expresses the component related with the option. Lines starting with # are comments. Not all the options and arguments are required because some of the components or functionallity could be disabled.


A test for ear.conf file can be found in the path `src/test/functionals/ear_conf`

# Services configuration parameters

1) MariaDB configuration

```
# The IP or hostname of the node where the MariaDB (MySQL) process is running.
MariaDBIp=172.30.2.101
#
MariaDBUser=ear_daemon
#
MariaDBPort=0
#
MariaDBPassw=
#
MariaDBDatabase=EAR_DB
```

2) EAR Global Manager (EARGMD)

```
# Verbose level
GlobalManagerVerbose=1
# Period T1 and period T2 are specified in seconds. T1 must be less than T2. (i.e. 10 min and 1 month)
GlobalManagerPeriodT1=90
GlobalManagerPeriodT2=259200
# Units field, Can be '-' (Joules), 'K' KiloJoules or 'M' MegaJoules
GlobalManagerUnits=K
#
GlobalManagerEnergyLimit=550000
# Use '1' or not '0' aggregated metrics to compute total energy.
GlobalManagerUseAggregated=1
# The IP or hostname of the node where the EARGMD demon is running.
GlobalManagerHost=hostname
# Port where EARGMD will be listening.
GlobalManagerPort=50000
# Global manager modes. Two modes are supported '0' (manual) or '1' (automatic)
GlobalManagerMode=0
# A mail can be sent reporting the warning level (and the action taken in automatic mode). 'nomail' means no mail is sent. This option is independendent of the node
GlobalManagerMail=nomail
# Thee values must be provided corresponding with DEFCON_L4,DEFCON_L3 and DEFCON_L2 (higer values means PANIC).
GlobalManagerWarningsPerc=85,90,95
# Number of "grace" T1 periods before doing a new re-evaluation
GlobalManagerGracePeriods=3
```

3) EAR Daemon (EARD)

```
# The verbosity level [0..4]
NodeDaemonVerbose=1
# Frequency used by power monitoring service, in seconds.
NodeDaemonPowermonFreq=60
# Maximum supported frequency (1 means nominal, no turbo).
NodeDaemonMaxPstate=1
# Enable (1) or disable (0) the turbo frequency
NodeDaemonTurbo=0
# The port where the EARD will be listening
NodeDaemonPort=5000
# Enables the use of the database
NodeUseDB=1
# Inserts data to MySQL sendint that data to the EARDBD (1) or directly (0).
NodeUseEARDBD=1
# '1' means EAR is totally controlling frequencies (targeting production systems) and 0 means EAR will not change the frequencies when users are not using EAR library (targeted to benchmarking systems)
NodeForceFrequencies=1
```

4) EAR Database Manager Daemon (EARDBD)

```
# In seconds, time of accumulating data in every aggregation
DBDaemonAggregationTime=60
# In seconds, time between insert the buffered data
DBDaemonInsertionTime=30
# Port where the EARDBD server is listening
DBDaemonPortTCP=4711 , This port is used for main EARDBD
# Port where the EARDBD mirror is listening
DBDaemonPortSecTCP=4712
# Port is used to synchronize the server and mirror
DBDaemonSyncPort=4713
# Memory allocated per process. It means that if there is a server and mirror in a node a double of that value will be allocated. It is expressed in MegaBytes.
DBDaemonMemorySize=120
# The percentage of the memory block used by each type. These types are: mpi, non-mpi and learning applications, loops, energy metrics and aggregations and events, in that order.
DBDaemonMemorySizePerType=40,20,5,24,5,1,5
```

5) EAR Library (EARL)

```
# Path where coefficients are installed
CoefficientsDir=etc/ear/coeffs
# Number of levels used by DynAIS algorithm.
DynAISLevels=4
# Windows size used by DynAIS, the higher the size the higer the overhead.
DynAISWindowSize=500
# Maximum time in seconds that EAR will wait until a signature is computed. After this value, if no signature is computed, EAR will go to periodic mode.
DynaisTimeout=30
# Time in seconds to compute every application signature when the EAR goes to periodic mode.
LibraryPeriod=30
# Number of MPI calls whether EAR must go to periodic mode or not.
CheckEARModeEvery=1000
```

6) Paths

```
# Path used for communitation files, shared memory, etc. It must be PRIVATE per compute node and with read/write permissions.
TmpDir=/tmp/ear
# Path where coefficients and configuration are stored. It must be readable in all compute nodes.
EtcDir=/etc/ear
# Path where metrics are generated in text files when no database is installed. A suffix is included.
DataBasePathName=/etc/ear/dbs/dbs.
```

7) Energy policies

```
# Default policy (MONITORING_ONLY, MIN_TIME_TO_SOLUTION, and MIN_ENERGY_TO_SOLUTION).
DefaultPowerPolicy=MIN_TIME_TO_SOLUTION
# List of allowed policies for normal users (it is a subset of all three policies).
SupportedPolicies=MONITORING_ONLY,MIN_TIME_TO_SOLUTION
# Starting P_STATE for each policy, specified in the following order: MIN_ENERGY, MIN_TIME and MONITORING_ONLY.
DefaultPstates=1,4,4
# Threshold used for MIN_TIME_TO_SOLUTION policy.
MinEfficiencyGain=0.7
# Threshold used for MIN_ENERGY_TO_SOLUTION policy.
MaxPerformancePenalty=0.1
# Minimum time between two energy readings for performance accuracy
MinTimePerformanceAccuracy=10000000
```

8) Security

Authorized users that are allowed to change policies, thresholds and frequencies are supposed to be administrators. A list of users, Linux groups, and/or SLURM accounts can be provided to allow normal users to perform that actions.

```
AuthorizedUsers=user1,user2
AuthorizedAccounts=acc1,acc2,acc3
AuthorizedGroups=xx,yy
```

Energy tags are pre-defined configurations for some applications (EAR library is not loaded). This energy tags accept a user ids, groups and SLURM accounts of users allowed to use that tag.

```
EnergyTag=memory-intensive pstate=4 users=user1,user2 groups=group1,group2 accounts=acc1,acc2
```

9) Special nodes

Describes nodes with some special characteristic such as different default P_STATEs, default coefficients file and/or policy thresholds.

```
# The 'MaxPerformancePenalty' and 'MinEfficiencyGain' accepts values between 0 and 1.
NodeName=nodename_list CPUs=24 DefaultPstates=2,5,5   DefCoefficientsFile=filename MaxPerformancePenalty=def_th MinEfficiencyGain=def_th
```

10) Island description

Nodes are grouped in islands, this section is mandatory since it is used for cluster description. Normally nodes are grouped in islands shares the same hardware characteristics and also its database daemons (EARDBDS).

Remember that there are two kinds of database daemons. One called 'server' and other one called 'mirror'. Both performs the metrics buffering process, but just one performs the insert. The mirror will do that insert in case the 'server' process crashes or the node fails.

It is recommended that all islands to have symmetry. For example, if the island I0 and I1 have the server N0 and the mirror N1, the next island would have to point the same N0 and N1 or point to new ones N2 and N3.

Multiple EARDBDs are supported in the same island, so more than one line per island is required, but the condition of symmetry have to be met.

It is recommended that for a island to the server and the mirror running in different nodes. However, the EARDBD program could be both server and mirror at the same time. This means that the islands I0 and I1 could have the N0 server and the N2 mirror, and the islands I2 and I3 the N2 server and N0 mirror, fullfing the symmetry requirements.

```
Island=0 Nodes=nodename_list DBIP=EARDB_server_hostname   DBSECIP=EARDB_mirror_hostname
```

Detailed island accepted values:
- nodename_list accepts the following formats:
    - Nodes=`node1,node2,node3`
    - Nodes=`node[1-3]`
    - Nodes=`node[1,2,3]`
- Any combination of the two latter options will work, but if nodes have to be specified individually (the first format) as of now they have to be specified in their own line. As an example:
    - Valid formats:
        - Island=1 Nodes=`node1,node2,node3`
        - Island=1 Nodes=`node[1-3],node[4,5]`
    - Invalid formats:
        - Island=1 Nodes=`node[1,2],node3`
        - Island=1 Nodes=`node[1-3],node4`

Please visit the [islands example](./README.islands.md) for more information and examples of a cluster configuration in form of islands.
