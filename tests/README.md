Tests
-----
The EAR package includes two type of tests. The **check tests**, prepared to be executed after the `make` by typing `make check`, with and without privileges, and also [functional tests](https://github.com/BarcelonaSupercomputingCenter/EAR/blob/development/tests/functionals/README.md), which tests library functionalities against your software and hardware platform.

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
