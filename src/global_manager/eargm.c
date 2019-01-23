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

#define _XOPEN_SOURCE 700 //to get rid of the warning

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <common/states.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/database/db_helper.h>
#include <common/types/generic.h>
#include <common/types/gm_warning.h>
#include <common/types/configuration/cluster_conf.h>
#include <global_manager/eargm_server_api.h>
#include <daemon/eard_rapi.h>

#if SYSLOG_MSG
#include <syslog.h>
#endif

/*
*	EAR Global Manager constants
*/

#define NO_PROBLEM 	3
#define WARNING_3	2
#define WARNING_2	1
#define PANIC		0
#define NUM_LEVELS  4
#define DEFCON_L4	0
#define DEFCON_L3	1
#define DEFCON_L2	2
#define BASIC_U		1
#define KILO_U		1000
#define MEGA_U		1000000

ulong th_level[NUM_LEVELS]={10,10,5,0};
ulong pstate_level[NUM_LEVELS]={3,2,1,0};
uint use_aggregation;
uint units;
uint policy;
uint divisor = 1;

uint t1_expired=0;
uint must_refill=0;

pthread_t eargm_server_api_th;
cluster_conf_t my_cluster_conf;
char my_ear_conf_path[GENERIC_NAME];	
uint total_nodes=0;
char  unit_name[128],unit_energy[128],unit_power[128];

/* 
* EAR Global Manager global data
*/
int verbose_arg=-1;
uint period_t1,period_t2;
ulong total_energy_t2,energy_t1;
uint my_port;
uint current_sample=0,total_samples=0;
ulong *energy_consumed;
ulong energy_budget;
ulong power_budget;
uint aggregate_samples;
uint in_action=0;
double perc_energy,perc_time,perc_power;
double avg_power_t2,avg_power_t1;




void update_eargm_configuration(cluster_conf_t *conf)
{
	verb_level=conf->eargm.verbose;
	verb_channel=2;
	if (verbose_arg>0) verb_level=verbose_arg;

	period_t1=conf->eargm.t1;
	period_t2=conf->eargm.t2;
	energy_budget=conf->eargm.energy;
	power_budget=conf->eargm.energy;
	my_port=conf->eargm.port;
	use_aggregation=conf->eargm.use_aggregation;
	units=conf->eargm.units;
	policy=conf->eargm.policy;
    switch(units)
    {
        case BASIC:divisor=BASIC_U;
			switch (policy){
				case MAXENERGY:strcpy(unit_name,"Joules");break;
				case MAXPOWER:strcpy(unit_name,"Watts");break;
			}
			strcpy(unit_energy,"J");
			strcpy(unit_power,"W");
			
			break;
        case KILO:divisor=KILO_U;
			switch (policy){
				case MAXENERGY:strcpy(unit_name,"Kilo Joules");break;
				case MAXPOWER:strcpy(unit_name,"Kilo Watts");break;
			}
			strcpy(unit_energy,"KJ");
			strcpy(unit_power,"KW");
			break;
        case MEGA:divisor=MEGA_U;	
			switch (policy){
				case MAXENERGY:strcpy(unit_name,"Mega Joules");break;
				case MAXPOWER:strcpy(unit_name,"Mega Watts");break;
			}
			strcpy(unit_energy,"MJ");
			strcpy(unit_power,"MW");
			break;
        default:break;
    }

}


static void my_signals_function(int s)
{
	uint punits;
	uint ppolicy;
	if (s==SIGALRM){
		alarm(period_t1);
		t1_expired=1;	
		return;
	}
	if (s==SIGHUP){
		verbose(0,"Reloading EAR configuration");
		free_cluster_conf(&my_cluster_conf);
		// Reading the configuration
    	if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        	verbose(0," Error reading cluster configuration\n");
    	}
    	else{
        	print_cluster_conf(&my_cluster_conf);
			update_eargm_configuration(&my_cluster_conf);
			must_refill=1;
			if (ppolicy!=policy){
				verbose(0," Error policy can not be change on the fly, stop & start EARGM\n");
				policy=ppolicy;
			}
			if (policy==MAXENERGY){ 
				verbose(0,"Using new energy limit %lu",energy_budget);
			}
			if (policy==MAXPOWER){ 
				verbose(0,"Using new power limit %lu",power_budget);
			}
    	}
	}else{
		verbose(0,"Exiting");
	    #if SYSLOG_MSG
	    closelog();
	    #endif

		exit(0);
	}
}

void print_time(char *msg,time_t *t)
{
	char buff[128];
	struct tm *tmp;

    tmp = localtime(t);
    if (tmp == NULL) {
		fprintf(stderr,"eargmd:localtime %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

   if (strftime(buff, sizeof(buff), "%x - %I:%M:%S%p", tmp) == 0) {
        fprintf(stderr, "eargmd: strftime returned 0");
        exit(EXIT_FAILURE);
    }

   fprintf(stdout,"time %u: %s %s", *t,msg,buff);
}
static void catch_signals()
{
    sigset_t set;
    struct sigaction my_action;

    sigemptyset(&set);
    sigaddset(&set,SIGHUP);
    sigaddset(&set,SIGTERM);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGALRM);
    if (sigprocmask(SIG_SETMASK,&set,NULL)<0){
        verbose(0,"Setting signal mask (%s)\n",strerror(errno));
        exit(1);
    }

    my_action.sa_handler=my_signals_function;
    sigemptyset(&my_action.sa_mask);
    my_action.sa_flags=0;
    if (sigaction(SIGHUP,&my_action,NULL)<0){
        verbose(0," sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGTERM,&my_action,NULL)<0){
        verbose(0,"sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGINT,&my_action,NULL)<0){
        verbose(0," sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGALRM,&my_action,NULL)<0){
        verbose(0," sigaction for SIGALRM (%s)\n",strerror(errno));
        exit(1);
    }

}

void new_energy_sample(ulong result)
{
	energy_consumed[current_sample]=result;
	current_sample=(current_sample+1)%aggregate_samples;
	if (total_samples<aggregate_samples) total_samples++;
}

ulong compute_energy_t2()
{
	ulong energy=0;
	uint i;
	int limit=aggregate_samples;
	if (total_samples<aggregate_samples) limit=total_samples;
	for (i=0;i<limit;i++){
		energy+=energy_consumed[i];
	}
	return energy;
}



uint defcon(ulong e_t2,ulong e_t1,ulong load)
{
  double perc;
  switch (policy){
    case MAXENERGY:
      perc_energy=((double)e_t2/(double)energy_budget)*(double)100;
      perc_time=((double)total_samples/(double)aggregate_samples)*(double)100;
      verbose(0,"Percentage over energy budget %.2lf%% (total energy t2 %lu , energy limit %lu)\n",perc_energy,e_t2,energy_budget);
      if (perc_time<100.0){
        if (perc_energy>perc_time){
            verbose(0,"WARNING %.2lf%% of energy vs %.2lf%% of time!!\n",perc_energy,perc_time);
        }
      }
      perc=perc_energy;
      break;
    case MAXPOWER:;
      avg_power_t1=(e_t1/period_t1);
      avg_power_t2=(e_t2/period_t2);
      verbose(0,"Avg. Power for T1 %lf\nAvg. Power for T2 %lf\n",avg_power_t1,avg_power_t2);
      perc_power=(double)avg_power_t1/(double)power_budget;
      perc=perc_power;
      break;
  }
  if (perc<my_cluster_conf.eargm.defcon_limits[DEFCON_L4]) return NO_PROBLEM;
  if ((perc>=my_cluster_conf.eargm.defcon_limits[DEFCON_L4]) && (perc<my_cluster_conf.eargm.defcon_limits[DEFCON_L3]))  return WARNING_3;
  if ((perc>=my_cluster_conf.eargm.defcon_limits[DEFCON_L3]) && (perc<my_cluster_conf.eargm.defcon_limits[DEFCON_L2]))  return WARNING_2;
  return PANIC;

}

void fill_periods(ulong energy)
{
	int i;
	ulong e_persample;
	e_persample=energy/aggregate_samples;
	for (i=0;i<aggregate_samples;i++){
		energy_consumed[i]=e_persample;
	}
	total_samples=aggregate_samples;
	current_sample=0;
	switch (policy)
	{
	case MAXENERGY:
		verbose(1,"Initializing T2 period with %lu %s, each sample with %lu %s",energy,unit_name,e_persample,unit_name);
		break;
	case MAXPOWER:
		verbose(1,"AVG power in last %d seconds %u %s\n",period_t1,e_persample/period_t1,unit_name);
		break;
	}
}

void send_mail(uint level, double energy)
{
	char buff[128];
	char command[1024];
	char mail_filename[128];
	int fd;
	sprintf(buff,"Detected WARNING level %u, %lfi %% of energy from the total energy limit\n",level,energy);
	sprintf(mail_filename,"%s/warning_mail.txt",my_cluster_conf.tmp_dir);
	fd=open(mail_filename,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd<0){
		verbose(0,"Warning mail file cannot be created at %s (%s)",mail_filename,strerror(errno));
		return;
	}
	write(fd,buff,strlen(buff));
	close(fd);
	sprintf(command,"mail -s \"Energy limit warning\" %s < %s",my_cluster_conf.eargm.mail,mail_filename);
	if (strcmp(my_cluster_conf.eargm.mail,"nomail")) system(command);
	else { verbose(1,"%s",command); }
	
}

/*
*
*	THREAD attending external commands. 
*
*/

void process_remote_requests(int clientfd)
{
    eargm_request_t command;
    uint req;
    ulong ack=EAR_SUCCESS;
    verbose(2,"connection received\n");
    req=read_command(clientfd,&command);
    switch (req){
        case EARGM_NEW_JOB:
			// Computes the total number of nodes in use
            verbose(1,"new_job command received %d num_nodes %u\n",command.req,command.num_nodes);
			total_nodes+=command.num_nodes;
            break;
        case EARGM_END_JOB:
			// Computes the total number of nodes in use
            verbose(1,"end_job command received %d num_nodes %u\n",command.req,command.num_nodes);
			total_nodes-=command.num_nodes;
            break;
        default:
            verbose(0,"Invalid remote command\n");
    }  
    send_answer(clientfd,&ack);
}


void *eargm_server_api(void *p)
{
	int eargm_fd,eargm_client;
	struct sockaddr_in eargm_con_client;

    verbose(2,"Creating scoket for remote commands,using port %u",my_port);
    eargm_fd=create_server_socket(my_port);
    if (eargm_fd<0){
        verbose(0,"Error creating socket\n");
        pthread_exit(0);
    }
    do{
        verbose(2,"waiting for remote commands port=%u\n",my_port);
        eargm_client=wait_for_client(eargm_fd,&eargm_con_client);
        if (eargm_client<0){
            verbose(0," wait_for_client returns error\n");
        }else{
            process_remote_requests(eargm_client);
            close(eargm_client);
        }
    }while(1);
    verbose(0,"exiting\n");
    close_server_socket(eargm_fd);
	return NULL;

}

/*
*	ACTIONS for WARNING and PANIC LEVELS
*/
ulong eargm_increase_th_all_nodes(int level)
{
	int i,rc;
	ulong th;
	th=th_level[level];
	increase_th_all_nodes(th,my_cluster_conf);
	return th;
}
ulong eargm_reduce_frequencies_all_nodes(int level)
{
    int i,rc;
    ulong ps;
	ps=pstate_level[level];
	reduce_frequencies_all_nodes(ps,my_cluster_conf);
	return ps;
}
void report_status(gm_warning_t *my_warning)
{
    #if SYSLOG_MSG
    syslog(LOG_DAEMON|LOG_ERR,"Warning level %d: p_state %u energy/power perc %.3lf inc_th %.2lf \n",my_warning->level,my_warning->new_p_state,my_warning->energy_percent,my_warning->inc_th);
	#endif
}




/*
*
*	EAR GLOBAL MANAGER
*
*
*/
void usage(char *app)
{
    printf("Usage: %s [-h]|[--help]|verbose_level] \n", app);
	printf("This program controls the energy consumed in a period T2 seconds defined in $ETC/ear/ear.conf file\n");
	printf("energy is checked every T1 seconds interval\n");
	printf("Global manager can be configured in active or passive mode. Automatic actions are taken in active mode (defined in ear.conf file)\n");
    exit(0);
}


void parse_args(char *argv[])
{
	if (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0){
		usage(argv[0]);
		exit(0);
	}
	verbose_arg=atoi(argv[1]);	
}

#define GM_DEBUG 1

void main(int argc,char *argv[])
{
	sigset_t set;
	int ret;
	ulong result,result2;
	gm_warning_t my_warning;
    if (argc > 2) usage(argv[0]);
	if (argc==2) parse_args(argv);
    // We read the cluster configuration and sets default values in the shared memory
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }
    #if SYSLOG_MSG
    openlog("eargm",LOG_PID|LOG_PERROR,LOG_DAEMON);
    #endif

	verbose(0,"Using %s as EARGM configuration file",my_ear_conf_path);
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        verbose(0," Error reading cluster configuration\n");
    }
    else{
        print_cluster_conf(&my_cluster_conf);
    }
    update_eargm_configuration(&my_cluster_conf);


	switch (policy){
		case MAXENERGY:
			verbose(0,"MAXENERGY policy configured with limit %u %s\n",energy_budget,unit_name);
			break;
		case MAXPOWER:
			verbose(0,"MAXPOWER policy configured with limit %u %s\n",power_budget,unit_name);
			break;
	}	
	
		

	if ((period_t1<=0) || (period_t2<=0) || (energy_budget<=0)) usage(argv[0]);

	aggregate_samples=period_t2/period_t1;
	if ((period_t2%period_t1)!=0){
		verbose(2,"warning period_t2 is not multiple of period_t1\n");
		aggregate_samples++;
	}

	energy_consumed=malloc(sizeof(ulong)*aggregate_samples);



	catch_signals();

	/* This thread accepts external commands */
    if (ret=pthread_create(&eargm_server_api_th, NULL, eargm_server_api, NULL)){
        errno=ret;
		verbose(0,"error creating eargm_server for external api %s\n",strerror(errno));
    }

	
    time_t start_time, end_time;
	double perc_energy,perc_time;
   	
	
	sigfillset(&set);
	sigdelset(&set,SIGALRM);
	sigdelset(&set,SIGHUP);
	sigdelset(&set,SIGTERM);
	sigdelset(&set,SIGINT);

    #if DB_MYSQL
    verbose(1,"Connecting with EAR DB");
	/*strcpy(my_cluster_conf.database.database,"Report2");*/
    init_db_helper(&my_cluster_conf.database);
    #endif
	
    
	
	time(&end_time);
	start_time=end_time-period_t2;
	result = db_select_acum_energy( start_time, end_time, divisor, use_aggregation);
	fill_periods(result);
	/*
	*
	*	MAIN LOOP
	*			
	*/
	alarm(period_t1);
	while(1){
		// Waiting a for period_t1
		sigsuspend(&set);
		// ALARM RECEIVED
		if (t1_expired){
			t1_expired=0;

			// Compute the period
			time(&end_time);
			start_time=end_time-period_t1;
	
    
	    	result = db_select_acum_energy( start_time, end_time, divisor, use_aggregation);
	    	if (!result){ 
				verbose(2,"No results in that period of time found\n");
	    	}else{ 
				if (result < 0) exit(1);
			}
			verbose(1,"Energy consumed in last %lu seconds %lu %s. Avg power %u %s\n",period_t1,result,unit_energy,result/period_t1,unit_power);
			
	
			new_energy_sample(result);
			#if 0
			/* we can use this approach for some debugging purposses, uncomment this code and comment total_energy_t2=compute_energy_t2() */
			start_time=end_time-period_t2;
    		result2 = db_select_acum_energy( start_time, end_time, divisor, use_aggregation);
			total_energy_t2=result2;
			#endif
			energy_t1=result;
			total_energy_t2=compute_energy_t2();	

			#if 0
			perc_energy=((double)total_energy_t2/(double)energy_budget)*(double)100;
			perc_time=((double)total_samples/(double)aggregate_samples)*(double)100;
			verbose(0,"Percentage over energy budget %.2lf%% (total energy t2 %lu , energy limit %lu)\n",perc_energy,total_energy_t2,energy_budget);
		
			if (perc_time<100.0){	
				if (perc_energy>perc_time){
					verbose(0,"WARNING %.2lf%% of energy vs %.2lf%% of time!!\n",perc_energy,perc_time);
				}
			}
			#endif
			if (!in_action){
			switch(defcon(total_energy_t2,energy_t1,total_nodes)){
			case NO_PROBLEM:
				verbose(2," Safe area. energy budget %.2lf%% \n",perc_energy);
				break;
			case WARNING_3:
				in_action+=my_cluster_conf.eargm.grace_periods;
				verbose(0,"****************************************************************");
				verbose(0,"WARNING... we are close to the maximum energy budget %.2lf%% \n",perc_energy);
				verbose(0,"****************************************************************");
	
				my_warning.level=WARNING_3;
				if (my_cluster_conf.eargm.mode){ // my_cluster_conf.eargm.mode==1 is AUTOMATIC mode
					my_warning.inc_th=eargm_increase_th_all_nodes(WARNING_3);            
				}else{
					my_warning.inc_th=0;
				}
				my_warning.energy_percent=perc_energy;
				send_mail(WARNING_3,perc_energy);
				#if DB_MYSQL	
				db_insert_gm_warning(&my_warning);
				#endif
				report_status(&my_warning);
				break;
			case WARNING_2:
				in_action+=my_cluster_conf.eargm.grace_periods;
				verbose(0,"****************************************************************");
				verbose(0,"WARNING... we are close to the maximum energy budget %.2lf%% \n",perc_energy);
				verbose(0,"****************************************************************");
				my_warning.level=WARNING_2;
				if (my_cluster_conf.eargm.mode){ // my_cluster_conf.eargm.mode==1 is AUTOMATIC mode
					my_warning.new_p_state=eargm_reduce_frequencies_all_nodes(WARNING_2);
					my_warning.inc_th=eargm_increase_th_all_nodes(WARNING_2);            
				}else{
					my_warning.inc_th=0;my_warning.new_p_state=0;
				}
				my_warning.energy_percent=perc_energy;
				send_mail(WARNING_2,perc_energy);
				#if DB_MYSQL	
				db_insert_gm_warning(&my_warning);
				#endif
				report_status(&my_warning);
				break;
			case PANIC:
				in_action+=my_cluster_conf.eargm.grace_periods;
				verbose(0,"****************************************************************");
				verbose(0,"PANIC!... we are close or over the maximum energy budget %.2lf%% \n",perc_energy);
				verbose(0,"****************************************************************");
				my_warning.level=PANIC;
				if (my_cluster_conf.eargm.mode){ // my_cluster_conf.eargm.mode==1 is AUTOMATIC mode
					my_warning.new_p_state=eargm_reduce_frequencies_all_nodes(PANIC);
					my_warning.inc_th=eargm_increase_th_all_nodes(PANIC);            
				}else{
					my_warning.inc_th=0;my_warning.new_p_state=0;
				}
				my_warning.energy_percent=perc_energy;
				send_mail(PANIC,perc_energy);
				#if DB_MYSQL	
				db_insert_gm_warning(&my_warning);
				#endif
				report_status(&my_warning);
				break;
			}
			}else in_action--;
		}// ALARM
		if (must_refill){
			must_refill=0;
    		aggregate_samples=period_t2/period_t1;
    				if ((period_t2%period_t1)!=0){
        				verbose(2,"warning period_t2 is not multiple of period_t1\n");
        				aggregate_samples++;
    		}
			if (energy_consumed!=NULL) free(energy_consumed);
    		energy_consumed=malloc(sizeof(ulong)*aggregate_samples);
		    time(&end_time);
    		start_time=end_time-period_t2;
    		result = db_select_acum_energy( start_time, end_time, divisor, use_aggregation);
    		fill_periods(result);
		}
	}

    #if SYSLOG_MSG
    closelog();
    #endif


    

    exit(1);
}




