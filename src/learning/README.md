Energy Aware Runtime learning phase
-----------------------------------
This is a phase prior to the normal EAR utilization, and it is called learning phase since is a kind of hardware characterization of the nodes. During the learning phase a matrix of coefficients which caracterizes the relationship between power and performance, is computed on all nodes and stored. To get them, during the phase, a set of preselected stress tools or benchmarks (also called kernels) are executed with the range frequencies of the system. 

Benchmarks or stress test
-------------------------
Benchmarks are the stressing programs which learning phase will execute to get node's coefficient characterization. These coefficients will be used later by the EAR library to dynamically adjust each node frequency during an application execution, to save energy.

| File   | Reference                                                                        |
| ------ | -------------------------------------------------------------------------------- |
| dgemm  | https://software.intel.com/en-us/mkl-tutorial-c-multiplying-matrices-using-dgemm |
| stream | http://www.cs.virginia.edu/stream/ref.html                                       |
| bt-mz  | https://www.nas.nasa.gov/publications/npb.html                                   |
| sp-mz  | " |
| lu-mz  | " |
| ep     | " |
| lu     | " |
| ua     | " |

Saved files
-----------
Visit [EAR lib page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/library/README.md) for more information.

Execution of the learning phase basics
--------------------------------------
Three scripts are provided for speed up with minimum edition requirements. These files are placed in the `etc` folder in your EAR installation folder pointed by *EAR_INSTALL_PATH*, variable which is loaded the environment module. They are `learning_phase_compile.sh`, `learning_phase_execute.sh` and `learning_phase_helper.sh`.

`learning_phase_helper.sh` used by the others, and doesn't have to be launched. `learning_phase_compile.sh` is used for compile the kernels and could be launched without any job scheduler if you have access to a compile node. Also this script could be used to test the execution times of the benchmarks prior to execute the learning phase. `learning_phase_execute.sh` as the name says, it is used to perform a complete learning phase.

Compiling benchmarks
--------------------
1) Open `learning_phase_compile.sh` and look for these lines:
```
# Edit architecture values
export CORES=28
export SOCKETS=2
export CORES_PER_SOCKET=14

# Update the paths
export EAR_SRC_PATH=$HOME/git/EAR
```
2) Update the following parameters:<br />
- **CORES**: the total number of cores in a single computing node.<br />
- **SOCKETS**: the total number of sockets in a single computing node.<br />
- **CORES_PER_SOCKET**: the total number of cores per socket in a single computing node.<br />
- **EAR_SRC_PATH**: the path to the place where you unzipped EAR package.<br />
3) Launch the compiling phase by typing `learning_phase_compile.sh compile` in your compile node.

Testing benchmarks
------------------
Once compiled, execute a test in a computing node. If you're using SLURM, launch a command like `srun -N1 -n1 --exclusive learning_phase_compile.sh test`, allocating a single but complete computing node (because the script will open new threads as it needs).

If you are doing it manually, follow the following steps:
1) Open the script `learning_phase_helper` and look for these lines:
```
function launching
{
    ...
}

function launching_disabled
{
    # Edit region
    export EAR_TMP=/tmp
    export EAR_COEFF_DB_PATHNAME=$EAR_TMP/coeff
    export EAR_USER_DB_PATHNAME=$EAR_TMP/sum
    export EAR_DB_PATHNAME=$EAR_TMP/db
    ...
    mpiexec.hydra -n $2 -genvall -genv                 \
        LD_PRELOAD=$EAR_INSTALL_PATH/ear/lib/libEAR.so \
        $BENCHS_BIN_PATH/$1
}
```
2) Replace the word `_disabled` from `launching_disabled` adding it to the current `launching` one. 
3) Edit the environment variables of the edit region for the paths where you want to store the files. For more information about these environment variables and its default values visit the [SLURM plugin configuration section](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_slurm_plugin/README.md).
4) Launch the test using your MPI scheduling program by typing something like `mpirun -n 1 -host computin_node1 learning_phase_compile.sh test`.

Once the test is done, inspect the summary file pointed by the environment variable EAR_USER_DB_PATHNAME of your ‘ear.conf’ file, in case you are using SLURM, or you wrote manually in the helper file. Look at the field time (in brackets):
```
USERNAME;JOB_ID;NODENAME;APPNAME;FREQ;[[TIME]];CPI;TPI...
xuser;838;xnode;bt-mz.C.28;2600000;[[35.460251]];0.401421;1.560220...
xuser;838;xnode;sp-mz.C.28;2600000;[[52.380336]];0.500832;4.614459...
xuser;838;xnode;stream_mpi;2600000;[[83.232500]];2.714464;87.458571...
xuser;838;xnode;ep.D.28;2600000;[[307.452702]];1.647134;0.020879;0.029462...
```
If the time (in seconds) of all benchmarks is between 60 and 120, the compilation, installation and test are done and you can proceed to the execution of the learning phase section. In case some benchmarks are not between these times, it is recommended to compile using different parameters. This is important because improves the quality of EAR prediction model.

Fast benchmark modification
---------------------------
If one of these benchmarks is `lu-mz`, `bt-mz`, `sp-mz`, `lu`, `ep` or `ua`, you could easily alter its behavior by changing its class letter in the same `learning_phase_compile.sh` script. For example if you want to increase its execution time of a kernel compiled with class letter C, switch it by D. Or if you want to decrease the execution time of a kernel compiled with class letter B, switch the letter by A. Then compile and execute again. 

You could see where you have to edit in the following example:
```
# Compiling or executing the different kernels
learning-phase lu-mpi C
learning-phase ep D
learning-phase bt-mz C
learning-phase sp-mz C
learning-phase lu-mz C
learning-phase ua C
learning-phase dgemm
learning-phase stream
```
As you can see, there are no class letters for `dgemm` or `stream` kernels. Stream is a well known benchmark and there is no need to manual modification because varies its behavior itself. For `dgemm` or for a class letter benchmark which doesn’t fit in your goals, it’s recommended to do a manual kernel modification. 

Source benchmark modification
-----------------------------
Depending on the kernel, modify its key file inside ‘ear_learning_phase/kernels‘ path of the uncompressed EAR package:
- **bt-mz**, **sp-mz** and **lu-mz**: `NPB3.3.1-MZ/NPB3.3-MZ-MPI/sys/setparams.c`
- **lu** and **ep**: `NPB3.3.1/NPB3.3-MPI/sys/setparams.c`
- **ua**: `NPB3.3.1/NPB3.3-OMP/sys/setparams.c`
- **stream**: `STREAM/stream_mpi.f`
- **dgemm**: `DGEMM/mkl_fortran_samples/matrix_multiplication/src/dgemm_example.f`

As you can see, some of themm shares a file called `setparams.c`. If you want to add or subtract load, locate a function called `write_sp_info` in this file, the class letter you want to use it, and modify the number of iterations variable `niter`. 

For `dgemm`, you have to edit the file ‘dgemm_example.f’. Take a look to PARAMETER variable definition in the first line, which sets the size of the computing matrix. Increase or decrease that values equally depending if you want to add or subtract computing time. Lastly, th

Executing the learning phase
----------------------------
Once the kernels have been compiled, installed and tested, you are ready to execute the learning phase by the following steps:
1) Open the script `learning_phase_execute.sh`. This script will launch all kernels in all nodes in isolation, stressing the machine and saving metrics to train the EAR prediction model.
2) Look at these lines
```
# Edit architecture values
export CORES=28
export SOCKETS=2
export CORES_PER_SOCKET=14

# Edit learning phase parameters
export EAR_MIN_P_STATE=1
export EAR_MAX_P_STATE=6
```
3) Update the following parameters:<br />
- **CORES**: the total number of cores in a single computing node.<br />
- **SOCKETS**: the total number of sockets in a single computing node.<br />
- **CORES_PER_SOCKET**: the total number of cores per socket in a single computing node.<br />
- **EAR_MIN_P_STATE**: defines the maximum frequency to set during the learning phase. The default value is 1, meaning that the nominal frequency will be the maximum frequency that your cluster nodes will set. In the current version of EAR turbo support is not included.<br />
- **EAR_MAX_P_STATE**: defines the minimum frequency to test during the learning phase. If 6 is set and EAR_MIN_P_STATE is 1, it means that 6 frequencies will be set during the learning phase, from 1 to 6. This set of frequencies have to match with the set of frequencies that your cluster nodes are able to set during computing time.<br />
4) If you are using SLURM, execute the learning phase in all your nodes by typing a command like: `srun -N4 -n4 --exclusive -t 180 learning_phase_execute.sh`. In this example, it was did it in 4 nodes.
5) If you are doing it manually, execute the MPI scheduling program of your MPI distribution by typing something like: `mpirun -n 4 -ppn 1 -hosts node1,node2,node3,node4 learning_phase_execute.sh`
6) Check that there are the correct number of coefficients files stored in the folder pointed by the environment variable *EAR_COEFF_DB_PATHNAME*.
