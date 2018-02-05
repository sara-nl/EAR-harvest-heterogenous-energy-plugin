#ifndef _EAR_DAEMON_COMMON_H
#define _EAR_DAEMON_COMMON_H
#include "types_generic.h"

// Number of services supported
#ifdef MULTIPLE_SERVICES
#define ear_daemon_client_requests 5
#define freq_req 0
#define uncore_req 1
#define rapl_req 2
#define system_req 3
#define node_energy_req 4
#else
#define ear_daemon_client_requests 1
#define freq_req 0
#define uncore_req 0
#define rapl_req 0
#define system_req 0
#define node_energy_req 0
#endif

// Data type to send the requests
union daemon_req_opt{
    unsigned long req_value;
    struct App_info app;    
};

struct daemon_req{
    unsigned long req_service;
    union daemon_req_opt req_data;
};  


// Services related with frequency

#define SET_FREQ 0
#define START_GET_FREQ 1
#define END_GET_FREQ 2
#define SET_TURBO 3
#define DATA_SIZE_FREQ 4
#define CONNECT_FREQ 5
#define START_APP_COMP_FREQ 6
#define END_APP_COMP_FREQ 7

#define END_COMM 1000

#define EAR_COM_OK     	 0
#define EAR_COM_ERROR 	-1

// Services related with uncore counters
#define START_UNCORE 100
#define RESET_UNCORE 101
#define READ_UNCORE 102
#define DATA_SIZE_UNCORE 103
#define CONNECT_UNCORE 104

// Services related with rapl counters 
#define START_RAPL 200
#define RESET_RAPL 201
#define READ_RAPL 202
#define DATA_SIZE_RAPL 203
#define CONNECT_RAPL 204

// Services to contact with the rest of the system, such as writting data
#define WRITE_APP_SIGNATURE 300
#define CONNECT_SYSTEM 301

// Services related to node energy management
#define READ_DC_ENERGY 400
#define DATA_SIZE_ENERGY_NODE 401
#define CONNECT_ENERGY 402

#else
#endif
