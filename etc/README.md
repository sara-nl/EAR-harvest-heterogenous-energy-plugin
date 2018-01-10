Environment variables configuration
-----------------------------------
The EAR library and daemon behaviour is defined by a set of enviroment variables. You can define this environment variables before launching a job. Also, the SLURM plugin and the scripts provided for launching jobs directly calling MPI, makes use of a default configuration file, which gives value to these environment variables if aren't set before.

Environment variables list
--------------------------
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
