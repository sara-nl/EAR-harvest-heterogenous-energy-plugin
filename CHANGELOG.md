## Unreleased
- eard_api non-blocking calls
- Working in a dynamic loading for power/energy policies

### Changed
- lock to avoid simultaneous ipmi access
- assert removed from ipmi functions. replaced by condition+error message
- support for dynamic management of multiple contexts
- eardbd host name included in aggregated metrics
- eargm reporting gloabl status every T1 period
- ereport now has a filter for eadbds/islands
- eard_rapi new_job and end_job calls are now non-blocking
- ereport now has an option to report Global_energy records

