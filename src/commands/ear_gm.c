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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <common/config.h>

typedef unsigned int uint;
#if DB_MYSQL
#include <mysql/mysql.h>
#define SUM_QUERY   "SELECT SUM(dc_energy)/? FROM Report.Periodic_metrics WHERE start_time" \
                    ">= ? AND end_time <= ?"
#endif


void usage(char *app)
{
	printf("Usage: %s period_t1 (in secs) period_t2 (in seconds) max_energy (in Joules) \n", app);
	exit(1);
}

#if DB_MYSQL

long long stmt_error(MYSQL_STMT *statement)
{
    fprintf(stderr, "Error preparing statement (%d): %s\n", 
            mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    return -1;
}

long long get_sum(MYSQL *connection, int start_time, int end_time, unsigned long long divisor)
{

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        fprintf(stderr, "Error creating statement (%d): %s\n", mysql_errno(connection), 
                mysql_error(connection));
        return -1;
    }
    
    if (mysql_stmt_prepare(statement, SUM_QUERY, strlen(SUM_QUERY)))
                                                return stmt_error(statement);


    //Query parameters binding
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = 1;

    bind[1].buffer_type = bind[2].buffer_type = MYSQL_TYPE_LONG;

    bind[0].buffer = (char *)&divisor;
    bind[1].buffer = (char *)&start_time;
    bind[2].buffer = (char *)&end_time;


    //Result parameters
    MYSQL_BIND res_bind[1];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    long long result = 0;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(statement);
    if (mysql_stmt_execute(statement)) return stmt_error(statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    mysql_stmt_close(statement);
    return result;


}
#endif

static void my_signals_function(int s)
{
	if (s==SIGALRM)	return;
	exit(0);
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
        fprintf(stderr,"eargmd: setting signal mask (%s)\n",strerror(errno));
        exit(1);
    }

    my_action.sa_handler=my_signals_function;
    sigemptyset(&my_action.sa_mask);
    my_action.sa_flags=0;
    if (sigaction(SIGHUP,&my_action,NULL)<0){
        fprintf(stderr,"eargmd: sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGTERM,&my_action,NULL)<0){
        fprintf(stderr,"eargmd: sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGINT,&my_action,NULL)<0){
        fprintf(stderr,"eargmd: sigaction for SIGINT (%s)\n",strerror(errno));
        exit(1);
    }
    if (sigaction(SIGALRM,&my_action,NULL)<0){
        fprintf(stderr,"eargmd: sigaction for SIGALRM (%s)\n",strerror(errno));
        exit(1);
    }

}

uint current_sample=0,total_samples=0;
long long *energy_consumed;
long long energy_budget;
uint aggregate_samples;
void new_energy_sample(long long result)
{
	energy_consumed[current_sample]=result;
	current_sample=(current_sample+1)%aggregate_samples;
	if (total_samples<aggregate_samples) total_samples++;
}

long long compute_energy_t2()
{
	long long energy=0;
	uint i;
	int limit=aggregate_samples;
	if (total_samples<aggregate_samples) limit=total_samples;
	for (i=0;i<limit;i++){
		energy+=energy_consumed[i];
	}
	return energy;
}

#define DEFCON_L3 85.0
#define DEFCON_L2 95.0


uint defcon(double perc)
{
	if (perc<DEFCON_L3) return 3;
	if ((perc>=DEFCON_L3) && (perc<DEFCON_L2))	return 2;
	return 1;
}


void main(int argc,char *argv[])
{
	uint period_t1,period_t2;
	sigset_t set;
	unsigned long long divisor = 1000;
	long long total_energy_t2;

    if (argc !=4) usage(argv[0]);
	period_t1=atoi(argv[1]);
	period_t2=atoi(argv[2]);
	energy_budget=atoll(argv[3]);
	if ((period_t1<=0) || (period_t2<=0) || (energy_budget<=0)) usage(argv[0]);

	aggregate_samples=period_t2/period_t1;
	if ((period_t2%period_t1)!=0){
		fprintf(stderr,"eargmd: warning period_t2 is not multiple of period_t1\n");
		aggregate_samples++;
	}

	energy_consumed=malloc(sizeof(long long)*aggregate_samples);

	catch_signals();
	

#if DB_MYSQL
	// This section must be configurable
    char *db_ip = "127.0.0.1";
    char *db_user = "root";
    char *db_pass = "";
    

    fprintf(stdout,"eargmd: Using user: %s\n", db_user);
    time_t start_time, end_time;
	double perc_energy,perc_time;
   	
	
	sigfillset(&set);
	sigdelset(&set,SIGALRM);
	sigdelset(&set,SIGHUP);
	sigdelset(&set,SIGTERM);
	sigdelset(&set,SIGINT);

	
    
    MYSQL *connection = mysql_init(NULL);
    if (!connection)
    {
        fprintf(stderr, "Error creating MYSQL object\n");
        exit(1);
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, NULL, 0, NULL, 0))
    {
        fprintf(stderr, "Error connecting to the database (%d) :%s\n",
                mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(1);
    }
	while(1){
		// Waiting a for period_t1
		alarm(period_t1);
		sigsuspend(&set);

		// Compute the period
		time(&end_time);
		start_time=end_time-period_t1;

		#if 0
		print_time("start_time",&start_time);
		fprintf(stdout,"\n");
		print_time("end_time",&end_time);
		fprintf(stdout,"\n");
		#endif
    
	    long long result = get_sum(connection, start_time, end_time, divisor);
	    if (!result) fprintf(stdout,"No results in that period of time found\n");
	    else if (result < 0) exit(1);

		new_energy_sample(result);
		total_energy_t2=compute_energy_t2();	
		perc_energy=((double)total_energy_t2/(double)energy_budget)*(double)100;
		perc_time=((double)total_samples/(double)aggregate_samples)*(double)100;
	    //fprintf(stdout,"Total energy spent int last period_t1: %lli\n", result);
	    //fprintf(stdout,"Total energy spent int last period_t2: %lli\n", total_energy_t2);
		fprintf(stdout,"Percentage over energy budget %.2lf%% \n",perc_energy);
		fprintf(stdout,"Percentage over the period_t2 %.2lf%% \n",perc_time);
	
		if (perc_time<100.0){	
			if (perc_energy>perc_time){
				fprintf(stderr,"eargmd: WARNING %.2lf%% of energy vs %.2lf%% of time!!\n",perc_energy,perc_time);
			}
		}
		switch(defcon(perc_energy)){
		case 3:
			fprintf(stderr,"eargmd: Safe area. energy budget %.2lf%% \n",perc_energy);
			break;
		case 2:
			fprintf(stderr,"eargmd: WARNING... we are close to the maximum energy budget %.2lf%% \n",perc_energy);
			break;
		case 1:
			fprintf(stderr,"eargmd: PANIC!... we are close or over the maximum energy budget %.2lf%% \n",perc_energy);
			break;
		}
	}

    mysql_close(connection);
    
    

#endif

    exit(1);
}




