Energy Aware Runtime scripts
----------------------------
A set of bash scripts are provided to make the process as simple and contained as possible. The two main goals of these scripts are the following:
1) Automatize the learning phase process.
2) If you are not using SLURM:
    * Launch the EAR daemon through SSH.
    * Launch jobs by MPI commands.

Subfolders and contents
-----------------------
The directory `etc` placed inside the installation path contains a subfolder called `scripts`. Inside, you will find the following subfolders:

The `running` folder, which contains a set of scripts for launch MPI applications.
- **mpi_exec.sh**: contains the `mpirun` command with a prepared environment to launch the application next to the EAR library in a set of specified nodes. Depending on the MPI distribution, it may be necessary to edit the `mpirun` comand by `mpiexec.hydra` or similar.
- **ssh_daemon_start.sh**: loads the EAR daemon in a a specified set of nodes.
- **ssh_daemon_exit.sh**: closes the EAR daemon of a specified set of nodes.

The `learning` folder, which contains a set of scripts to compile, test and run the learning phase.
- **learning_phase_compile.sh**: compiles in the local node all the benchmarks that are part of the learning phase.
- **learning_phase_execute.sh**: performs the complete tour of the learning phase, including all the benchmarks for all the selected frequencies. This script is launched by `mpi_exec.sh` script, not directly executed locally by `./learning_phase_execute.sh`. This is because all the nodes have to execute this script completely, unlike other scripts that  remotely execute just some instruction or third party programs in the cluster nodes.
- **learning_phase_helper.sh**: wraps all the learning phase functions, avoiding the repetition of functions and code.

The `environment` folder, which contains two script which defines the environment of the daemon and the library:
- **ear_vars.sh**: defines the EAR environment. You could over write the variables in order to change the library or daemon behaviour.
- **lib_vars.sh**: defines third party libraries path. These paths were written when `configure` was launched.

The `examples` folder just contains some examples of the use of these scripts.

Script dependancies
-------------------
The following picture shows the dependancies between these scripts. This way of organizing favors the simplicity of editing, allowing that smallest change to take effect on all scripts.

<img src="https://github.com/BarcelonaSupercomputingCenter/EAR/blob/new_kernel_params/etc/images/scripts.png" align="left">
