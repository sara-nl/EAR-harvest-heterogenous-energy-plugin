#ifndef _EAR_DAEMON_COMMON_H
#define _EAR_DAEMON_COMMON_H

// Number of services supported
#define ear_daemon_client_requests 5
#define freq_req 0
#define uncore_req 1
#define rapl_req 2
#define system_req 3
#define node_energy_req 4

// Services related with frequency

#define SET_FREQ 0
#define START_GET_FREQ 1
#define END_GET_FREQ 2
#define SET_TURBO 3
#define DATA_SIZE_FREQ 4
#define CONNECT_FREQ 5

#define END_COMM 100

#define EAR_COM_OK     	 0
#define EAR_COM_ERROR 	-1

// Services related with uncore counters
#define START_UNCORE 0
#define RESET_UNCORE 1
#define READ_UNCORE 2
#define DATA_SIZE_UNCORE 3
#define CONNECT_UNCORE 4

// Services related with rapl counters 
#define START_RAPL 0
#define RESET_RAPL 1
#define READ_RAPL 2
#define DATA_SIZE_RAPL 3
#define CONNECT_RAPL 4

// Services to contact with the rest of the system, such as writting data
#define WRITE_APP_SIGNATURE 0
#define CONNECT_SYSTEM 1

// Services related to node energy management
#define READ_DC_ENERGY 0
#define DATA_SIZE_ENERGY_NODE 1
#define CONNECT_ENERGY 2

#else
#endif
