EAR Library 
------------------
The EAR library is the core of the EAR package. The EARL offers a ligthweigth and simple solution to select the optiomal frequency for MPI applications at runtime. EARL is dynamically loaded with applications using the PMPI interface used by many other runtime solutions. The current EARL version is only supported with this mechanism but it is under development an API to be inserted in the OpenMPI library. However, the actual distribution includes EARL versions to be dynamically loaded with Intel and OpenMPI libraries.  

At runtime, EARL goes trough the following phase:
1. Automatic detection of application outer loops. This is done using DynAIS, our own Dynamic Application Iterative Structure detector algorithm. Dynais is highly optimized for new Intel architectures reporting a low overhead. 
2. Computation of application Signature. Once Dynais starts reporting iterations for the outer loop, EAR starts computing the application signature. The application signature includes: CPI, Iteration time, DC node Power, and TPI (transactions per instruction). Since DC node power measurements error highly depends on the hardware, EAR automatically detects the hardware characteristics and sets a minimum time to compute the signature in order to minimize the average error.
3. Project performance and power model. EAR incorporate one performance and power models. These models uses, as input, the application signature and the system signature. The system signature is a set of coefficients characterizing each node in the system. They are computed at EAR installation time (tipically one once). EAR projects power and performance (time), for all the available frequencies in the system.
4. Apply the selected power policy. EAR includes two power policies selected at runtime: Minimize time to solution and Minimize energy to solution. These policies are configured by the sysadmin, as well as which policies can be used by users. At this point , EAR executes the power policy, using the projections computed in the previous phase, and selects the optimal frequency for this application and this particular run.


How to run MPI applications with EARL
-------------------------------------

To load EARL with MPI jobs, it is only needed to set the LD_PRELOAD environment variable. EARL will be loaded at runtime and MPI calls will be intercepted calling EAR API automatically. 



EAR SLURM
-------------------------
Just call the `srun` program with, at least, the argument `--ear`, which loads the EAR library together with your application using just the default configuration. For a complete list of parameters, please visit the [SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/tree/development/ear_slurm_plugin).

In example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`

Launching jobs calling MPI directly
-----------------------------------
This way doesn't make use of any cluster job scheduler, so a couple scripts are provided to make it easy that task. You can launch these scripts with empty parameters to view it's usage.

0) In case the daemon is not running in the node, execute the script `etc/scripts/running/ssh_daemon_start` by typing `./ssh_daemon_start computing_node1 1 /tmp`, where `computing_node1` is the node name where the daemon will be launched and the last number the default *P_STATE*. It also accepts a file containing the list of nodes where you want to wake that daemon. The last parameter is the place where the data base files will be stored.
1) In the same folder, execute the `mpi_exec.sh` script to launch the job. In example `./mpi_exec.sh computing_node1 28 28 MONITORING_ONLY`, where both numbers are the MPI processes and the MPI's per node, an the last one is the policy.

Stored files
------------
Three types of files will be generated during the learning phase or any job submission.

The first type, the **coefficient files**, contains EAR prediction model trined values, or a hardware signature of each node. There are as many files as nodes and number of P_STATEs os these nodes (nodes in the cluster * number of P_STATEs). Once stored, these files remains untouched and must be write protected against users. Its storing path is defined by the environment variable `*EAR_USER_DB_PATHNAME*`

In case a node doesn’t find its coefficients files, it will search in the location pointed by the environment variable plus the word default and node available frequencies suffixes: `$EAR_COEFF_DB_PATHNAME_default{frequency}`.

The second type are **databases** which stores metrics about the previously executed applications of all users as historic in plaintext form, meaning that new data is appendded by any normal user job submission (including learning phase). The coefficients files are generated from these database files when nothing more than learning phase benchmarking tools are registered in that files. Its storing path is defined by the environment variable `*EAR_DB_PATHNAME*`

The third type of files are **plaintext summaries**, a record of the launched applications that by the user. Are generated, like the database files, on every user job submission. Its storing path is defined by the environment variable ’*EAR_USER_DB_PATHNAME*’, but if you are using SLURM, you can also use the argument `--ear-user-db`.

