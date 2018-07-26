SLURM plugin configuration guide
--------------------------------
EAR SLURM plugin allows to load dynamically the EAR library for a SLURM job, if the enabling argument is set. The library will be loaded over each job step, intercepting all MPI calls. The daemon will be loaded and closed next to SLURM's daemon (*slurmd*).

Configuration
-------------
1) Locate and edit the library and daemon configuration file example in the installation path, `etc/ear.conf`.
2) Modify the values of the variables, taking into account that only accepts complete paths or names, not appending from other environment variables like `$HOME`. You can find a complete list in [environment variables configuration page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/etc/README.md).
1) Add in SLURM's `plugstack.conf` file, a new entry with the paths of the compiled plugin `earplug.so` and also the `ear.conf`, taking a look in the example `etc/plugstack.conf`.
3) Reset the system or kill and run SLURM's slurmctld and slurmd daemons.

As previously said, EAR library and daemon behaviour depends on environment variables defined in **ear.conf** file. These environment variables could be redefined by the end user with some *srun* arguments added by the plugin (see execution section below) or exporting the same variables before sending a job.

New SLURM arguments
-------------------
After the plugin configuration, a set of additional arguments are included in SLURM's `srun` for the ease of use of EAR. You can see a complete list of those arguments below.

| Argument               | Description                    | Type / comment                                              |
| ---------------------- | ------------------------------ | ----------------------------------------------------------- |
| --ear                  | Enables EAR                    | Just for default values.                                    |
| --ear-policy           | Energy/time policy             | See *EAR_POWER_POLICY* environment variable.                |
| --ear-policy-th        | Threshold of the policy        | Value between 0 and 1.                                      |
| --ear-verbose          | Verbosity level                | Value between 0 and 4.                                      |
| --ear-learning-phase   | Enables the learning phase     | *P_STATE* [0..n].                                           |
| --ear-traces           | Saves trace files with metrics | Path where the files will be placed.                        |
| --job-name             | App/job name for debugging     | Name for the job/application.                               |
| --ear-user-db			 | Use-db filename root			  | Specifies the file to save the user applications metrics summary'file.nodename.csv' file  |
| --ear-cpufreq          | Sets the job frequency         | Just for *MONITORING_ONLY* policy.                          |

* For more information consult `srun --help` output or see configuration options sections for more detailed description.
* Example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`
* Maximum *P_STATE* depends on the CPU.
* Most of the options can only be used by Authorized users (specified at ear.conf file)
