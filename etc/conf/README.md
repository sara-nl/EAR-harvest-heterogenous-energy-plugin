EAR configuration file
----------------------
ear.conf is a text file describing EAR options cluster description. It must be readable at all compute nodes and at nodes where commands are executed. Lines starting with # are comments. Some of the arguments are optional. A test for ear.conf file can be found at src/test/functionals/ear_conf

Parameters
----------

# Services configuration

1-Mariadb CONFIGURATION

**MariaDBIp**=172.30.2.101  
**MariaDBUser**=ear_daemon  
**MariaDBPassw**=  
**MariaDBPort**=0  
**MariaDBHost**=hostname  
**MariaDBDatabase**=EAR_DB  

2-EAR Global Manager

**GlobalManagerVerbose**=1   
**GlobalManagerPeriodT1**=90, Period T1 and Period T2 are specified in seconds T1 (ex. must be less than T2, ex. 10min and 1 month)  
**GlobalManagerPeriodT2**=259200  
**GlobalManagerEnergyLimit**=550000 , expressed in Joules  
**GlobalManagerHost**=name  
**GlobalManagerPort**=50000 , port were EARGM will be executed  
**GlobalManagerMode**=0, Two modes are supported 0=manual 1= means automatic  
**GlobalManagerMail**=nomail, independetly on the mode, a mail can be sent reporting the warning level (and the action taken in automatic mode). nomail means no mail is sent  
**GlobalManagerWarningsPerc**=85,90,95 , Thee values must be provided corresponding with DEFCON_L4,DEFCON_L3, and DEFCON_L2 (higer values means PANIC)  
**GlobalManagerGracePeriods**=3 ,number of "grace" T1 periods before doing a new re-evaluation  

3-EARD

**NodeDaemonVerbose**=1, different verbose levels are supported (0..4)  
**NodeDaemonPowermonFreq**=60,  in seconds  
**NodeDaemonMaxPstate**=1 , 1 means nominal frequency (no turbo)  
**NodeDaemonTurbo**=0 , 0 means no turbo frequency  
**NodeDaemonPort**=5000  

4-DATABASE MANAGER

**DBDaemonAggregationTime**=300  , in seconds  
**DBDaemonPortTCP**=4711  
**DBDaemonPortUDP**=4712  
**DBDaemonMemorySize**=120, Max memory size used by EARDB. Memory size expressed in MB  

# Library configuration

**CoefficientsDir=** (ETC)/ear/coeffs , path where coefficients are installed  
**DynAISLevels**=4, number of levels used by DynAIS algorithm  
**DynAISWindowSize**=500,  Windows size used by DynAIS, the higher the size the higer the overhead 
**DynaisTimeout**=30 ,Maximum time (in seconds) EAR will wait until a signature is computed. After DynaisTimeout seconds, if no signature is computed, EAR will go to periodic mode   
**LibraryPeriod**=30, When EAR goes to periodic mode, it will compute the Application signature every "LibraryPeriod" seconds  
**CheckEARModeEvery**=1000, EAR will check every N mpi calls whether it must go to periodic mode or not  


# Paths
**TmpDir**=/var/ear, this path is used for communitation files, shared memory etc. It must be PRIVATE per compute node and with read/write permissions  
**EtcDir**=/etc/ear, etc path where coefficients and configuration are stored. It must be readable in all compute nodes (it can be replicated).   
**DataBasePathName**=/etc/ear/dbs/dbs., when no DB is installed, metrics are generated in text files.  


# Energy policies configuration   
**DefaultPowerPolicy**=MIN_TIME_TO_SOLUTION , default policy 
**SupportedPolicies**=MONITORING_ONLY,MIN_TIME_TO_SOLUTION  , list of allowed policies for normal users  (it is a subset of MONITORING_ONLY,MIN_TIME_TO_SOLUTION,MIN_ENERGY_TO_SOLUTION)  
**DefaultPstates**=1,4,4, specified in the following order: MIN_ENERGY_TO_SOLUTION,MIN_TIME_TO_SOLUTION,MONITORING_ONLY  
**MinEfficiencyGain**=0.7,threshold used for MIN_TIME_TO_SOLUTION policy  
**MaxPerformancePenalty**=0.1, threshold used for MIN_ENERGY_TO_SOLUTION policy  
**MinTimePerformanceAccuracy**=10000000, min time between two energy readings for performance accuracy

# Security configuration

Authorized users, groups, and slurm accounts are allowed to change policies, thresholds, frequencies etc they are supposed to be admins 

**AuthorizedUsers**=user1,user2  
**AuthorizedAccounts**=acc1,acc2,acc3  
**AuthorizedGroups**=xx,yy  


List of energy tags and users/groups/slurm accounts authorized to use each one. These energy tags implies pre-defined configurations for applications (EAR library is not loaded)   
**EnergyTag**=memory-intensive **pstate**=4 **users**=user1,user2 **groups**=group1,group2 **accounts**=acc1,acc2  

# Special nodes

**NodeName**=r22u[21,23,25,27] **CPUs**=24 **DefaultPstates**=2,5,5   , describes nodes with some special characteristic  

# Island description

Nodes are grouped in islands, this section is mandatory since it is used for cluster description
more than one line per island must be supported to specify dbip ports

**Island**=0 **Nodes**=r22u21,r22u23,r22u25,r22u27 **DBIP**=r22u21.hpc.eu.lenovo.com   

