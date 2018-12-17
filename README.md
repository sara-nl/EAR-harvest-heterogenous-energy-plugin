# Energy Aware Runtime
Overview
--------

<img src="etc/images/logo.png" align="right" width="440">
Energy Aware Runtime (EAR) package provides an energy efficient solution for super computers. EAR includes a runtime library that dynamically selects the CPU frequency for MPI applications based on the program characterization done at runtime, node characteristics and power policy settings. The library doesn't need neither a prior information nor user input.

EAR also provides mechanism for those experts users that perfectly knows their application, to change the frequency and view the effects in form of metrics.

EAR library has been integrated in a full energy management system incluing a complete accounting mechanism, and a global energy manager. All the componets together provides three main services:

1) A **simple and ligthweigth system** to automatically select the optimal CPU frequency according to the application, the node, and the power policy.  This  services is provided by two components: the EAR library (**EARL**) and the EAR daemon (**EARD**). EARL is a smart component which selects the CPU frequency for the running applications. EARD is provides basic services to the rest of components (not only EARL).

2) A complete **energy accounting system** based on MySQL database. The energy accounting system is configurable in terms of application details and updates frequency.

3) A **global energy manager** which monitors and controls the energy consumed in the system. Energy control is configurable and dynamically adapts power policy settings based on global energy limits and application characteristics.

All three components are configurable using a single, centralized, and simple text file called 'ear.conf'. This mechanism makes easy the cluster definition and configuration. This 'ear.conf' includes default values, pre-defined application configurations, etc. More details can be found in [configuration section](./etc/conf/README.md).

Please visit [the main components page](./src/README.md) for a detailed description of each of the main components of EAR.

Requirements
------------
EAR requires some third party libraríes and headers to compile and run, in addition to the basic requirements such as the compiler and Autoconf. This is a list of these **libraries**, minimum **tested** versions and its references:

| Library   | Required / comment    | Minimum version | References                                        |
| --------- | --------------------- | --------------- | ------------------------------------------------- |
| PAPI      | Yes / with RAPL       | 5.4.3.0         | [Website](http://icl.utk.edu/papi/)               |
| GSL       | Yes                   | 1.4             | [Website](https://www.gnu.org/software/gsl/)      |
| CPUPower  | Yes                   | Kernel 3.10*    | [Information](https://wiki.archlinux.org/index.php/CPU_frequency_scaling) |
| FreeIPMI  | Yes                   | 1.5.7           | [Website](https://www.gnu.org/software/freeipmi/) |
| SLURM     | Just for SLURM plugin | 17.02.6         | [Website](https://slurm.schedmd.com/)             |
| MPI       | Yes                   | -               | -                                                  |
| MySQL     | No                    | -               | -                                                  |
* Depending on the version, may you have to change the name of the library function call (or the parameter).

Also, some **drivers** has to be present and loaded in the system:

| Driver      | File                                    | Kernel version | References      |
| ----------- | --------------------------------------- | -------------- | --------------- |
| CPUFreq     | kernel/drivers/cpufreq/acpi-cpufreq.ko  | 3.10           | [Information](https://wiki.archlinux.org/index.php/CPU_frequency_scaling) |
| Open IPMI   | kernel/drivers/char/ipmi/*.ko           | 3.10           | [Information](https://docs.oracle.com/en/database/oracle/oracle-database/12.2/cwlin/configuring-the-open-ipmi-driver.html) |

Lastly, the **compilers**:

| Compiler                          | Comment                    | Minimum version | References      |
| --------------------------------- | -------------------------- | --------------- | --------------- |
| GNU Compiler Collection (GCC)     | For the library and daemon | 4.8.5           | [Website](https://gcc.gnu.org/)                               |
| Intel C Compiler (ICC)            | For the library and daemon | 17.0.1          | [Website](https://software.intel.com/en-us/c-compilers)       |
| Intel Fortran Compiler (MPIIFort) | For some kernels              | 17.0.1          | [Website](https://software.intel.com/en-us/fortran-compilers) |

Just one C compiler next to its MPI scripts is needed.

Brief installation guide
------------------------
1) Generate Autoconf's `configure` program by typing `autoreconf -i`.
2) Compile the library by typing `./configure`, `make` and `make install` in the root directory. Consider the option of `./configure --PREFIX=<path>` if you want to specify the installation path. It could be useful to run ‘./configure --help’ for listing the options details.
3) Type `make etc.install` if you are interested in install the content of `ETC`. You can find a list of that content in the in the below section.

Customize installation
----------------------
`configure` is based on shell variables which initial value could be given by setting variables in the command line, or in the environment. Take a look to the table with the most popular variables:

| Variable | Description                                                                                                  |
| -------- | ------------------------------------------------------------------------------------------------------------ |
| DEBUG    | Enables debug messages [0..4]. Debug messages with lower or equal level are printed.                         |
| MPICC    | C compiler MPI script.                                                                                       |
| OMPICC   | OpenMPI compiler.                                                                                            |
| CC       | C compiler command.                                                                                          |
| CFLAGS   | C compiler flags.                                                                                            |
| LDFLAGS  | Linker flags. E.g. ‘-L\<lib dir\>’ if you have libraries in a nonstandard directory \<lib dir\>.             |
| LIBS     | Libraries to pass to the linker. E.g. ‘-l<library>’.                                                         |
| CPPFLAGS | C/C++ preprocessor flags, e.g. -I\<include dir\> if you have headers in a nonstandard directory \<include dir\>. |
| TMP      | Defines the node local storage as 'var', 'tmp' or other tempfs file system (default: /var/ear) (you can alo use --localstatedir=DIR) |
| ETC      | Defines the read-only single-machine data as 'etc' (default: EPREFIX/etc) (you can also use --sharedstatedir=DIR)            |
| MAN      | Defines the manual directory (default: PREFIX/man) (you can use also --mandir=DIR)                         |
| DOC      |  Defines the documentation directory (default: PREFIX/doc) (you can use also --docdir=DIR)                         |

- This is an example of `CC`, `CFLAGS` and `DEBUG` variables overwriting: </br>
`./configure CC=c99 CFLAGS=-g DEBUG=4`

You can choose the root folder by typing `./configure --PREFIX=<path>`. But there are other options in the following table:

| Definition      | Default directory    | Content / description                |
| --------------- | -------------------- | ------------------------------------ |
| \<*PREFIX*\>    | /usr/local           | Installation path                    |
| \<*ETC*\>       | \<*PREFIX*\>/etc     | Configuration files.                 |
| \<*TMP*\>       | /var/ear             | Pipes and temporal files.            |

This is the list of the inner installation folders and their content:

| Directory    | Root         | Content / description                |
| ------------ | ------------ | ------------------------------------ |
| /lib         | \<*PREFIX*\> | Libraries.                           |
| /bin         | \<*PREFIX*\> | Tools and benchmark kernels.         |
| /bin/kernels | \<*PREFIX*\> | Benchmarks (or stress tests).        |
| /sbin        | \<*PREFIX*\> | Privileged components.               |
| /scripts     | \<*PREFIX*\> | Scripts.                            |
| /man         | \<*PREFIX*\> | Documentation.                      |
| /ear         | \<*ETC*\>    | Configuration file.                  |
| /ear/coeffs  | \<*ETC*\>    | Coefficient files store.             |
| /module      | \<*ETC*\>    | Environment module.                  |
| /slurm       | \<*ETC*\>    | plugstack.conf.                      |
| /systemd     | \<*ETC*\>    | Unit services.                       |

You have more installation options information by typing `./configure --help`.

Adding required libraries installed in custom locations
-------------------------------------------------------
You can help `configure` to find PAPI, SLURM, or other required libraries in case you installed in a custom location. It is necessary to add its root path for the compiler to see include headers and libraries for the linker. You can do this by adding to it the following arguments:

| Argument                 | Description                                  |
| ------------------------ | -------------------------------------------- |
| --with-papi=\<path\>     | Specifies the path to PAPI installation.     |
| --with-gsl=\<path\>      | Specifies the path to GSL installation.      |
| --with-cpupower=\<path\> | Specifies the path to CPUPower installation. |
| --with-slurm=\<path\>    | Specifies the path to SLURM installation.    |
| --with-freeipmi=\<path\> | Specifies the path to FreeIPMI installation. |
| --with-mysql=\<path\>    | Specify path to MySQL installation.          |
* This is an example of ‘CC‘ overwriting and PAPI path specification:<br />
`./configure --with-papi=/path/to/PAPI`

If unusual procedures must be done to compile the package, please try to figure out how `configure` could check whether to do them and contact the team to be considered for the next release. In the meantime, you can overwrite shell variables or export its paths to the environment (e.g. LD_LIBRARY).

Unit services
-------------
The way to launch the EAR daemons is by unit services method. The generated unit services for the EAR Daemon, EAR Global Manager Daemon and EAR Database Daemon are generated and installed in `$(ETC)/systemd`. You have to copy those unit service files to your `systemd` operating system folder. Once copied, use the `systemctl` command to run the daemons.

Check the [EARD](./src/daemon/README.md), [EARDBD](./src/database_cache/README.md), [EARGMD](./src/global_manager/README.md) pages to find the precise execution commands.

Finally, when using `systemctl` commands, you can check messages reported by the stderr using journalctl. For instance:
`journalctl -u eard -f`

Configuration
-------------
1) EAR works together with **SLURM**, so please, visit the [SLURM plugin page](./src/slurm_plugin/README.md) to add the plugin to your SLURM installation.
2) Pass the learning phase in all your computing nodes by visiting the [learning phase page](./etc/scripts/learning/README.md) and follow this guide.

User guide
----------
Finally, you can launch any MPI application next to EAR library by following the [library user guide](./src/library/README.md).

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.
