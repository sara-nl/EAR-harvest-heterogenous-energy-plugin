Library User Guide
------------------
The library is the core of the EAR package. It has the most of the functionality, via the interception of MPI function calls, and also controls other components of the EAR package like the daemon.

Options
-------
The library behaviour depends on variables defined in environment. You could find a complete list of these environment variables and its default values in [environment variables list](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/etc/README.md).

Make sure to understand especially the variables **EAR_POWER_POLICY**, which controls the EAR policy, and also **EAR_USER_DB_PATHNAME** and **EAR_APP_NAME**.

Launching jobs with SLURM
-------------------------
Just call the `srun` program with, at least, the argument `--ear`, which loads the EAR library together with your application using just the default configuration. For a complete list of parameters, please visit the [SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/tree/development/ear_slurm_plugin).

In example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`

Launching jobs calling MPI directly
-----------------------------------
This way doesn't make use of any cluster job scheduler, so a couple scripts are provided to make it easy that task. You can launch these scripts with empty parameters to view it's usage.

0) In case the daemon is not running in the node, execute the script `etc/scripts/running/ssh_daemon_start` by typing `./ssh_daemon_start computing_node1 1`, where `computing_node1` is the node name where the daemon will be launched and the last number the default *P_STATE*. It also accepts a file containing the list of nodes where you want to wake that daemon.
1) In the same folder, execute the `mpi_exec.sh` script to launch the job. In example `./mpi_exec.sh computing_node1 28 28 MONITORING_ONLY`, where both numbers are the MPI processes and the MPI's per node, an the last one is the policy.

Stored files
------------
Three types of files will be generated during the learning phase or any job submission.

The first type, the **coefficient files**, contains EAR prediction model trined values, or a hardware signature of each node. There are as many files as nodes and number of P_STATEs os these nodes (nodes in the cluster * number of P_STATEs). Once stored, these files remains untouched and must be write protected against users. Its storing path is defined by the environment variable `*EAR_USER_DB_PATHNAME*`

In case a node doesn’t find its coefficients files, it will search in the location pointed by the environment variable plus the word default and node available frequencies suffixes: `$EAR_COEFF_DB_PATHNAME_default{frequency}`.

The second type are **databases** which stores metrics about the previously executed applications as historic in binary form, meaning that new data is appendded by any normal user job submission (including learning phase). The coefficients files are generated from these database files when nothing more than learning phase benchmarking tools are registered in that files. Its storing path is defined by the environment variable `*EAR_DB_PATHNAME*`

The third type of files are **plaintext summaries**, a record of the launched applications that can be consulted by users or administrators. Are generated, like the database files, on every user job submission. Its storing path is defined by the environment variable ’*EAR_USER_DB_PATHNAME*’, but if you are using SLURM, you can also use the argument `*--ear-user-db*`

Traces and graphical interface
------------------------------
In development.
