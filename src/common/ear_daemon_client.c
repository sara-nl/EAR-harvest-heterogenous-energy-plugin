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
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <common/ear_verbose.h>
#include <common/ear_daemon_common.h>
#include <common/ear_daemon_client.h>

#include <common/types/generic.h>
#include <common/states.h>

static int ear_fd_req[ear_daemon_client_requests],ear_fd_ack[ear_daemon_client_requests];
static unsigned long uncore_size,rapl_size,freq_size,energy_size;
char ear_commreq[1024],ear_commack[1024];
char ear_ping[1024];
int ear_ping_fd;
#define MAX_TRIES 5
char *ear_tmp;

void warning(int return_value, int expected, char *msg)
{
	if (return_value!=expected) ear_verbose(0,"ear_daemon_client %s\n",msg);
	if (return_value<0) 		ear_verbose(0,"ear_daemon_client %s\n",strerror(errno));
}

int ear_daemon_client_connect()
{
        char nodename[256];
	unsigned long ret,ack;
	struct daemon_req req;
	int tries=0,connected=0;
        int i;

        // These files connect EAR with EAR_COMM
        ear_tmp=getenv("EAR_TMP");
       if (ear_tmp==NULL){
        	ear_tmp=getenv("TMP");
        	if (ear_tmp==NULL) ear_tmp=getenv("HOME");
        }
        if (gethostname(nodename,sizeof(nodename))<0){
			ear_verbose(0,"EAR_daemon_client:Error getting node name:%s\n",strerror(errno));
			return EAR_ERROR;
        }
		for (i=0;i<ear_daemon_client_requests;i++){
			ear_fd_req[i]=-1;
			ear_fd_ack[i]=-1;
		}
		req.req_data.req_value=getpid();
		for (i=0;i<ear_daemon_client_requests;i++){
			ear_debug(2,"ear_client connecting with service %d\n",i);
        	sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
        	sprintf(ear_commack,"%s/.ear_comm.ack_%d.%d",ear_tmp,i,getpid());
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
					return EAR_ERROR;
				}
				// ping pipe is used just for synchronization 
				sprintf(ear_ping,"%s/.ear_comm.ping.%d",ear_tmp,getpid());
				ret=mknod(ear_ping,S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,0);
				if (ret<0) ear_verbose(0,"ear_client, error connecting ear_daemon\n");
				ear_ping_fd=open(ear_ping,O_RDWR);
				if (ear_ping_fd<0){
					ear_verbose(0,"EAR_daemon_client: error opening ping pipe %s\n",strerror(errno));
					return EAR_ERROR;
				}
				ear_debug(2,"ear_client ping conn created\n");
			}else{
				// ear_daemon will send an ack when pipe is created
        		if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY))<0){
					ear_verbose(0,"EAR_daemon_client:Error opening ear communicator for requests %s\n",ear_commreq);
					return EAR_ERROR;
				} 
			}
			switch(i){
			case freq_req:
				// When using a single communicator, we should send only a frequency connection request 
				req.req_service=CONNECT_FREQ; 
				break;
#ifdef MULTIPLE_SERVICES
			case uncore_req:
				req.req_service=CONNECT_UNCORE;
				break;
			case rapl_req:
				req.req_service=CONNECT_RAPL;
				break;	
			case system_req:
				req.req_service=CONNECT_SYSTEM;
				break;
			case node_energy_req:
				req.req_service=CONNECT_ENERGY;
				break;
#endif
			}
			if (ear_fd_req[i]>0){ 
				warning(write(ear_fd_req[i],&req,sizeof(req)),sizeof(req),"writting req_service in ear_daemon_client_connect");
			}
			ear_verbose(2,"EAR_daemon_client:comm_req %s [%d] connected\n",nodename,i);
        	if (ear_fd_req[i]>=0){
				// ear_demon sends an ack when ack pipe for specific service is created
				ear_debug(1,"ear_client: waiting for ear_daemon ok\n");
				if (read(ear_ping_fd,&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
					ear_verbose(0,"EAR_daemon_client: error reading ping conn (%d) %s\n",i,strerror(errno));
				}	
				ear_debug(1,"ear_client: ear_daemon ok for service %d, opening ack\n",i);
        		if ((ear_fd_ack[i]=open(ear_commack,O_RDONLY))<0){
        			ear_verbose(0,"EAR_daemon_client:Error opening ear communicator for ack(%s):%s\n",ear_commack,strerror(errno));
					return EAR_ERROR;
        		}
				ear_verbose(2,"EAR_daemon_client:comm_ack %s [%d] connected\n",nodename,i);
        	}
	}
	ear_debug(1,"EAR_daemon_client:Connected\n");
	return EAR_SUCCESS;

}
void ear_daemon_client_disconnect()
{
	int i;
	struct daemon_req req;
	req.req_service=END_COMM;
	ear_debug(1,"EAR_daemon_client:Disconnecting\n");
    if (ear_fd_req[0]>=0) warning(write(ear_fd_req[0],&req,sizeof(req)),sizeof(req),"witting req in ear_daemon_client_disconnect");
	for (i=0;i<ear_daemon_client_requests;i++){
        	if (ear_fd_req[i]>=0){
       		 	close(ear_fd_req[i]);
        		close(ear_fd_ack[i]);
        	}

	}
}
//////////////// SYSTEM REQUESTS
unsigned long ear_daemon_client_write_app_signature(application_t *app_signature)
{
	int com_fd=system_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	ear_debug(2,"EAR_daemon_client:asking ear_daemon to write app_signature\n");
	req.req_service=WRITE_APP_SIGNATURE;
	memcpy(&req.req_data.app, app_signature, sizeof(application_t));
	if (ear_fd_req[com_fd]>=0){
		if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
			ear_verbose(0,"EAR: Error sending request ear_daemon_client_write_app_signature\n");
			return EAR_ERROR;
		}	
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
        	ear_verbose(0,"EAR: Error ear_daemon_client_write_app_signature ack:%s\n",strerror(errno));
			return EAR_ERROR;
        }   
	}else{
		ear_debug(0,"EAR_daemon_client ear_daemon_client_write_app_signature service not provided\n");
		ack=EAR_SUCCESS;
	}
	return ack;
}
//////////////// FREQUENCY REQUESTS
unsigned long ear_daemon_client_get_data_size_frequency()
{
	int com_fd=freq_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	ear_debug(2,"EAR_daemon_client:asking for frequency data size \n");
    req.req_service=DATA_SIZE_FREQ;
    if (ear_fd_req[com_fd]>=0){
            if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_frequency:%s\n",strerror(errno));
					return EAR_ERROR;
            }   
            if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_frequency:%s\n",strerror(errno));
					return EAR_ERROR;
            }   
        }   
    else{
        ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_frequency service not provided\n");
    }   
	freq_size=ack;	
    return ack;
}
void ear_daemon_client_begin_compute_turbo_freq()
{
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

	ear_debug(2,"EAR_daemon_client:start getting turbo freq \n");
    req.req_service=START_GET_FREQ;
    if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_begin_compute_turbo_freq:%s\n",strerror(errno));
					return ;
            }   
            if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_begin_compute_turbo_freq:%s\n",strerror(errno));
					return ;
            }   
			return;
   }   
   ear_debug(0,"EAR_daemon_client: ear_daemon_client_begin_compute_turbo_freq service not provided\n");
   return;
}

unsigned long ear_daemon_client_end_compute_turbo_freq()
{
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	ear_debug(2,"EAR_daemon_client:end getting turbo freq \n");
    req.req_service=END_GET_FREQ;

        if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req))
			{
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_end_compute_turbo_freq:%s\n",
							strerror(errno));
				return EAR_ERROR;
            }   
            if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long))
			{
                    ear_verbose(0,"EAR: Error ear_daemon_client_end_compute_turbo_freq ACK:%s\n",
								strerror(errno));
					return EAR_ERROR;
            }
		ear_verbose(2,"EAR_daemon_client: TURBO freq computed as  %lu\n", ack);
        }else{
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_end_compute_turbo_freq service not provided\n");
    	}   
    return ack;
}
void ear_daemon_client_begin_app_compute_turbo_freq()
{
    struct daemon_req req;
    unsigned long ack=EAR_SUCCESS;
    ear_debug(2,"EAR_daemon_client:start getting turbo freq \n");
    req.req_service=START_APP_COMP_FREQ;
    if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_begin_app_compute_turbo_freq:%s\n",strerror(errno));
                    return ;
            }
            if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_begin_app_compute_turbo_freq:%s\n",strerror(errno));
                    return ;
            }
            return;
   }
   ear_debug(0,"EAR_daemon_client: ear_daemon_client_begin_app_compute_turbo_freq service not provided\n");
   return;
}
unsigned long ear_daemon_client_end_app_compute_turbo_freq()
{
        struct daemon_req req;
        unsigned long ack=EAR_SUCCESS;
        ear_debug(2,"EAR_daemon_client:end getting turbo freq \n");
        req.req_service=END_APP_COMP_FREQ;
        if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_end_app_compute_turbo_freq:%s\n",strerror(errno));
                return EAR_ERROR;
            }
            if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_app_end_compute_turbo_freq ACK:%s\n",strerror(errno));
                    return EAR_ERROR;
            }
            ear_verbose(2,"EAR_daemon_client: TURBO freq computed as  %lu\n",ack);
        }else{
            ear_debug(0,"EAR_daemon_client: ear_daemon_client_end_app_compute_turbo_freq service not provided\n");
        }
        return ack;
}

void ear_daemon_client_set_turbo()
{
	struct daemon_req req;
	unsigned long ack;
	req.req_service=SET_TURBO;
	ear_debug(2,"EAR_daemon_client:Set turbo\n");
	if (ear_fd_req[freq_req]>=0){
		if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
			ear_verbose(0,"EAR: Error setting turbo(SW) on:%s\n",strerror(errno));
			return;
		}   
		if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
			ear_verbose(0,"EAR: Error setting turbo ACK:%s\n",strerror(errno));
			return;
		}   
		ear_verbose(3,"EAR_daemon_client: turbo activated\n");
		return;
	}   
	ear_debug(0,"EAR_daemon_client: turbo not provided\n");
    return;

}
unsigned long ear_daemon_client_change_freq(unsigned long newfreq)
{
	unsigned long real_freq = EAR_ERROR;
	struct daemon_req req;

	req.req_service = SET_FREQ;
	req.req_data.req_value = newfreq;

	ear_debug(2,"EAR_daemon_client:NewFreq %u requested\n", newfreq);

	if (ear_fd_req[freq_req] >= 0)
	{
		if (write(ear_fd_req[freq_req],&req, sizeof(req)) != sizeof(req))
		{
			ear_verbose(0,"EAR: Error changing node frequency:%s\n", strerror(errno));
			_exit(1);
		}

		if (read(ear_fd_ack[freq_req], &real_freq, sizeof(unsigned long)) != sizeof(unsigned long))
		{
			ear_verbose(0,"EAR: Error changing node frequency ACK:%s\n", strerror(errno));
			_exit(1);
		}

		ear_verbose(3,"EAR_daemon_client: Frequency_changed to %lu\n", real_freq);
	} else {
		real_freq = 0;
		ear_debug(0,"EAR_daemon_client: change_freq service not provided\n");
	}

	return real_freq;
}

// END FREQUENCY SERVICES
//////////////// UNCORE REQUESTS
unsigned long ear_daemon_client_get_data_size_uncore()
{
    int com_fd=uncore_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	ear_debug(2,"EAR_daemon_client:asking for uncore data size \n");
    req.req_service=DATA_SIZE_UNCORE;
        if (ear_fd_req[com_fd]>=0){
            if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_uncore:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_uncore:%s\n",strerror(errno));
					return EAR_ERROR;
            }
        } else{
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_uncore service not provided\n");
			ack=EAR_ERROR;
    	}
    uncore_size=ack;
    return ack;
}


int ear_daemon_client_reset_uncore()
{
   	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	req.req_service=RESET_UNCORE;
	ear_debug(2,"EAR_daemon_client:reset uncore\n");
    if (ear_fd_req[uncore_req]>=0){
            if (write(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error requesting RESET uncore counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            if (read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading RESET ACK uncore counters from EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            ear_debug(3,"EAR_daemon_client: RESET ACK Uncore counters received from daemon \n");
	    return EAR_SUCCESS;
    }
	ear_debug(0,"EAR_daemon_client:reset uncore service not provided \n");
	return EAR_ERROR;
}

int ear_daemon_client_start_uncore()
{
	struct daemon_req req;
	unsigned long ack;
	int ret;
    req.req_service=START_UNCORE;
	ear_debug(2,"EAR_daemon_client:start uncore\n");
    if (ear_fd_req[uncore_req]>=0){
            if (write(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error requesting uncore counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            if ((ret=read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long)))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading START ACK uncore counters from EAR daemon:ret=%d %s\n",ret,strerror(errno));
					return EAR_ERROR;
            }
            ear_debug(3,"EAR_daemon_client: START ACK Uncore counters received from daemon \n");
		return EAR_SUCCESS;
    }
	ear_debug(0,"EAR_daemon_client: start uncore service not provided \n");
	return EAR_ERROR;
}

int ear_daemon_client_read_uncore(unsigned long long *values)
{
	struct daemon_req req;
	unsigned long ack;
	
	req.req_service=READ_UNCORE;
	ear_debug(2, "EAR_daemon_client:reading uncore counters\n");

	if (ear_fd_req[uncore_req]>=0){
		// There is not request for uncore...only answer
        	if (write(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)){
                	ear_verbose(0,"EAR: Error requesting STOP&READ uncore counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
        	}
        	if (read(ear_fd_ack[uncore_req],values,uncore_size)!=uncore_size){
                 	ear_verbose(0,"EAR: Error reading VALUES uncore counters from EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
        	}else{
			ack=EAR_SUCCESS;
		}
		int i;
		unsigned long long cas_client=0;
		for (i=0;i<uncore_size/sizeof(unsigned long long);i++) cas_client+=values[i];
		ear_debug(4,"CLIENT UNCORE cas %llu values %d\n",cas_client,uncore_size/sizeof(unsigned long long));
	}else{
		ack=EAR_ERROR;
		ear_debug(0,"EAR_daemon_client: read uncore service not provided\n");
	}
	return ack;
}
// END UNCORE SERVICES

//////////////// RAPL REQUESTS
unsigned long ear_daemon_client_get_data_size_rapl() // size in bytes
{
    int com_fd=rapl_req;
    struct daemon_req req;
	unsigned long ack;

    ear_debug(2,"EAR_daemon_client:asking for rapl data size \n");
    req.req_service=DATA_SIZE_RAPL;
    ack=EAR_SUCCESS;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_get_data_size_rapl:%s\n",strerror(errno));
				return EAR_ERROR;
        }
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_get_data_size_rapl:%s\n",strerror(errno));
					return EAR_ERROR;
        }
    }else {
        	ear_debug(0,"EAR_daemon_client: ear_daemon_client_get_data_size_rapl service not provided\n");
			ack=EAR_ERROR;
	}
    rapl_size=ack;
    return ack;
}


int ear_daemon_client_reset_rapl()
{
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
    req.req_service=RESET_RAPL;
	ear_debug(2,"EAR_daemon_client:reset rapl\n");
    if (ear_fd_req[rapl_req]>=0){
            if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error requesting RESET rapl counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            if (read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading RESET ACK rapl counters from EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
			ear_debug(3,"EAR_daemon_client: RESET ACK rapl counters received from daemon \n");
			return ack;
    }
	ear_debug(0,"EAR_daemon_client: reset rapl service not provided\n");
	return EAR_ERROR;

}

int ear_daemon_client_start_rapl()
{
	struct daemon_req req;
	unsigned long ack;
	int ret;
    req.req_service=START_RAPL;
	ear_debug(2,"EAR_daemon_client:start rapl\n");
    if (ear_fd_req[rapl_req]>=0){
            if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
                    ear_verbose(0,"EAR: Error requesting rapl counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
            }
            if ((ret=read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long)))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error reading START ACK rapl counters from EAR daemon ret=%d:%s\n",ret,strerror(errno));
					return EAR_ERROR;
            }
			ear_debug(3,"EAR_daemon_client: START ACK rapl counters received from daemon \n");
		return EAR_SUCCESS;
    }
	ear_debug(0,"EAR_daemon_client: start rapl service not provided\n");
	return EAR_ERROR;
}

int ear_daemon_client_read_rapl(unsigned long long *values)
{
	struct daemon_req req;
    	unsigned long ack;
	
	req.req_service=READ_RAPL;
	
	unsigned long long acum_energy=0;
	int i;
	ear_debug(2,"EAR_daemon_client:reading rapl counters\n");
	if (ear_fd_req[rapl_req]>=0){
		// There is not request for uncore...only answer
        	if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
                	ear_verbose(0,"EAR: Error requesting STOP&READ rapl counters to EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
        	}
        	if (read(ear_fd_ack[rapl_req],values,rapl_size)!=rapl_size){
                 	ear_verbose(0,"EAR: Error reading VALUES rapl counters from EAR daemon:%s\n",strerror(errno));
					return EAR_ERROR;
        	}else{
			ack=EAR_SUCCESS;
			}
			for (i=0;i<rapl_size/sizeof(unsigned long long);i++){
				acum_energy+=values[i];
			}
			ear_debug(4,"EAR_RAPL_CLIENT acum energy %llu\n",acum_energy);
	}else{
		ear_debug(0,"EAR_daemon_client: read rapl service not provided\n");
		ack=EAR_ERROR;	
	}
	return ack;
}
// END RAPL SERVICES
// NODE ENERGY SERVICES
unsigned long ear_daemon_client_node_energy_data_size()
{
    int com_fd=node_energy_req;
    struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

    ear_debug(2,"EAR_daemon_client:asking for node energy data size \n");
    req.req_service=DATA_SIZE_ENERGY_NODE;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_node_energy_data_size:%s\n",strerror(errno));
                return EAR_ERROR; 
        }   
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_node_energy_data_size:%s\n",strerror(errno));
                    return EAR_ERROR; 
        }   
    }else {
            ear_debug(0,"EAR_daemon_client: ear_daemon_client_node_energy_data_size service not provided\n");
            ack=EAR_ERROR;
    }   
    energy_size=ack;
    return ack;
}
int ear_daemon_client_node_dc_energy(unsigned long *energy)
{
    int com_fd=node_energy_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

    ear_debug(2,"EAR_daemon_client:asking for node dc energy \n");
    req.req_service=READ_DC_ENERGY;
    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                ear_verbose(0,"EAR: Error sending request ear_daemon_client_node_dc_energy:%s\n",strerror(errno));
                return EAR_ERROR; 
        }   
        if (read(ear_fd_ack[com_fd],energy,sizeof(unsigned long))!=sizeof(unsigned long)){
                    ear_verbose(0,"EAR: Error ear_daemon_client_node_dc_energy:%s\n",strerror(errno));
                    return EAR_ERROR; 
        }   
    }else {
            ear_debug(0,"EAR_daemon_client: ear_daemon_client_node_dc_energy service not provided\n");
            ack=EAR_ERROR;
    }   
    return ack;

}

//END NODE ENERGY SERVICES
