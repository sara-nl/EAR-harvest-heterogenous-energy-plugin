Energy Aware Runtime scripts
----------------------------
A set of bash scripts are provided to make some processes as simple and contained as possible. The two main goals of these scripts are the following:
1) Automatize the learning phase process.
2) If you are not using SLURM:
    * Launch the EAR daemon through SSH.
    * Launch jobs by MPI commands. In this case, the scripts are responsible of launching your applications together with EAR library.

Subfolders and contents
-----------------------
The directory `etc` placed inside the installation path contains a subfolder called `scripts`. Inside, you will find the following subfolders:

The `learning` folder, which contains a set of scripts to compile, test and run the learning phase.
- **learning_phase_compile.sh**: compiles in the local node all the benchmarks that are part of the learning phase.
- **learning_phase_execute.sh**: performs the complete tour of the learning phase, including all the benchmarks for all the selected frequencies. This script is launched by your `mpirun` command or the `mpi_exec.sh` script, not directly executed locally by `./learning_phase_execute.sh`. This is because all the nodes have to execute this script completely in isolation, hence first you have to give instructions to those nodes to execute the script.
- **learning_phase_helper.sh**: wraps previous learning phase scripts functions, avoiding the repetition of code.

The `running` folder, which contains a set of scripts for launch MPI applications. You can launch these scripts with empty parameters to view its usage.
- **mpi_exec.sh**: contains the `mpirun` command with a prepared environment to launch the application next to the EAR library in a set of specified nodes. Depending on the MPI distribution, may be is necessary to edit the launching command, because this could make not working other scripts:
```
## Starting the application
mpiexec.hydra -l -genv LD_PRELOAD=${EAR_LIB_PATH} -genvall ${MPI_HOST} -n ${MPI} -ppn=${PPN} ${BINARY}
```
So, head to this line and replace the `mpiexec.hydra` by `mpirun` similar.
- **ssh_daemon_start.sh**: loads the EAR daemon in a a specified set of nodes.
- **ssh_daemon_exit.sh**: closes the EAR daemon of a specified set of nodes.

The `environment` folder, which contains two script which defines the environment of the daemon and the library:
- **ear_vars.sh**: defines the EAR environment and therefore the behaviour of the library and daemon by default. Please, head [environment variables configuration page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/etc/README.md) to learn more about this variables and customize this file values to fit the needs of your cluster.
- **lib_vars.sh**: defines third party libraries paths. These paths were written during the `configure`.

The `examples` folder just contains some examples of the use of these scripts.

Script dependancies
-------------------
The following picture shows the dependancies between these scripts. This way of organizing favors the simplicity of editing, allowing that smallest change to take effect on all scripts.

<img src="https://github.com/BarcelonaSupercomputingCenter/EAR/blob/new_kernel_params/etc/images/scripts.png" align="left">
