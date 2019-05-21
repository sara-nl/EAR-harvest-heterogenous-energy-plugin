## Unreleased
### Added
- lock to avoid simultaneous ipmi access

### Changed
- assert removed from ipmi functions. replaced by condition+error message
- support for dynamic management of multiple contexts
- eardbd host name included in aggregated metrics
- eargm reporting gloabl status every T1 period
- ereport now has a filter for eadbds/islands
- eard_rapi new_job and end_job calls are now non-blocking

