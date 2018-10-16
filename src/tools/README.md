Tools list
----------

| Name              | Description                                                      | Basic arguments                         |
| ----------------- | ---------------------------------------------------------------- | --------------------------------------- |
| coeffs_compute    | Computes the learning coefficients                               | <save.path> <min.frequency> <node.name> |
| coeffs_comparison | Compares the computed coefficients                               |                                         |
| coeffs_show       | Shows the computed coefficients file in text format              | <file.path>                             |
| coeffs_jobproj    | For an executed application, applies the coefficients projection | <job.i> <step.id>                       |
| coeffs_quality    | Evaluates the quality of the learning coefficients               | <node.name <ref.freq>                   |
| get_active_nodes  | Shows the acvite nodes in the cluster                            |                                         |
| get_events        | Show the EAR events recorded in the database                     |                                         |
| learning_delete   | Deletes the previous learning phase data of the database         | <node.name>                             |
| learning_validate | Evaluates the quality of the learning phase results              |                                         |
| learning_show     | Shows the learning phase applications per node                   | <node.name>                             |

* Use the argument `--help` to expand the application information and list the admitted flags.

Examples
--------
- Compute the coefficients for the node `node1001` in which the minimum frequency set during the learning phase was 1900000 KHz

`/compute_coeffs /etc/coeffs 1900000 node1001`