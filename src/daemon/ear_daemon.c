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

#include <control/frequency.h>
#include <metrics/ipmi/energy_node.h>
#include <metrics/custom/bandwidth.h>
#include <metrics/custom/hardware_info.h>
#include <metrics/papi/energy_cpu.h>
#include <common/ear_daemon_common.h>
#include <common/types/generic.h>
#include <common/environment.h>
#include <common/ear_verbose.h>
#include <common/states.h>

#ifdef POWER_MONITORING
#include <pthread.h>
#include <daemon/power_monitoring.h>
unsigned int power_mon_freq=3000000;
pthread_t power_mon_th; // It is pending to see whether it works with threads
//int power_mon_th;
#endif
#ifdef SHARED_MEMORY
#include <pthread.h>
#include <common/shared_configuration.h>
#include <daemon/dynamic_configuration.h>
pthread_t dyn_conf_th;
#endif

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define MAX_PATH_SIZE 	256
#define RAPL_METRICS 	4

static const char *__NAME__ = "EARD";

char database_bin_path[PATH_MAX];
char database_csv_path[PATH_MAX];

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

unsigned long energy_freq;
struct daemon_req req;

void eard_exit();
void eard_close_comm();
int is_new_application(int pid);
int is_new_service(int req,int pid);
int application_timeout();
int RAPL_counting=0;
int eard_must_exit=0;

// SIGNALS management
void f_signals(int s)
{
	if (s==SIGPIPE){
		eard_close_comm();
	}
	// eard exits here
	if ((s==SIGTERM)||(s==SIGINT)){
		eard_must_exit=1;
		if (s==SIGTERM) ear_verbose(0,"eard SIGTERM received.....\n");
		if (s==SIGINT) ear_verbose(0,"eard SIGINT received.....\n");
		if (ear_ping_fd>0){
			ear_verbose(1,"eard application is still connected!.....\n");
			eard_close_comm();
		}
	
		// Maybe we should just wait for threads
#ifdef POWER_MONITORING
		pthread_join(power_mon_th,NULL);
		//kill(power_mon_th,SIGKILL);
		//waitpid(power_mon_th,NULL,0);
#endif
#ifdef SHARED_MEMORY
		pthread_kill(dyn_conf_th,SIGUSR1);
		pthread_join(dyn_conf_th,NULL);
#endif
		eard_exit();
		ear_verbose(0,"eard exits.....\n");
	}	
}
void catch_signals()
{
    struct  sigaction sa; 
    int s;
	sigset_t set;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = f_signals;
    sa.sa_flags=0;
    s=SIGPIPE;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"eard doing sigaction of signal s=%d, %s\n",s,strerror(errno));
    s=SIGTERM;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"eard doing sigaction of signal s=%d, %s\n",s,strerror(errno));
    s=SIGINT;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"eard doing sigaction of signal s=%d, %s\n",s,strerror(errno));

}
// END SIGNALS management

// Lock unlock functions are used to be sure a single daemon is running per node
void eard_lock(char *tmp_dir,char *nodename)
{
	int ret;
	ret=mkdir(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if ((ret<0) && (errno!=EEXIST)){
		ear_verbose(0,"eard: ear tmp dir cannot be created (%s)",strerror(errno));
		exit(0);
	}
	
    chmod(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
	sprintf(eard_lock_file, "%s/%s.eard_lock", tmp_dir, nodename);

	if ((eard_lockf=open(eard_lock_file,O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR))<0)
	{
		if (errno != EEXIST) {
			ear_verbose(0,"eard: error, creating lock file %s fails (%s)\n",
						eard_lock_file, strerror(errno));
		} else {
            ear_verbose(0, "eard: Error opening daemon lock file in %s (%s)\n",
						eard_lock_file, strerror(errno));}
		exit(0);
	}
}

void eard_unlock()
{
	close(eard_lockf);
	ear_verbose(2,"eard: removing file %s\n",eard_lock_file);
	if (unlink(eard_lock_file)<0) ear_verbose(0, "eard error when removing lock file %s:%s\n",eard_lock_file,strerror(errno));
}

// Creates a pipe to receive requests for applications. eard creates 1 pipe (per node) and service to receive requests
void create_connector(char *ear_tmp,char *nodename,int i)
{
	char ear_commreq[MAX_PATH_SIZE];
	sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
	// ear_comreq files will be used to send requests from the library to the eard
	if (mknod(ear_commreq,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
		if (errno!=EEXIST){
			ear_verbose(0,"eard:Error creating ear communicator for requests %s\n",strerror(errno));
		}
	}
	chmod(ear_commreq,S_IRUSR|S_IWUSR|S_IRUSR|S_IWGRP|S_IROTH|S_IWOTH);
}

void connect_service(int req,unsigned long pid)
{
	char ear_commack[MAX_PATH_SIZE];
	unsigned long ack;
	int connect=1;
	int alive;

	// Let's check if there is another application
	VERBOSE_N(2, "request for connection at service %d", req);

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
		sprintf(ear_commack, "%s/.ear_comm.ack_%d.%lu", ear_tmp, req, pid);
		application_id = pid;

		// ear_commack will be used to send ack's or values (depending on the
		// requests) from eard to the library
		VERBOSE_N(3, "reating ack comm %s pid=%lu", ear_commack,pid);

		if (mknod(ear_commack, S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,0) < 0)
		{
			if (errno != EEXIST){
				VERBOSE_N(0, "ERROR WHEN creating ear communicator for ack %s", strerror(errno));
				eard_close_comm();
			}
		}

		chmod(ear_commack,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

		// At first service connection, we use the ping conn file
		if (req == 0)
		{
			// We open ping connection  for writting
			sprintf(ear_ping, "%s/.ear_comm.ping.%lu", ear_tmp, pid);

			VERBOSE_N(1, "application %lu connected", pid);
			VERBOSE_N(3, "opening ping conn for %lu", pid);
			ear_ping_fd = open(ear_ping,O_WRONLY);

			if (ear_ping_fd < 0)
			{
				VERBOSE_N(0,"ERROR while opening ping pipe %s (%s)", ear_ping, strerror(errno));
				eard_close_comm();
			}
#ifdef POWER_MONITORING
			powermon_mpi_init(pid);
#endif
		}

		VERBOSE_N(3, "sending ack for service %d",req);
		if (write(ear_ping_fd, &ack, sizeof(ack)) != sizeof(ack)) {
			VERBOSE_N(0,"WARNING while writting for ping conn for %lu", pid);
		}

		VERBOSE_N(2, "connecting service %s", ear_commack);
		if ((ear_fd_ack[req]=open(ear_commack,O_WRONLY)) < 0){
			VERBOSE_N(0,"ERROR when opening ear communicator for ack (%s)", strerror(errno));
			eard_close_comm();
		}
	} else {
		// eard only suppports one application connected, the second one will block
		VERBOSE_N(0, "application with pid %lu rejected", pid);
	}
	VERBOSE_N(2, "service %d connected", req);
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

// HIGHLIGHT: DAEMON EXIT
void eard_exit()
{
	char ear_commreq[MAX_PATH_SIZE];
	int i;

	VERBOSE_N(1, "Exiting");
	eard_unlock();

	// Come disposes
	print_rapl_metrics();

	// Recovering old frequency and governor configurations.
	VERBOSE_N(1,"frequency_recover_previous_policy");	
	frequency_recover_previous_policy();
	frequency_dispose();

	VERBOSE_N(1,"Releasing node resources");
	// More disposes
	node_energy_dispose();
	dispose_uncores();
	aperf_dispose();

	VERBOSE_N(1,"Releasing files");

	// Removing files (services)
	for (i = 0; i < ear_daemon_client_requests; i++)
	{
		close(ear_fd_req[i]);

		//sprintf(ear_commreq, "%s/.ear_comm.req_%d", ear_tmp, i);
		//VERBOSE_N(2, "removing file %s", ear_commreq);

		//if (unlink(ear_commreq) < 0) {
		//	VERBOSE_N(0, "error when removing com file %s (%s)", ear_commreq, strerror(errno));
		//}
	}

	exit(1);
}

// HIGHLIGHT: LIBRARY DISPOSE (2/2)
void eard_close_comm()
{
	unsigned long long values[RAPL_EVS];
	char ear_commack[MAX_PATH_SIZE];
	int dis_pid = application_id;
	int i;

	// closes communication with an application
	VERBOSE_N(2, "closing comm in %s for %d", nodename, application_id);

	// Stop counting
	frequency_recover_previous_frequency();

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
		VERBOSE_N(2, "removing file %s", ear_commack);

		if (unlink(ear_commack) < 0) {
			VERBOSE_N(0, "ERROR when removing ack file %s (%s)", ear_commack, strerror(errno));
		}
	}

	close(ear_ping_fd);
#ifdef POWER_MONITORING
	powermon_mpi_finalize(application_id);
#endif

	application_id = -1;
	ear_ping_fd = -1;

	VERBOSE_N(2, "removing file %s", ear_ping);

	if (unlink(ear_ping) < 0) {
		VERBOSE_N(0, "ERROR when removing ping file %s (%s)", ear_ping, strerror(errno));
	}
	VERBOSE_N(1, "application %d disconnected", dis_pid);
}

// Node_energy services
int eard_node_energy(int must_read)
{
	unsigned long ack;
	if (must_read){
    if (read(ear_fd_req[node_energy_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error reading info at eard_node_energyi (%s)\n",strerror(errno));
	}
    switch (req.req_service){
		case CONNECT_ENERGY:
			connect_service(node_energy_req,req.req_data.req_value);
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

void form_database_paths()
{
	char node_name[PATH_MAX];
	char *db_pathname;

	db_pathname = get_ear_db_pathname();

	if (db_pathname != NULL) {
		gethostname(node_name, sizeof(node_name));

		sprintf(database_bin_path, "%s%s.db.bin", db_pathname, node_name);
		sprintf(database_csv_path, "%s%s.db.csv", db_pathname, node_name);
	}

	VERBOSE_N(2, "DB binary file: %s", database_bin_path);
	VERBOSE_N(2, "DB pain-text file: %s", database_csv_path);
}

int eard_system(int must_read)
{
	unsigned long ack;
	 int ret1, ret2, size;

	if (must_read)
	{
		if (read(ear_fd_req[system_req],&req,sizeof(req))!=sizeof(req))
			ear_verbose(0,"eard error reading info at eard_system(%s)\n",strerror(errno));
	}

	switch (req.req_service)
	{
		case CONNECT_SYSTEM:
			connect_service(system_req,req.req_data.req_value);
			break;
		case WRITE_APP_SIGNATURE:
			ret1 = append_application_binary_file(database_bin_path, &req.req_data.app);
			ret2 = append_application_text_file(database_csv_path, &req.req_data.app);

			if (ret1 == EAR_SUCCESS && ret2 == EAR_SUCCESS)
			{
				DEBUG_F(1, "application signature correctly written");
				ack = EAR_COM_OK;
			} else {
				VERBOSE_N(1, "ERROR while application signature writing");
				ack = EAR_COM_ERROR;
			}
			size = sizeof(unsigned long);

			if (write(ear_fd_ack[system_req], &ack, size) != size)
			{
				VERBOSE_N(0, "ERROR while writing system service ack, closing connection...");
				eard_close_comm();
			}
			break;
		default: return 0;
	}
	return 1;
}

// FREQUENCY FUNCTIONALLITY: max_freq is the limit
void eard_set_freq(unsigned long new_freq,unsigned long max_freq)
{
	unsigned long ear_ok,freq;
	if (new_freq<=max_freq) freq=new_freq;
	else{ 
		ear_verbose(1,"eard: warning, maximum freq is limited to %lu\n",max_freq);
		freq=max_freq;
	}
	ear_ok=frequency_set_all_cpus(freq);
	write(ear_fd_ack[freq_req],&ear_ok,sizeof(unsigned long));  
}

int eard_freq(int must_read)
{
	unsigned long ack;

	if (must_read)
	{
		if (read(ear_fd_req[freq_req],&req,sizeof(req)) != sizeof(req))
			ear_verbose(0, "eard error when reading info at eard_freq (%s)\n", strerror(errno));
	}

	switch (req.req_service) {
		case CONNECT_FREQ:
			// HIGHLIGHT: LIBRARY INIT
			connect_service(freq_req,req.req_data.req_value);
			frequency_save_previous_frequency();
			break;
		case SET_FREQ:
			ear_debug(1,"eard: Setting node frequency\n");
#ifndef SHARED_MEMORY
			eard_set_freq(req.req_data.req_value,eard_max_freq);
#else
			eard_set_freq(req.req_data.req_value,min(eard_max_freq,max_dyn_freq()));
#endif	
			break;
		case START_GET_FREQ:
			aperf_get_avg_frequency_init_all_cpus();
			ack=EAR_COM_OK;
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_GET_FREQ:
			ear_debug(1,"eard: get node frequency (trubo)\n");
			ack = aperf_get_avg_frequency_end_all_cpus();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case SET_TURBO:
			frequency_set_performance_governor_all_cpus();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_COMM:
			ear_debug(1,"eard: Closing comunication\n");
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
			ear_debug(1,"eard: get app node frequency (trubo)\n");
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
	if (read(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at eard_uncore \n");
	}

	switch (req.req_service)
	{
	    case CONNECT_UNCORE:
			connect_service(uncore_req,req.req_data.req_value);
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
    	if (read(ear_fd_req[comm_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at eard_rapl\n");
	}
    switch (req.req_service){
		case CONNECT_RAPL:
			connect_service(rapl_req,req.req_data.req_value);
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

// Depending on MULTIPLE_SERVICES we can have multiple entry points or just one
#ifdef MULTIPLE_SERVICES
void select_service(int i)
{
	int ret=0;
	switch (i){
		case freq_req:
			ear_debug(1,"eard: frequency service\n");
			ret=eard_freq(1);
			break;
		case uncore_req:
			ear_debug(1,"eard uncore service\n");
			ret=eard_uncore(1);
			break;
		case rapl_req:
			ear_debug(1,"eard rapl service \n");
			ret=eard_rapl(1);
			break;  
		case system_req:
			ear_debug(1,"eard system service \n");
			ret=eard_system(1);
			break;
		case node_energy_req:
			ear_debug(1,"eard node_energy service\n");
			ret=eard_node_energy(1);
			break;
		default:
			ear_verbose(0,"eard: Error, request received not supported\n");
			eard_close_comm();
	}
	if (ret==0){
			ear_verbose(0,"eard: Error, request received not supported\n");
			eard_close_comm();
	}
}
#else
void select_service(int fd)
{
    if (read(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at select_service\n");
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
	ear_verbose(0,"eard: Error, request received not supported\n");
	eard_close_comm();
}
#endif
//
// MAIN eard: eard default_freq [path for communication files]
//
// eard creates different pipes to receive requests and to send values requested
void Usage(char *app)
{
	fprintf(stderr,"Usage: %s default_pstate [path for communication files] [verbose_level] \n",app);
	fprintf(stderr,"\tEAR_TMP is used as default path, then TMP or HOME, in this order\n"); 
	_exit(1);
}

void main(int argc,char *argv[])
{
	struct timeval tv,*my_to;
	char ear_commreq[MAX_PATH_SIZE];
	unsigned long ear_node_freq;
	int numfds_ready, numfds_req = 0;
	fd_set rfds, rfds_basic;
	int i,  cpu_model;
	sigset_t eard_mask;
	char *my_ear_tmp;
	int max_fd = -1;
	int ret;

	// binary P_STATE <path.to.tmp> verbosity_level
	if (argc < 2) {
		Usage(argv[0]);
	}

	//HIGHLIGHT: DAEMON INIT
	ear_daemon_environment();

	// checking verbose
	if (argc >= 4)
	{
		EAR_VERBOSE_LEVEL = atoi(argv[3]);

		if ((EAR_VERBOSE_LEVEL < 0) || (EAR_VERBOSE_LEVEL > 4)) {
			Usage(argv[0]);
		}

		set_ear_verbose(EAR_VERBOSE_LEVEL);
	}

	catch_signals();

	// We initialize frecuency
	if (frequency_init() < 0) {
		VERBOSE_N(0, "ERROR, frequency information can't be initialized");
		exit(1);
	}

	// It is assumed that no one will touch this governor. So it will
	// remain the same during the daemon runtime.
	frequency_save_previous_policy();
	frequency_set_userspace_governor_all_cpus();

	//
	eard_max_pstate = atoi(argv[1]);

	if (eard_max_pstate < 0) Usage(argv[0]);
	if (eard_max_pstate >= frequency_get_num_pstates()) Usage(argv[0]);

	ear_node_freq = frequency_pstate_to_freq(eard_max_pstate);
	eard_max_freq = ear_node_freq;

	// Aperf (later on inside frequency_init(), but no more
	uint num_cpus = frequency_get_num_online_cpus();
	uint max_freq = frequency_get_nominal_freq();
	aperf_init_all_cpus(num_cpus, max_freq);

	// We get nodename to create per_node files 
	if (gethostname(nodename, sizeof(nodename)) < 0)
	{
		VERBOSE_N(0, "Error getting node name (%s)", strerror(errno));
		_exit(1);
	}

    // Argv[2] is the path where eard will create files
    if (argc >= 3){
		my_ear_tmp = argv[2];
		set_ear_tmp(my_ear_tmp);
    }else{
		my_ear_tmp = get_ear_tmp();
    }

	strcpy(ear_tmp,my_ear_tmp);
    eard_lock(ear_tmp,nodename);
	// At this point, only one daemon is running

	ear_verbose(1,"Starting eard...................pid %d\n",getpid());
	ear_verbose(2,"eard: Creating comm files in %s with default freq %lu verbose set to %d\n",
				nodename,ear_node_freq,EAR_VERBOSE_LEVEL);

	// PAST: we had here a frequency set

	// We initiaize uncore counters
	cpu_model = get_model();
	num_uncore_counters = init_uncores(cpu_model);
	ear_verbose(1,"eard %d imc uncore counters detected\n",num_uncore_counters);

	// We initialize rapl counters
	init_rapl_metrics();
	// We initilize energy_node
	if (node_energy_init()<0){
		ear_verbose(0,"eard: node_energy_init cannot be initialized,DC node emergy metrics will not be provided\n");
	}
	energy_freq=node_energy_frequency();
	ear_verbose(1,"eard suggested time between for power performance accuracy us %lu usec.\n",energy_freq);
	
	form_database_paths();

	// HW initialized HERE...creating communication channels
	ear_verbose(2,"eard: Creating comm in %s for node %s\n",ear_tmp,nodename);
	FD_ZERO(&rfds);
	// We support a set of "types" of requests
	for (i=0;i<ear_daemon_client_requests;i++){
		create_connector(ear_tmp,nodename,i);
		ear_fd_ack[i]=-1;
	}
	
	for (i=0;i<ear_daemon_client_requests;i++){
		sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
		if ((ear_fd_req[i]=open(ear_commreq,O_RDWR))<0){
			ear_verbose(0,"eard:Error opening ear communicator (%s)for requests %s\n",ear_commreq,strerror(errno));
			eard_close_comm();
		}
        FD_SET(ear_fd_req[i], &rfds);
		max_fd=max(max_fd,ear_fd_req[i]);
		ear_debug(2,"eard: Adding %d fd to mask\n",ear_fd_req[i]);
		numfds_req=max_fd+1;
		ear_debug(3,"eard: fd %d added to rdfd mask max=%d FD_SETSIZE=%d\n",ear_fd_req[i],numfds_req,FD_SETSIZE);
	}
	rfds_basic=rfds;
#ifdef POWER_MONITORING
	if (ret=pthread_create(&power_mon_th, NULL, eard_power_monitoring, (void *)&power_mon_freq)){
		errno=ret;
		ear_verbose(0,"eard: error creating power_monitoring thread %s\n",strerror(errno));
	}
#if 0
	power_mon_th=fork();
	if (power_mon_th==0) eard_power_monitoring(&power_mon_freq);
#endif
#endif
#ifdef SHARED_MEMORY
	if (ret=pthread_create(&dyn_conf_th, NULL, eard_dynamic_configuration, (void *)ear_tmp)){
		ear_verbose(0,"eard: error creating dynamic_configuration thread \n");
	}
#endif
	ear_verbose(1,"eard:Communicator for %s ON\n",nodename);
	// we wait until EAR daemon receives a request
	// We support requests realted to frequency and to uncore counters
	// rapl support is pending to be supported
	// EAR daemon main loop
	sigfillset(&eard_mask);
	sigdelset(&eard_mask,SIGPIPE);
	sigdelset(&eard_mask,SIGTERM);
	sigdelset(&eard_mask,SIGINT); 
	sigprocmask(SIG_SETMASK,&eard_mask,NULL);
	tv.tv_sec=20;tv.tv_usec=0;
	my_to=NULL;
	ear_debug(1,"eard waiting.....\n");
	while((numfds_ready=select(numfds_req,&rfds,NULL,NULL,my_to))>=0){
		ear_debug(1,"eard unblocked with %d readys.....\n",numfds_ready);
		if (numfds_ready>0){
		for (i=0;i<ear_daemon_client_requests;i++){
			if (FD_ISSET(ear_fd_req[i],&rfds)){
				select_service(i);
			}	// IF FD_ISSET
		} //for
		// We have to check if there is something else
		}else{ //timeout
				ear_verbose(2,"eard timeout...checking for application status\n");
				ear_verbose(2,"eard...application connected\n");	
				if (check_ping()) application_timeout();
		}
		// If application is disconnected, we wait for a new connection
		if (check_ping()){
			tv.tv_sec=20;tv.tv_usec=0;
			my_to=&tv;
		}else my_to=NULL;
		rfds=rfds_basic;
		ear_debug(1,"eard waiting.....\n");
	}//while
	//eard is closed by SIGTERM signal, we should not reach this point
	ear_verbose(0,"eard: END\n");
	eard_close_comm();	
	eard_exit();
}
