API
---
The global manager offers a simple API `eargm_api.c` to notify the execution and finalization of the jobs. The API just notifies the EARGMD about the number of nodes to be used and released after the execution of the job. The SLURM plugin automatically does this task.