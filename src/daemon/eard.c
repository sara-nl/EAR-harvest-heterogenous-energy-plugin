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
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <linux/limits.h>
#include <pthread.h>

#include <common/config.h>
#include <control/frequency.h>
#include <metrics/ipmi/energy_node.h>
#include <metrics/custom/bandwidth.h>
#include <metrics/custom/hardware_info.h>
#include <metrics/papi/energy_cpu.h>
#include <common/types/generic.h>
#include <common/types/job.h>
#include <common/types/log.h>
#include <common/types/loop.h>
#include <common/types/services.h>
#include <common/environment.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <daemon/eard_conf_api.h>

#include <common/types/cluster_conf.h>
#include <daemon/power_monitor.h>
#include <daemon/dynamic_configuration.h>
#include <daemon/shared_configuration.h>
#if USE_EARDB
#include <database_cache/eardbd_api.h>
#endif


unsigned int power_mon_freq=POWERMON_FREQ;
pthread_t power_mon_th; // It is pending to see whether it works with threads
pthread_t dyn_conf_th;
char my_ear_conf_path[GENERIC_NAME];
cluster_conf_t	my_cluster_conf;
my_node_conf_t 	*my_node_conf;
policy_conf_t default_policy_context;
ear_conf_t *dyn_conf;
uint signal_sighup=0;

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define MAX_PATH_SIZE 	256
#define RAPL_METRICS 	4

// These two variables are used by the verbosity macros
static const char *__NAME__ = "EARD ";
static const char *__HOST__;


int ear_fd_req[ear_daemon_client_requests];
int ear_fd_ack[ear_daemon_client_requests];
unsigned long eard_max_freq;
int eard_max_pstate;
int num_uncore_counters;

char ear_ping[MAX_PATH_SIZE];
int ear_ping_fd=-1;
char ear_tmp[MAX_PATH_SIZE];
char nodename[MAX_PATH_SIZE];
int eard_lockf;
char eard_lock_file[MAX_PATH_SIZE];
int EAR_VERBOSE_LEVEL=0;
int application_id=-1;

ulong energy_freq;
ulong current_node_freq;
struct daemon_req req;

void eard_exit(uint restart);
void eard_close_comm();
int is_new_application(int pid);
int is_new_service(int req,int pid);
int application_timeout();
void configure_new_values(ear_conf_t *dyn,cluster_conf_t *cluster,my_node_conf_t *node);
void set_default_policy(policy_conf_t *policy);
int RAPL_counting=0;
int eard_must_exit=0;

void set_global_eard_variables()
{
	strcpy(ear_tmp,my_cluster_conf.tmp_dir);
	EAR_VERBOSE_LEVEL=my_cluster_conf.eard.verbose;
	eard_max_pstate=my_cluster_conf.eard.max_pstate;
	
	
}

// Lock unlock functions are used to be sure a single daemon is running per node

void  create_tmp(char *tmp_dir)
{
    int ret;
    ret=mkdir(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if ((ret<0) && (errno!=EEXIST)){
        eard_verbose(0,"ear tmp dir cannot be created (%s)",strerror(errno));
        _exit(0);
    }

    if (chmod(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH)<0){
		eard_verbose(0,"ear_tmp permissions cannot be set (%s)",strerror(errno));
        _exit(0);
	}
}

void eard_lock(char *tmp_dir,char *nodename)
{
	int ret;
	sprintf(eard_lock_file, "%s/%s.eard_lock", tmp_dir, nodename);

	if ((eard_lockf=open(eard_lock_file,O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR))<0)
	{
		if (errno != EEXIST) {
			eard_verbose(0,"error, creating lock file %s fails (%s)\n",
						eard_lock_file, strerror(errno));
		} else {
            eard_verbose(0, "Error opening daemon lock file in %s (%s)\n",
						eard_lock_file, strerror(errno));}
		_exit(0);
	}
}

void eard_unlock()
{
	close(eard_lockf);
	eard_verbose(2,"removing file %s\n",eard_lock_file);
	if (unlink(eard_lock_file)<0) eard_verbose(0, "eard error when removing lock file %s:%s\n",eard_lock_file,strerror(errno));
}

// Creates a pipe to receive requests for applications. eard creates 1 pipe (per node) and service to receive requests
void create_connector(char *ear_tmp,char *nodename,int i)
{
	char ear_commreq[MAX_PATH_SIZE];
	sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
	// ear_comreq files will be used to send requests from the library to the eard
	if (mknod(ear_commreq,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
		if (errno!=EEXIST){
			eard_verbose(0,"Error creating ear communicator for requests %s\n",strerror(errno));
		}
	}
	chmod(ear_commreq,S_IRUSR|S_IWUSR|S_IRUSR|S_IWGRP|S_IROTH|S_IWOTH);
}

void connect_service(int req,application_t *new_app)
{
    char ear_commack[MAX_PATH_SIZE];
    unsigned long ack;
    int connect=1;
    int alive;
	job_t *new_job=&new_app->job;
	int pid=create_ID(new_job->id,new_job->step_id);
    // Let's check if there is another application
    eard_verbose(2, "request for connection at service %d", req);
    if (is_new_application(pid) || is_new_service(req, pid)) {
        connect=1;
    } else {
        connect=0;

        //FIXME: implicit
        if (check_ping()) alive = application_timeout();
        if (alive == 0) connect = 1;
    }


    // Creates 1 pipe (per node) to send acks.
    if (connect)
    {
		eard_verbose(2,"Connected new job job_id=%d step_id=%d\n",new_job->id,new_job->step_id);
        sprintf(ear_commack, "%s/.ear_comm.ack_%d.%lu", ear_tmp, req, pid);
        application_id = pid;

        // ear_commack will be used to send ack's or values (depending on the
        // requests) from eard to the library
        eard_verbose(2, "Creating ack comm %s pid=%lu", ear_commack,pid);

        if (mknod(ear_commack, S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,0) < 0)
        {
            if (errno != EEXIST){
                eard_verbose(0, "Error when creating ear communicator for ack %s", strerror(errno));
                eard_close_comm();
            }
        }

        chmod(ear_commack,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

        // At first service connection, we use the ping conn file
        if (req == 0)
        {
            // We open ping connection  for writting
            sprintf(ear_ping, "%s/.ear_comm.ping.%lu", ear_tmp, pid);

            eard_verbose(2, "application %lu connected", pid);
            eard_verbose(2, "opening ping conn for %lu", pid);
            ear_ping_fd = open(ear_ping,O_WRONLY);

            if (ear_ping_fd < 0)
            {
                eard_verbose(0,"ERROR while opening ping pipe %s (%s)", ear_ping, strerror(errno));
                eard_close_comm();
            }
            // We must modify the client api to send more information
			powermon_mpi_init(new_app);

        	eard_verbose(2, "sending ack for service %d",req);
        	if (write(ear_ping_fd, &ack, sizeof(ack)) != sizeof(ack)) {
        	    eard_verbose(0,"WARNING while writting for ping conn for %lu", pid);
        	}

        	eard_verbose(2, "connecting service %s", ear_commack);
        	if ((ear_fd_ack[req]=open(ear_commack,O_WRONLY)) < 0){
        	    eard_verbose(0,"ERROR when opening ear communicator for ack (%s)", strerror(errno));
        	    eard_close_comm();
        	}
    	}else{
        	// eard only suppports one application connected, the second one will block
        	eard_verbose(2, "Process pid %lu rejected as master", pid);
    	}
    	eard_verbose(2, "Process pid %lu selected as master", pid);
    	eard_verbose(2, "service %d connected", req);
	}
}

// Checks application connections
int is_new_application(pid)
{
	if (application_id==-1) return 1;
	else return 0;
} 
// Application is already connected but it is a new service
int is_new_service(int req,int pid)
{
	if ((application_id==pid) && (ear_fd_ack[req]==-1)) return 1;
	else return 0;
}
// Checks ping fd is ok
int check_ping()
{
	return (ear_ping_fd>=0);
}
// Checks the application is still alive
int application_timeout()
{
	int ret;
	int alive=1;
	if (check_ping()){
		ret=write(ear_ping_fd,"1",1);
		// If we can write, application is still alive
		if (ret==1){
			int fd; 
			char c;
			fd=open(ear_ping,O_RDONLY);
			if (fd<0){
				alive=0;
				ear_ping_fd=-1;
			}else{
				read(fd,&c,1);
				close(fd);  
			}
		}else alive=0;
    }else alive=0;   
	return alive;
}

/*
*	Restarts again EARD
*/

void eard_restart()
{
	char *ear_install;
	char my_bin[MAX_PATH_SIZE];
	ear_install=getenv("EAR_INSTALL_PATH");
	if (ear_install==NULL){
		eard_verbose(0,"EAR_INSTALL_PATH is NULL\n");
		sprintf(my_bin,"eard");
	}else sprintf(my_bin,"%s/sbin/eard",ear_install);
	eard_verbose(0,"LD_LIBRARY_PATH=%s\n",getenv("LD_LIBRARY_PATH"));
	eard_verbose(0,"Restarting to %s p_state=1 ear_tmp=%s verbose=0\n",my_bin,ear_tmp);
	end_service("eard");
	// Do we want to maintain verbose level?
	execlp(my_bin,my_bin,"1",NULL);
	eard_verbose(0,"Restarting EARD %s\n",strerror(errno));
	_exit(1);	
}

/*
*	Depending on restart argument, exits eard or restart it
*/
void eard_exit(uint restart)
{
	char ear_commreq[MAX_PATH_SIZE];
	int i;

	eard_verbose(1, "Exiting");
	#if EARD_LOCK
	eard_unlock();
	#endif

	// Come disposes
	//print_rapl_metrics();

	// Recovering old frequency and governor configurations.
	eard_verbose(1,"frequency_dispose");	
	frequency_dispose();

	eard_verbose(1,"Releasing node resources");
	// More disposes
	node_energy_dispose();
	dispose_uncores();
	aperf_dispose();
    // Database cache daemon disconnect
    #if USE_EARDB
    eardbd_disconnect();
    #endif


	eard_verbose(1,"Releasing files");

	// Removing files (services)
	for (i = 0; i < ear_daemon_client_requests; i++)
	{
		close(ear_fd_req[i]);

		//sprintf(ear_commreq, "%s/.ear_comm.req_%d", ear_tmp, i);
		//eard_verbose(2, "removing file %s", ear_commreq);

		//if (unlink(ear_commreq) < 0) {
		//	eard_verbose(0, "error when removing com file %s (%s)", ear_commreq, strerror(errno));
		//}
	}
	ear_conf_shared_area_dispose(ear_tmp);

	if (restart){ 
		eard_verbose(0,"Restarting EARD\n");
	}else{ 
		eard_verbose(0,"Exiting EARD\n");
	}
	if (restart==0){ 
		end_service("eard");
		_exit(0);
	}else{
		eard_restart();
	}
}

// HIGHLIGHT: LIBRARY DISPOSE (2/2)
void eard_close_comm()
{
	unsigned long long values[RAPL_EVS];
	char ear_commack[MAX_PATH_SIZE];
	int dis_pid = application_id;
	int i;

	// closes communication with an application
	eard_verbose(2, "closing comm in %s for %d", nodename, application_id);

	// Stop counting
	if (RAPL_counting)
	{
		stop_rapl_metrics(values);
		//reset_rapl_metrics();
		RAPL_counting=0;
	}

	for (i = 0; i < ear_daemon_client_requests; i++)
	{
		close(ear_fd_ack[i]);
		ear_fd_ack[i] = -1;

		sprintf(ear_commack, "%s/.ear_comm.ack_%d.%d", ear_tmp, i, application_id);
		eard_verbose(2, "removing file %s", ear_commack);

		if (unlink(ear_commack) < 0) {
			eard_verbose(0, "ERROR when removing ack file %s (%s)", ear_commack, strerror(errno));
		}
	}

	close(ear_ping_fd);

	// We must send the app signature to the powermonitoring thread
	powermon_mpi_finalize();

	application_id = -1;
	ear_ping_fd = -1;

	eard_verbose(2, "removing file %s", ear_ping);

	if (unlink(ear_ping) < 0) {
		eard_verbose(0, "ERROR when removing ping file %s (%s)", ear_ping, strerror(errno));
	}
	eard_verbose(1, "application %d disconnected", dis_pid);
}

// Node_energy services
int eard_node_energy(int must_read)
{
	unsigned long ack;
	if (must_read){
    if (read(ear_fd_req[node_energy_req],&req,sizeof(req))!=sizeof(req)) eard_verbose(0,"eard error reading info at eard_node_energyi (%s)\n",strerror(errno));
	}
    switch (req.req_service){
		case CONNECT_ENERGY:
			connect_service(node_energy_req,&req.req_data.app);
			break;
        case READ_DC_ENERGY:
			read_dc_energy(&ack);
            write(ear_fd_ack[node_energy_req],&ack,sizeof(unsigned long));
            break;
		case DATA_SIZE_ENERGY_NODE:
			ack=(unsigned long)count_energy_data_length();
            write(ear_fd_ack[node_energy_req],&ack,sizeof(unsigned long));
			break;
		case ENERGY_FREQ:
			ack=(ulong)node_energy_frequency();
            write(ear_fd_ack[node_energy_req],&ack,sizeof(unsigned long));
			break;
        default:
			return 0;
    }
	return 1;

}


int eard_system(int must_read)
{
	unsigned long ack;
	 int ret1, ret2, size;

	if (must_read)
	{
		if (read(ear_fd_req[system_req],&req,sizeof(req))!=sizeof(req))
			eard_verbose(0,"eard error reading info at eard_system(%s)\n",strerror(errno));
	}

	switch (req.req_service)
	{
		case CONNECT_SYSTEM:
			connect_service(system_req,&req.req_data.app);
			break;
		case WRITE_APP_SIGNATURE:
			powermon_mpi_signature(&req.req_data.app);
            ack=EAR_COM_OK;
            if (write(ear_fd_ack[system_req], &ack, sizeof(ulong)) != sizeof(ulong))
            {
                eard_verbose(0, "ERROR while writing system service ack, closing connection...");
                eard_close_comm();
            }

			break;
		case WRITE_EVENT:
			ack=EAR_COM_OK;
			#if !USE_EARDB
			#if DB_MYSQL
			ret1=db_insert_ear_event(&req.req_data.event);
			#endif
			#else
			ret1=eardbd_send_event(&req.req_data.event);
			#endif
			if (ret1 == EAR_SUCCESS) ack=EAR_COM_OK;
			else ack=EAR_COM_ERROR;
			write(ear_fd_ack[system_req], &ack, sizeof(ulong));

			break;

		case WRITE_LOOP_SIGNATURE:
			ack=EAR_COM_OK;
			#if !LARGE_CLUSTER
			#if !USE_EARDB
			#if DB_MYSQL
			req.req_data.loop.loop.job=&req.req_data.loop.job;
			ret1 = db_insert_loop (&req.req_data.loop.loop);
			#endif
			#else
			ret1=eardbd_send_loop(&req.req_data.loop.loop);
			#endif
			#endif
			if (ret1 == EAR_SUCCESS) ack=EAR_COM_OK;
			else ack=EAR_COM_ERROR;
			write(ear_fd_ack[system_req], &ack, sizeof(ulong));

		break;
		default: return 0;
	}
	return 1;
}

// FREQUENCY FUNCTIONALLITY: max_freq is the limit
void eard_set_freq(unsigned long new_freq,unsigned long max_freq)
{
	unsigned long ear_ok,freq;
	eard_verbose(1,"setting node frequency . requested %lu, max %lu\n",new_freq,max_freq);
	if (new_freq<=max_freq){ 
		freq=new_freq;
	}else{ 
		eard_verbose(1," warning, maximum freq is limited to %lu\n",max_freq);
		freq=max_freq;
	}
	ear_ok=frequency_set_all_cpus(freq);
	current_node_freq=freq;
	if (ear_ok!=freq) eard_verbose(1," warning, frequency is not correctly changed\n");
	write(ear_fd_ack[freq_req],&ear_ok,sizeof(unsigned long));  
}

int eard_freq(int must_read)
{
	unsigned long ack;

	if (must_read)
	{
		if (read(ear_fd_req[freq_req],&req,sizeof(req)) != sizeof(req))
			eard_verbose(0, "eard error when reading info at eard_freq (%s)\n", strerror(errno));
	}

	switch (req.req_service) {
		case CONNECT_FREQ:
			// HIGHLIGHT: LIBRARY INIT
			connect_service(freq_req,&req.req_data.app);
			break;
		case SET_FREQ:
			ear_debug(1,"Setting node frequency\n");
			eard_set_freq(req.req_data.req_value,min(eard_max_freq,max_dyn_freq()));
			break;
		case START_GET_FREQ:
			aperf_get_avg_frequency_init_all_cpus();
			ack=EAR_COM_OK;
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_GET_FREQ:
			ear_debug(1,"get node frequency (trubo)\n");
			ack = aperf_get_avg_frequency_end_all_cpus();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case SET_TURBO:
			frequency_set_performance_governor_all_cpus();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_COMM:
			ear_debug(1,"Closing comunication\n");
			// HIGHLIGHT: LIBRARY DISPOSE (1/2)
			eard_close_comm();
			break;
		case DATA_SIZE_FREQ:
			ack=sizeof(unsigned long);
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case START_APP_COMP_FREQ:
			aperf_get_global_avg_frequency_init_all_cpus();
			ack=EAR_COM_OK;
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_APP_COMP_FREQ:
			ear_debug(1," get app node frequency (trubo)\n");
			ack=aperf_get_global_avg_frequency_end_all_cpus();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;

		default:
			return 0;

	}
	return 1;
}

int eard_uncore(int must_read)
{

	unsigned long ack=EAR_SUCCESS;
	unsigned long long values[num_uncore_counters];

	if (must_read){	
	if (read(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)) eard_verbose(0,"eard error when reading info at eard_uncore \n");
	}

	switch (req.req_service)
	{
	    case CONNECT_UNCORE:
			connect_service(uncore_req,&req.req_data.app);
			break;
		case START_UNCORE:
			ear_debug(1,"EAR_daemon_server: start uncore\n");
			start_uncores();
			write(ear_fd_ack[uncore_req],&ack,sizeof(ack));
			break;
		case RESET_UNCORE:
			ear_debug(1,"EAR_daemon_server: reset uncore\n");
			reset_uncores();
			write(ear_fd_ack[uncore_req],&ack,sizeof(ack));
			break;
		case READ_UNCORE:
		{
			unsigned long demon_cas=0;
			int i;
			ear_debug(1,"EAR_daemon_server: read uncore\n");
			stop_uncores(values);
			write(ear_fd_ack[uncore_req], values, sizeof(unsigned long long) * num_uncore_counters);
			for (i=0; i < num_uncore_counters; i++) demon_cas += values[i];
			ear_debug(3,"DAEMON cas %lu %d values\n", demon_cas, num_uncore_counters);
		}
			break;
		case DATA_SIZE_UNCORE:
			ack=sizeof(unsigned long long) * num_uncore_counters;
			write(ear_fd_ack[uncore_req],&ack,sizeof(unsigned long));
			break;
		default:
			return 0;
		
	}
	return 1;
}


////// RAPL SERVICES
int eard_rapl(int must_read)
{
	unsigned long comm_req=rapl_req;
    unsigned long ack=0;
    unsigned long long values[RAPL_EVS];
	if (must_read){
    	if (read(ear_fd_req[comm_req],&req,sizeof(req))!=sizeof(req)) eard_verbose(0,"eard error when reading info at eard_rapl\n");
	}
    switch (req.req_service){
		case CONNECT_RAPL:
			connect_service(rapl_req,&req.req_data.app);
			break;
        case START_RAPL:
    		ear_debug(1,"EAR_daemon_server: start RAPL counters\n");
            start_rapl_metrics();
			RAPL_counting=1;
            write(ear_fd_ack[comm_req],&ack,sizeof(ack));
            break;
        case RESET_RAPL:
    		ear_debug(1,"EAR_daemon_server: reset RAPL\n");
            reset_rapl_metrics();
            write(ear_fd_ack[comm_req],&ack,sizeof(ack));
            break;
        case READ_RAPL:
    		ear_debug(1,"EAR_daemon_server: read RAPL\n");
            read_rapl_metrics(values);
			RAPL_counting=0;
            write(ear_fd_ack[comm_req],values,sizeof(unsigned long long)*RAPL_EVS);
            break;
        case DATA_SIZE_RAPL:
            ack=sizeof(unsigned long long)*RAPL_EVS;
            write(ear_fd_ack[comm_req],&ack,sizeof(unsigned long));
            break;
        default:
			return 0;

    }
	return 1;
}
/// END RAPL SERVICES

void select_service(int fd)
{
    if (read(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)) eard_verbose(0,"eard error when reading info at select_service\n");
	if (eard_freq(0)){ 
		ear_debug(0,"eard frequency service\n");
		return;
	}
	if (eard_uncore(0)){ 
		ear_debug(0,"eard uncore service\n");
		return;
	}
	if (eard_rapl(0)){ 
		ear_debug(0,"eard rapl service\n");
		return;
	}
	if (eard_system(0)){ 
		ear_debug(0,"eard system service\n");
		return;
	}
	if (eard_node_energy(0)){ 
		ear_debug(0,"eard node energy service\n");
		return;
	}
	eard_verbose(0," Error, request received not supported\n");
	eard_close_comm();
}
//
// MAIN eard: eard default_freq [path for communication files]
//
// eard creates different pipes to receive requests and to send values requested
void Usage(char *app)
{
	fprintf(stderr,"Usage: %s [-h|verbose_level] \n", app);
	fprintf(stderr,"\tear.conf file is used to define node settings. It must be available at\n"); 
	fprintf(stderr,"\t $ETC/ear/ear.conf \n");
	_exit(1);
}

//region SIGNALS
void signal_handler(int sig)
{
	if (sig == SIGPIPE){ eard_verbose(0, "signal SIGPIPE received. Application terminated abnormally");}
	if (sig == SIGTERM){ eard_verbose(0, "signal SIGTERM received. Finishing");}
	if (sig == SIGINT){  eard_verbose(0, "signal SIGINT received. Finishing");}
	if (sig == SIGHUP){  eard_verbose(0, "signal SIGHUP received. Reloading EAR conf");signal_sighup=1;}

	// The PIPE was closed, so the daemon connection ends
	if (sig == SIGPIPE) {
		eard_close_comm();
	}


	// Someone wants EARD to get closed or restarted
	if ((sig == SIGTERM) || (sig == SIGINT) )
	{
		eard_must_exit = 1;

		if (ear_ping_fd > 0)
		{
			eard_verbose(1, "application status = connected");
			eard_close_comm();
		}
		#if 0
		eard_verbose(0,"Sending SIGUSR1 to powermon %u and dyn_conf %u threads\n",(uint)power_mon_th,(uint)dyn_conf_th);
		pthread_kill(power_mon_th, SIGUSR1);
		pthread_kill(dyn_conf_th, SIGUSR1);
		pthread_join(power_mon_th,NULL);
		pthread_join(dyn_conf_th, NULL);
		#endif
	}
	if ((sig == SIGTERM) || (sig == SIGINT)){
		eard_exit(0);
	}
	if (sig == SIGHUP){
        free_cluster_conf(&my_cluster_conf);
        // Reading the configuration
        if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
            eard_verbose(0," Error reading cluster configuration\n");
        }
        else{
			eard_verbose(0,"Loading EAR configuration");
            print_cluster_conf(&my_cluster_conf);
	        my_node_conf=get_my_node_conf(&my_cluster_conf,nodename);
	        if (my_node_conf==NULL){
     	       eard_verbose(0," Error in cluster configuration, node %s not found\n",nodename);
     	   	}else{
				print_my_node_conf(my_node_conf);
				set_global_eard_variables();
    			set_default_policy(&default_policy_context);
    			configure_new_values(dyn_conf,&my_cluster_conf,my_node_conf);
    			eard_verbose(0,"shared memory updated max_freq %lu th %lf resched %d\n",dyn_conf->max_freq,dyn_conf->th,dyn_conf->force_rescheduling);
			}

        }
	}
	eard_verbose(0,"Configuration reloaded");
	
}

void signal_catcher()
{
	struct sigaction action;
	sigset_t set;
	int signal;
	sigset_t eard_mask;

    sigfillset(&eard_mask);
    sigdelset(&eard_mask,SIGPIPE);
    sigdelset(&eard_mask,SIGTERM);
    sigdelset(&eard_mask,SIGINT);
    sigdelset(&eard_mask,SIGHUP);
    //sigprocmask(SIG_SETMASK,&eard_mask,NULL);
    pthread_sigmask(SIG_SETMASK,&eard_mask,NULL);


	sigemptyset(&action.sa_mask);
	action.sa_handler = signal_handler;
	action.sa_flags = 0;

	signal = SIGPIPE;
	if (sigaction(signal, &action, NULL) < 0) {
		eard_verbose(0, "sigaction error on signal s=%d (%s)", signal, strerror(errno));
	}

	signal = SIGTERM;
	if (sigaction(signal, &action, NULL) < 0) {
		eard_verbose(0, "sigaction error on signal s=%d (%s)", signal, strerror(errno));
	}

	signal = SIGINT;
	if (sigaction(signal, &action, NULL) < 0) {
		eard_verbose(0, "sigaction error on signal s=%d (%s)", signal, strerror(errno));
	}

    signal = SIGHUP;
    if (sigaction(signal, &action, NULL) < 0) {
        eard_verbose(0, "sigaction error on signal s=%d (%s)", signal, strerror(errno));
    }

}
//endregion

void set_default_policy(policy_conf_t *policy)
{
	policy->th=1;
	policy->p_state=EAR_MIN_P_STATE;
}
void configure_new_values(ear_conf_t *dyn,cluster_conf_t *cluster,my_node_conf_t *node)
{
    policy_conf_t *my_policy;
    ulong deff;
    my_policy=get_my_policy_conf(cluster,node,cluster->default_policy);
    if (my_policy==NULL){
        // This should not happen
        eard_verbose(0,"Default policy  not found in ear.conf");
        my_policy=&default_policy_context;
    }
    deff=frequency_pstate_to_freq(my_policy->p_state);
    dyn->max_freq=frequency_pstate_to_freq(my_cluster_conf.eard.max_pstate);
    dyn->def_freq=deff;
    dyn->th=my_policy->th;
	dyn->force_rescheduling=1;
    eard_verbose(0,"configure_new_values max_freq %lu def_freq %lu th %.2lf\n",dyn->max_freq,dyn->def_freq,dyn->th);
}

void configure_default_values(ear_conf_t *dyn,cluster_conf_t *cluster,my_node_conf_t *node)
{
	policy_conf_t *my_policy;
	ulong deff;
	my_policy=get_my_policy_conf(cluster,node,cluster->default_policy);
	if (my_policy==NULL){
		// This should not happen
		eard_verbose(0,"Default policy  not found in ear.conf");
		my_policy=&default_policy_context;
	}
    deff=frequency_pstate_to_freq(my_policy->p_state);
	dyn->max_freq=frequency_pstate_to_freq(my_cluster_conf.eard.max_pstate);
    dyn->def_freq=deff;
    dyn->th=my_policy->th;
	eard_verbose(0,"configure_default_values max_freq %lu def_freq %lu th %.2lf\n",dyn->max_freq,dyn->def_freq,dyn->th);
}



/*
*
*
*	EARD MAIN
*
*
*/

void main(int argc,char *argv[])
{
	struct timeval *my_to;
	struct timeval tv;

	char ear_commreq[MAX_PATH_SIZE];
	char *my_ear_tmp;

	unsigned long ear_node_freq;
	int cpu_model;

	int numfds_ready;
	int numfds_req = 0;

	fd_set rfds;
	fd_set rfds_basic;

	int max_fd = -1;
	int ret;
	int i;
	char *shortnode;

	// Usage
	if (argc > 2) {
		Usage(argv[0]);
	}
    // We get nodename to create per_node files
    if (gethostname(nodename, sizeof(nodename)) < 0)
    {
        eard_verbose(0, "Error getting node name (%s)", strerror(errno));
        _exit(1);
    }
	strtok(nodename, ".");
	__HOST__=nodename;
	eard_verbose(1,"Executed in node name %s",nodename);
	/** CONFIGURATION **/
	// We read the cluster configuration and sets default values in the shared memory
	if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
		eard_verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
		_exit(0);
	}
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        eard_verbose(0," Error reading cluster configuration\n");
        _exit(1);
    }else{
        print_cluster_conf(&my_cluster_conf);
        my_node_conf=get_my_node_conf(&my_cluster_conf,nodename);
        if (my_node_conf==NULL){
            eard_verbose(0," Error in cluster configuration, node %s not found\n",nodename);
        }
		print_my_node_conf(my_node_conf);
    }
	set_global_eard_variables();
	create_tmp(ear_tmp);
	/** Shared memory is used between EARD and EARL **/
    eard_verbose(0,"creating shared memory tmp=%s\n",ear_tmp);
    dyn_conf=create_ear_conf_shared_area(ear_tmp,eard_max_freq);
    if (dyn_conf==NULL){
        eard_verbose(0,"Error creating shared memory\n");
        _exit(0);
    }
	// We initialize frecuency
	if (frequency_init(metrics_get_node_size()) < 0) {
		eard_verbose(0, "ERROR, frequency information can't be initialized");
		_exit(1);
	}
    configure_default_values(dyn_conf,&my_cluster_conf,my_node_conf);
    eard_verbose(0,"shared memory created max_freq %lu th %lf resched %d\n",dyn_conf->max_freq,dyn_conf->th,dyn_conf->force_rescheduling);

	/** We must control if we are come from a crash **/	
	// We check if we are recovering from a crash
	int must_recover=new_service("eard");
	if (must_recover){
		eard_verbose(0,"We must recover from a crash");
	}
	// Check
	if (argc == 2)
	{
		if (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0) Usage(argv[0]);
		EAR_VERBOSE_LEVEL = atoi(argv[1]);

		if ((EAR_VERBOSE_LEVEL < 0) || (EAR_VERBOSE_LEVEL > 4)) {
			Usage(argv[0]);
		}

	}
	set_ear_verbose(EAR_VERBOSE_LEVEL);

	// We catch signals
	signal_catcher();


    
	if (eard_max_pstate < 0) Usage(argv[0]);
	if (eard_max_pstate >= frequency_get_num_pstates()) Usage(argv[0]);

	ear_node_freq = frequency_pstate_to_freq(eard_max_pstate);
	eard_max_freq = ear_node_freq;
	eard_verbose(0, "Default max frequency defined to %lu\n",eard_max_freq);

	// Aperf (later on inside frequency_init(), but no more
	uint num_cpus = frequency_get_num_online_cpus();
	uint max_freq = frequency_get_nominal_freq();
	aperf_init_all_cpus(num_cpus, max_freq);


	#if EARD_LOCK
    eard_lock(ear_tmp,nodename);
	#endif
	// At this point, only one daemon is running

	eard_verbose(1,"Starting eard...................pid %d\n",getpid());
	eard_verbose(2,"Creating comm files in %s with default freq %lu verbose set to %d\n",
				nodename,ear_node_freq,EAR_VERBOSE_LEVEL);

	// PAST: we had here a frequency set

	// We initiaize uncore counters
	cpu_model = get_model();
	num_uncore_counters = init_uncores(cpu_model);
	eard_verbose(1,"eard %d imc uncore counters detected\n",num_uncore_counters);

	// We initialize rapl counters
	init_rapl_metrics();
	start_rapl_metrics();
	// We initilize energy_node
	if (node_energy_init()<0){
		eard_verbose(0,"node_energy_init cannot be initialized,DC node emergy metrics will not be provided\n");
	}
	energy_freq=node_energy_frequency();
	eard_verbose(1,"eard suggested time between for power performance accuracy us %lu usec.\n",energy_freq);
	

	// HW initialized HERE...creating communication channels
	eard_verbose(2,"Creating comm in %s for node %s\n",ear_tmp,nodename);
	FD_ZERO(&rfds);
	// We support a set of "types" of requests
	for (i=0;i<ear_daemon_client_requests;i++){
		create_connector(ear_tmp,nodename,i);
		ear_fd_ack[i]=-1;
	}
	
	for (i=0;i<ear_daemon_client_requests;i++){
		sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
		if ((ear_fd_req[i]=open(ear_commreq,O_RDWR))<0){
			eard_verbose(0,"Error opening ear communicator (%s)for requests %s\n",ear_commreq,strerror(errno));
			eard_close_comm();
		}
        FD_SET(ear_fd_req[i], &rfds);
		max_fd=max(max_fd,ear_fd_req[i]);
		ear_debug(2,"Adding %d fd to mask\n",ear_fd_req[i]);
		numfds_req=max_fd+1;
		ear_debug(3,"fd %d added to rdfd mask max=%d FD_SETSIZE=%d\n",ear_fd_req[i],numfds_req,FD_SETSIZE);
	}
	rfds_basic=rfds;
    // Database cache daemon
    #if USE_EARDB
	// use eardb configuration is pending
    if (eardbd_connect(my_node_conf->db_ip, my_cluster_conf.db_manager.udp_port, UDP)!=EAR_SUCCESS){
		eard_verbose(0,"Error connecting with EARDB");
	}
    #endif
	#if !USE_EARDB && DB_MYSQL
	eard_verbose(1,"Connecting with EAR DB");
	init_db_helper(&my_cluster_conf.database);
	#endif

	power_mon_freq=my_cluster_conf.eard.period_powermon;
	eard_verbose(1,"Using  %d seconds for periodic power monitoring",power_mon_freq);
	if (ret=pthread_create(&power_mon_th, NULL, eard_power_monitoring, (void *)&power_mon_freq)){
		errno=ret;
		eard_verbose(0,"error creating power_monitoring thread %s\n",strerror(errno));
	}
	if (ret=pthread_create(&dyn_conf_th, NULL, eard_dynamic_configuration, (void *)ear_tmp)){
		eard_verbose(0,"error creating dynamic_configuration thread \n");
	}

	eard_verbose(1,"Communicator for %s ON\n",nodename);
	// we wait until EAR daemon receives a request
	// We support requests realted to frequency and to uncore counters
	// rapl support is pending to be supported
	// EAR daemon main loop
	tv.tv_sec=20;tv.tv_usec=0;
	my_to=NULL;
	ear_debug(1,"eard waiting.....\n");
	/*
	*
	*	MAIN LOOP 
	*
	*/
	while (((numfds_ready=select(numfds_req,&rfds,NULL,NULL,my_to))>=0) || ((numfds_ready<0) && (errno==EINTR))){
			eard_verbose(4,"eard unblocked with %d readys.....\n",numfds_ready);
			if (numfds_ready>=0){ 
				if (numfds_ready>0){
					for (i=0;i<ear_daemon_client_requests;i++){
						if (FD_ISSET(ear_fd_req[i],&rfds)){
							select_service(i);
						}	// IF FD_ISSET
					} //for
				// We have to check if there is something else
				}else{ //timeout
						eard_verbose(2,"eard timeout...checking for application status\n");
						eard_verbose(2,"eard...application connected\n");	
						if (check_ping()) application_timeout();
				}
				// If application is disconnected, we wait for a new connection
				if (check_ping()){
					tv.tv_sec=20;tv.tv_usec=0;
					my_to=&tv;
				}else{ 
					my_to=NULL;
				}
			}else{// Signal received
				my_to=NULL;
				eard_verbose(0,"signal received");
			}
			rfds=rfds_basic;
			ear_debug(1,"eard waiting.....\n");
	}//while
	//eard is closed by SIGTERM signal, we should not reach this point
	eard_verbose(0,"END\n");
	eard_close_comm();	
	eard_exit(0);
}
