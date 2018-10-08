EAR Library 
-----------
The EAR library is the core of the EAR package. The EARL offers a ligthweigth and simple solution to select the optional frequency for MPI applications at runtime.

EARL is dynamically loaded next to the running applications using the PMPI interface, which it's used by many other runtime solutions. The current EARL version only supports this mechanism but it is under development an API to be inserted in the OpenMPI library.

At runtime, EARL goes through the following phase:

<img src="../../etc/images/EAR_stack.png" align="right" width="280">

1. Automatic detection of application outer loops. This is done by dynamically intercepting MPI calls (using `LD_PRELOAD`) and invoking the Dynamic Application Iterative Structure detector algorithm. DynAIS is highly optimized for new Intel architectures reporting low overhead.

2. Computation of the application signature. Once DynAIS starts reporting iterations for the outer loop, EAR starts computing the application signature. This signature includes: iteration time, DC power consumption, CPI and TPI (transactions per instruction). Since DC power measurements error highly depends on the hardware, EAR automatically detects the hardware characteristics and sets a minimum time to compute the signature in order to minimize the average error.

<img src="../../etc/images/Models.png"  align="centered" width="640">

3. Power and performance projection. EAR has its own performance and power models which requires as an input, the application and the system signatures. The system signature is a set of coefficients characterizing each node in the system. They are computed at the learning phase at the EAR configuration time. EAR projects the power used and computing time (performance) of the running application for all the available frequencies in the system.

<img src="../../etc/images/Projections.png"  align="centered" width="640">

4. Apply the selected power policy. EAR includes two power policies to be selected at runtime: 'minimize time to solution' and 'minimize energy to solution' if are permitted by the system administrator. At this point, EAR executes the power policy, using the projections computed in the previous phase, and selects the optimal frequency for this application and this particular run.

Configuration
-------------
The EAR Library uses the `$(ETC)/ear.conf` file to be configured. Please visit the [EAR configuration file page](../../etc/conf/README.md) for more information about the options of EARL and other components.

The library receives the its specific settings through a shared memory regiones initialized by EARD.

How to run MPI applications with EARL
-------------------------------------
To load EARL with MPI jobs, it is only required to set the `LD_PRELOAD` environment variable with the library binary 'libear.so' path name, before your application starts. EARL will be loaded at runtime and MPI calls will be intercepted calling EAR API automatically.

To use this feature you have to rely on SLURM plugin. Check the [EAR SLURM plugin page](../slurm_plugin/README.md) for information about all the SLURM arguments. The plugin will deal with the configuration of the application and will contact the EARD and EAR Global Manager.

The library can be configured by the system administrator to be loaded by default or not. When EAR is configured by default, it is not needed to add any option to `srun` nor `sbatch`. If EAR is disabled by default, users can enable it by using the argument `--ear=on` .

The following example will run application with EAR library with default configuration (defined by sysadmin):

`./srun -N2 -n2 --ear=on application`

Also, the following example will run application with EAR library and will select MIN_ENERGY_TO_SOLUTION power policy. If the user is not allowed to use this policy, the default settings will be applied:

`./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION application`

If your application is not an MPI application, the benefits of the EAR library won't be applied. But the SLURM plugin would contact with the daemons in order to monitorize the application metrics and take a decision in case the energy budget is surpassed.

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.
