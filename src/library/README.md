Library User Guide
------------------
The library is the core of the EAR package. It has the most of the functionality, via the interception of MPI function calls, and also controls other components of the EAR package like the daemon.

Configuration
-------------
The library behaviour depends on variables defined in environment. You could find a complete list of these environment variables and its default values in [SLURM plugin configuration section](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/slurm_plugin/README.md).

Manual Execution
----------------
0) Remember to pass first the learning phase to get and store all node coefficients if you want to launch a normal job (see [learning phase page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/learning/README.md) for more information).
1) Make sure the EAR daemon is running (see [daemon manual execution section](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/daemon/README.md)).
2) Append the path of the library (**libEAR.so**) to LD_PRELOAD environment variable.
3) Launch your application.

Execution managed by EAR SLURM plugin
-------------------------------------
For a detailed guide, please visit the
[SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/tree/development/ear_slurm_plugin).

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
