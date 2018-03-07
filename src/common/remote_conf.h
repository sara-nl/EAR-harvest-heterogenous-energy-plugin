/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/


#ifndef REMOTE_CONF_H
#define REMOTE_CONF_H
#define DAEMON_PORT_NUMBER          50000

typedef union req_data{
        uint job_id;
        ulong max_freq;
}req_data_t;


typedef struct request{
    uint        req;
    req_data_t  my_req;
}request_t;

#define EAR_RC_NEW_JOB     0
#define EAR_RC_END_JOB     1
#define EAR_RC_MAX_FREQ    100
#define NO_COMMAND 100000


#else
#endif
