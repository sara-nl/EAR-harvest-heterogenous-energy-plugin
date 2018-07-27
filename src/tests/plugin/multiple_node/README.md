Single node tests
-----------------

These tests are designed to evaluate the different plugin options to be supported by slurm plugin, EARD, and EARL. They run in 4 nodes. To change it for more nodes just edit the test and change the following environment variables:

```
export PPN=NUM_PROCESSES_PER_NODE
export NODES=NUM_NODES
export OMP_NUM_THREADS=NUM_THREADS_PER_PROCESS
```

Tests 1..15 executes an MPI+OpenMP application supporting a maximum of 24 mpi processes. It is not a scalabillity test, so it is not designed for big number of nodes. It is an scalable application, so for few number of nodes and MIN_TIME_TO_SOLUTION policy EAR is supposed to select high frequencies. 


* Assuming ear=on, some results depend on user privilege

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
| test16 | --ear-user-db=DB/sp_metrics. --ear-verbose=1 | application metrics must be reported in four (1xnode) DB/sp_metrics.host.csv file. Intermediate folders must exists |


It is recommented to test changing ear.conf configuration


