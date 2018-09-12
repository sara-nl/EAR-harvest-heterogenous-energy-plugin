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
A set of scripts are provided for speed up with minimum edition requirements. These files are placed in the `bin/scripts` folder in your EAR installation folder pointed by *EAR_INSTALL_PATH* variable, which is loaded the environment module. For a detailed information about the content of these scripts visit [EAR scripts page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/etc/scripts/README.md).

Compiling benchmarks
--------------------
1) Open `learning_phase_compile.sh` and look for these lines:
```
# Edit architecture values
export CORES=28
export SOCKETS=2
export CORES_PER_SOCKET=14
```
2) Update the following parameters:<br />
- **CORES**: the total number of cores in a single computing node.<br />
- **SOCKETS**: the total number of sockets in a single computing node.<br />
- **CORES_PER_SOCKET**: the total number of cores per socket in a single computing node.<br />
3) Launch the compiling phase by typing `./learning_phase_compile.sh` in your compile node.

Testing benchmarks
------------------
Once compiled, execute a test in a computing node.

To do that launch `./learning_phase_test.sh <hostlist>`, with the path of a file containg a list of hosts in which you want to perform a kernel test. Also remember to correctly edit the architecture environment variables of the file.

Once the test is done, check the elapsed time of each kernel execution.

If the time (in seconds) of all benchmarks is between 60 and 120, the compilation, installation and test are done and you can proceed to the execution of the learning phase section. In case some benchmarks are not between these times, it is recommended to compile the kernels using different parameters. This is important because improves the quality of EAR prediction model.

Fast benchmark modification
---------------------------
If one of these benchmarks is `lu-mz`, `bt-mz`, `sp-mz`, `lu`, `ep` or `ua`, you could easily alter its behavior by changing its class letter in the file `bin/scripts/learning/helpers/kernels_iterator.sh` script. For example if you want to increase its execution time of a kernel compiled with class letter C, switch it by D. Or if you want to decrease the execution time of a kernel compiled with class letter B, switch the letter by A. Then compile and execute again.

You could see where you have to edit in the following example:
```
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

Benchmarks manual modification
------------------------------
The following table breaks down the key variables and its possible values to alter the behaviour of the whole benchmarks.

| Kernel   | File                                      | Function      | Var   |
| -------- | ----------------------------------------- | ------------- | ----- |
| bt-mz    | NPB3.3.1-MZ/NPB3.3-MZ-MPI/sys/setparams.c | write_bt_info | niter |
| lu-mz    | NPB3.3.1-MZ/NPB3.3-MZ-MPI/sys/setparams.c | write_lu_info | itmax |
| sp-mz    | NPB3.3.1-MZ/NPB3.3-MZ-MPI/sys/setparams.c | write_sp_info | niter |
| ep       | NPB3.3.1/NPB3.3-MPI/sys/setparams.c       | write_ep_info | m     |
| lu       | NPB3.3.1/NPB3.3-MPI/sys/setparams.c       | write_lu_info | itmax |
| ua       | NPB3.3.1/NPB3.3-OMP/sys/setparams.c       | write_ua_info | niter |

Depending on your system you have to increase or decrease its value. As a reference, it is provided a table containing the letter for the script and the value of the variable for a couple of CPU architectures:

| Kernel   | Haswell      | Skylake      |
| -------- | ------------ | ------------ |
| bt-mz    | C / 200      | C / 600      |
| lu-mz    | C / 250      | C / 800      |
| sp-mz    | C / 800      | C / 2000     |
| ep       | D / 36       | D / 37       |
| lu       | C / 250      | C / 750      |
| ua       | C / 200      | C / 200      |

For `dgemm`, you have to edit the file ‘dgemm_example.f’. Take a look to PARAMETER variable definition in the first line, which sets the size of the computing matrix. Increase or decrease that values equally depending if you want to add or subtract computing time.

Once the variable is modified, rerun the test and check again the timings. Once done, you can proceed to the next section and execute the learning phase.

Executing the learning phase
----------------------------
Having the kernels compiled, installed and tested, you are ready to execute the learning phase by the following steps:
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
4) Execute the learning phase in all of your nodes by typing a command like: `./learning_phase_execute.sh <hostlist>`, passing a file containing the list of nodes where you want to perform the learning phase. An `sbatch` will be launched exclusively in every node, performing a `srun` series of the kernel in the same node.
6) Check that there are the correct number of coefficients files in the folder pointed by `EAR_ETC` environment variable.

Editing learning phase SLURM commands
-------------------------------------


