Energy Aware Runtime: (Node) Daemon
-----------------------------------
The node daemon is the component in charge of providing any kind of service that requires privileged capabilities. Current version is conceived as an external process executed with root privileges. The EARD provides two basic services, each one covered by one thread:
- Provides privileged metrics such as average frequency, uncore integrated memory controller counters to compute the memory bandwidth, and also energy metrics (DC node, DRAM and package energy).
- Implements a periodic power monitoring service. This service allows EAR package to control the total energy consumed in the system.

Requirements
------------
EARD uses CPUPower, FreeIPMI and PAPI (with RAPL component enabled). Paths to these libraries must be specified during the configure process when they are not installed in defaults paths.

When executed in production, EARD connects with [EARDBD](../database_cache/REAME.md), service that have to be up before starting the node daemon, otherwise values reported by EARD to the EAR database will be lost.

Configuration
-------------
The EAR Daemon uses the `$(ETC)/ear.conf` file to be configured. It can be dynamically configured by reloading the service.

```
# Fields related to the Daemon
# Lines starting with # are comments

# Verbose level
NodeDaemonVerbose=1

# Frequency used by power monitoing service (in seconds)
NodeDaemonPowermonFreq=10

# Maximum p_state supported (1 nominal)
NodeDaemonMaxPstate=1

# Turbo frequencies supported (0 no)
NodeDaemonTurbo=0

# Port to be used by the remote API
NodeDaemonPort=5000
```

Execution
---------
To execute this component, this `systemctl` command examples are provided:
- `sudo systemctl start eard` to start the EARD service.
- `sudo systemctl stop eard` to stop the EARD service.
- `sudo systemctl reload eard` to force to reload the configuration of the EARD service.

API
---
The (node) Daemon offers a simple API to request changes on the frequency, modify the current node settings, and reload the system configuration by reading `$(ETC)/ear.conf`

Three APIs are provided:
- Local API, to be used by [EARL](../library/README.md) (or any other runtime). It can be found in [eard_api.h](eard_api.h). This API involves complex data types and is not public.

- Local API, to be used by applications. It is a subset of the EARD api and designed to be used by any applications to contact the privileged metric service offered by EARD. This API is public and can be used without restrictinos, so doesn't include functions to change the frequency. It can be found at [TBD](.).

- Remote API, to be used by the [EARGMD](../global_manager/README.md) or system commands and tools such as the `econtrol`. Can be found at [eard_rapi.h](eard_rapi.h) and is not public.

License
-------
All the files in the EAR framework are under the LGPLv2.1 license. See the [COPYING](../../COPYING) file in the EAR root directory.

