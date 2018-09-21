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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mysql/mysql.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/string_enhanced.h>
#include <common/database/db_helper.h>
#include <common/database/mysql_io_functions.h>

static const char *__NAME__ = "db_helper";
db_conf_t *db_config = NULL;
int current_step_id = 0;
int current_job_id = 0;

#define _MAX(X,Y)			(X > Y ? X : Y)
#define _MMAAXX(W,X,Y,Z) 	(_MAX(W,X) > _MAX(Y,Z) ? _MAX(W,X) : _MAX(Y,Z))
#define _BULK_ELMS(V)		USHRT_MAX / V
#define _BULK_SETS(T,V)		T / V
#define APP_VARS	        APPLICATION_ARGS
#define PSI_VARS	        POWER_SIGNATURE_ARGS
#define NSI_VARS	        SIGNATURE_ARGS
#define JOB_VARS	        JOB_ARGS
#define PER_VARS	        PERIODIC_METRIC_ARGS

void init_db_helper(db_conf_t *conf)
{
    db_config = conf;
}

int db_insert_application(application_t *application)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_application(connection, application) < 0)
    {
        VERBOSE_N(0, "ERROR while writing application to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;

}

int db_batch_insert_applications(application_t *applications, int num_apps)
{
    int bulk_elms = _BULK_ELMS(_MMAAXX(APP_VARS, PSI_VARS, NSI_VARS, JOB_VARS));
	int bulk_sets = _BULK_SETS(num_apps, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_init(NULL);

    if (connection == NULL) {   
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL) {   
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }
        
    if (!mysql_real_connect(connection, db_config->ip, db_config->user,
            db_config->pass, db_config->database, db_config->port, NULL, 0)) 
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    // Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
    {
    	if (mysql_batch_insert_applications(connection, &applications[e], bulk_elms) < 0) {
        	VERBOSE_N(0, "ERROR while batch writing applications to database.");
        	return EAR_ERROR;
    	}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_apps)
	{
		if (mysql_batch_insert_applications(connection, &applications[e], num_apps - e) < 0) {
            VERBOSE_N(0, "ERROR while batch writing applications to database.");
            return EAR_ERROR;
        }
	}	 

	//
	mysql_close(connection);

    return EAR_SUCCESS;
}

int db_batch_insert_applications_no_mpi(application_t *applications, int num_apps)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_batch_insert_applications_no_mpi(connection, applications, num_apps) < 0)
    {
        VERBOSE_N(0, "ERROR while batch writing applications to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;

}

int db_insert_loop(loop_t *loop)
{    
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_loop(connection, loop) < 0)
    {
        VERBOSE_N(0, "ERROR while writing loop signature to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_batch_insert_loops(loop_t *loops, int num_loops)
{    
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_batch_insert_loops(connection, loops, num_loops) < 0)
    {
        VERBOSE_N(0, "ERROR while batch writing loop signature to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_power_signature(power_signature_t *pow_sig)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }
    
    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_power_signature(connection, pow_sig) < 0)
    {
        VERBOSE_N(0, "ERROR while writing power_signature to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_periodic_aggregation(periodic_aggregation_t *per_agg)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }


    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_periodic_aggregation(connection, per_agg) < 0)
    {
        VERBOSE_N(0, "ERROR while writing periodic_aggregation to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_periodic_metric(periodic_metric_t *per_met)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_periodic_metric(connection, per_met) < 0)
    {
        VERBOSE_N(0, "ERROR while writing periodic_metric to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_batch_insert_periodic_metrics(periodic_metric_t *per_mets, int num_mets)
{
	int bulk_elms = _BULK_ELMS(PER_VARS);
	int bulk_sets = _BULK_SETS(num_mets, bulk_elms);
	int e, s;

	//
	MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user,
			db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_periodic_metrics(connection, &per_mets[e], bulk_elms) < 0) {
			VERBOSE_N(0, "ERROR while batch writing periodic metrics to database.");
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_mets)
	{
		if (mysql_batch_insert_periodic_metrics(connection, &per_mets[e], num_mets - e) < 0) {
			VERBOSE_N(0, "ERROR while batch writing periodic metrics to database.");
			return EAR_ERROR;
		}
	}

	//
	mysql_close(connection);

    return EAR_SUCCESS;
}

int db_batch_insert_periodic_aggregations(periodic_aggregation_t *per_aggs, int num_aggs)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_batch_insert_periodic_aggregations(connection, per_aggs, num_aggs) < 0)
    {
        VERBOSE_N(0, "ERROR while batch writing periodic metrics to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);

    return EAR_SUCCESS;
}

int db_insert_ear_event(ear_event_t *ear_ev)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_ear_event(connection, ear_ev) < 0)
    {
        VERBOSE_N(0, "ERROR while writing ear_event to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_batch_insert_ear_event(ear_event_t *ear_evs, int num_events)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_batch_insert_ear_events(connection, ear_evs, num_events) < 0)
    {
        VERBOSE_N(0, "ERROR while batch writing ear_event to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_gm_warning(gm_warning_t *warning)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_gm_warning(connection, warning) < 0)
    {
        VERBOSE_N(0, "ERROR while writing gm_warning to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

ulong stmt_error(MYSQL_STMT *statement)
{
    fprintf(stderr, "Error preparing statement (%d): %s\n",
            mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    return -1;
}

#define METRICS_SUM_QUERY       "SELECT SUM(DC_energy)/? FROM Periodic_metrics WHERE start_time" \
                                ">= ? AND end_time <= ?"
#define AGGREGATED_SUM_QUERY    "SELECT SUM(DC_energy)/? FROM Periodic_aggregations WHERE start_time"\
                                ">= ? AND end_time <= ?"

ulong db_select_acum_energy(int start_time, int end_time, ulong  divisor, char is_aggregated)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }


    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        VERBOSE_N(0, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        return -1;
    }

    if (is_aggregated)
    {
        if (mysql_stmt_prepare(statement, AGGREGATED_SUM_QUERY, strlen(AGGREGATED_SUM_QUERY)))
                                                return stmt_error(statement);
    }
    else
    {
        if (mysql_stmt_prepare(statement, METRICS_SUM_QUERY, strlen(METRICS_SUM_QUERY)))
                                                return stmt_error(statement);
    }

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
    ulong result = 0;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(statement);
    if (mysql_stmt_execute(statement)) return stmt_error(statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    mysql_stmt_close(statement);
    mysql_close(connection);

    return result;


}


int db_read_applications(application_t **apps,uint is_learning, int max_apps, char *node_name)
{
    int num_apps = 0;
    MYSQL *connection = mysql_init(NULL);

    if (max_apps < 1)
    {
        fprintf(stderr, "ERROR: querying less than 1 app is not possible (%d requested).\n", max_apps);
        return EAR_ERROR;
    }

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }
    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
		return num_apps;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "Error connecting to the database(%d):%s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
		return num_apps;
    }


    char query[512];
    if (is_learning && node_name != NULL)
        sprintf(query,  "SELECT Learning_applications.* FROM Learning_applications INNER JOIN "\
                        "Learning_jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Learning_jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d AND node_id='%s' GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id, node_name);
    else if (is_learning && node_name == NULL)
        sprintf(query,  "SELECT Learning_applications.* FROM Learning_applications INNER JOIN "\
                        "Learning_jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Learning_jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id);
    else if (!is_learning && node_name != NULL)
        sprintf(query,  "SELECT Applications.* FROM Applications INNER JOIN "\
                        "Jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d AND node_id='%s' GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id, node_name);
    else
        sprintf(query,  "SELECT Applications.* FROM Applications INNER JOIN "\
                        "Jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id);

   	num_apps = mysql_retrieve_applications(connection, query, apps, is_learning);
   
  	if (num_apps == EAR_MYSQL_ERROR){
        VERBOSE_N(0, "Error retrieving information from database (%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
		return num_apps;
    }
    if (num_apps > 0)
        current_job_id = (*apps)[num_apps - 1].job.id;
    mysql_close(connection);
	return num_apps;
}

#define LEARNING_APPS_QUERY "SELECT COUNT(*) FROM Learning_applications WHERE node_id = '%s'"
#define APPS_QUERY          "SELECT COUNT(*) FROM Applications WHERE node_id = '%s'"

ulong get_num_applications(char is_learning, char *node_name)
{
    
    MYSQL *connection = mysql_init(NULL);
    char query[256];
    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_config == NULL)
    {
        VERBOSE_N(0, "Database configuration not initialized.");
        return EAR_ERROR;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }



    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        VERBOSE_N(0, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        return -1;
    }

    if (is_learning)
        sprintf(query, LEARNING_APPS_QUERY, node_name);
    else
        sprintf(query, APPS_QUERY, node_name);

    if (mysql_stmt_prepare(statement, query, strlen(query)))
                                      return mysql_statement_error(statement);
    //Result parameters
    MYSQL_BIND res_bind[1];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    ulong result = 0;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(statement);
    if (mysql_stmt_execute(statement)) return stmt_error(statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    mysql_stmt_close(statement);
    mysql_close(connection);

    return result;

}

