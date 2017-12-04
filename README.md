# Energy Aware Runtime
Description
-----------
<img src="https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/misc/logo.png" align="right" width="140">
Energy Aware Runtime (EAR) library is designed to provide an energy efficient solution for MPI applications. It aims at finding the optimal frequency for a job according to its selected energy policy, being totally dynamic and transparent. The library is designed to work independently of any scheduler or cluster manager, however we provide a SLURM plugin to meake easy its utilization in an environment with a large number of nodes.

Overview
--------
EAR library interceps the Profiling MPI Interface (PMPI) symbols using the dynamic loader environment variable LD_PRELOAD. The intercepted call is then saved in an internal, fast and small historic, which allows to our powerfull DynAIS algorithm to detect the tipical repetitive sequences of code found in the regular HPC applications (see [library page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_lib/README.md) for more information about DynAIS). Once found a new repetitive sequence,  metrics like CPI, bandwith or power are calculated and stored.

This data allows the library to predict the best frequency for the upcoming iterations and set the proper CPU clock. I.e, if a detected bucle iteration is memory intensive, setting lower clocks holds the performance but saves a lot of power consumption. Because of that, the CPU frequency is constantly switching, maximizing the efficiency of the cluster.

In order to be able to provide good predictions, the library must be trained to obtain good EAR model coefficients. This training is done through a simple and fast provided tool that stress the machine and computes this coefficients. Also, to interact with protected SO calls and files the EAR privileged daemon must be running.

Components summary
------------------
EAR package is then divided in 4 main components.

1) EAR library, which holds the main functionality and is loaded next to your job application. Visit [EAR lib page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_lib/README.md) for more information.
2) EAR daemon, which manages the privileged reads, writes and calls required by the library. Visit [EAR daemon page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_daemon/README.md) for more information.
3) EAR learning phase tools, which computes node coefficients. Visit [EAR learning phase page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_learning_phase/README.md) for more information.
4) EAR slurm plugin, making life easy in case you are using SLURM for your cluster job management. Visit [EAR slurm plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_slurm_plugin/README.md) for more information.

Requirements
------------
EAR requires some third party libraríes and headers to compile and run. This is a list of this libraries and its references:

| Library  | Requirement / comment | Minimum version | References                                        |
| -------- | --------------------- | --------------- | ------------------------------------------------- |
| PAPI     | Yes / with RAPL       | 5.4.3.0         | [Website](http://icl.utk.edu/papi/)               |
| GSL      | Yes                   | 1.4             | [Website](https://www.gnu.org/software/gsl/)      |
| CPUPower | Yes                   | Kernel 3.10     | [Information](https://wiki.archlinux.org/index.php/CPU_frequency_scaling) |
| FreeIPMI | Yes                   | 1.5.7           | [Website](https://www.gnu.org/software/freeipmi/) |
| SLURM    | Just for SLURM plugin | 17.02.6         | [Website](https://slurm.schedmd.com/)             |
| MPI      | Yes                   | -               | -                                                 |

Brief installation guide
------------------------
Compile the library by typing `./configure`, `make` and `make install` in the root directory. Consider the option of `./configure --PREFIX=<path>` if you want to specify the installation path. It could be useful to run ‘./configure --help’ for listing the options details.

Customize installation
----------------------
Configure is based on shell variables which initial value could be given by setting variables in the command line, or in the environment. Take a look to the table with the most popular variables:

| Variable | Description                                                                                                  |
| -------- | ------------------------------------------------------------------------------------------------------------ |
| DEBUG    | Enables debug messages [0..4]. Debug messages with lower or equal level are printed.                         |
| CC       | C compiler command                                                                                           |
| CFLAGS   | C compiler flags                                                                                             |
| LDFLAGS  | Linker flags. E.g. ‘-L<lib dir>’ if you have libraries in a nonstandard directory <lib dir>.                 |
| LIBS     | Libraries to pass to the linker. E.g. ‘-l<library>’.                                                         |
| CPPFLAGS | C/C++ preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>. |
- This is an example of `CC`, `CFLAGS` and `DEBUG` variables overwriting:</ br>
`./configure CC=c99 CFLAGS=-g DEBUG=4`

You can choose the root folder by typing `./configure --PREFIX=<path>`. But there is another option `---exec-prefix=<path>` or *EPREFIX*, which by default equals *PREFIX*, which also its default value is `/usr/local/`. You have more installation options information by typing `./configure --help`.

This is the list of installation folder and their content:

| Root          | Directory    | Content / description                |
| ------------- | ------------ | ------------------------------------ |
| \<*PREFIX*\>  | .            | Root folder.                         |
| \<*EPREFIX*\> | /lib         | EAR libraries.                       |
| \<*EPREFIX*\> | /bin         | Daemon and tools.                    |
| \<*EPREFIX*\> | /bin/kernels | Benchmarks (or stress tests).        |
| \<*EPREFIX*\> | /bin         | Coming soon.                         |
| \<*PREFIX*\>  | /etc         | Scripts and configuration examples.  |

Adding required libraries installed in custom locations
-------------------------------------------------------
You can help *Configure* to find PAPI, SLURM, or other required libraries in case you installed in a custom location. It is necessary add its root path for the compiler to see include files, and libraries for the linker. by adding to it the following arguments:

| Argument                 | Description                                  |
| ------------------------ | -------------------------------------------- |
| --with-papi=\<path\>     | Specifies the path to PAPI installation.     |
| --with-gsl=\<path\>      | Specifies the path to GSL installation.      |
| --with-cpufreq=\<path\>  | Specifies the path to CPUFreq installation.  |
| --with-slurm=\<path\>    | Specifies the path to SLURM installation.    |
| --with-freeipmi=\<path\> | Specifies the path to FreeIPMI installation. |
* This is an example of ‘CC‘ overwriting and PAPI path specification:</ br>
`./configure --with-papi=/path/to/PAPI`

If unusual procedures must be done to compile the package, please try to figure out how `configure` could check whether to do them and contact the team to be considered for the next release. In the meantime, you can use overwrite shell variables or export them to the environment (e.g. LD_LIBRARY) or specify the paths of external requirements. 

After the installation
----------------------
After the installation you need to pass the learning phase in all your computing nodes, so you will have to visit [EAR learning phase page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_learning_phase/README.md) to follow its guide.

But before that, make sure your linker is aware of the required libraries and the environment variable *EAR_INSTALL_PATH* is set. This variable defines the folder of EAR binaries, libraries and tools. For the ease of use, an **environment module** is also configured next to the EAR compilation. So locate in `autootools` folder the module file with name `ear-{version}`, and copy it to your module collection folder (e.g. `cp autotools/ear-1.0 /hpc/base/ctt/modulefiles/libraries`).

Changelog
---------

License
-------
