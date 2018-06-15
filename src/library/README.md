EAR Library 
------------------
The EAR library is the core of the EAR package. The EARL offers a ligthweigth and simple solution to select the optiomal frequency for MPI applications at runtime. EARL is dynamically loaded with applications using the PMPI interface used by many other runtime solutions. The current EARL version is only supported with this mechanism but it is under development an API to be inserted in the OpenMPI library. However, the actual distribution includes EARL versions to be dynamically loaded with Intel and OpenMPI libraries.  

At runtime, EARL goes trough the following phase:
1. Automatic detection of application outer loops. This is done using DynAIS, our own Dynamic Application Iterative Structure detector algorithm. Dynais is highly optimized for new Intel architectures reporting a low overhead. 
2. Computation of application Signature. Once Dynais starts reporting iterations for the outer loop, EAR starts computing the application signature. The application signature includes: CPI, Iteration time, DC node Power, and TPI (transactions per instruction). Since DC node power measurements error highly depends on the hardware, EAR automatically detects the hardware characteristics and sets a minimum time to compute the signature in order to minimize the average error.
3. Project performance and power model. EAR incorporate one performance and power models. These models uses, as input, the application signature and the system signature. The system signature is a set of coefficients characterizing each node in the system. They are computed at EAR installation time (tipically one once). EAR projects power and performance (time), for all the available frequencies in the system.
4. Apply the selected power policy. EAR includes two power policies selected at runtime: Minimize time to solution and Minimize energy to solution. These policies are configured by the sysadmin, as well as which policies can be used by users. At this point , EAR executes the power policy, using the projections computed in the previous phase, and selects the optimal frequency for this application and this particular run.


Configuration
-------------
The EAR Library uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.


How to run MPI applications with EARL
-------------------------------------

To load EARL with MPI jobs, it is only needed to set the LD_PRELOAD environment variable. EARL will be loaded at runtime and MPI calls will be intercepted calling EAR API automatically. 



EAR SLURM
-------------------------
Just call the `srun` program with, at least, the argument `--ear`, which loads the EAR library together with your application using just the default configuration. For a complete list of parameters, please visit the [SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/tree/development/ear_slurm_plugin).

In example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`

If your application is not an MPI application, the benefits of the EAR Library won't be applied. But the SLURM plugin would contact with the daemons in order to monitorize the application metrics and take a decision in case the energy budget is surpassed.

Launching applications calling MPI directly
-------------------------------------------
This way doesn't make use of any cluster job scheduler, so a a script is provided to make it easy that task. You can launch the script with empty parameters to view it's usage.

In the folder `$(ETC)/scripts/launching`, execute the `mpi_exec.sh` script to launch the job. In example `./mpi_exec.sh computing_node1 28 28 MONITORING_ONLY`, where both numbers are the MPI processes and the MPI's per node, an the last one is the policy. This is script will use the `LD_PRELOAD` environment variable to load the library next to your MPI applications.

Offline
-------

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.

