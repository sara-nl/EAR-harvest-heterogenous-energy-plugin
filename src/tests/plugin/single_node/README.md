Single node tests
-----------------

These tests are designed to evaluate the different plugin options to be supported by slurm plugin, EARD, and EARL
* Assuming ear=on, some results depend on user privilege
* Test from 1..100 are designed to test srun options. They must be executes in the command line without arguments (./test1.sh) . They use kernels binaries, so they must be compiled before using the tests. It is also assumed they are executed in the same folder since executable path is relative. 
* tests test1XX.sh replicates testXX.sh but the are testing sbatch usage. They must be submitted using sbatch testXX.sh. Folder OUTS must be manually created before execution.

| Name   |Case             | Expected behavoiour                                                         |
| -------|----------------- | --------------------------------------------------------------------------- |
| test1  |No arguments     | Application is executed with default policy and th                          |
| test2  |--ear-verbose=1  | Application must be executed with default policy and th					 |
| test3  |--ear-policy=MONITORING_ONLY | if MONITORING_ONLY is supported, it must be selected without privileges |
| test4  |--ear-policy=MIN_TIME_TO_SOLUTION --ear-policy-th=0.65 | Depending on privileges, th will be changed or not |
| test5  |--ear-policy=MIN_TIME_TO_SOLUTION --ear-policy-th=0.75 | Depending on privileges, th will be changed or not |
| test6  |--ear-learning=1 --ear-verbose=1 | if authorized --> ok (freq=nominal), default otherwise |
| test7  |--ear-learning=2 --ear-verbose=1 | if authorized --> ok (freq=nominal-100000), default otherwise |
| test8  |--ear-tag=cpu-intensive --ear-verbose=1 | if energy-tag user--> select pstate and ear=off, default otherwise |
| test9  |--ear-tag=memory-intensive --ear-verbose=1 | if energy-tag user--> select pstate and ear=off, default otherwise |
| test10 |--ear-learning=1 --ear-tag=memory-intensive --ear-verbose=1 |if energy-tag user --> energy tag, else, if authorized, learning, else default |
| test11 |--ear-tag=invalid --ear-verbose=1 | default |
| test12 |--account=colbsc --ear-tag=cpu-intensive --ear-verbose=1|if energy-tag user --> energy tag, else, if authorized, learning, else default |
| test13 |--ear-cpufreq=2000000 --ear-policy=MONITORING_ONLY --ear-verbose=1 | if authorized, execute application at 2.0Ghz, at default freq otherwise |
| test14 |--ear-cpufreq=2000000 --ear-policy=MIN_TIME_TO_SOLUTION --ear-verbose=1 | if authorized, execute application at 2.0Ghz with MIN_TIME_TO_SOLUTION, at default freq otherwise |
| test15 | --ear-cpufreq=200000 --ear-policy=MIN_TIME_TO_SOLUTION --ear-verbose=1| It is an invalid frequency, default frequency is selected|
| test16 | --ear-user-db=DB/sp_metrics. --ear-verbose=1 -J name| application metrics must be reported in DB/sp_metrics.host.csv file. Intermediate folders must exists. App name must be name |
| test17 | --ear-user-db=DB/sp_metrics. --ear-verbose=1 | application metrics must be reported in DB/sp_metrics.host.csv file. Intermediate folders must exists. App name must be executable |
| test18 | --ear-user-db=DB/stream_mpi. --ear-verbose=1 -J $kernel| it is memory intensive application, low freqs must be selected |
| test19 | --ear-policy=MIN_ENERGY_TO_SOLUTION --ear-verbose=1 | If user is authorized, MIN_ENERGY will be selected as policy. It is a cpu app |
| test20 |--ear-policy=MIN_ENERGY_TO_SOLUTION --ear-verbose=1| If user is authorized, MIN_ENERGY will be selected as policy. It is an memory intensive app|
| test21 |openmpi test, not ready||
| test22 |openmpi test, not ready||
| test23 | --ear=off | Application is MPI and it is executed without EAR library|
| test24 | no ear flags | not mpi applications. It must be executed without problems |
| test25 | --ear-verbose=1  | not mpi applications. It must be executed without problems |
|test150 | Same than test103 (based on test1) with two srun's. | Options must be the same since there are defined in sbatch|

It is recommented to test changing ear.conf configuration


