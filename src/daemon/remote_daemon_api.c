/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <linux/limits.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <common/states.h>

// 2000 and 65535
#define DAEMON_PORT_NUMBER 			5000
#define DAEMON_EXTERNAL_CONNEXTIONS 10

int create_server_socket()
{
     int eard_sockfd;
     struct addr_in eard_server;
     int n;
     eard_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (eard_sockfd < 0) return EAR_ERROR; 
     memset((char *) &eards_server, 0,sizeof(eard_server));
     eard_server.sin_family = AF_INET;
     eard_server.sin_addr.s_addr = INADDR_ANY;
     eard_server.sin_port = htons(DAEMON_PORT_NUMBER);
     if (bind(eard_sockfd, (struct sockaddr *) &eard_server, sizeof(eard_serverserv_addr)) < 0) return EAR_ERROR;
     if (listen(eard_sockfd,DAEMON_EXTERNAL_CONNEXTIONS)< 0) return EAR_ERROR;
	 return eard_sockfd;
}
int wait_for_client(int sockfd,struct sockaddr_in *client)
{
	int new_sock;
	int cllient_addr_size;

    cllient_addr_size = sizeof(struct sockaddr_in);
	 do{
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          error("ERROR on accept");
     	bzero(buffer,256);
}
