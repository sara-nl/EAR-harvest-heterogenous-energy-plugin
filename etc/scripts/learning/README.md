Overview of the learning phase
------------------------------

This is a phase prior to the normal EAR utilization, and it is called learning phase since is a kind of hardware characterization of the nodes.

During the learning phase a matrix of coefficients which
allows to predict the power and performance of each repetitive iteration, is computed on all nodes and stored. To get them, during the phase, a set of preselected stress tools or benchmarks (also called kernels), included in the EAR package, are executed using the selected range of frequencies of the system.

**NOTE:** A set of scripts to simplify the learning phase is installed the `bin` folder. However, the details of its functionality will be discussed later. Before of that it is better to understand what is the learning phase. This note was written because those scripts will be referenced during the learning phase overview.

The learning phase consists in four major steps.

Step 1, kernel compilation
--------------------------
The first step is the kernel compilation. In case you want to compile one by one manually, open the script `bin/scripts/learning/helpers/kernels_executor.sh` and look at the function `learning_phase()`. Here you will find all the  paths and compile instructions (make) and install (move) each kernel when `$BENCH_MODE` variable is set to `compile`.

As you can see there are some confusing letters and numbers. This characters are related with the kernel customization, looking for maximize the nodes stress to obtain the maximum quality coefficients.

This customization will take the number of processes to fill the total number of CPUS (not counting hyperthreading). Also a class letter defining the stress level of the benchmark could be required. The class letter goes from lighter levels of stress (A) to heavier levels (D, E...).

You can see some information about all the kernels in the following table:

| Kernel | Reference                                                                        |
| ------ | -------------------------------------------------------------------------------- |
| dgemm  | https://software.intel.com/en-us/mkl-tutorial-c-multiplying-matrices-using-dgemm |
| stream | http://www.cs.virginia.edu/stream/ref.html                                       |
| bt-mz  | https://www.nas.nasa.gov/publications/npb.html                                   |
| sp-mz  | " |
| lu-mz  | " |
| ep     | " |
| lu     | " |
| ua     | " |

Step 2, kernel launch
---------------------
To launch the kernels the cluster queue manager SLURM will be used. To complete the learning phase all the kernels will have to be launched in every node of your cluster in the different selected frequencies. You can also maximize the precision of the gathered data by repeating the execution more than 1 time.

It is required the `srun` SLURM's command or `mpirun` bootstraping with SLURM. The `--ear-learning=$P_STATE` parameter next to the selected P_STATE, allows the EAR SLURM's plugin to execute a kernel in learning phase mode.

These are two examples of the `srun` and `mpirun` commands for a node of 40 CPUs:

`srun -N 1 -n 40 -J "bt-mz" -w node1001 --ear-policy=MONITORING_ONLY --ear-learning=1 /installation.path/bin/kernels/bt-mz.C.40`

`mpirun -n 40 -bootstrap slurm -bootstrap-exec-args="-J 'bt-mz' -w node1001 --ear-verbose=1 --ear-learning=1 /installation.path/bin/kernels/bt-mz.C.40`

It is required to use the same name for a kernel launched at different P_STATEs, because the binary which calculates the coefficients employs the name to classify the kernels.

Step 3, kernel customization
----------------------------
If after the launching of a learning phase kernel at P_STATE 1 the elapsed (in seconds) is between 60 and 120, then it is good quality kernel. In case some benchmarks are not between these times, you can increase or decrease the class letter at compilation time.

If no letter can adjust the kernels to your node, you can surf to every kernel configuration time and switch the values summarized in the following table:

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

Once the customization is done, you have to run again your customized kernels to complete the learning phase. Also, it is recommended to clean the customized kernels records of your database.

Step 4, coefficients computing
------------------------------
Once launched all the kernels at the different frequencies (or P_STATES), the coefficients have to be computed using the installed binary `/bin/compute_coefficients`.

This binary will compute the coefficients and also store the file in the location specified by the configuration file `ear.conf`. There is just one file per node, so the binary have to be run one time per node in a node of the same hardware architecture, because it checks the range of P_STATEs.

The path of the coefficients, the nominal frequency of the node an also de node name have to be passed to correctly compute the coefficients. In case the node name is not present, the binary will get it's the host name.

This is an example:
` ./compute_coefficients /etc/ear/coeffs 2400000 node1001`

Remember to load the EAR module, which specifies the location of the `ear.conf` configuration file.

Automatized kernels compilation script
--------------------------------------
A set of scripts are provided for speed up with minimum edition requirements. These files are placed in the `bin/scripts` folder in your EAR installation folder.

The compiling script is located in `bin/scripts/learning/learning_compile.sh`. Before execute it, you have to perform some adjustments:
1) Open `learning_compile.sh` and look for these lines:
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
3) Launch the compiling phase by typing `./learning_compile.sh` in your compile node.

Also you can easily customize your kernels by adjusting the script located in `bin/scripts/learning/helpers/kernels_iterator.sh`. For example if you want to increase its execution time of a kernel compiled with class letter C, switch it by D. Or if you want to decrease the execution time of a kernel compiled with class letter B, switch the letter by A. Then compile and execute again.

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

Automatized kernels execution script
------------------------------------
Next to the kernels compilation script, the executing version is also provided. Having the kernels compiled, installed and tested, you are ready to execute the learning phase.

 Before that, you have to perform some adjustments:
1) Open the script `bin/scripts/learning/learning_execute.sh`.
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
4) Edit the execution command located in `bin/scripts/learning/helpers/kernels_executor.sh` in the function `launching_slurm()`. By default it will use the `srun` command, but you can switch it by other one, like `mpirun`. Just try to figure how to translate the written command to yours.
5) Execute the learning phase in all of your nodes by typing a command like: `./learning_execute.sh <hostlist>`, passing a the path of a file containing the list of nodes where you want to perform the learning phase. An `sbatch` will be launched exclusively in every node, performing a `srun` series of the kernel in the same node.
6) Execute the coefficients compute binary by typing `./learning_coeffs.sh <hostlist>` in a node which shares the same architecture (or at least the P_STATEs list) of the nodes of the completed learning phase.
7) Check that there are the correct number of coefficients in the selected coefficients installation path.