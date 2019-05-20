/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/output/error.h>
//#define SHOW_DEBUGS 1
#include <common/output/debug.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/app_api/app_conf_api.h>
#include <metrics/ipmi/energy_node.h>
#include <daemon/power_monitor.h>

#define close_app_connection()

static char app_to_eard[MAX_PATH_SIZE];
static char eard_to_app[MAX_PATH_SIZE];

static int fd_app_to_eard=-1;
static int fd_eard_to_app=-1;

#define MAX_FDS 100
#define MAX_TRIES 100
typedef struct connect{
	int recv;
	int send;
	int pid;
}connect_t;
static int connections=0;
static connect_t apps_eard[MAX_FDS];
static fd_set rfds_basic;
static int numfds_req;

extern cluster_conf_t my_cluster_conf;
extern int eard_must_exit;
static energy_handler_t my_eh;

/*********************************************************/
/***************** PRIVATE FUNCTIONS in this module ******/
/*********************************************************/

static void app_api_set_sigterm()
{
    sigset_t set;
    struct  sigaction sa;
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK,&set,NULL); 

}


static int connect_with_app()
{
    if (fd_eard_to_app>=0) return EAR_SUCCESS;
    else return EAR_ERROR;
}

static int send_app_answer(int fd_out,app_recv_t *data)
{
	if (write(fd_out,data,sizeof(app_recv_t))!=sizeof(app_recv_t))	return EAR_ERROR;
	return EAR_SUCCESS;
}



/*********************************************************/
/***************** FUNCTIONS USED BY EARD ************/
/*********************************************************/
int create_app_connection(char *root)
{
	mode_t old_mask;
	int i;
	debug("create_app_connection\n");
	for (i=0;i<MAX_FDS;i++){
		apps_eard[i].recv=-1;
		apps_eard[i].send=-1;
		apps_eard[i].pid=-1;
	}
    sprintf(app_to_eard,"%s/.app_to_eard",root);
    /*sprintf(eard_to_app,"%s/.eard_to_app",root);*/
	old_mask=umask(0);
	// app_to_eard is used to send requests from app to eard
    if (mknod(app_to_eard,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
			return EAR_ERROR;
        }
    }
	#if 0
	if (mknod(eard_to_app,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
            return EAR_ERROR;
        }
    }
	#endif
	umask(old_mask);
	fd_app_to_eard=open(app_to_eard,O_RDWR);
	if (fd_app_to_eard<0){
		return EAR_ERROR;
	}
	#if 0
	fd_eard_to_app=open(eard_to_app,O_RDWR);
	if (fd_eard_to_app<0)	return EAR_ERROR;
	#endif
	return EAR_SUCCESS;
}

int find_first_free()
{
	int i;
	while ((apps_eard[i].recv==-1) && (i<MAX_FDS)) i++;
	if (i==MAX_FDS) return -1;
	return i;	
}

void check_connection(char *root,int i);
void remove_connection(char *root,int i);

void accept_new_connection(char *root,int pid)
{
	int i,tries=0;	
	char eard_to_app[MAX_PATH_SIZE];
	char app_to_eard[MAX_PATH_SIZE];
	debug("accepting connection from %d\n",pid);
	sprintf(app_to_eard,"%s/.app_to_eard.%d",root,pid);
	sprintf(eard_to_app,"%s/.eard_to_app.%d",root,pid);
	if (connections<MAX_FDS) i=connections;
	else{
		i=find_first_free();
		if (i<0){ 
			/* At this point we must check if connections are still alive */
			for (i=0;i<MAX_FDS;i++){
				if (apps_eard[i].recv>=0){ check_connection(root,i);}
			}
			if ((i=find_first_free())<0) return;
		}
	}
	apps_eard[i].recv=open(app_to_eard,O_RDONLY|O_NONBLOCK);
	apps_eard[i].send=open(eard_to_app,O_WRONLY|O_NONBLOCK);	
	if (apps_eard[i].send<0){
		if (errno==ENXIO){
		while((tries<MAX_TRIES) && ((apps_eard[i].send=open(eard_to_app,O_WRONLY|O_NONBLOCK))<0) && (errno==ENXIO)) tries++;
		}
	}

	if ((apps_eard[i].recv>=0) && (apps_eard[i].send>=0)){
		debug("Connection established pos %d  fds=%d-%d\n",i,apps_eard[i].recv,apps_eard[i].send);
		FD_SET(apps_eard[i].recv,&rfds_basic);
		if (apps_eard[i].recv>=numfds_req) numfds_req=apps_eard[i].recv+1;
		apps_eard[i].pid=pid;
		connections++;
		debug("connections %d numfds_req %d",connections,numfds_req);
	}else{
		debug("Not connected %d-%d",apps_eard[i].recv,apps_eard[i].send);
		close(apps_eard[i].recv);
		close(apps_eard[i].send);
		apps_eard[i].recv=-1;
		apps_eard[i].send=-1;
	}
}

void check_connection(char *root,int i)
{
    char eard_to_app[MAX_PATH_SIZE];
	
    debug("checking connection from %d\n",apps_eard[i].pid);
    sprintf(eard_to_app,"%s/.eard_to_app.%d",root,apps_eard[i].pid);
	close(apps_eard[i].send);
    apps_eard[i].send=open(eard_to_app,O_WRONLY|O_NONBLOCK);
    if (apps_eard[i].send>=0){
        debug("Connection is still alive\n");
	}else{
		remove_connection(root,i);
	}
}
/* This function is similar to close but we must clean the environment */
void remove_connection(char *root,int i)
{
	int fd_in,max=-1,new_con=0,pid;
	char eard_to_app[MAX_PATH_SIZE];
	char app_to_eard[MAX_PATH_SIZE];
	pid=apps_eard[i].pid;
	debug("cleaning connection from %d\n",pid);
	sprintf(app_to_eard,"%s/.app_to_eard.%d",root,pid);
	sprintf(eard_to_app,"%s/.eard_to_app.%d",root,pid);
	fd_in=apps_eard[i].recv;
	close(fd_in);
	apps_eard[i].recv=-1;
	apps_eard[i].pid=-1;
	FD_CLR(fd_in,&rfds_basic);
    /* We reconfigure the arguments */
    for (i=0;i<MAX_FDS;i++){
        if (apps_eard[i].recv>=0){
            if (apps_eard[i].recv>max) max=apps_eard[i].recv;
            new_con=i;
        }
    }
    connections=new_con;
    numfds_req=max+1;
	unlink(app_to_eard);
	unlink(eard_to_app);
}
void close_connection(int fd_in,int fd_out)
{
	int i,found=0;
	int max=-1,new_con=-1;
	debug("closing connections %d - %d\n",fd_in,fd_out);
	close(fd_in);
	close(fd_out);
	for (i=0;i<connections && !found;i++){
		if (apps_eard[i].recv==fd_in){
			found=1;
			debug("Cleaning position %d fds=%d-%d",i,apps_eard[i].recv,apps_eard[i].send);
			apps_eard[i].recv=-1;
			apps_eard[i].send=-1;
			apps_eard[i].pid=-1;
			FD_CLR(fd_in,&rfds_basic);
		}
	}
	/* We reconfigure the arguments */
	max=fd_app_to_eard;
	for (i=0;i<MAX_FDS;i++){
		if (apps_eard[i].recv>=0){
			if (apps_eard[i].recv>max) max=apps_eard[i].recv;		
			new_con=i;
		}
	}
	connections=new_con+1;
	numfds_req=max+1;
	debug("connections %d numfds_req %d\n",connections,numfds_req);
	
}

uint read_app_command(int fd_in,app_send_t *app_req)
{
	int ret;
    int flags,orig_flags,tries;

	if ((ret=read(fd_in,app_req,sizeof(app_send_t)))!=sizeof(app_send_t)){
		if (ret<0){		
			error("Error reading NON-EARL application request\n");
			return INVALID_COMMAND;
		}else{    
			/* If we have read something different, we read in non blocking mode */
			orig_flags = fcntl(fd_in, F_GETFD);
		    if (orig_flags<0){
        		error("Error getting flags for pipe %s",app_to_eard);
    		}
    		flags = orig_flags | O_NONBLOCK;
    		if (fcntl(fd_in, F_SETFD, flags)<0){
        		error("Error setting O_NONBLOCK flag for pipe %s",app_to_eard);
    		}
			/* It is a short type, it has no sense to execute a loop */
			int pending=sizeof(app_send_t)-ret,pos=ret;
			tries=0;
			while((pending >0) && (tries<MAX_TRIES)){
				ret=read(fd_in,(char *)app_req+pos,pending);
				if ((ret<0) && (errno!=EAGAIN)) pending=0;
				else if (ret>=0){
					pending-=ret;
					pos+=ret;
				}
				tries++;
			}	
			/* Set again original flags */
			fcntl(fd_in, F_SETFD, orig_flags);
			if (pending>0){	
				error("Error reading NON-EARL application request\n");
				return INVALID_COMMAND;
			}
		}
	}
	return app_req->req;
}


/** Releases resources to connect with applications */
void dispose_app_connection()
{
	close(fd_app_to_eard);
	unlink(app_to_eard);
	#if 0
	close(fd_eard_to_app);
	unlink(eard_to_app);
	#endif
}

/*********************************************************/
/***************** FUNCTIONS EXPORTED BY EARD ************/
/*********************************************************/

/** Returns the energy in mJ and the time in ms  */
void ear_energy(int fd_out)
{
	int fd_ack;
	app_recv_t data;
	ulong energy_mj,time_ms;
	
	/* Execute specific request */

	debug("ear_energy command\n");
	energy_mj=0;time_ms=0;

	read_dc_energy_time_try(my_eh,&energy_mj,&time_ms);

	#if 0
	/* Create connection */
	if (connect_with_app()!=EAR_SUCCESS){
		verbose(0,"Error connecting with NON-EARL application \n");
		return;
	}
	#endif

	/* Prepare the answer */
	data.ret=EAR_SUCCESS;
	data.my_data.my_energy.energy_mj=energy_mj;
	data.my_data.my_energy.time_ms=time_ms;

	send_app_answer(fd_out,&data);
	#if 0
	/* Close the connection with app */
	close_app_connection();	
	#endif

	
	return;

}

void ear_energy_debug(int fd_out)
{
        int fd_ack;
        app_recv_t data;
        ulong energy_j,energy_mj,time_sec,time_ms;
        struct timespec tspec;

        memset(&data.my_data.my_energy,sizeof(data.my_data.my_energy),0);

        /* Execute specific request */
        data.ret=EAR_SUCCESS;

        if (clock_gettime(CLOCK_REALTIME, &tspec)) {
                data.ret=EAR_ERROR;
                data.my_data.my_energy.os_time_sec=0;
                data.my_data.my_energy.os_time_ms=0;
        }else{
                data.my_data.my_energy.os_time_sec=tspec.tv_sec;
                data.my_data.my_energy.os_time_ms=tspec.tv_nsec/1000000;
        }
				energy_j=0;energy_mj=0;time_sec=0;time_ms=0;
        read_dc_energy_time_debug_try(my_eh,&energy_j,&energy_mj,&time_sec,&time_ms);

		#if 0
        /* Create connection */
        if (connect_with_app()!=EAR_SUCCESS){
                verbose(0,"Error connecting with NON-EARL application \n");
                return;
        }
		#endif


        /* Prepare the answer */
        data.my_data.my_energy.energy_j=energy_j;
        data.my_data.my_energy.energy_mj=energy_mj;
        data.my_data.my_energy.time_sec=time_sec;
        data.my_data.my_energy.time_ms=time_ms;

        send_app_answer(fd_out,&data);

		#if 0
        /* Close the connection with app */
        close_app_connection();
		#endif


        return;

}





/***************************************************************/
/***************** THREAD IMPLEMENTING NON-EARL API ************/
/***************************************************************/

void process_connection(char *root)
{
	app_send_t req;
	debug("process_connection\n");
	if (read(fd_app_to_eard,&req,sizeof(app_send_t))!=sizeof(app_send_t)) return;
	switch (req.req){
		case CONNECT:
			accept_new_connection(root,req.pid);
			break;
	}	
}

int get_fd_out(int fd_in)
{
	int i;
	for(i=0;i<connections;i++){
		if (apps_eard[i].recv==fd_in) return apps_eard[i].send;
	}
	return EAR_ERROR;
}

void process_request(int fd_in)
{
	int req;
	app_send_t app_req;
	int fd_out;
	debug("process_request from %d\n",fd_in);
	fd_out=get_fd_out(fd_in);
	if (fd_out==EAR_ERROR){ 
		debug("fd_out not found for %d",fd_in);
		return;
	}
	req=read_app_command(fd_in,&app_req);
	switch (req){
	case DISCONNECT:
        close_connection(fd_in,fd_out);
		break;
	case ENERGY_TIME:
		ear_energy(fd_out);
		break;
     case ENERGY_TIME_DEBUG:
     	ear_energy_debug(fd_out);
     	break;
	case INVALID_COMMAND:
		verbose(0,"PANIC, invalid command received and not recognized\n");
		break;
	default:
		verbose(0,"PANIC, non-earl command received and not recognized\n");
		break;
	}
	
}

void *eard_non_earl_api_service(void *noinfo)
{
	fd_set rfds;
	int numfds_ready;
	int max_fd,i;

	app_api_set_sigterm();

	/* Create connections */
	if (create_app_connection(my_cluster_conf.tmp_dir)!= EAR_SUCCESS){ 
		verbose(0,"Error creating files for non-EARL requests\n");
		pthread_exit(0);
	}
	FD_ZERO(&rfds);
	FD_SET(fd_app_to_eard, &rfds);
	/* fd_eard_to_app is created the last */
    max_fd=fd_app_to_eard;
    numfds_req=max_fd+1;
	rfds_basic=rfds;

	/* Initializing energy */
	node_energy_init(&my_eh);
	/* Wait for messages */
	verbose(0,"Waiting for non-earl requestst\n");
	while ((eard_must_exit==0) && (numfds_ready=select(numfds_req,&rfds,NULL,NULL,NULL))>=0){
		if (numfds_ready>0){
			for (i=0;i<numfds_req;i++){
				if (FD_ISSET(i,&rfds)){
					if (i==fd_app_to_eard){
						process_connection(my_cluster_conf.tmp_dir);
					}else{
						process_request(i);	
					}
				}
			}	
		}
		rfds=rfds_basic;	
	}
	/* Close and remove files , never reached if thread is killed */
	node_energy_dispose(my_eh);
	dispose_app_connection();
	pthread_exit(0);
}


