Energy Aware Runtime SLURM plugin
---------------------------------
EAR SLURM plugin allows to load and unload dynamically both the EAR library and EAR daemon for a SLURM job, if the enabling argument is set. The EAR library will be loaded over each job step, intercepting all MPI calls. The daemon will proceed in the same way. 

Configuration
-------------
1) Add in SLURM's **plugstack.conf** file, a new entry with the path of the compiled plugin and also the path of the library and daemon configuration file. You can take a look at the example **etc/plugstack.conf** if needed.
2) Edit library and plugin configuration file example located in installation path, **etc/ear.conf**, if you want to adjust library or daemon default behavior. This task normally belongs to the admin. (The plugin only accepts complete paths or names, not appending from other environment variables like `$HOME`).
3) Reset the system or kill and run SLURM's slurmctld and slurmd daemons.

As previously said EAR library and daemon behaviour depends on environment variables defined in **ear.conf** file. This environment variables could be redefined by the end user with some *srun* arguments added by the plugin (see execution section below) or export of the same variables in user environment or *sbatch* script.

### EAR library configuration file options (ear.conf) ###
**EAR**<br />
Enables or disables EAR by default.<br />
**EAR_DB_PATHNAME**<br />
Defines the path to database saved files. These files stores all nodes metrics as data history, and its used to compute  all nodes coefficients. They are not plain text files.<br />
**EAR_COEFF_DB_PATHNAME**<br />
Defines the path to coefficients saved files. These files contains the coefficients of all nodes for all working frequencies.<br />
**EAR_USER_DB_PATHNAME**<br />
Defines the path to summary saved files. These files contains summaries of all executed applications as data historic. Can be read because they are plain text files.<br />
**EAR_DYNAIS_LEVELS**<br />
Number of levels of DynAIS algorithm (see [library page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_lib/README.md) for more information).<br />
**EAR_DYNAIS_WINDOW_SIZE**<br />
Windows size of DynAIS algorithm (see [library page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_lib/README.md) for more information).<br />
**EAR_LEARNING_PHASE**<br />
Enables the learning phase.<br />
**EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN**<br />
Minimum performance efficiency gain for *MIN_TIME_TO_SOLUTION* policy in the form of percentage (e.g. 0.15 meaning 15%). Could be overrided by `srun --ear-policy-th` argument. It prevents switching the CPU to the next value in the range of available frequencies, if the performance increase indicator do not scale over this efficiency gain value, preventing the waste of energy.<br />
**EAR_PERFORMANCE_PENALTY**<br />
Maximum performance degradation for *MIN_ENERGY_TO_SOLUTION* policy in the form of percentage (e.g. 0.15 meaning 15%). Could be overrided by `srun --ear-policy-th` argument. It means that prevents scalating the CPU to the next value in the range of available frequencies, if the performance degradation indicator of that frequency is over the penalty.<br />
**EAR_POWER_POLICY**<br />
Sets by default an EAR policy. Could be overrided by `srun` arguments. The types are *MIN_ENERGY_TO_SOLUTION*, which minimizes the energy consumed with a limit to the performance degradation (*EAR_PERFORMANCE_PENALTY*), and *MIN_TIME_TO_SOLUTION*, which improves the execution time while guaranteeing a minimum performance efficiency that justifies more energy consumption (*EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN*). The last policy is *MONITORING_ONLY*, just to analyze your application and get metrics.<br />
**EAR_PERFORMANCE_ACCURACY**<br />
Minimum time interval between two power measurements for a representative value.<br />
**EAR_RESET_FREQ**<br />
Set frequency to nominal at the begining of a new loop or not.<br />
**EAR_TURBO**<br />
Enables turbo frequencies: those over the nominal frequency.<br />
**EAR_APP_NAME**<br />
Default application name, used to save summarized metrics in DB.<br />
**EAR_TMP**<br />
Path were temporary files will be placed.<br />
**EAR_GUI_PATH**<br />
Path where the traces and graphic interface streaming files will be placed (see [library page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_lib/README.md) for more information about the traces or GUI).<br />
**EAR_VERBOSE**<br />
Defines the debug level. Higher the level, more messages.<br />

Execution
---------
0) Remember to pass first the learning phase to get and store all node coefficients if you want to launch a normal job (see [learning phase page](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/ear_learning_phase/README.md) for more information). 
1) Adjust the behavior of EAR library and daemon for the next job by adding some of the following arguments (remember that these arguments overwrite its equivalent default environment variable defined in earconf.sh). 

| Argument               | Description                    | Type / comment                                              |
| ---------------------- | ------------------------------ | ----------------------------------------------------------- |
| --ear                  | Enables EAR                    | Just for default values.                                    |
| --ear-policy           | Energy/time policy             | See *EAR_POWER_POLICY* environment variable.                |
| --ear-policy-th        | Threshold of the policy        | Value between 0 and 1.                                      |
| --ear-verbose          | Verbosity level                | Value between 0 and 4.                                      |
| --ear-learning-phase   | Enables the learning phase     | *P_STATE* [0..n].                                           |
| --ear-traces           | Saves trace files with metrics | Path where the files will be placed.                        |
| --job-name             | App/job name for debugging     | Name for the job/application.                               |
| --cpu-freq             | Sets the job frequency         | Just for *MONITORING_ONLY* policy.                          |
* For more information consult `srun --help` output or see configuration options sections for more detailed description.
* Example: `./srun -N2 -n2 --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-policy-th=0.9 application`
* Maximum *P_STATE* depends on the CPU.
