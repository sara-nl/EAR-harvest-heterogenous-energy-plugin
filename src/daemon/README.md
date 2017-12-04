Energy Aware Runtime daemon
---------------------------
EAR daemon is the EAR component in charge of providing any kind of service that requires privileged capabilities. Current version is conceived as an external process executed with root privileges, launched before application starts and finished when application ends. But it is being refactored to run as a permanent process.

The daemon must be manually launched once per node or managed by the EAR SLURM plugin. The daemon is fully distributed, neither exists shared information nor coordination between the different instances executed in different nodes. 

Manual execution
----------------
1) Make sure any library dependencies like PAPI and CPUPower libraries are seen by the loader. 
2) Type `sudo ./ear_daemon 1 /tmp 2` to launch the EAR daemon*. It receives as an arguments; the default *P_STATE* (or frequency) to be used in the node as default, the path where the files used between the library and daemon to communicate each other will be placed (named pipes) and finally the level of verbosity ([0..4]). 
3) Launch an MPI application preloading the EAR library (using a script or manually configuring any resource manager).

(*) LD_LIBRARY_PATH must be updated because sudo starts with a clean environment

Daemon testing
--------------
It's included with the package a tool to test the daemon: the binary `ear_daemon_test`. This program is a simple client that periodically (every 5 secs. at this moment) contacts with the daemon and asks for "cas_count" events (reads and writes) and pack and dram energy consumed. Based on that metrics, it computes memory bandwith, average power and prints the results. The test tool accepts as an argument the number of iterations to be executed, for example: `./ear_daemon_test 100 /tmp`

Execution managed by EAR SLURM plugin
-------------------------------------
For a detailed information, please visit the
[EAR SLURM plugin page](https://github.com/BarcelonaSupercomputingCenter/EAR/tree/development/ear_slurm_plugin).
