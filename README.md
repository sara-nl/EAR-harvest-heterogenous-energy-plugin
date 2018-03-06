# Energy Aware Runtime
Description
-----------
<img src="etc/images/logo.png" align="right" width="140">
Energy Aware Runtime (EAR) library is designed to provide an energy efficient solution for MPI applications. It aims at finding the optimal frequency for a job according to its selected energy policy, being totally dynamic and transparent.

Overview
--------
EAR library interceps the Profiling MPI Interface (PMPI) symbols using the dynamic loader environment variable LD_PRELOAD. The intercepted call is then saved in an internal, fast and small historic buffer. This allows the powerfull DynAIS algorithm to detect the tipical repetitive sequences of code found in the regular HPC applications. Once found a new repetitive sequence, metrics like CPI, bandwith and power are calculated and stored.

This data allows the library to predict the best frequency for the upcoming iterations and in this way, set the proper CPU clock. I.e, if a detected bucle iteration is memory intensive, setting lower clocks holds the performance but saves a lot of power consumption. Because of that, the CPU frequency is constantly switching, maximizing the efficiency of the cluster. For all that privileged functions, the lightweight daemon is provided and must be running in the cluster nodes.

In order to be able to provide good predictions, the library must be trained to obtain good EAR model coefficients. This training is done through a simple and fast tool provided that stresses the nodes and computes their coefficients.

The library is designed to work together with SLURM, a popular cluster job manager. Even so, a couple of scripts are provided in case you want to use directly MPI commands.

Requirements
------------
EAR requires some third party libraríes and headers to compile and run, in addition to the basic requirements such as the compiler and Autoconf. This is a list of this **libraries**, minimum **tested** versions and its references:

| Library   | Required / comment    | Minimum version | References                                        |
| --------- | --------------------- | --------------- | ------------------------------------------------- |
| PAPI      | Yes / with RAPL       | 5.4.3.0         | [Website](http://icl.utk.edu/papi/)               |
| GSL       | Yes                   | 1.4             | [Website](https://www.gnu.org/software/gsl/)      |
| CPUPower  | Yes                   | Kernel 3.10*    | [Information](https://wiki.archlinux.org/index.php/CPU_frequency_scaling) |
| FreeIPMI  | Yes                   | 1.5.7           | [Website](https://www.gnu.org/software/freeipmi/) |
| SLURM     | Just for SLURM plugin | 17.02.6         | [Website](https://slurm.schedmd.com/)             |
| MPI       | Yes                   | -               | -                                                 |
* Depending on the version, may you have to change the name of the library function call (or the parameter).

Also, some **drivers** has to be present and loaded in the system:

| Driver      | File                                    | Kernel version | References      |
| ----------- | --------------------------------------- | -------------- | --------------- |
| CPUFreq     | kernel/drivers/cpufreq/acpi-cpufreq.ko  | 3.10           | [Information](https://wiki.archlinux.org/index.php/CPU_frequency_scaling) |
| OPEN IPMI   | kernel/drivers/char/ipmi/*.ko           | 3.10           | [Information](https://docs.oracle.com/en/database/oracle/oracle-database/12.2/cwlin/configuring-the-open-ipmi-driver.html) |

Lastly, the **compilers**:

| Compiler                          | Comment                    | Minimum version | References      |
| --------------------------------- | -------------------------- | --------------- | --------------- |
| GNU Compiler Collection (GCC)     | For the library and daemon | 4.8.5           | [Website](https://gcc.gnu.org/) |
| Intel Fortran Compiler (MPIIFort) | For some kernels           | 17.0.1          | [Website](https://software.intel.com/en-us/fortran-compilers) |

Brief installation guide
------------------------
1) Generate Autoconf's `configure` program by typing `autoreconf -i`.
2) Compile the library by typing `./configure`, `make` and `make install` in the root directory. Consider the option of `./configure --PREFIX=<path>` if you want to specify the installation path. It could be useful to run ‘./configure --help’ for listing the options details.

Customize installation
----------------------
`configure` is based on shell variables which initial value could be given by setting variables in the command line, or in the environment. Take a look to the table with the most popular variables:

| Variable | Description                                                                                                  |
| -------- | ------------------------------------------------------------------------------------------------------------ |
| DEBUG    | Enables debug messages [0..4]. Debug messages with lower or equal level are printed.                         |
| MPICC    | MPI compiler command.                                                                                        |
| CC       | C compiler command.                                                                                          |
| CFLAGS   | C compiler flags.                                                                                            |
| LDFLAGS  | Linker flags. E.g. ‘-L\<lib dir\>’ if you have libraries in a nonstandard directory \<lib dir\>.             |
| LIBS     | Libraries to pass to the linker. E.g. ‘-l<library>’.                                                         |
| CPPFLAGS | C/C++ preprocessor flags, e.g. -I\<include dir\> if you have headers in a nonstandard directory \<include dir\>. |
- This is an example of `CC`, `CFLAGS` and `DEBUG` variables overwriting:</ br>
`./configure CC=c99 CFLAGS=-g DEBUG=4`

You can choose the root folder by typing `./configure --PREFIX=<path>`. But there is another option `---exec-prefix=<path>` or *EPREFIX*, which by default equals *PREFIX*, which also its default value is `/usr/local/`. You have more installation options information by typing `./configure --help`.

This is the list of installation folder and their content:

| Root          | Directory    | Content / description                |
| ------------- | ------------ | ------------------------------------ |
| \<*PREFIX*\>  | .            | Root folder.                         |
| \<*EPREFIX*\> | /lib         | Libraries.                           |
| \<*EPREFIX*\> | /bin         | Tools and benchmark kernels.         |
| \<*EPREFIX*\> | /bin/kernels | Benchmarks (or stress tests).        |
| \<*EPREFIX*\> | /sbin        | Node daemon.                         |
| \<*PREFIX*\>  | /etc         | Scripts and configuration examples.  |

Adding required libraries installed in custom locations
-------------------------------------------------------
You can help `configure` to find PAPI, SLURM, or other required libraries in case you installed in a custom location. It is necessary to add its root path for the compiler to see include headers and libraries for the linker. You can do this by adding to it the following arguments:

| Argument                 | Description                                  |
| ------------------------ | -------------------------------------------- |
| --with-papi=\<path\>     | Specifies the path to PAPI installation.     |
| --with-gsl=\<path\>      | Specifies the path to GSL installation.      |
| --with-cpupower=\<path\> | Specifies the path to CPUPower installation.  |
| --with-slurm=\<path\>    | Specifies the path to SLURM installation.    |
| --with-freeipmi=\<path\> | Specifies the path to FreeIPMI installation. |
* This is an example of ‘CC‘ overwriting and PAPI path specification:<br />
`./configure --with-papi=/path/to/PAPI`

If unusual procedures must be done to compile the package, please try to figure out how `configure` could check whether to do them and contact the team to be considered for the next release. In the meantime, you can overwrite shell variables or export its paths to the environment (e.g. LD_LIBRARY).

After the installation
----------------------
You can test the installation by typing `make check`. For more info visit the [tests page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/tests/README.md).

First of all, make sure your linker is aware of the required libraries and the environment variable *EAR_INSTALL_PATH* is set. This variable defines the folder of the EAR binaries, libraries and tools. For the ease of use, an **environment module** is also configured next to the EAR compilation. So locate in `etc` folder the module file with name `ear-{version}`, and copy it to your module collection folder (e.g. `cp etc/ear-1.0 /hpc/base/ctt/modulefiles/libraries`).

Configuration
-------------
1) Depending on your environment:
    * In case you are going to use **EAR together with SLURM**, visit the [SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/slurm_plugin/README.md) to add the plugin to your SLURM installation.
    * In case you are going to use **EAR with direct MPI calls**, visit the [scripts page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/etc/scripts/README.md) for more information about the scripts that simplify the launch.
2) Pass the learning phase in all your computing nodes by visiting the [learning phase page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/learning/README.md) to follow its guide.

User guide
----------
Finally, you can launch any MPI application next to EAR library by following the [library user guide](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/src/library/README.md).

Changelog
---------

License
-------
