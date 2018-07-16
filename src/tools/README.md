EAR tools
---------

EAR tools and command line tools used during the learning phase or to manipulate, analyze coefficients. Not all of them have been intensivelly tested. 
---------------------------------------------------------------------------------------------------
| Name                          | Checking                                                         | Version   |
| ----------------------------- | ---------------------------------------------------------------- | --------  |
| coefficients_quality_control  | Used to evaluate the quality of coefficients                     | V2	       |
| coefficients_v2_to_v3         | converts coefficients from ear v2 to ear v3                      | v2/v3     |
| coefficients_to_csv           | reads coefficients and prints it in both stdout and csv file     | v2 and v3 |
| compute_coefficients          | computes coefficients for ear v2 using as input a user-db filei, it must be executed in compute nodes | v2|
| compute_coefficients_v3       | coputes coefficients for ear v3 reading applications from DB     | v3        |
| compute_coefficients_remote   | computes coefficients for ear v2 using as input a user-db filei, it can be executed anywhere | v2 |
| get_app_list                  | reads application from DB (normal or learning) and prints metrics in stdout | v3 |

Examples
--------
- coefficients_v2_to_v3 v2_path(root) v3_path(directory) nodename max_fre min_freq island
coefficients_v2_to_v3 /etc/ear/coeffs/coeffs. $HOME/test r22u27 2600000 2000000 0

- coefficients_to_csv version(2) v2_path(root) nodename max_fre min_freq
coefficients_to_csv 2 /etc/ear/coeffs/coeffs. r22u21 2600000 2000000

- coefficients_to_csv version(3) v3_path(directory) max_fre min_freq island --> it uses current nodename
coefficients_to_csv 3 /etc/ear/coeffsv3 2600000 2000000 0
