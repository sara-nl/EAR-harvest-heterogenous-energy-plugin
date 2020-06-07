/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <linux/limits.h>


#include <common/config.h>

#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/states.h>

static int max_fd=-1;
fd_set rfds,rfds_sys;
int pipe_for_new_conn[2];

extern int eard_must_exit;


state_t init_active_connections_list()
{
	int i;
	FD_ZERO(&rfds);
	/* Nex sockets will be written in the pipe and the thread will wake up */
	if (pipe(pipe_for_new_conn)<0){
		error("Creating pipe for remote connections management");
		return EAR_ERROR;
	}
	debug("Channel for remote connection notification included [%d,%d]",pipe_for_new_conn[0],pipe_for_new_conn[1]);
	FD_SET(pipe_for_new_conn[0],&rfds);
	max_fd = pipe_for_new_conn[0] +1;
	return EAR_SUCCESS;
}

/*********** This function notify a new remote connetion ***************/

state_t notify_new_connection(int fd)
{
	int i=0,found=0;
	if (write(pipe_for_new_conn[1],&fd,sizeof(int)) < 0){
		error("Error sending new fd for remote command %s",strerror(errno));
		return EAR_ERROR;
	}
	return EAR_SUCCESS;
}

state_t add_new_connection()
{
	int ret;
	int new_fd;
	/* New fd will be read from the pipe */
	if (read(pipe_for_new_conn[0],&new_fd,sizeof(int)) < 0){
		error("Error sending new fd for remote command %s",strerror(errno));
		return EAR_ERROR;
	}
	if (new_fd < 0){
		error("New fd is less than 0 ");
		return EAR_ERROR;
	}
	if (new_fd == FD_SETSIZE){
		error("Maximum number of active remote connections reached, rejecting the connection");
		close(new_fd);
		return EAR_ERROR;
	}
	/* we have a new valid fd */
	debug("New remote connection %d",new_fd);
	FD_SET(new_fd, &rfds);
	if (new_fd >= max_fd) max_fd = new_fd + 1;
	return EAR_SUCCESS;
}

state_t remove_remote_connection(int fd)
{
  int i=0,found=0;
	debug("Closing remote connection %d",fd);
	FD_CLR(fd, & rfds);
	if (fd >= max_fd){
		/* We must update the maximum */	
		max_fd = -1;
		for (i=0;i< FD_SETSIZE; i++){
			if (FD_ISSET(i,&rfds) && (i >= max_fd)) max_fd = i+1;
		}
	}
	close(fd);
  return EAR_SUCCESS;
}

void check_sockets(fd_set *fdlist)
{
	int i;
	struct stat fdstat;
	for (i=0;i<max_fd;i++){
		if (FD_ISSET(i,fdlist)){
			if (fstat(i,&fdstat)<0){
				remove_remote_connection(i);
			}
		}
	}
}

/************* This thread will process the remote requests */
extern void process_remote_requests(int clientfd);
void * process_remote_req_th(void * arg)
{
	int i;
	int numfds_ready;
	debug("Thread to process remote requests created ");
	rfds_sys=rfds;
  while ((numfds_ready = select(max_fd, &rfds_sys, NULL, NULL, NULL)) && (eard_must_exit == 0))
  {
		if (numfds_ready > 0) {
			/* This is the normal use case */
      for (i = 0; i < max_fd; i++) {
          if (FD_ISSET(i, &rfds)){
						if (i == pipe_for_new_conn[0]){ 
							add_new_connection();
						}else{ 
							process_remote_requests(i);
						}
          }   
      } 
     } else if (numfds_ready == 0) { 
				/* This shouldn't happen since we are not using timeouts*/
				debug("numfds_ready is zero in remote connections activity"); 
		}else{
				/* This should be an error */
				if (errno == EINTR){
					debug("Signal received in remote commads");
				}else if (errno == EBADF){
					check_sockets(&rfds_sys);
				}else{
					error("Unexpected error in processing remote connections %s ",strerror(errno));
				}
		}
		rfds_sys=rfds;
	}
	pthread_exit(0);
}
