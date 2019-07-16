## Unreleased
- Power policies loaded with plugins 
- Power models loaded with plugins (first prototype). dir_plug included to be used with plugins
- The energy reading system now works through plugins. (5bf9dddcbe0d815cc9bd9a39ccc296cf1c29bfb9)
- eargm,earlib,daemon warnings fixed with -Wall and gcc 8
- verbose messages converted to debug or error
- error.h and debug.h included in verbose.h
- minor change fixed in wait_for_cient function. Argument for accept was incorrect.
- eard_api non-blocking calls
- increased  MAX_TRIES for non-blocking calls
- NO_COMMAND set when message failure and non-blocking calls are used
- eargmd. Warning was not initialized for NO_PROBLEM case
- Working in a dynamic loading for power/energy policies
- Cleaned COORDINATE_FREQUENCIES, MEASURE_DYNAIS_OV, EAR_PERFORMANCE_TESTS and IN_MPI_TIME.
- Cleaned dynamic policies.
- Deleted unused files.

### Changed
- new IPMI interface thread-safe. Each EARD thread creates a new energy_handler_t
- lock to avoid simultaneous ipmi access
- assert removed from ipmi functions. replaced by condition+error message
- eard_rapi new_job and end_job calls are now non-blocking
- support for dynamic management of multiple contexts

- earl: error.h included in ear_api.c. It was generating a crash when invalid step_id is used
- earl: master lock files removed when invalid step id is used

- eardbd_api error when using global variables. eardbd_api is not thread-safe. Two variables moved from global to local and loc    k included in sockets_send for atomic sent
- eardbd host name included in aggregated metrics

- eargm reporting gloabl status every T1 period

- new ereport with options -i (filter by island) and -g (show global manager records)
- ereport now has a filter for eadbds/islands
- ereport now has an option to report Global_energy records
- ereport -i "all" option now also reports avg power
- ereport -g now can be used in conjunction with -s
- new eacct -x option to see EAR events

- fixed an error where edb_create would not output a correct user creation query
