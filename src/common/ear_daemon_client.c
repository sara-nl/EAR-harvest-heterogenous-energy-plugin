/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ear_verbose.h>
#include <ear_daemon_common.h>
#include <ear_daemon_client.h>
#include <ear_db_type.h>


static int ear_daemon_client_connected=0;
static int ear_fd_req[ear_daemon_client_requests],ear_fd_ack[ear_daemon_client_requests];
static unsigned long ear_data_sizes[ear_daemon_client_requests];
char ear_commreq[1024],ear_commack[1024];
char ear_ping[1024];
int ear_ping_fd;
#define MAX_TRIES 5
char *ear_tmp;
int ear_daemon_client_connect()
{
        int status,i,retval;
        char nodename[256];
		unsigned long ret,req[2],ack;
		int tries=0,connected=0;

        // These files connect EAR with EAR_COMM
        ear_tmp=getenv("EAR_TMP");
       if (ear_tmp==NULL){
        	ear_tmp=getenv("TMP");
        	if (ear_tmp==NULL) ear_tmp=getenv("HOME");
        }
        if (gethostname(nodename,sizeof(nodename))<0){
			ear_verbose(0,"EAR_daemon_client:Error getting node name:%s\n",strerror(errno));
			return EAR_COM_ERROR;
        }
		for (i=0;i<ear_daemon_client_requests;i++){
			ear_fd_req[i]=-1;
			ear_fd_ack[i]=-1;
		}
		req[1]=getpid();
		for (i=0;i<ear_daemon_client_requests;i++){
			ear_verbose(1,"ear_client connecting with service %d\n",i);
        	sprintf(ear_commreq,"%s/.ear_comm_%s.req_%d",ear_tmp,nodename,i);
        	sprintf(ear_commack,"%s/.ear_comm_%s.ack_%d.%d",ear_tmp,nodename,i,getpid());
			// Sometimes ear_daemon needs some time to startm we will wait for the first one
			if (i==0){
				// First connection is special, we should wait
				do{
        			if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY|O_NONBLOCK))<0) tries++;
        			else connected=1;
					if (!connected) sleep(1);
				}while ((tries<MAX_TRIES) && !connected);
				if (!connected){		
					// Not possible to connect with ear_daemon
					ear_verbose(0,"EAR_daemon_client:Error opening ear communicator for requests %s (%d attempts to connect):%s\n",ear_commreq,tries,strerror(errno));
					return EAR_COM_ERROR;
				}
				// ping pipe is used just for synchronization 
				sprintf(ear_ping,"%s/.ear_comm_%s.ping.%d",ear_tmp,nodename,getpid());
				ret=mknod(ear_ping,S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,0);
				if (ret<0) ear_verbose(0,"ear_client, error connecting ear_daemon\n");
				ear_ping_fd=open(ear_ping,O_RDWR);
				if (ear_ping_fd<0){
					ear_verbose(0,"EAR_daemon_client: error opening ping pipe %s\n",strerror(errno));
					return EAR_COM_ERROR;
				}
				ear_verbose(1,"ear_client ping conn created\n");
			}else{
				// ear_daemon will send an ack when pipe is created
        		if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY))<0){
					ear_verbose(0,"EAR_daemon_client:Error opening ear communicator for requests %s\n",ear_commreq);
					return EAR_COM_ERROR;
				} 
			}
			switch(i){
			case freq_req:
				req[0]=CONNECT_FREQ; 
				break;
			case uncore_req:
				req[0]=CONNECT_UNCORE;
				break;
			case rapl_req:
				req[0]=CONNECT_RAPL;
				break;	
			case system_req:
				req[0]=CONNECT_SYSTEM;
				break;
			case node_energy_req:
				req[0]=CONNECT_ENERGY;
				break;
			}
			if (ear_fd_req[i]>0) write(ear_fd_req[i],req,sizeof(req));
			ear_verbose(2,"EAR_daemon_client:comm_req %s [%d] connected\n",nodename,i);
        	if (ear_fd_req[i]>=0){
				// ear_demon sends an ack when ack pipe for specific service is created
				ear_verbose(1,"ear_client: waiting for ear_daemon ok\n");
				if (read(ear_ping_fd,&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
					ear_verbose(0,"EAR_daemon_client: error reading ping conn (%d) %s\n",i,strerror(errno));
				}	
				ear_verbose(1,"ear_client: ear_daemon ok for service %d, opening ack\n",i);
        		if ((ear_fd_ack[i]=open(ear_commack,O_RDONLY))<0){
        			ear_verbose(0,"EAR_daemon_client:Error opening ear communicator for ack(%s):%s\n",ear_commack,strerror(errno));
					return EAR_COM_ERROR;
        		}
				ear_verbose(2,"EAR_daemon_client:comm_ack %s [%d] connected\n",nodename,i);
        	}
	}
	ear_debug(1,"EAR_daemon_client:Connected\n");
	return EAR_COM_OK;

}
void ear_daemon_client_disconnect()
{
	int i;
    unsigned long end_com=END_COMM,ack;
	ear_debug(1,"EAR_daemon_client:Disconnecting\n");
    if (ear_fd_req[0]>=0) write(ear_fd_req[0],&end_com,sizeof(unsigned long));
	for (i=0;i<ear_daemon_client_requests;i++){
        	if (ear_fd_req[i]>=0){
       		 	close(ear_fd_req[i]);
        		close(ear_fd_ack[i]);
        	}

	}
}
//////////////// SYSTEM REQUESTS
unsigned long ear_daemon_client_write_app_signature(struct App_info *app_signature)
{
	int com_fd=system_req;
	struct req{
		unsigned long service;
		struct App_info app;
	}my_req;
	unsigned long req[2];
	ear_debug(2,"EAR_daemon_client:asking ear_daemon to write app_signature\n");
	my_req.service=WRITE_APP_SIGNATURE;
	memcpy(&my_req.app, app_signature, sizeof(struct App_info));
	if (ear_fd_req[com_fd]>=0){
		if (write(ear_fd_req[com_fd],&my_req,sizeof(struct req))!=sizeof(struct req)){
			ear_verbose(0,"EAR: Error sending request ear_daemon_client_write_app_signature\n");
			return 0;
		}	
        if (read(ear_fd_ack[com_fd],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
        	ear_verbose(0,"EAR: Error ear_daemon_client_write_app_signature ack:%s\n",strerror(errno));
        	_exit(1);
        }   
	}else{
		ear_debug(0,"EAR_daemon_client ear_daemon_client_write_app_signature service not provided\n");
		req[1]=0;
	}
	return req[1];
}
//////////////// FREQUENCY REQUESTS
unsigned long ear_daemon_client_get_data_size_frequency()
{
	int com_fd=freq_req;
    unsigned long req[2];
	ear_debug(2,"EAR_daemon_client:asking for frequency data size \n");
    req[0]=DATA_SIZE_FREQ;
    if (ear_fd_req[com_fd]>=0){
            if (write(ear_fd_req[com_fd],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_frequency:%s\n",strerror(errno));
                    _exit(1);
            }   
            if (read(ear_fd_ack[com_fd],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_frequency:%s\n",strerror(errno));
                    _exit(1);
            }   
        }   
    else{
        ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_frequency service not provided\n");
		req[1]=0;
    }   
	ear_data_sizes[com_fd]=req[1];	
    return req[1];
}
void ear_daemon_client_begin_compute_turbo_freq()
{
    unsigned long old_freq=EAR_COM_ERROR;
    unsigned long req[2];
	ear_debug(2,"EAR_daemon_client:start getting turbo freq \n");
    req[0]=START_GET_FREQ;
    req[1]=0;
    if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_begin_compute_turbo_freq:%s\n",strerror(errno));
                    _exit(1);
            }   
            if (read(ear_fd_ack[freq_req],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_begin_compute_turbo_freq:%s\n",strerror(errno));
                    _exit(1);
            }   
			return;
   }   
        ear_debug(0,"EAR_daemon_client: ear_daemon_client_begin_compute_turbo_freq service not provided\n");
    return;
}
unsigned long ear_daemon_client_end_compute_turbo_freq()
{
    unsigned long req[2];
	ear_debug(2,"EAR_daemon_client:end getting turbo freq \n");
    req[0]=END_GET_FREQ;
    req[1]=0;
        if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_end_compute_turbo_freq:%s\n",strerror(errno));
                    _exit(1);
            }   
            if (read(ear_fd_ack[freq_req],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_end_compute_turbo_freq ACK:%s\n",strerror(errno));
                    _exit(1);
            }   
            ear_verbose(2,"EAR_daemon_client: TURBO freq computed as  %u\n",req[1]);
        }else{
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_end_compute_turbo_freq service not provided\n");
			req[1]=0;
    	}   
    return req[1];
}
void ear_daemon_client_set_turbo()
{
	unsigned long req[2];
	req[0]=SET_TURBO;
	ear_debug(2,"EAR_daemon_client:Set turbo\n");
	if (ear_fd_req[freq_req]>=0){
		if (write(ear_fd_req[freq_req],req,sizeof(unsigned long))!=sizeof(unsigned long)){
			ear_verbose(0,"EAR: Error setting turbo(SW) on:%s\n",strerror(errno));
			_exit(1);
		}   
		if (read(ear_fd_ack[freq_req],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
			ear_verbose(0,"EAR: Error setting turbo ACK:%s\n",strerror(errno));
			_exit(1);
		}   
		ear_verbose(3,"EAR_daemon_client: turbo activated\n");
		return;
	}   
	ear_debug(0,"EAR_daemon_client: turbo not provided\n");
    return;

}
unsigned long ear_daemon_client_change_freq(unsigned long newfreq)
{
	unsigned long real_freq=EAR_COM_ERROR;
	unsigned long req[2];
	ear_debug(2,"EAR_daemon_client:NewFreq %u requested\n",newfreq);
	req[0]=SET_FREQ;
	req[1]=newfreq;
        if (ear_fd_req[freq_req]>=0){
        	if (write(ear_fd_req[freq_req],req,sizeof(req))!=sizeof(req)){
                	ear_verbose(0,"EAR: Error changing node frequency:%s\n",strerror(errno));
                	_exit(1);
        	}
        	if (read(ear_fd_ack[freq_req],&real_freq,sizeof(unsigned long))!=sizeof(unsigned long)){
                 	ear_verbose(0,"EAR: Error changing node frequency ACK:%s\n",strerror(errno));
                 	_exit(1);
        	}
        	ear_verbose(3,"EAR_daemon_client: Frequency_changed to %u\n",real_freq);
        }
	else{
		real_freq=0;
		ear_debug(0,"EAR_daemon_client: change_freq service not provided\n");
	}
	return real_freq;
}
// END FREQUENCY SERVICES
//////////////// UNCORE REQUESTS
unsigned long ear_daemon_client_get_data_size_uncore()
{
    int com_fd=uncore_req;
    unsigned long req[2];

	ear_debug(2,"EAR_daemon_client:asking for uncore data size \n");
    req[0]=DATA_SIZE_UNCORE;
    req[1]=0;
        if (ear_fd_req[com_fd]>=0){
            if (write(ear_fd_req[com_fd],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_uncore:%s\n",strerror(errno));
                    _exit(1);
            }
            if (read(ear_fd_ack[com_fd],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_uncore:%s\n",strerror(errno));
                    _exit(1);
            }
        } else{
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_uncore service not provided\n");
			req[1]=EAR_COM_ERROR;
    	}
    ear_data_sizes[com_fd]=req[1];
    return req[1];
}


int ear_daemon_client_reset_uncore()
{
    unsigned long req=RESET_UNCORE;
	unsigned long ack;
	ear_debug(2,"EAR_daemon_client:reset uncore\n");
    if (ear_fd_req[uncore_req]>=0){
            if (write(ear_fd_req[uncore_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error requesting RESET uncore counters to EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            if (read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading RESET ACK uncore counters from EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            ear_verbose(3,"EAR_daemon_client: RESET ACK Uncore counters received from daemon \n");
			return 0;
    }
	ear_debug(0,"EAR_daemon_client:reset uncore service not provided \n");
	return -1;
}

int ear_daemon_client_start_uncore()
{
    unsigned long req=START_UNCORE;
	unsigned long ack;
	ear_debug(2,"EAR_daemon_client:start uncore\n");
    if (ear_fd_req[uncore_req]>=0){
            if (write(ear_fd_req[uncore_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error requesting uncore counters to EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            if (read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading START ACK uncore counters from EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            ear_debug(3,"EAR_daemon_client: START ACK Uncore counters received from daemon \n");
		return 0;
    }
	ear_debug(0,"EAR_daemon_client: start uncore service not provided \n");

	return -1;
}

int ear_daemon_client_read_uncore(unsigned long long *values)
{
	unsigned long req=READ_UNCORE;
    unsigned long ack;
	unsigned long metrics;
	ear_debug(2,"EAR_daemon_client:reading uncore counters\n");
	if (ear_fd_req[uncore_req]>=0){
		// There is not request for uncore...only answer
        	if (write(ear_fd_req[uncore_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                	ear_verbose(0,"EAR: Error requesting STOP&READ uncore counters to EAR daemon:%s\n",strerror(errno));
					return -1;
        	}
        	if (read(ear_fd_ack[uncore_req],values,ear_data_sizes[uncore_req])!=ear_data_sizes[uncore_req]){
                 	ear_verbose(0,"EAR: Error reading VALUES uncore counters from EAR daemon:%s\n",strerror(errno));
					return -1;
        	}else{
			ack=EAR_COM_OK;
		}
		int i;
		unsigned long long cas_client=0;
		for (i=0;i<ear_data_sizes[uncore_req]/sizeof(unsigned long long);i++) cas_client+=values[i];
		ear_debug(4,"CLIENT UNCORE cas %llu values %d\n",cas_client,ear_data_sizes[uncore_req]/sizeof(unsigned long long));
	}else{
		ack=EAR_COM_ERROR;
		ear_debug(0,"EAR_daemon_client: read uncore service not provided\n");
	}
	return ack;
}
// END UNCORE SERVICES

//////////////// RAPL REQUESTS
unsigned long ear_daemon_client_get_data_size_rapl() // size in bytes
{
    int com_fd=rapl_req;
    unsigned long req[2];

    ear_debug(2,"EAR_daemon_client:asking for rapl data size \n");
    req[0]=DATA_SIZE_RAPL;
    req[1]=0;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_rapl:%s\n",strerror(errno));
				return -1;
        }
        if (read(ear_fd_ack[com_fd],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_rapl:%s\n",strerror(errno));
					return -1;
        }
    }else {
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_rapl service not provided\n");
			req[1]=EAR_COM_ERROR;
	}
    ear_data_sizes[com_fd]=req[1];
    return req[1];
}


int ear_daemon_client_reset_rapl()
{
    unsigned long req=RESET_RAPL;
	unsigned long ack;
	ear_debug(2,"EAR_daemon_client:reset rapl\n");
    if (ear_fd_req[rapl_req]>=0){
            if (write(ear_fd_req[rapl_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error requesting RESET rapl counters to EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            if (read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading RESET ACK rapl counters from EAR daemon:%s\n",strerror(errno));
					return -1;
            }
			ear_verbose(3,"EAR_daemon_client: RESET ACK rapl counters received from daemon \n");
			return 0;
    }
	ear_debug(0,"EAR_daemon_client: reset rapl service not provided\n");
	return -1;

}

int ear_daemon_client_start_rapl()
{
    unsigned long req=START_RAPL;
	unsigned long ack;
	ear_debug(2,"EAR_daemon_client:start rapl\n");
    if (ear_fd_req[rapl_req]>=0){
            if (write(ear_fd_req[rapl_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error requesting rapl counters to EAR daemon:%s\n",strerror(errno));
					return -1;
            }
            if (read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading START ACK rapl counters from EAR daemon:%s\n",strerror(errno));
					return -1;
            }
			ear_verbose(3,"EAR_daemon_client: START ACK rapl counters received from daemon \n");
		return 0;
    }
	ear_debug(0,"EAR_daemon_client: start rapl service not provided\n");
	return -1;
}

int ear_daemon_client_read_rapl(unsigned long long *values)
{
	unsigned long req=READ_RAPL;
    unsigned long ack;
	unsigned long metrics;
	
	unsigned long long acum_energy=0;
	int i;
	ear_debug(2,"EAR_daemon_client:reading rapl counters\n");
	if (ear_fd_req[rapl_req]>=0){
		// There is not request for uncore...only answer
        	if (write(ear_fd_req[rapl_req],&req,sizeof(unsigned long))!=sizeof(unsigned long)){
                	ear_verbose(0,"EAR: Error requesting STOP&READ rapl counters to EAR daemon:%s\n",strerror(errno));
					return -1;
        	}
        	if (read(ear_fd_ack[rapl_req],values,ear_data_sizes[rapl_req])!=ear_data_sizes[rapl_req]){
                 	ear_verbose(0,"EAR: Error reading VALUES rapl counters from EAR daemon:%s\n",strerror(errno));
					return -1;
        	}else{
			ack=EAR_COM_OK;
			}
			for (i=0;i<ear_data_sizes[rapl_req]/sizeof(unsigned long long);i++){
				acum_energy+=values[i];
			}
			ear_debug(4,"EAR_RAPL_CLIENT acum energy %llu\n",acum_energy);
	}else{
		ear_debug(0,"EAR_daemon_client: read rapl service not provided\n");
		ack=EAR_COM_ERROR;	
	}
	return ack;
}
// END RAPL SERVICES
// NODE ENERGY SERVICES
unsigned long ear_daemon_client_node_energy_data_size()
{
    int com_fd=node_energy_req;
    unsigned long req[2];

    ear_debug(2,"EAR_daemon_client:asking for node energy data size \n");
    req[0]=DATA_SIZE_ENERGY_NODE;
    req[1]=0;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_node_energy_data_size:%s\n",strerror(errno));
                return -1; 
        }   
        if (read(ear_fd_ack[com_fd],&req[1],sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_node_energy_data_size:%s\n",strerror(errno));
                    return -1; 
        }   
    }else {
            ear_debug(0,"EAR_daemon_client: ear_daemon_client_node_energy_data_size service not provided\n");
            req[1]=EAR_COM_ERROR;
    }   
    ear_data_sizes[com_fd]=req[1];
    return req[1];
}
int ear_daemon_client_node_dc_energy(unsigned long *energy)
{
    int com_fd=node_energy_req;
    unsigned long req[2];

    ear_debug(2,"EAR_daemon_client:asking for node dc energy \n");
    req[0]=READ_DC_ENERGY;
    req[1]=0;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],req,sizeof(unsigned long))!=sizeof(unsigned long)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_node_dc_energy:%s\n",strerror(errno));
                return -1; 
        }   
        if (read(ear_fd_ack[com_fd],energy,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_node_dc_energy:%s\n",strerror(errno));
                    return -1; 
        }   
    }else {
            ear_debug(0,"EAR_daemon_client: ear_daemon_client_node_dc_energy service not provided\n");
            req[1]=EAR_COM_ERROR;
    }   
    return req[1];

}

//END NODE ENERGY SERVICES
