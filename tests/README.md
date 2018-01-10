Tests
-----
The EAR package includes two type of tests. The **check tests**, prepared to be executed after the `make` by typing `make check`, with and without privileges, and also **functional tests and misc test tools**, which tests library functionalities against your software and hardware platform.

Make check tests list
---------------------
| Name                | Checking                                                         |
| ------------------- | ---------------------------------------------------------------- |
| cpu_examinable      | If the CPU examinable by the library.                            |
| cpu_aperf           | If the CPU APERF counter is available.                           |
| cpu_uncores         | If there are CPU uncore counters available.                      |
| cpu_uncores_all     | If there are all CPU uncore counters available.                  |
| papi_version        | If the PAPI version is greater or equal than the reference.      |
| papi_init           | If PAPI initializes correctly.                                   |
| papi_comp_available | If PAPI perf counters events are available.                      |
| papi_comp_enabled   | If PAPI perf counters events are enabled.                        |
| papi_comp_available | If PAPI perf uncore counters events are available.               |
| papi_comp_enabled   | If PAPI perf uncore counters events are enabled.                 |
| papi_comp_available | If PAPI libmsr events are available.                             |
| papi_comp_enabled   | If PAPI libmsr events are enabled.                               |
| papi_comp_available | If PAPI rapl events are available.                               |
| papi_comp_enabled   | If PAPI rapl events are enabled.                                 |
| gsl_version         | If the GSL version is greater or equal than the reference.       |
| slurm_version       | If the SLURM version is greater or equal than the reference.     |
| free_ipmi_version   | If the Free IPMI version is greater or equal than the reference. |

Functional tests and test tools list
------------------------------------
| Name                     | Description                                                                             |
| ------------------------ | --------------------------------------------------------------------------------------- |
| bandwith_over_time       | Monitors the bandwith of the node during a period of time                               |
| dynais_input             | Feed the DynAIS algorithm with an input and summarizes its process.                     |
| eard_monitor             | Checks if the EAR daemon is working correctly.                                          |
| freeipmi_overhead        | Gets the overhead of calling FreeIPMI library to read hardware counters.                |
| hardware_scanner         | Reveals some CPU info.                                                                  |
| intel_pci_uncores_stress | Gets the total bandwith readed in the uncore counters after the execution of a program. |
| papi_cpu_gflops          | Tests the GFlops values given by PAPI.                                                  |
| simd_power_metrics       | Stress the AVX CPU hardware and computes some performance and power metrics.            |
