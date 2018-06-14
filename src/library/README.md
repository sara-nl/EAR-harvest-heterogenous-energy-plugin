Energy Aware Runtime: Library
-----------------------------
The library (EARL) is the core of the EAR package. It has the most of the functionality, via the interception of MPI function calls, and also controls other components of the EAR package like the daemon.

Configuration
-------------
The EAR Library uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.

```
```

Launching jobs with SLURM
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

