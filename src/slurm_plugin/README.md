SLURM plugin configuration guide
--------------------------------
EAR SLURM plugin allows to load dynamically the EAR library for the SLURM jobs, if the enabling argument is set or is enabled by default. The library will be loaded over each job step, intercepting all MPI calls.

Configuration
-------------
Add in SLURM's `plugstack.conf` file, a new entry with the path of the compiled plugin `earplug.so`. Take a look into the example `etc/ear.plugstack.conf`.

You will the argument `prefix` which points to the EAR installation path and it is used to load the library using `LD_PRELOAD` mechanism. Also you will find `localstatedir` used to contact with the EARD, which by default points the path you set during the `./configure` using `--localstatedir` or `EAR_TMP` arguments.

Finally, the arguments `eargmd_host` and `eargmd_port`, for the hostname and port of the node in case you are using EARGMD.

New SLURM arguments
-------------------
After the plugin configuration, a set of additional arguments are included in SLURM's `srun` for the ease of use of EAR. You can see a complete list of those arguments below.

| Argument                   | Description                    | Type / comment                                              |
| -------------------------- | ------------------------------ | ----------------------------------------------------------- |
| --ear                      | Enables EAR                    | Just for default values.                                    |
| --ear-policy               | Energy/time policy             | See *EAR_POWER_POLICY* environment variable.                |
| --ear-policy-th            | Threshold of the policy        | Value between 0 and 1.                                      |
| --ear-verbose              | Verbosity level                | Value between 0 and 4.                                      |
| --ear-learning             | Enables the learning phase     | *P_STATE* [0..n].                                           |
| --job-name                 | App/job name for debugging     | Name for the job/application.                               |
| --ear-user-db			     | Use-db filename root			  | Specifies the file to save the user applications metrics summary'file.nodename.csv' file  |
| --ear-cpufreq              | Sets the job frequency         | Just for *MONITORING_ONLY* policy.                          |

* For more information consult `srun --help` output or see configuration options sections for more detailed description.
* Example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`
* Maximum *P_STATE* depends on the CPU.
* Most of the options can only be used by Authorized users (specified at ear.conf file)
