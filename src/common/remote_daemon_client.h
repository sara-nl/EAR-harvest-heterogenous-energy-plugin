/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _REMOTE_CLIENT_API_H
#define _REMOTE_CLIENT_API_H

int eards_remote_connect(char *nodename);

int eards_new_job(int job_id);
int eards_end_job(int job_id);
int eards_set_freq(unsigned long freq);

int eards_remote_disconnect();


#else
#endif
