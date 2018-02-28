/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _REMOTE_API_H_
#define _REMOTE_API_H_

#include <sys/socket.h>
#include <netinet/in.h>


int create_server_socket();
int wait_for_client(int sockfd,struct sockaddr_in *client);
void close_server_socket(int sock);

#else
#endif
