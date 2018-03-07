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
#include <netinet/ip.h>
#include <netdb.h>

#include <common/states.h>
#include <common/remote_conf.h>
#include <common/ear_verbose.h>

// 2000 and 65535
#define DAEMON_EXTERNAL_CONNEXIONS 1
static const char *__NAME__ = "connexion-api";


static  int sfd;
// based on getaddrinfo man pages
int create_server_socket()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
	char buff[50]; // This must be checked

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

	sprintf(buff,"%d",DAEMON_PORT_NUMBER);

   	s = getaddrinfo(NULL, buff, &hints, &result);
    if (s != 0) {
		VERBOSE_N(0,"getaddrinfo fails for port %s \n",buff);
		return EAR_ERROR;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

       if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

       close(sfd);
    }

   	if (rp == NULL) {               /* No address succeeded */
		VERBOSE_N(0,"bind fails for eards server\n");
		return EAR_ERROR;
    }else{
		VERBOSE_N(0,"socket and bind for erads socket success\n");
	}

   	freeaddrinfo(result);           /* No longer needed */

   	if (listen(sfd,DAEMON_EXTERNAL_CONNEXIONS)< 0){
		VERBOSE_N(0,"listen eards socket fails\n");
		close(sfd);
 		return EAR_ERROR;
	}
	VERBOSE_N(0,"eards socket listen ready!\n");
 	return sfd;
}
int wait_for_client(int s,struct sockaddr_in *client)
{
	int new_sock;
	int client_addr_size;

    client_addr_size = sizeof(struct sockaddr_in);
    new_sock = accept(s, (struct sockaddr *) &client, &client_addr_size);
    if (new_sock < 0){ 
		VERBOSE_N(0,"accept for eards socket fails %s\n",strerror(errno));
		return EAR_ERROR;
	}
	VERBOSE_N(0,"eards new connection \n");
	return new_sock;
}
void close_server_socket(int sock)
{
	close(sock);
}

int read_command(int s,request_t *command)
{
	int ret;
	ret=read(s,command,sizeof(request_t));
	if ((ret<0) || (ret!=sizeof(request_t))){
		VERBOSE_N(0,"Error reading remote command\n");
		if (ret<0) VERBOSE_N(0,"errno %s\n",strerror(errno));	
		command->req=NO_COMMAND;
	}
	return command->req;
}
void send_answer(int s,ulong *ack)
{
	if (write(s,ack,sizeof(ulong))!=sizeof(ulong)) VERBOSE_N(0,"Error sending the answer\n");
}



