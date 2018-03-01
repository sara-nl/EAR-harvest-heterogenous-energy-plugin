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

#define MAX_TRIES 5

// Name constant
static const char *__NAME__ = "EARD_CLIENT";

// Pipes
static int ear_fd_req[ear_daemon_client_requests];
static int ear_fd_ack[ear_daemon_client_requests];
char ear_commreq[1024];
char ear_commack[1024];
char ear_ping[1024];
int ear_ping_fd;
char *ear_tmp;

// Sizes
static unsigned long uncore_size;
static unsigned long energy_size;
static unsigned long rapl_size;
static unsigned long freq_size;

void warning(int return_value, int expected, char *msg)
{
	if (return_value != expected) VERBOSE_N(0,"ear_daemon_client %s",msg);
	if (return_value < 0) VERBOSE_N(0,"ear_daemon_client %s",strerror(errno));
}

int ear_daemon_client_connect()
{
	char nodename[256];
	unsigned long ret,ack;
	struct daemon_req req;
	int tries=0,connected=0;
	int i;

	// These files connect EAR with EAR_COMM
	ear_tmp = getenv("EAR_TMP");
	if (ear_tmp==NULL){
		ear_tmp=getenv("TMP");
		if (ear_tmp==NULL) ear_tmp=getenv("HOME");
	}

	if (gethostname(nodename,sizeof(nodename))<0){
		VERBOSE_N(0,"ERROR, getting node name (%s)",strerror(errno));
		return EAR_ERROR;
	}

	for (i=0;i<ear_daemon_client_requests;i++) {
		ear_fd_req[i]=-1;
		ear_fd_ack[i]=-1;
	}

	req.req_data.req_value=getpid();
	for (i=0;i<ear_daemon_client_requests;i++)
	{
		DEBUG_F(2,"connecting with service %d" ,i);

		sprintf(ear_commreq,"%s/.ear_comm.req_%d", ear_tmp,i);
		sprintf(ear_commack,"%s/.ear_comm.ack_%d.%d", ear_tmp, i, getpid());

		// Sometimes ear_daemon needs some time to startm we will wait for the first one
		if (i==0)
		{
			// First connection is special, we should wait
			do {
				if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY|O_NONBLOCK))<0) tries++;
				else connected=1;
				if (!connected) sleep(1);
			} while ((tries<MAX_TRIES) && !connected);

			if (!connected){
				// Not possible to connect with ear_daemon
				VERBOSE_N(0,"ERROR, opening ear communicator for requests %s (%d attempts to connect) (%s)",
						  ear_commreq, tries, strerror(errno));
				return EAR_ERROR;
			}
			
			// ping pipe is used just for synchronization
			sprintf(ear_ping,"%s/.ear_comm.ping.%d",ear_tmp,getpid());
			ret=mknod(ear_ping,S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,0);

			if (ret<0) VERBOSE_N(0,"ERROR, connecting ear_daemon");
			ear_ping_fd=open(ear_ping,O_RDWR);
			if (ear_ping_fd<0){
				VERBOSE_N(0, "ERROR, opening ping pipe %s",strerror(errno));
				return EAR_ERROR;
			}
			DEBUG_F(2, "ping connection created");
		}else{
			// ear_daemon will send an ack when pipe is created
			if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY))<0) {
				VERBOSE_N(0,"ERROR, opening ear communicator for requests %s", ear_commreq);
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
			warning(write(ear_fd_req[i],&req,sizeof(req)),sizeof(req),
					"writting req_service in ear_daemon_client_connect");
		}
		VERBOSE_N(2,"comm_req %s [%d] connected",nodename,i);

		if (ear_fd_req[i]>=0){
			// ear_demon sends an ack when ack pipe for specific service is created
			DEBUG_F(1,"WAITING for ear_daemon ok");
			if (read(ear_ping_fd,&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
				VERBOSE_N(0," ERROR, reading ping conn (%d) %s",i,strerror(errno));
			}
			DEBUG_F(1,"OK for service %d, opening ack",i);
			if ((ear_fd_ack[i]=open(ear_commack,O_RDONLY))<0){
				VERBOSE_N(0,"ERROR, opening ear communicator for ack(%s) (%s)",ear_commack,strerror(errno));
				return EAR_ERROR;
			}
			VERBOSE_N(2,"comm_ack %s [%d] connected",nodename,i);
		}
	}
	DEBUG_F(1,"daemon onnected");
	return EAR_SUCCESS;

}
void ear_daemon_client_disconnect()
{
	int i;
	struct daemon_req req;
	req.req_service=END_COMM;

	DEBUG_F(1, "disconnecting");

    if (ear_fd_req[0]>=0) warning(write(ear_fd_req[0],&req,sizeof(req)),sizeof(req),
								  "witting req in ear_daemon_client_disconnect");

	for (i=0;i<ear_daemon_client_requests;i++) {
		if (ear_fd_req[i]>=0) {
			close(ear_fd_req[i]);
			close(ear_fd_ack[i]);
		}
	}
}

unsigned long ear_daemon_client_write_app_signature(application_t *app_signature)
{
	int com_fd=system_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

	DEBUG_F(2, "asking ear_daemon to write app_signature");
	req.req_service=WRITE_APP_SIGNATURE;
	memcpy(&req.req_data.app, app_signature, sizeof(application_t));

	if (ear_fd_req[com_fd] >= 0)
	{
		if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0, "ERROR, sending request ear_daemon_client_write_app_signature");
			return EAR_ERROR;
		}	
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
        	VERBOSE_N(0, "ERROR, ear_daemon_client_write_app_signature ack (%s)", strerror(errno));
			return EAR_ERROR;
        }   
	} else {
		DEBUG_F(0, "WARNING, ear_daemon_client_write_app_signature service not provided");
		ack=EAR_SUCCESS;
	}
	return ack;
}

unsigned long ear_daemon_client_get_data_size_frequency()
{
	int com_fd=freq_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

	DEBUG_F(2,"asking for frequency data size ");
    req.req_service=DATA_SIZE_FREQ;

    if (ear_fd_req[com_fd]>=0){
		if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, sending request ear_daemon_client_get_data_size_frequency (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
			VERBOSE_N(0,"ERROR, ear_daemon_client_get_data_size_frequency (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
	}
    else{
        DEBUG_F(0, "WARNING, ear_daemon_client_get_data_size_frequency service not provided");
    }

	freq_size=ack;	
    return ack;
}
void ear_daemon_client_begin_compute_turbo_freq()
{
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

	DEBUG_F(2, "start getting turbo freq");
    req.req_service=START_GET_FREQ;

    if (ear_fd_req[freq_req]>=0)
	{
		if (write(ear_fd_req[freq_req],&req, sizeof(req)) != sizeof(req)){
				VERBOSE_N(0,"ERROR, sending request ear_daemon_client_begin_compute_turbo_freq (%s)",
						  strerror(errno));
				return;
		}   
		if (read(ear_fd_ack[freq_req],&ack, sizeof(unsigned long)) != sizeof(unsigned long)){
				VERBOSE_N(0,"ERROR, ear_daemon_client_begin_compute_turbo_freq (%s)"
						,strerror(errno));
				return;
		}   
		return;
   }   
   DEBUG_F(0, "WARNING, ear_daemon_client_begin_compute_turbo_freq service not provided");
   return;
}

unsigned long ear_daemon_client_end_compute_turbo_freq()
{
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

	DEBUG_F(2,"end getting turbo freq ");
    req.req_service=END_GET_FREQ;

	if (ear_fd_req[freq_req]>=0)
	{
		if (write(ear_fd_req[freq_req],&req, sizeof(req)) != sizeof(req))
		{
			VERBOSE_N(0,"ERROR, sending request ear_daemon_client_end_compute_turbo_freq (%s)",
						strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long))
		{
				VERBOSE_N(0,"ERROR, ear_daemon_client_end_compute_turbo_freq ACK (%s)",
							strerror(errno));
				return EAR_ERROR;
		}

		// SPAM
		VERBOSE_N(2,"TURBO freq computed as  %lu", ack);

	} else {
		DEBUG_F(0,"WARNING, ear_daemon_client_end_compute_turbo_freq service not provided");
	}
    return ack;
}

void ear_daemon_client_begin_app_compute_turbo_freq()
{
    struct daemon_req req;
    unsigned long ack=EAR_SUCCESS;

    DEBUG_F(2, "start getting turbo freq");
    req.req_service=START_APP_COMP_FREQ;
    if (ear_fd_req[freq_req]>=0)
	{
		if (write(ear_fd_req[freq_req],&req,sizeof(req)) != sizeof(req)){
				VERBOSE_N(0,"ERROR, sending request ear_daemon_client_begin_app_compute_turbo_freq (%s)",
						  strerror(errno));
				return;
		}
		if (read(ear_fd_ack[freq_req],&ack, sizeof(unsigned long)) != sizeof(unsigned long)) {
				VERBOSE_N(0,"ERROR, ear_daemon_client_begin_app_compute_turbo_freq (%s)",
						  strerror(errno));
				return;
		}
		return;
   }

   DEBUG_F(0,"WARNING, ear_daemon_client_begin_app_compute_turbo_freq service not provided");
   return;
}

unsigned long ear_daemon_client_end_app_compute_turbo_freq()
{
        struct daemon_req req;
        unsigned long ack=EAR_SUCCESS;
        DEBUG_F(2,"end getting turbo freq ");
        req.req_service=END_APP_COMP_FREQ;
        if (ear_fd_req[freq_req]>=0){
            if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
                VERBOSE_N(0,"ERROR, sending request ear_daemon_client_end_app_compute_turbo_freq (%s)",
						  strerror(errno));
                return EAR_ERROR;
            }
            if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    VERBOSE_N(0,"ERROR, ear_daemon_client_app_end_compute_turbo_freq ACK (%s)",
							  strerror(errno));
                    return EAR_ERROR;
            }
            VERBOSE_N(2," TURBO freq computed as  %lu",ack);
        }else{
            DEBUG_F(0," ear_daemon_client_end_app_compute_turbo_freq service not provided");
        }
        return ack;
}

void ear_daemon_client_set_turbo()
{
	struct daemon_req req;
	unsigned long ack;
	req.req_service=SET_TURBO;
	DEBUG_F(2,"Set turbo");
	if (ear_fd_req[freq_req]>=0){
		if (write(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, setting turbo(SW) on (%s)",strerror(errno));
			return;
		}   
		if (read(ear_fd_ack[freq_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
			VERBOSE_N(0,"ERROR, setting turbo ACK (%s)",strerror(errno));
			return;
		}   
		VERBOSE_N(3," turbo activated");
		return;
	}   
	DEBUG_F(0," turbo not provided");
    return;

}
unsigned long ear_daemon_client_change_freq(unsigned long newfreq)
{
	unsigned long real_freq = EAR_ERROR;
	struct daemon_req req;

	req.req_service = SET_FREQ;
	req.req_data.req_value = newfreq;

	DEBUG_F(2,"NewFreq %lu requested", newfreq);

	if (ear_fd_req[freq_req] >= 0)
	{
		if (write(ear_fd_req[freq_req],&req, sizeof(req)) != sizeof(req))
		{
			VERBOSE_N(0,"ERROR, changing node frequency (%s)", strerror(errno));
			_exit(1);
		}

		if (read(ear_fd_ack[freq_req], &real_freq, sizeof(unsigned long)) != sizeof(unsigned long))
		{
			VERBOSE_N(0,"ERROR, changing node frequency ACK (%s)", strerror(errno));
			_exit(1);
		}

		VERBOSE_N(3," Frequency_changed to %lu", real_freq);
	} else {
		real_freq = 0;
		DEBUG_F(0," change_freq service not provided");
	}

	return real_freq;
}

unsigned long ear_daemon_client_get_data_size_uncore()
{
    int com_fd=uncore_req;
	struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;
	DEBUG_F(2,"asking for uncore data size ");
    req.req_service=DATA_SIZE_UNCORE;
        if (ear_fd_req[com_fd]>=0){
            if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                    VERBOSE_N(0,"ERROR, sending request ear_daemon_client_get_data_size_uncore (%s)",
							  strerror(errno));
					return EAR_ERROR;
            }
            if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    VERBOSE_N(0,"ERROR, ear_daemon_client_get_data_size_uncore (%s)",strerror(errno));
					return EAR_ERROR;
            }
        } else{
        	DEBUG_F(0,"WARNING; ear_daemon_client_get_data_size_uncore service not provided");
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
	DEBUG_F(2,"reset uncore");

	if (ear_fd_req[uncore_req]>=0)
	{
		if (write(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, requesting RESET uncore counters to EAR daemon (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
			VERBOSE_N(0,"ERROR, reading RESET ACK uncore counters from EAR daemon (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
		DEBUG_F(3," reset ack uncore counters received from daemon ");
	    return EAR_SUCCESS;
    }
	DEBUG_F(0, "WARNING, reset uncore service not provided");
	return EAR_ERROR;
}

int ear_daemon_client_start_uncore()
{
	struct daemon_req req;
	unsigned long ack;
	int ret;

    req.req_service=START_UNCORE;
	DEBUG_F(2,"start uncore");

	if (ear_fd_req[uncore_req]>=0)
	{
		if (write(ear_fd_req[uncore_req],&req, sizeof(req)) != sizeof(req)){
				VERBOSE_N(0,"ERROR, requesting uncore counters to EAR daemon (%s)",strerror(errno));
				return EAR_ERROR;
		}
		if ((ret=read(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long))) != sizeof(unsigned long)) {
				VERBOSE_N(0,"ERROR, reading START ACK uncore counters from EAR daemon:ret=%d %s",
						  ret, strerror(errno));
				return EAR_ERROR;
		}
		DEBUG_F(3, "start ack uncore counters received from daemon");
		return EAR_SUCCESS;
    }

	DEBUG_F(0, "start uncore service not provided");
	return EAR_ERROR;
}

int ear_daemon_client_read_uncore(unsigned long long *values)
{
	struct daemon_req req;
	unsigned long long cas_client=0;
	unsigned long ack;
	int i;

	req.req_service=READ_UNCORE;
	DEBUG_F(2, "reading uncore counters");

	if (ear_fd_req[uncore_req]>=0)
	{
		// There is not request for uncore...only answer
		if (write(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req))
		{
			VERBOSE_N(0,"ERROR, requesting stop uncore counters to EAR daemon (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[uncore_req],values,uncore_size)!=uncore_size){
			VERBOSE_N(0,"ERROR, reading values uncore counters from EAR daemon (%s)",
					  strerror(errno));
			return EAR_ERROR;
		} else {
			ack=EAR_SUCCESS;
		}

		for (i=0;i<uncore_size/sizeof(unsigned long long);i++) cas_client+=values[i];
		DEBUG_F(4,"uncore cas %llu values %lu",cas_client,uncore_size/sizeof(unsigned long long));
	}else{
		ack=EAR_ERROR;
		DEBUG_F(0, "WARNING, read uncore service not provided");
	}

	return ack;
}

unsigned long ear_daemon_client_get_data_size_rapl() // size in bytes
{
    int com_fd=rapl_req;
    struct daemon_req req;
	unsigned long ack;

    DEBUG_F(2,"asking for rapl data size ");
    req.req_service=DATA_SIZE_RAPL;
    ack=EAR_SUCCESS;

    if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
                VERBOSE_N(0,"ERROR, sending request ear_daemon_client_get_data_size_rapl (%s)",
						  strerror(errno));
				return EAR_ERROR;
        }
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
                    VERBOSE_N(0,"ERROR, ear_daemon_client_get_data_size_rapl (%s)",strerror(errno));
					return EAR_ERROR;
        }
    }else {
        	DEBUG_F(0," ear_daemon_client_get_data_size_rapl service not provided");
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
	DEBUG_F(2,"reset RAPL");

	if (ear_fd_req[rapl_req]>=0)
	{
		if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, requesting RESET RAPL counters to EAR daemon (%s)",strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
			VERBOSE_N(0,"ERROR, reading reset ack RAPL counters from EAR daemon (%s)",strerror(errno));
			return EAR_ERROR;
		}

		DEBUG_F(3," reset ack RAPL counters received from daemon ");

		return ack;
    }

	DEBUG_F(0,"WARNING, RAPL service not provided");

	return EAR_ERROR;
}

int ear_daemon_client_start_rapl()
{
	struct daemon_req req;
	unsigned long ack;
	int ret;

	DEBUG_F(2,"starting rapl");
	req.req_service=START_RAPL;

	if (ear_fd_req[rapl_req]>=0)
	{
		if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, requesting RAPL counters to EAR daemon (%s)", strerror(errno));
			return EAR_ERROR;
		}
		if ((ret=read(ear_fd_ack[rapl_req],&ack,sizeof(unsigned long))) != sizeof(unsigned long)) {
			VERBOSE_N(0,"ERROR, reading start ack RAPL counters from EAR daemon ret=%d (%s)",
					  ret, strerror(errno));
			return EAR_ERROR;
		}

		DEBUG_F(3, "start ack RAPL counters received from daemon");

		return EAR_SUCCESS;
    }

	DEBUG_F(0,"WARNING, RAPL service not provided");
	return EAR_ERROR;
}

int ear_daemon_client_read_rapl(unsigned long long *values)
{
	struct daemon_req req;
	unsigned long long acum_energy = 0;
	unsigned long ack;
	int i;

	DEBUG_F(2,"reading rapl counters");
	req.req_service=READ_RAPL;
	
	if (ear_fd_req[rapl_req]>=0)
	{
		// There is not request for uncore...only answer
		if (write(ear_fd_req[rapl_req],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, requesting STOP&READ rapl counters to EAR daemon (%s)",
					  strerror(errno));
			return EAR_ERROR;
		}
		if (read(ear_fd_ack[rapl_req],values,rapl_size)!=rapl_size){
			VERBOSE_N(0,"ERROR, reading VALUES rapl counters from EAR daemon (%s)",strerror(errno));
			return EAR_ERROR;
		} else {
		ack = EAR_SUCCESS;
		}

		for (i=0;i<rapl_size/sizeof(unsigned long long);i++){
			acum_energy+=values[i];
		}

		DEBUG_F(4,"RAPL acum energy %llu",acum_energy);

	} else {
		DEBUG_F(0,"WARNING, read rapl service not provided");
		ack=EAR_ERROR;	
	}

	return ack;
}

unsigned long ear_daemon_client_node_energy_data_size()
{
    int com_fd=node_energy_req;
    struct daemon_req req;
	unsigned long ack=EAR_SUCCESS;

    DEBUG_F(2,"asking for node energy data size ");
    req.req_service=DATA_SIZE_ENERGY_NODE;
    if (ear_fd_req[com_fd]>=0)
	{
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)){
			VERBOSE_N(0,"ERROR, sending request ear_daemon_client_node_energy_data_size (%s)",strerror(errno));
			return EAR_ERROR;
        }   
        if (read(ear_fd_ack[com_fd],&ack,sizeof(unsigned long))!=sizeof(unsigned long)) {
			VERBOSE_N(0,"ERROR, ear_daemon_client_node_energy_data_size (%s)",strerror(errno));
			return EAR_ERROR;
        }   
    }else {
		DEBUG_F(0, "ERROR, ear_daemon_client_node_energy_data_size service not provided");
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

    DEBUG_F(2, "asking for node dc energy ");
    req.req_service=READ_DC_ENERGY;

	if (ear_fd_req[com_fd]>=0){
        if (write(ear_fd_req[com_fd],&req,sizeof(req))!=sizeof(req)) {
			VERBOSE_N(0,"ERROR, sending request ear_daemon_client_node_dc_energy (%s)",
					  strerror(errno));
			return EAR_ERROR;
        }   
        if (read(ear_fd_ack[com_fd],energy,sizeof(unsigned long))!=sizeof(unsigned long)){
			VERBOSE_N(0,"ERROR, ear_daemon_client_node_dc_energy (%s)", strerror(errno));
			return EAR_ERROR;
        }   
    } else {
		DEBUG_F(0, "ERROR, ear_daemon_client_node_dc_energy service not provided");
		ack=EAR_ERROR;
    }

    return ack;
}