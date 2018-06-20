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



#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <common/ear_verbose.h>
#include <daemon/eard_api.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <common/states.h>
#include <common/config.h>


#define MAX_TRIES 1

static const char *__NAME__ = "EARL->EARD";

// Pipes
static int ear_fd_req[ear_daemon_client_requests];
static int ear_fd_ack[ear_daemon_client_requests];
char ear_commreq[1024];
char ear_commack[1024];
char ear_ping[1024];

int ear_ping_fd;
char *ear_tmp;
static uint8_t app_connected=0;

// Metrics
static ulong uncore_size;
static ulong energy_size;
static ulong rapl_size;
static ulong freq_size;

uint warning(int return_value, int expected, char *msg)
{
	if (return_value!=expected){ 
		app_connected=0;
		VERBOSE_N(0, "eards: %s", msg);
	}
	if (return_value<0){ 
		app_connected=0;
		VERBOSE_N(0, "eards: %s (%s)", msg,strerror(errno));
	}
	return (return_value!=expected);
}

int eards_connected()
{
	return app_connected;
}

int eards_connect(application_t *my_app)
{
	char nodename[256];
	ulong ret,ack;
	struct daemon_req req;
	int tries=0,connected=0;
	int i;
	int my_id;
	if (app_connected) return EAR_SUCCESS;

	// These files connect EAR with EAR_COMM
	ear_tmp=getenv("EAR_TMP");

	if (ear_tmp == NULL)
	{
		ear_tmp=getenv("TMP");

		if (ear_tmp == NULL) {
			ear_tmp=getenv("HOME");
		}
	}

	if (gethostname(nodename,sizeof(nodename)) < 0){
		VERBOSE_N(0, "ERROR while getting the node name (%s)", strerror(errno));
		return EAR_ERROR;
	}

	for (i=0;i<ear_daemon_client_requests;i++){
		ear_fd_req[i]=-1;
		ear_fd_ack[i]=-1;
	}

	copy_application(&req.req_data.app,my_app);

	// We create a single ID
	my_id=create_ID(my_app->job.id,my_app->job.step_id);
	VERBOSE_N(2,"Connecting with daemon job_id=%d step_id%d\n",my_app->job.id,my_app->job.step_id);
	for (i = 0; i < ear_daemon_client_requests; i++)
	{
		DEBUG_F(2, "ear_client connecting with service %d", i);
		sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
		sprintf(ear_commack,"%s/.ear_comm.ack_%d.%d",ear_tmp,i,my_id);

		// Sometimes EARD needs some time to startm we will wait for the first one
		if (i==0){
			// First connection is special, we should wait
			do{
				DEBUG_F(0,"Connecting with EARD using file %s\n",ear_commreq);
				if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY|O_NONBLOCK))<0) tries++;
				else connected=1;
				if ((MAX_TRIES>1) && (!connected)) sleep(1);
			}while ((tries<MAX_TRIES) && !connected);

			if (!connected) {
				// Not possible to connect with ear_daemon
				VERBOSE_N(1, "ERROR while opening the communicator for requests %s (%d attempts) (%s)",
						  ear_commreq, tries, strerror(errno));
				return EAR_ERROR;
			}

			// ping pipe is used just for synchronization
			sprintf(ear_ping,"%s/.ear_comm.ping.%d",ear_tmp,my_id);
			ret=mknod(ear_ping,S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,0);

			if (ret < 0) {
				VERBOSE_N(0, "ERROR while creating the ping file");
			}

			if ((ear_ping_fd = open(ear_ping,O_RDWR)) < 0){
				VERBOSE_N(0, "ERROR while opening ping pipe (%s)", strerror(errno));
				ear_fd_req[i]=-1;
				return EAR_ERROR;
			}

			DEBUG_F(2, "ping connection created");
		}else{
			// ear_daemon will send an ack when pipe is created
			if ((ear_fd_req[i]=open(ear_commreq,O_WRONLY)) < 0) {
				VERBOSE_N(0, "ERROR while ipening the communicator for requests %s",
							ear_commreq);
				return EAR_ERROR;
			}
		}

		switch(i)
		{
			case freq_req:
				// When using a single communicator, we should send only a frequency connection request 
				req.req_service=CONNECT_FREQ; 
				break;
		}

		if (ear_fd_req[i]>0)
		{
			DEBUG_F(1,"Sending connection request to EARD\n");
			if (warning(write(ear_fd_req[i], &req, sizeof(req)), sizeof(req),
					"writting req_service in ear_daemon_client_connect"))
			{ 
				ear_fd_req[i]=-1;
				return EAR_ERROR;
			}
		}

		DEBUG_F(2, "req communicator %s [%d] connected", nodename, i);
		if (ear_fd_req[i]>=0) {
			// ear_demon sends an ack when ack pipe for specific service is created
			DEBUG_F(0, "ear_client: waiting for ear_daemon ok");
			if (warning(read(ear_ping_fd,&ack,sizeof(ulong)),sizeof(ulong),
				"ERROR while reading ping communicator "))
			{ 
				ear_fd_req[i]=-1;
				return EAR_ERROR;
			}
			DEBUG_F(1, "ear_client: ear_daemon ok for service %d, opening ack", i);
			// At this point, if ear_commack doesn't exist, that means we are not the master
			if ((ear_fd_ack[i]=open(ear_commack,O_RDONLY))<0)
			{
				VERBOSE_N(0, "ERROR while opening ack communicator (%s) (%s)",
			    			  ear_commack, strerror(errno));
				ear_fd_req[i]=-1;
				return EAR_ERROR;
			}
			VERBOSE_N(2, "ack communicator %s [%d] connected", nodename, i);
		}
	}
	app_connected=1;
	VERBOSE_N(2, "Connected");
	return EAR_SUCCESS;

}
void eards_disconnect()
{
	int i;
	struct daemon_req req;
	req.req_service=END_COMM;

	DEBUG_F(1, "Disconnecting");
	if (!app_connected) return;
    if (ear_fd_req[0] >= 0) {
		warning(write(ear_fd_req[0], &req,sizeof(req)), sizeof(req),
				"witting req in ear_daemon_client_disconnect");
	}

	for (i = 0; i < ear_daemon_client_requests; i++)
	{
		if (ear_fd_req[i]>=0)
		{
			close(ear_fd_req[i]);
			close(ear_fd_ack[i]);
		}
	}
	app_connected=0;
}
//////////////// SYSTEM REQUESTS
ulong eards_write_app_signature(application_t *app_signature)
{
	int com_fd=system_req;
	struct daemon_req req;
	ulong ack=EAR_SUCCESS;

	if (!app_connected) return EAR_SUCCESS;
	DEBUG_F(2, "asking the daemon to write the whole application signature (DB)");

	req.req_service = WRITE_APP_SIGNATURE;
	memcpy(&req.req_data.app, app_signature, sizeof(application_t));

	if (ear_fd_req[com_fd] >= 0)
	{
		DEBUG_F(2, "writing request...");
		if (warning(write(ear_fd_req[com_fd], &req, sizeof(req)), sizeof(req),
			"ERROR writing request for app signature")) return EAR_ERROR;
        
		DEBUG_F(2, "reading ack...");
		if (warning(read(ear_fd_ack[com_fd], &ack, sizeof(ulong)),sizeof(ulong),
        		"ERROR reading ack for app signature")) return EAR_ERROR;
	}else{
		DEBUG_F(0, "writting application signature (DB) service unavailable");
		ack=EAR_SUCCESS;
	}

	return ack;
}

ulong eards_write_event(ear_event_t *event)
{
	int com_fd=system_req;
    struct daemon_req req;
    ulong ack=EAR_SUCCESS;

    if (!app_connected) return EAR_SUCCESS;
    DEBUG_F(2, "asking the daemon to write an event)");

    req.req_service = WRITE_EVENT;
    memcpy(&req.req_data.event, event, sizeof(ear_event_t));

    if (ear_fd_req[com_fd] >= 0)
    {
        DEBUG_F(2, "writing request...");
        if (warning(write(ear_fd_req[com_fd], &req, sizeof(req)), sizeof(req),
            "ERROR writing request for event")) return EAR_ERROR;

        DEBUG_F(2, "reading ack...");
        if (warning(read(ear_fd_ack[com_fd], &ack, sizeof(ulong)),sizeof(ulong),
                "ERROR reading ack event")) return EAR_ERROR;
    }else{
        DEBUG_F(0, "writting application signature (DB) service unavailable");
        ack=EAR_SUCCESS;
    }

    return ack;
	
}
ulong eards_write_loop_signature(loop_t *loop_signature)
{
    int com_fd=system_req;
    struct daemon_req req;
    ulong ack=EAR_SUCCESS;
	eard_loop_t my_loop;

    if (!app_connected) return EAR_SUCCESS;
    DEBUG_F(2, "asking the daemon to write the loop signature (DB)");

    req.req_service = WRITE_LOOP_SIGNATURE;
    memcpy(&req.req_data.loop.loop, loop_signature, sizeof(loop_t));
    memcpy(&req.req_data.loop.job, loop_signature->job, sizeof(job_t));

	

    if (ear_fd_req[com_fd] >= 0)
    {
        DEBUG_F(2, "writing request...");
        if (warning(write(ear_fd_req[com_fd], &req, sizeof(req)), sizeof(req),
            "ERROR writing request for loop signature")) return EAR_ERROR;

        DEBUG_F(2, "reading ack...");
        if (warning(read(ear_fd_ack[com_fd], &ack, sizeof(ulong)),sizeof(ulong),
                "ERROR reading ack for loop signature")) return EAR_ERROR;
    }else{
        DEBUG_F(0, "writting loop signature (DB) service unavailable");
        ack=EAR_SUCCESS;
    }

    return ack;
}


//////////////// FREQUENCY REQUESTS
ulong eards_get_data_size_frequency()
{
	int com_fd=freq_req;
	struct daemon_req req;
	ulong ack = EAR_SUCCESS;

	if (!app_connected) return sizeof(ulong);
	DEBUG_F(2, "asking for frequency data size ");
    req.req_service=DATA_SIZE_FREQ;

    if (ear_fd_req[com_fd] >= 0)
	{
		if (warning(write(ear_fd_req[com_fd],&req,sizeof(req)) , sizeof(req),
			"ERROR writing request for frequency data size")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[com_fd],&ack, sizeof(ulong)) , sizeof(ulong),
			"ERROR reading ack for frequency data size ")) return EAR_ERROR;
	}
    else{
        DEBUG_F(0, "ear_daemon_client_get_data_size_frequency service not provided");
    }   
	freq_size=ack;	
    return ack;
}
void eards_begin_compute_turbo_freq()
{
	struct daemon_req req;
	ulong ack=EAR_SUCCESS;
	if (!app_connected) return;
	DEBUG_F(2, "start getting turbo freq ");
    req.req_service=START_GET_FREQ;

    if (ear_fd_req[freq_req]>=0)
	{
		if (warning(write(ear_fd_req[freq_req],&req,sizeof(req)) , sizeof(req),
			"ERROR writing request for starting frequency ")) return ;
		if (warning(read(ear_fd_ack[freq_req],&ack, sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for starting frequency ")) return ;
		return;
   }   
   DEBUG_F(0, "ear_daemon_client_begin_compute_turbo_freq service not provided");
   return;
}

ulong eards_end_compute_turbo_freq()
{
	struct daemon_req req;
	ulong ack=EAR_SUCCESS;

	if (!app_connected) return 0;

	DEBUG_F(2, "end getting turbo freq ");
    req.req_service = END_GET_FREQ;

	if (ear_fd_req[freq_req] >= 0)
	{
		if (warning(write(ear_fd_req[freq_req],&req, sizeof(req)) ,sizeof(req),
			 "ERROR writing request for finishing frequency")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[freq_req],&ack, sizeof(ulong)) , sizeof(ulong),
				"ERROR reading ack for finishing frequency ")) return EAR_ERROR;

		VERBOSE_N(2, "TURBO freq computed as  %lu",  ack);
	} else {
		DEBUG_F(0, "ear_daemon_client_end_compute_turbo_freq service not provided");
	}
    return ack;
}
void eards_begin_app_compute_turbo_freq()
{
    struct daemon_req req;
    ulong ack = EAR_SUCCESS;
	if (!app_connected) return;

    DEBUG_F(2, "start getting turbo freq");
    req.req_service = START_APP_COMP_FREQ;

	if (ear_fd_req[freq_req]>=0)
	{
		if (warning(write(ear_fd_req[freq_req],&req, sizeof(req)) , sizeof(req),
			"ERROR writing request for starting frequency for app")) return;
		if (warning(read(ear_fd_ack[freq_req],&ack,sizeof(ulong)) , sizeof(ulong),
			"ERROR reading ack for starting frequency")) return;
		return;
   }
   DEBUG_F(0, "ear_daemon_client_begin_app_compute_turbo_freq service not provided");
   return;
}
ulong eards_end_app_compute_turbo_freq()
{
	struct daemon_req req;
	ulong ack=EAR_SUCCESS;
	if (!app_connected) return 0;
	DEBUG_F(2, "end getting turbo freq");
	req.req_service = END_APP_COMP_FREQ;

	if (ear_fd_req[freq_req]>=0)
	{
		if (warning(write(ear_fd_req[freq_req],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for finishing frequency for app")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[freq_req],&ack,sizeof(ulong)) , sizeof(ulong),
			"ERROR writing ack for finishing frequency for app ")) return EAR_ERROR;

		VERBOSE_N(2, "TURBO freq computed as  %lu", ack);
	} else {
		DEBUG_F(0, "ear_daemon_client_end_app_compute_turbo_freq service not provided");
	}
	return ack;
}

void eards_set_turbo()
{
	struct daemon_req req;
	ulong ack;

	if (!app_connected) return;
	req.req_service=SET_TURBO;
	DEBUG_F(2, "Set turbo");

	if (ear_fd_req[freq_req]>=0)
	{
		if (warning(write(ear_fd_req[freq_req],&req,sizeof(req)),sizeof(req),
			 "ERROR writing request for setting turbo")) return;
		if (warning(read(ear_fd_ack[freq_req],&ack,sizeof(ulong)),sizeof(ulong),
			 "ERROR reading ack for setting turbo ")) return;
		VERBOSE_N(3, "turbo activated");
		return;
	}   
	DEBUG_F(0, "turbo not provided");
    return;

}
ulong eards_change_freq(ulong newfreq)
{
	ulong real_freq = EAR_ERROR;
	struct daemon_req req;
	if (!app_connected) return newfreq;
	req.req_service = SET_FREQ;
	req.req_data.req_value = newfreq;

	DEBUG_F(2, "NewFreq %lu requested",  newfreq);

	if (ear_fd_req[freq_req] >= 0)
	{
		if (warning(write(ear_fd_req[freq_req],&req, sizeof(req)) , sizeof(req),
			 "ERROR writing request for changing frequency")) return EAR_ERROR;

		if (warning(read(ear_fd_ack[freq_req], &real_freq, sizeof(ulong)) , sizeof(ulong),
			"ERROR reading ack for changing frequency ")) return EAR_ERROR;

		VERBOSE_N(3, "Frequency_changed to %lu",  real_freq);
	} else {
		real_freq = 0;
		DEBUG_F(0, "change_freq service not provided");
	}

	return real_freq;
}

// END FREQUENCY SERVICES
//////////////// UNCORE REQUESTS
ulong eards_get_data_size_uncore()
{
    int com_fd = uncore_req;
	struct daemon_req req;
	ulong ack = EAR_SUCCESS;
	if (!app_connected) return sizeof(ulong);

	DEBUG_F(2, "asking for uncore data size ");
    req.req_service=DATA_SIZE_UNCORE;

	if (ear_fd_req[com_fd] >= 0) {
		if (warning(write(ear_fd_req[com_fd],&req, sizeof(req)) , sizeof(req),
			 "ERROR writing request for uncore data size ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[com_fd],&ack, sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for uncore data size ")) return EAR_ERROR;
	} else {
		DEBUG_F(0, "ear_daemon_client_get_data_size_uncore service not provided");
		ack=EAR_ERROR;
	}
    uncore_size=ack;
    return ack;
}


int eards_reset_uncore()
{
   	struct daemon_req req;
	ulong ack = EAR_SUCCESS;
	if (!app_connected) return EAR_SUCCESS;

	req.req_service = RESET_UNCORE;
	DEBUG_F(2, "reset uncore");

	if (ear_fd_req[uncore_req] >= 0)
	{
		if (warning(write(ear_fd_req[uncore_req],&req,sizeof(req)), sizeof(req),
			 "ERROR writing request for resetting uncores ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[uncore_req],&ack,sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for resetting uncores ")) return EAR_ERROR;
		DEBUG_F(3, "RESET ACK Uncore counters received from daemon ");
	    return EAR_SUCCESS;
    }
	DEBUG_F(0, "reset uncore service not provided ");
	return EAR_ERROR;
}

int eards_start_uncore()
{
	struct daemon_req req;
	ulong ack;
	int ret;
	if (!app_connected) return EAR_SUCCESS;
    req.req_service=START_UNCORE;
	DEBUG_F(2, "start uncore");

	if (ear_fd_req[uncore_req]>=0)
	{
		if (warning(write(ear_fd_req[uncore_req],&req, sizeof(req)) , sizeof(req),
			 "ERROR writing request for starting uncores ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[uncore_req],&ack,sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for starting uncores ")) return EAR_ERROR;
		DEBUG_F(3, "START ACK Uncore counters received from daemon ");
		return EAR_SUCCESS;
    }
	DEBUG_F(0, "start uncore service not provided ");
	return EAR_ERROR;
}

int eards_read_uncore(unsigned long long *values)
{
	struct daemon_req req;
	unsigned long long cas_client=0;
	ulong ack;
	int i;

	if (!app_connected){ values[0]=0;return EAR_SUCCESS;}

	req.req_service=READ_UNCORE;
	DEBUG_F(2, "reading uncore counters");

	if (ear_fd_req[uncore_req]>=0)
	{
		// There is not request for uncore...only answer
		if (warning(write(ear_fd_req[uncore_req], &req, sizeof(req)) , sizeof(req),
			"ERROR writing request for reading uncores ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[uncore_req],values,uncore_size) , uncore_size, 
			 "ERROR reading ack for reading uncores ")){ 
			return EAR_ERROR;
		} else {
			ack = EAR_SUCCESS;
		}

	}else{
		ack=EAR_ERROR;
		DEBUG_F(0, "read uncore service not provided");
	}
	return ack;
}

//////////////// RAPL REQUESTS
ulong eards_get_data_size_rapl() // size in bytes
{
    int com_fd=rapl_req;
    struct daemon_req req;
	ulong ack;
	if (!app_connected) return sizeof(ulong);

    DEBUG_F(2, "asking for rapl data size ");
    req.req_service=DATA_SIZE_RAPL;
    ack=EAR_SUCCESS;

    if (ear_fd_req[com_fd] >= 0)
	{
        if (warning(write(ear_fd_req[com_fd],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for RAPL data size ")) return EAR_ERROR;
        if (warning(read(ear_fd_ack[com_fd],&ack,sizeof(ulong)) , sizeof(ulong),
			"ERROR reading ack for RAPL data size ")) return EAR_ERROR;
    } else {
		DEBUG_F(0, "ear_daemon_client_get_data_size_rapl service not provided");
		ack=EAR_ERROR;
	}
    rapl_size=ack;
    return ack;
}


int eards_reset_rapl()
{
	struct daemon_req req;
	ulong ack=EAR_SUCCESS;
	if (!app_connected) return EAR_SUCCESS;
	req.req_service = RESET_RAPL;
	DEBUG_F(2, "reset rapl");

	if (ear_fd_req[rapl_req]>=0)
	{
		if (warning(write(ear_fd_req[rapl_req],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for resetting RAPL counters ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[rapl_req],&ack, sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for resetting RAPL counters ")) return EAR_ERROR;
		DEBUG_F(3, "RESET ACK rapl counters received from daemon ");
		return ack;
    }
	DEBUG_F(0, "reset rapl service not provided");
	return EAR_ERROR;
}

int eards_start_rapl()
{
	struct daemon_req req;
	ulong ack;
	int ret;
	if (!app_connected) return EAR_SUCCESS;
	req.req_service=START_RAPL;
	DEBUG_F(2, "start rapl");

    if (ear_fd_req[rapl_req]>=0)
	{
		if (warning(write(ear_fd_req[rapl_req],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for starting RAPL counters ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[rapl_req],&ack,sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for starting RAPL counters ")) return EAR_ERROR;
		DEBUG_F(3, "START ACK rapl counters received from daemon ");
		return EAR_SUCCESS;
    }
	DEBUG_F(0, "start rapl service not provided");
	return EAR_ERROR;
}

int eards_read_rapl(unsigned long long *values)
{
	struct daemon_req req;
	ulong ack;
	unsigned long long acum_energy=0;
	int i;
	if (!app_connected){ values[0]=0;return EAR_SUCCESS;}

	req.req_service=READ_RAPL;
	DEBUG_F(2, "reading rapl counters");

	if (ear_fd_req[rapl_req]>=0)
	{
		// There is not request for uncore...only answer
		if (warning(write(ear_fd_req[rapl_req],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for stopping RAPL counters ")) return EAR_ERROR;
		if (warning(read(ear_fd_ack[rapl_req],values,rapl_size) , rapl_size,
			 "ERROR reading ack for stopping RAPL counters "))
		{ 
			return EAR_ERROR;
		}else{
			ack = EAR_SUCCESS;
		}

	}else{
		DEBUG_F(0, "read rapl service not provided");
		ack = EAR_ERROR;
	}
	return ack;
}
// END RAPL SERVICES
// NODE ENERGY SERVICES
ulong eards_node_energy_data_size()
{
    int com_fd =node_energy_req;
    struct daemon_req req;
	ulong ack = EAR_SUCCESS;
	
	if (!app_connected) return sizeof(ulong);

    DEBUG_F(2, "asking for node energy data size ");
    req.req_service=DATA_SIZE_ENERGY_NODE;

    if (ear_fd_req[com_fd]>=0)
	{
        if (warning(write(ear_fd_req[com_fd],&req,sizeof(req)) , sizeof(req),
			 "ERROR writing request for IPMI data size ")) return EAR_ERROR;
        if (warning(read(ear_fd_ack[com_fd],&ack,sizeof(ulong)) , sizeof(ulong),
			 "ERROR reading ack for IPMI data size ")) return EAR_ERROR;
    } else {
		DEBUG_F(0, "ear_daemon_client_node_energy_data_size service not provided");
		ack=EAR_ERROR;
    }   
    energy_size=ack;
    return ack;
}
int eards_node_dc_energy(ulong *energy)
{
    int com_fd = node_energy_req;
	ulong ack=EAR_SUCCESS;
	struct daemon_req req;

	if (!app_connected){*energy=0;return EAR_SUCCESS;}

    DEBUG_F(2, "asking for node dc energy ");
    req.req_service=READ_DC_ENERGY;

	if (ear_fd_req[com_fd]>=0)
	{
        if (warning(write(ear_fd_req[com_fd],&req,sizeof(req)) , sizeof(req),
			"ERROR writing request for IPMI data size ")) return EAR_ERROR;
        if (warning(read(ear_fd_ack[com_fd],energy,sizeof(ulong)) , sizeof(ulong),
			"ERROR reading ack for IPMI data size ")) return EAR_ERROR;
    } else
	{
		DEBUG_F(0, "ear_daemon_client_node_dc_energy service not provided");
		ack=EAR_ERROR;
    }   
    return ack;
}
ulong eards_node_energy_frequency()
{
    int com_fd = node_energy_req;
    ulong ack=EAR_ERROR;
    struct daemon_req req;

	if (!app_connected) return 10000000;

    DEBUG_F(2, "asking for dc node energy frequency ");
    req.req_service=ENERGY_FREQ;
    if (ear_fd_req[com_fd]>=0)
    {
        if (warning(write(ear_fd_req[com_fd],&req,sizeof(req)) , sizeof(req),
             "ERROR writing request for energy freq of update ")) return EAR_ERROR;
        if (warning(read(ear_fd_ack[com_fd],&ack,sizeof(ulong)) , sizeof(ulong),
            "ERROR reading ack for energy freq of update ")) return ack;
    } else
    {
        DEBUG_F(0, "eards_node_energy_frequency service not provided");
        ack=EAR_ERROR;
    }
    return ack;

}
