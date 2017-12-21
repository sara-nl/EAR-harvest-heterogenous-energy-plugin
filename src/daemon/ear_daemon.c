/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <config.h>
#include <ear_verbose.h>
#include <ear_configuration.h>
#include <ear_arch_type.h>
#include <ear_daemon_common.h>
#include <ear_frequency.h>
#include <ear_rapl_metrics.h>
#include <ear_uncores.h>
#include <ear_db_type.h>
#include <ear_node_energy_metrics.h>

char my_errno[1024];
#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define MAX_PATH_SIZE 256
#define RAPL_METRICS 4
int ear_fd_req[ear_daemon_client_requests];
int ear_fd_ack[ear_daemon_client_requests];
unsigned long ear_daemon_max_freq;
int ear_daemon_max_pstate;
int num_uncore_counters;

char ear_ping[MAX_PATH_SIZE];
int ear_ping_fd=-1;
char ear_tmp[MAX_PATH_SIZE];
char nodename[MAX_PATH_SIZE];
int ear_daemon_lockf;
char ear_daemon_lock_file[MAX_PATH_SIZE];
int EAR_VERBOSE_LEVEL=0;
int application_id=-1;

unsigned long energy_freq;
struct daemon_req req;
int db_fd;

void ear_daemon_exit();
void ear_daemon_close_comm();
int is_new_application(int pid);
int is_new_service(int req,int pid);
int application_timeout();

// SIGNALS management
void f_signals(int s)
{
	if (s==SIGPIPE){
		ear_daemon_close_comm();
	}
	// ear_daemon exits here
	if ((s==SIGTERM)||(s==SIGINT)){
		if (s==SIGTERM) ear_verbose(0,"eard SIGTERM received.....\n");
		if (s==SIGINT) ear_verbose(0,"eard SIGINT received.....\n");
		if (ear_ping_fd>0){
			ear_verbose(1,"ear_damon application is still connected!.....\n");
			ear_daemon_close_comm();
		}
		ear_daemon_exit();
		ear_verbose(0,"ear_damon exits.....\n");
	}	
}
void catch_signals()
{
	struct  sigaction sa; 
    sigset_t mask;
    int s;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = f_signals;
    sa.sa_flags=0;
    s=SIGPIPE;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"ear_daemon doing sigaction of signal s=%d, %s\n",s,strerror(errno));
    s=SIGTERM;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"ear_daemon doing sigaction of signal s=%d, %s\n",s,strerror(errno));
    s=SIGINT;  
    if (sigaction(s, &sa, NULL) < 0)  ear_verbose(0,"ear_daemon doing sigaction of signal s=%d, %s\n",s,strerror(errno));

}
// END SIGNALS management

// Lock unlock functions are used to be sure a single daemon is running per node
void ear_daemon_lock(char *tmp_dir,char *nodename)
{
	int ret;
	ret=mkdir(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if ((ret<0) && (errno!=EEXIST)){
		ear_verbose(0,"ear_daemon: ear tmp dir cannot be created (%s)",strerror(errno));
		exit(0);
	}
	chmod(tmp_dir,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	sprintf(ear_daemon_lock_file,"%s/%s.ear_daemon_lock",tmp_dir,nodename);
	if ((ear_daemon_lockf=open(ear_daemon_lock_file,O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR))<0){ 
		if (errno!=EEXIST){
			ear_verbose(0,"ear_daemon: error, creating lock file %s fails: %s\n",ear_daemon_lock_file,strerror(errno));
		}
		else {ear_verbose(0,"ear_daemon: Error opening daemon lock file in %s. Exiting.%s\n",nodename,strerror(errno));}
		exit(0);
	}
}
void ear_daemon_unlock()
{
	close(ear_daemon_lockf);
	ear_verbose(2,"ear_daemon: removing file %s\n",ear_daemon_lock_file);
	if (unlink(ear_daemon_lock_file)<0) ear_verbose(0,"ear_daemon error when removing lock file %s:%s\n",ear_daemon_lock_file,strerror(errno));
}

// Creates a pipe to receive requests for applications. ear_daemon creates 1 pipe (per node) and service to receive requests
void create_connector(char *ear_tmp,char *nodename,int i)
{
	char ear_commreq[MAX_PATH_SIZE];
	sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
	// ear_comreq files will be used to send requests from the library to the ear_daemon
	if (mknod(ear_commreq,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
		if (errno!=EEXIST){
			ear_verbose(0,"ear_daemon:Error creating ear communicator for requests %s\n",strerror(errno));
		}
	}
	chmod(ear_commreq,S_IRUSR|S_IWUSR|S_IRUSR|S_IWGRP|S_IROTH|S_IWOTH);
}
// Creates 1 pipe (per node) to send acks. 
void connect_service(int req,unsigned long pid)
{
	char ear_commack[MAX_PATH_SIZE];
	unsigned long ack;
	int connect=1;
	int alive;
	// Let's check if there is another application
	ear_verbose(2,"ear_daemon request for connection at service %d\n",req);
	if (is_new_application(pid) || is_new_service(req,pid)){
		connect=1;
	}else{	
		connect=0;
		if (check_ping()) alive=application_timeout();	
		if (alive==0) connect=1;
		
	}
	if (connect){
    sprintf(ear_commack,"%s/.ear_comm.ack_%d.%d",ear_tmp,req,pid);
    application_id=pid;
    // ear_commack will be used to send ack's or values (depending on the requests) from ear_daemon to the library
    ear_verbose(3,"ear_daemon:creating ack comm %s pid=%d\n",ear_commack,pid);
    if (mknod(ear_commack,S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,0)<0){
        if (errno!=EEXIST){
            ear_verbose(0,"ear_daemon:Error creating ear communicator for ack %s\n",strerror(errno));
			ear_daemon_close_comm();
        }
    }  
    chmod(ear_commack,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	// At first service connection, we use the ping conn file
    if (req==0){
        // We open ping connection  for writting
		sprintf(ear_ping,"%s/.ear_comm.ping.%d",ear_tmp,pid);
		ear_verbose(1,"ear_daemon application %d connected\n",pid);
        ear_verbose(3,"ear_daemon: opening ping conn for %d\n",pid);
        ear_ping_fd=open(ear_ping,O_WRONLY);
        if (ear_ping_fd<0){
            ear_verbose(0,"Error opening ping pipe (%s) %s\n",ear_ping,strerror(errno));
            ear_daemon_close_comm();
        }
    }   
	ear_verbose(3,"ear_daemon sending ack for service %d\n",req);
	if (write(ear_ping_fd,&ack,sizeof(ack))!=sizeof(ack)) ear_verbose(0,"ear_daemon: warning writting ping conn for %d\n",pid);


    ear_verbose(2,"connect_service %s\n",ear_commack);
    if ((ear_fd_ack[req]=open(ear_commack,O_WRONLY))<0){
        ear_verbose(0,"ear_daemon: Error opening ear communicator for ack %s\n",strerror(errno));
		ear_daemon_close_comm();
    }
	}else{
		// ear_daemon only suppports one application connected, the second one will block
		ear_verbose(0,"ear_daemon: application with pid %d rejected\n",pid);
	}
	ear_verbose(2,"ear_daemon service %d connected\n",req);
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

// releases ear_daemon resources
void ear_daemon_exit()
{
        int i;
        unsigned long ack=0;
        char ear_commreq[MAX_PATH_SIZE];
	ear_verbose(1,"ear_daemon_exit_________\n");
        ear_daemon_unlock();
	close(db_fd);
        dispose_uncores();
        print_rapl_metrics();
        ear_cpufreq_end();
        node_energy_dispose();
        for (i=0;i<ear_daemon_client_requests;i++){
                close(ear_fd_req[i]);
                sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
                ear_verbose(2,"ear_dameon: removing file %s\n",ear_commreq);
                if (unlink(ear_commreq)<0) ear_verbose(0,"ear_daemon: error when removing com file %s : %s\n",ear_commreq,strerror(errno));
        }
		exit(1);
}
// closes communication with an application
void ear_daemon_close_comm()
{
	int i;
	unsigned long ack=0;
	int dis_pid=application_id;
	char ear_commack[MAX_PATH_SIZE];
	ear_verbose(2,"ear_daemon: Closing comm in %s for %d\n",nodename,application_id);
	for (i=0;i<ear_daemon_client_requests;i++){
		close(ear_fd_ack[i]);
		ear_fd_ack[i]=-1;
		sprintf(ear_commack,"%s/.ear_comm.ack_%d.%d",ear_tmp,i,application_id);
		ear_verbose(2,"ear_dameon: removing file %s\n",ear_commack);
		if (unlink(ear_commack)<0) ear_verbose(0,"ear_daemon: error when removing ack file %s : %s\n",ear_commack,strerror(errno));
	}
	close(ear_ping_fd);
	ear_ping_fd=-1;
	application_id=-1;
	ear_verbose(2,"ear_dameon: removing file %s\n",ear_ping);
	if (unlink(ear_ping)<0) ear_verbose(0,"ear_daemon: error when removing ping file %s : %s\n",ear_ping,strerror(errno));
	ear_verbose(1,"ear_daemon application %d disconnected\n",dis_pid);
}

// Node_energy services
int ear_daemon_node_energy(int must_read)
{
	unsigned long ack;
	if (must_read){
    if (read(ear_fd_req[node_energy_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error reading info at ear_daemon_node_energyi (%s)\n",strerror(errno));
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
        default:
			return 0;
    }
	return 1;

}
// System services
int open_db()
{
	int fd;
	char *ear_DB_name;
	char ear_db_def[MAX_PATH_SIZE];
	char ear_node_name[MAX_PATH_SIZE];
	gethostname(ear_node_name,sizeof(ear_node_name));
	ear_DB_name=get_ear_db_pathname();
    sprintf(ear_db_def,"%s.%s.db",ear_DB_name,ear_node_name);
    ear_verbose(2,"ear_daemon  DB name %s\n",ear_db_def);
    ear_DB_name=ear_db_def;
    ear_verbose(2,"ear_daemon is using %s file as DB\n",ear_DB_name);
    fd=open(ear_DB_name,O_WRONLY|O_APPEND);
    if (fd<0){
		ear_verbose(0,"ear_daemonn warning opening DB %s(%s)\n",ear_DB_name,strerror(errno));
    	if (errno==ENOENT){
			ear_verbose(1,"ear_daemon creating syste, db %s\n",ear_DB_name);
            fd=open(ear_DB_name,O_WRONLY|O_CREAT|O_TRUNC|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            if (fd<0){
                ear_verbose(0,"ear_daemon WARNING...it was impossible to create the DB file for historical information (%s)\n",strerror(errno));
				ear_verbose(0,"ear_daemon WARNING: check all the directories in %s exists\n",ear_DB_name);
            }
    	}else{
            ear_verbose(0,"ear_daemon WARNING...it was impossible to create the DB file for historical information (%s)\n",strerror(errno));
			ear_verbose(0,"ear_daemon WARNING: check all the directories in %s exists\n",ear_DB_name);
    	}
	}
	return fd;

}
void write_db(int fd,struct App_info *app_signature)
{
	if (fd<0) return;
	if (write(fd,app_signature,sizeof(struct App_info))!=sizeof(struct App_info)) {
		ear_verbose(0,"ear_daemon PANIC error writting in DB (%s)\n",strerror(errno));
		ear_daemon_close_comm();
	}
}
int ear_daemon_system(int must_read)
{
	unsigned long ack;
	if (must_read){
	if (read(ear_fd_req[system_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error reading info at ear_daemon_system(%s)\n",strerror(errno));
	}
	switch (req.req_service){
		case CONNECT_SYSTEM:
			connect_service(system_req,req.req_data.req_value);
			break;
		case WRITE_APP_SIGNATURE:
			if (db_fd>=0){
				write_db(db_fd,&req.req_data.app);
				ack=EAR_COM_OK;
				if (write(ear_fd_ack[system_req],&ack,sizeof(unsigned long))!=sizeof(unsigned long)){
						ear_verbose(0,"ear_daemon: invalid write to system_req ack\n");
						ear_daemon_close_comm();
				}
			}
			break;
		default:
			return 0;
	}
	return 1;
}

// FREQUENCY FUNCTIONALLITY: max_freq is the limit
void ear_daemon_set_freq(unsigned long new_freq,unsigned long max_freq)
{
	unsigned long ear_ok,freq;
	if (new_freq<=max_freq) freq=new_freq;
	else{ 
		ear_verbose(1,"ear_daemon: warning, maximum freq is limited to %u\n",max_freq);
		freq=max_freq;
	}
	ear_ok=ear_cpufreq_set_node(freq);
	write(ear_fd_ack[freq_req],&ear_ok,sizeof(unsigned long));  
}
int ear_daemon_freq(int must_read)
{
	unsigned long ack;
	if (must_read){
	if (read(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at ear_daemon_freq (%s)\n",strerror(errno));
	}
	switch (req.req_service){
		case CONNECT_FREQ:
			connect_service(freq_req,req.req_data.req_value);		
			break;
		case SET_FREQ:
			ear_debug(1,"ear_daemon: Setting node frequency\n");
			ear_daemon_set_freq(req.req_data.req_value,ear_daemon_max_freq);
			break;
		case START_GET_FREQ:
			ear_begin_compute_turbo_freq();
			ack=EAR_COM_OK;
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_GET_FREQ:
			ear_debug(1,"ear_daemon: get node frequency (trubo)\n");
			ack=ear_end_compute_turbo_freq();
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case SET_TURBO:
			ear_set_turbo();	
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		case END_COMM:
			ear_debug(1,"ear_daemon: Closing comunication\n");
			ear_daemon_close_comm();
			break;
		case DATA_SIZE_FREQ:
			ack=sizeof(unsigned long);
			write(ear_fd_ack[freq_req],&ack,sizeof(unsigned long));
			break;
		default:
			return 0;

	}
	return 1;
}

int ear_daemon_uncore(int must_read)
{

	unsigned long ack=EAR_SUCCESS;
	unsigned long long values[num_uncore_counters];

	if (must_read){	
	if (read(ear_fd_req[uncore_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at ear_daemon_uncore \n");
	}

	switch (req.req_service){
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
			write(ear_fd_ack[uncore_req],values,sizeof(unsigned long long) * num_uncore_counters);
			for (i=0; i < num_uncore_counters; i++) demon_cas+=values[i];
			ear_debug(3,"DAEMON cas %llu %d values\n", demon_cas, num_uncore_counters);
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
int ear_daemon_rapl(int must_read)
{
	unsigned long comm_req=rapl_req;
    unsigned long ack=0;
    unsigned long long values[EAR_RAPL_EVENTS];
	if (must_read){
    	if (read(ear_fd_req[comm_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at ear_daemon_rapl\n");
	}
    switch (req.req_service){
		case CONNECT_RAPL:
			connect_service(rapl_req,req.req_data.req_value);
			break;
        case START_RAPL:
    		ear_debug(1,"EAR_daemon_server: start RAPL counters\n");
            start_rapl_metrics();
            write(ear_fd_ack[comm_req],&ack,sizeof(ack));
            break;
        case RESET_RAPL:
    		ear_debug(1,"EAR_daemon_server: reset RAPL\n");
            reset_rapl_metrics();
            write(ear_fd_ack[comm_req],&ack,sizeof(ack));
            break;
        case READ_RAPL:
    		ear_debug(1,"EAR_daemon_server: read RAPL\n");
            stop_rapl_metrics(values);
            write(ear_fd_ack[comm_req],values,sizeof(unsigned long long)*EAR_RAPL_EVENTS);
            break;
        case DATA_SIZE_RAPL:
            ack=sizeof(unsigned long long)*EAR_RAPL_EVENTS;
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
			ear_debug(1,"ear_daemon: frequency service\n");
			ret=ear_daemon_freq(1);
			break;
		case uncore_req:
			ear_debug(1,"ear_daemon uncore service\n");
			ret=ear_daemon_uncore(1);
			break;
		case rapl_req:
			ear_debug(1,"ear_daemon rapl service \n");
			ret=ear_daemon_rapl(1);
			break;  
		case system_req:
			ear_debug(1,"ear_daemon system service \n");
			ret=ear_daemon_system(1);
			break;
		case node_energy_req:
			ear_debug(1,"ear_daemon node_energy service\n");
			ret=ear_daemon_node_energy(1);
			break;
		default:
			ear_verbose(0,"ear_daemon: Error, request received not supported\n");
			ear_daemon_close_comm();
	}
	if (ret==0){
			ear_verbose(0,"ear_daemon: Error, request received not supported\n");
			ear_daemon_close_comm();
	}
}
#else
void select_service(int fd)
{
    if (read(ear_fd_req[freq_req],&req,sizeof(req))!=sizeof(req)) ear_verbose(0,"eard error when reading info at select_service\n");
	if (ear_daemon_freq(0)){ 
		ear_debug(0,"eard frequency service\n");
		return;
	}
	if (ear_daemon_uncore(0)){ 
		ear_debug(0,"eard uncore service\n");
		return;
	}
	if (ear_daemon_rapl(0)){ 
		ear_debug(0,"eard rapl service\n");
		return;
	}
	if (ear_daemon_system(0)){ 
		ear_debug(0,"eard system service\n");
		return;
	}
	if (ear_daemon_node_energy(0)){ 
		ear_debug(0,"eard node energy service\n");
		return;
	}
	ear_verbose(0,"eard: Error, request received not supported\n");
	ear_daemon_close_comm();
}
#endif
//
// MAIN ear_daemon: ear_daemon default_freq [path for communication files]
//
// ear_daemon creates different pipes to receive requests and to send values requested
void Usage(char *app)
{
	fprintf(stderr,"Usage: %s default_pstate [path for communication files] [verbose_level] \n",app);
	fprintf(stderr,"\tEAR_TMP is used as default path, then TMP or HOME, in this order\n"); 
	_exit(1);
}
void main(int argc,char *argv[])
{
	char *my_ear_tmp;
	fd_set rfds,rfds_basic;
	int numfds_ready,numfds_req=0;
	unsigned long ear_node_freq,ear_ok=1;
	int i,  cpu_model,ret;
	int max_fd=-1;
	sigset_t ear_daemon_mask;
	struct timeval tv,*my_to;
	char ear_commreq[MAX_PATH_SIZE];

	// Default p_state is set in argv[2], it is mandatory 
	if (argc<2) Usage(argv[0]);
	ear_daemon_environment();
	// checking verbose
	if (argc>=4){
		EAR_VERBOSE_LEVEL=atoi(argv[3]);
		if ((EAR_VERBOSE_LEVEL<0)||(EAR_VERBOSE_LEVEL>4)) Usage(argv[0]);
		set_ear_verbose(EAR_VERBOSE_LEVEL);
	}
	catch_signals();
	// Default p_state
	// We initialize frecuency 
	if (ear_cpufreq_init()<0){
		ear_verbose(0,"ear_daemon: frequency information can not be initilised\n");
		exit(1);
	}
	ear_daemon_max_pstate=atoi(argv[1]);
	if (ear_daemon_max_pstate<0) Usage(argv[0]);
	if (ear_daemon_max_pstate>=ear_get_num_p_states()) Usage(argv[0]);
	ear_node_freq=ear_get_freq(ear_daemon_max_pstate);
	ear_daemon_max_freq=ear_node_freq;
	// We get nodename to create per_node files 
	if (gethostname(nodename,sizeof(nodename))<0){
       		ear_verbose(0,"ear_daemon:Error getting node name.%s\n.Exiting\n",strerror(errno));
			_exit(1);
	}
    // Argv[2] is the path where ear_daemon will create files
    if (argc>=3){ 
		my_ear_tmp=argv[2];
		set_ear_tmp(my_ear_tmp);
    }else{
		my_ear_tmp=get_ear_tmp();
    }   
	strcpy(ear_tmp,my_ear_tmp);
    ear_daemon_lock(ear_tmp,nodename);
	// At this point, only one daemon is running

	ear_verbose(1,"Starting ear_daemon...................pid %d\n",getpid());
	ear_verbose(2,"ear_daemon: Creating comm files in %s with default freq %u verbose set to %d\n",nodename,ear_node_freq,EAR_VERBOSE_LEVEL);	
	// We set the default frequency
	if (ear_cpufreq_set_node(ear_node_freq)==0){
		ear_verbose(0,"ear_daemon:Invalid frequency %u\n",ear_node_freq);
		ear_daemon_close_comm();
	}
	ear_print_daemon_environment();

	// We initiaize uncore counters
	cpu_model = get_model();
	num_uncore_counters = init_uncores(cpu_model);
	ear_verbose(1,"eard %d imc uncore counters detected\n",num_uncore_counters);

	// We initialize rapl counters
	init_rapl_metrics();

	// We initilize node_energy_metrics
	if (node_energy_init()<0){
		ear_verbose(0,"ear_daemon: node_energy_init cannot be initialized,DC node emergy metrics will not be provided\n");
	}
	energy_freq=node_energy_frequency();
	ear_verbose(1,"ear_daemon min time between two dc node energy measurements is %lu usec., we recomend to use %lu usec.\n",energy_freq,energy_freq*2);
	// We open the system_db
	db_fd=open_db();
	// HW initialized HERE...creating communication channels
	ear_verbose(2,"ear_daemon: Creating comm in %s for node %s\n",ear_tmp,nodename);
	FD_ZERO(&rfds);
	// We support a set of "types" of requests
	for (i=0;i<ear_daemon_client_requests;i++){
		create_connector(ear_tmp,nodename,i);
		ear_fd_ack[i]=-1;
	}
	
	for (i=0;i<ear_daemon_client_requests;i++){
		sprintf(ear_commreq,"%s/.ear_comm.req_%d",ear_tmp,i);
		if ((ear_fd_req[i]=open(ear_commreq,O_RDWR))<0){
			ear_verbose(0,"ear_daemon:Error opening ear communicator (%s)for requests %s\n",ear_commreq,strerror(errno));
			ear_daemon_close_comm();
		}
        FD_SET(ear_fd_req[i], &rfds);
		max_fd=max(max_fd,ear_fd_req[i]);
		ear_debug(2,"ear_daemon: Adding %d fd to mask\n",ear_fd_req[i]);
		numfds_req=max_fd+1;
		ear_debug(3,"ear_daemon: fd %d added to rdfd mask max=%d FD_SETSIZE=%d\n",ear_fd_req[i],numfds_req,FD_SETSIZE);
	}
	rfds_basic=rfds;
	ear_verbose(1,"EAR:Communicator for %s ON\n",nodename);
	// we wait until EAR daemon receives a request
	// We support requests realted to frequency and to uncore counters
	// rapl support is pending to be supported
	// EAR daemon main loop
	sigemptyset(&ear_daemon_mask);
	sigaddset(&ear_daemon_mask,SIGPIPE);
	sigaddset(&ear_daemon_mask,SIGTERM);
	sigaddset(&ear_daemon_mask,SIGINT);
	sigprocmask(SIG_UNBLOCK,&ear_daemon_mask,NULL);
	tv.tv_sec=20;tv.tv_usec=0;
	my_to=NULL;
	ear_debug(1,"ear_daemon waiting.....\n");
	while((numfds_ready=select(numfds_req,&rfds,NULL,NULL,my_to))>=0){
		ear_debug(1,"ear_daemon unblocked with %d readys.....\n",numfds_ready);
		if (numfds_ready>0){
		for (i=0;i<ear_daemon_client_requests;i++){
			if (FD_ISSET(ear_fd_req[i],&rfds)){
				select_service(i);
			}	// IF FD_ISSET
		} //for
		// We have to check if there is something else
		}else{ //timeout
				ear_verbose(2,"ear_daemon timeout...checking for application status\n");
				ear_verbose(2,"ear_daemon...application connected\n");	
				if (check_ping()) application_timeout();
		}
		// If application is disconnected, we wait for a new connection
		if (check_ping()){
			tv.tv_sec=20;tv.tv_usec=0;
			my_to=&tv;
		}else my_to=NULL;
		rfds=rfds_basic;
		ear_debug(1,"ear_daemon waiting.....\n");
	}//while
	//ear_daemon is closed by SIGTERM signal, we should not reach this point
	ear_verbose(0,"ear_daemon: END\n");
	ear_daemon_close_comm();	
	ear_daemon_exit();
}
