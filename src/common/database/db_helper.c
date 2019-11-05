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
#include <common/output/verbose.h>
#include <common/string_enhanced.h>
#include <common/database/db_helper.h>
#include <common/database/mysql_io_functions.h>

db_conf_t *db_config = NULL;
int current_step_id = -1;
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
#define LOO_VARS			LOOP_ARGS
#define AGG_VARS			PERIODIC_AGGREGATION_ARGS
#define EVE_VARS			EAR_EVENTS_ARGS
#define IS_SIG_FULL_QUERY   "SELECT COUNT(*) FROM information_schema.columns where TABLE_NAME='Signatures' AND TABLE_SCHEMA='%s'"
#define IS_NODE_FULL_QUERY  "SELECT COUNT(*) FROM information_schema.columns where TABLE_NAME='Periodic_metrics' AND TABLE_SCHEMA='%s'"


#define PAINT(N) \
	verbose(VDBH, "Elements to insert %d", N); \
	verbose(VDBH, "Bulk elements %d", bulk_elms); \
	verbose(VDBH, "Bulk sets %d", bulk_sets)

void init_db_helper(db_conf_t *conf)
{
    db_config = conf;
    char query[256];
    int num_sig_args, num_per_args;

    sprintf(query, IS_SIG_FULL_QUERY, db_config->database);
    num_sig_args = run_query_int_result(query);
    if (num_sig_args == EAR_ERROR)
        set_signature_simple(db_config->report_sig_detail);
    else if (num_sig_args < 20)
        set_signature_simple(0);
    else 
        set_signature_simple(1);


    sprintf(query, IS_NODE_FULL_QUERY, db_config->database);
    num_per_args = run_query_int_result(query);
    if (num_per_args == EAR_ERROR)
        set_node_detail(db_config->report_node_detail);
    else if (num_per_args < 8)
        set_node_detail(0);
    else
        set_node_detail(1);

}


int run_query_int_result(char *query)
{
    MYSQL_RES *result = db_run_query_result(query);
    int num_indexes;
    if (result == NULL) {
        verbose(VDBH, "Error while retrieving result");
        return EAR_ERROR;
    } else {
        MYSQL_ROW row;
        unsigned int num_fields;
        unsigned int i;

        num_fields = mysql_num_fields(result);
        while ((row = mysql_fetch_row(result)))
        {
            mysql_fetch_lengths(result);
            for(i = 0; i < num_fields; i++)
                num_indexes = atoi(row[i]);
        }
    }
    return num_indexes;
}

MYSQL *mysql_create_connection()
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        verbose(VDBH, "ERROR creating MYSQL object.");
        return NULL;
    }

    if (db_config == NULL)
    {
        verbose(VDBH, "Database configuration not initialized.");
        return NULL;
    }

    if (!mysql_real_connect(connection, db_config->ip, db_config->user, db_config->pass, db_config->database, db_config->port, NULL, 0))
    {
        verbose(VDBH, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return NULL;
    }

    return connection;
}

int db_insert_application(application_t *application)
{
    MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_application(connection, application) < 0)
    {
        verbose(VDBH, "ERROR while writing application to database.");
        mysql_close(connection);
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

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    // Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
    {
    	if (mysql_batch_insert_applications(connection, &applications[e], bulk_elms) < 0) {
        	verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
        	return EAR_ERROR;
    	}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_apps)
	{
		if (mysql_batch_insert_applications(connection, &applications[e], num_apps - e) < 0) {
            verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
            return EAR_ERROR;
        }
	}	 

	//
	mysql_close(connection);

    return EAR_SUCCESS;
}

int db_batch_insert_applications_learning(application_t *applications, int num_apps)
{
    int bulk_elms = _BULK_ELMS(_MMAAXX(APP_VARS, PSI_VARS, NSI_VARS, JOB_VARS));
	int bulk_sets = _BULK_SETS(num_apps, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    // Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
    {
    	if (mysql_batch_insert_applications(connection, &applications[e], bulk_elms) < 0) {
        	verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
        	return EAR_ERROR;
    	}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_apps)
	{
		if (mysql_batch_insert_applications(connection, &applications[e], num_apps - e) < 0) {
            verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
            return EAR_ERROR;
        }
	}	 

	//
	mysql_close(connection);

    return EAR_SUCCESS;
}


int db_batch_insert_applications_no_mpi(application_t *applications, int num_apps)
{
	int bulk_elms = _BULK_ELMS(_MMAAXX(APP_VARS, PSI_VARS, NSI_VARS, JOB_VARS));
	int bulk_sets = _BULK_SETS(num_apps, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_applications_no_mpi(connection, &applications[e], bulk_elms) < 0) {
			verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_apps)
	{
		if (mysql_batch_insert_applications_no_mpi(connection, &applications[e], num_apps - e) < 0) {
			verbose(VDBH, "ERROR while batch writing applications to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}

    mysql_close(connection);
    
    return EAR_SUCCESS;

}

int db_insert_loop(loop_t *loop)
{    
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_loop(connection, loop) < 0)
    {
        verbose(VDBH, "ERROR while writing loop signature to database.");
        mysql_close(connection);
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_batch_insert_loops(loop_t *loops, int num_loops)
{
	int bulk_elms = _BULK_ELMS(_MAX(LOO_VARS, NSI_VARS));
	int bulk_sets = _BULK_SETS(num_loops, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_loops(connection, &loops[e], bulk_elms) < 0) {
			verbose(VDBH, "ERROR while batch writing loop signature to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_loops)
	{
		if (mysql_batch_insert_loops(connection, &loops[e], num_loops - e) < 0) {
			verbose(VDBH, "ERROR while batch writing loop signature to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_power_signature(power_signature_t *pow_sig)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_power_signature(connection, pow_sig) < 0)
    {
        verbose(VDBH, "ERROR while writing power_signature to database.");
        mysql_close(connection);
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_periodic_aggregation(periodic_aggregation_t *per_agg)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_periodic_aggregation(connection, per_agg) < 0)
    {
        verbose(VDBH, "ERROR while writing periodic_aggregation to database.");
        mysql_close(connection);
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_periodic_metric(periodic_metric_t *per_met)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_periodic_metric(connection, per_met) < 0)
    {
        verbose(VDBH, "ERROR while writing periodic_metric to database.");
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
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_periodic_metrics(connection, &per_mets[e], bulk_elms) < 0) {
			verbose(VDBH, "ERROR while batch writing periodic metrics to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_mets)
	{
		if (mysql_batch_insert_periodic_metrics(connection, &per_mets[e], num_mets - e) < 0) {
			verbose(VDBH, "ERROR while batch writing periodic metrics to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}

	//
	mysql_close(connection);

    return EAR_SUCCESS;
}

int db_batch_insert_periodic_aggregations(periodic_aggregation_t *per_aggs, int num_aggs)
{
	int bulk_elms = _BULK_ELMS(AGG_VARS);
	int bulk_sets = _BULK_SETS(num_aggs, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_periodic_aggregations(connection, &per_aggs[e], bulk_elms) < 0) {
			verbose(VDBH, "ERROR while batch writing aggregations to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_aggs)
	{
		if (mysql_batch_insert_periodic_aggregations(connection, &per_aggs[e], num_aggs - e) < 0) {
			verbose(VDBH, "ERROR while batch writing aggregations to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}

    mysql_close(connection);

    return EAR_SUCCESS;
}

int db_insert_ear_event(ear_event_t *ear_ev)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_ear_event(connection, ear_ev) < 0)
    {
        verbose(VDBH, "ERROR while writing ear_event to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_batch_insert_ear_event(ear_event_t *ear_evs, int num_events)
{
	int bulk_elms = _BULK_ELMS(EVE_VARS);
	int bulk_sets = _BULK_SETS(num_events, bulk_elms);
	int e, s;

	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

	// Inserting full bulks one by one
	for (e = 0, s = 0; s < bulk_sets; e += bulk_elms, s += 1)
	{
		if (mysql_batch_insert_ear_events(connection, &ear_evs[e], bulk_elms) < 0) {
			verbose(VDBH, "ERROR while batch writing ear_event to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}
	// Inserting the lagging bulk, the incomplete last one
	if (e < num_events)
	{
		if (mysql_batch_insert_ear_events(connection, &ear_evs[e], num_events - e) < 0) {
			verbose(VDBH, "ERROR while batch writing ear_event to database.");
            mysql_close(connection);
			return EAR_ERROR;
		}
	}

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int db_insert_gm_warning(gm_warning_t *warning)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (mysql_insert_gm_warning(connection, warning) < 0)
    {
        verbose(VDBH, "ERROR while writing gm_warning to database.");
        mysql_close(connection);
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}

int stmt_error(MYSQL *connection, MYSQL_STMT *statement)
{
    verbose(VMYSQL, "Error preparing statement (%d): %s\n",
            mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    mysql_close(connection);
    return -1;
}

#define METRICS_SUM_QUERY       "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_metrics WHERE end_time" \
                                ">= ? AND end_time <= ? AND DC_energy <= %d"
#define AGGREGATED_SUM_QUERY    "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_aggregations WHERE end_time"\
                                ">= ? AND end_time <= ?"

int db_select_acum_energy(int start_time, int end_time, ulong divisor, char is_aggregated, uint *last_index, ulong *energy)
{
    char query[256];
    *energy = 0;
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        verbose(VDBH, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (is_aggregated)
    {
        if (mysql_stmt_prepare(statement, AGGREGATED_SUM_QUERY, strlen(AGGREGATED_SUM_QUERY)))
            return stmt_error(connection, statement);
    }
    else
    {
        sprintf(query, METRICS_SUM_QUERY, MAX_ENERGY);
        if (mysql_stmt_prepare(statement, query, strlen(query)))
            return stmt_error(connection, statement);
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
    MYSQL_BIND res_bind[2];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = res_bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    res_bind[0].buffer = energy;
    res_bind[1].buffer = last_index;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(connection, statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(connection, statement);
    if (mysql_stmt_execute(statement)) return stmt_error(connection, statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(connection, statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
    {
        return stmt_error(connection, statement);
    }

    mysql_stmt_close(statement);
    mysql_close(connection);

    return EAR_SUCCESS;

}

#if 0
#define METRICS_SUM_QUERY       "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_metrics WHERE end_time" \
                                ">= ? AND end_time <= ? AND DC_energy <= %d"
#define AGGREGATED_SUM_QUERY    "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_aggregations WHERE end_time"\
                                ">= ? AND end_time <= ?"
#endif

int db_select_acum_energy_nodes(int start_time, int end_time, ulong divisor, uint *last_index, ulong *energy, long num_nodes, char **nodes)
{
    char query[1024];
    int i;
    *energy = 0;
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        verbose(VDBH, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    sprintf(query, METRICS_SUM_QUERY, MAX_ENERGY);
    if (num_nodes > 0)
    {
        strcat(query, " AND node_id IN (");
        for (i = 0; i < num_nodes; i++)
        {
            strcat(query, "'");
            strcat(query, nodes[i]);
            i == num_nodes - 1 ? strcat(query, ")'") : strcat(query, "', ");
        }

    }



    if (mysql_stmt_prepare(statement, query, strlen(query)))
        return stmt_error(connection, statement);

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
    MYSQL_BIND res_bind[2];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = res_bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    res_bind[0].buffer = energy;
    res_bind[1].buffer = last_index;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(connection, statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(connection, statement);
    if (mysql_stmt_execute(statement)) return stmt_error(connection, statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(connection, statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
    {
        return stmt_error(connection, statement);
    }

    mysql_stmt_close(statement);
    mysql_close(connection);

    return EAR_SUCCESS;

}

#define METRICS_ID_SUM_QUERY       "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_metrics WHERE " \
                                "id > %d AND DC_energy <= %d"
#define AGGREGATED_ID_SUM_QUERY    "SELECT SUM(DC_energy)/? DIV 1, MAX(id) FROM Periodic_aggregations WHERE "\
                                "id > %d"

int db_select_acum_energy_idx(ulong divisor, char is_aggregated, uint *last_index, ulong *energy)
{
    char query[256];
    *energy = 0;
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        verbose(VDBH, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (is_aggregated)
    {
        sprintf(query, AGGREGATED_ID_SUM_QUERY, *last_index);
        if (mysql_stmt_prepare(statement, query, strlen(query)))
            return stmt_error(connection, statement);
    }
    else
    {
        sprintf(query, METRICS_ID_SUM_QUERY, MAX_ENERGY, *last_index);
        if (mysql_stmt_prepare(statement, query, strlen(query)))
            return stmt_error(connection, statement);
    }

    //Query parameters binding
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = 1;

    bind[0].buffer = (char *)&divisor;

    //Result parameters
    MYSQL_BIND res_bind[2];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = res_bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    res_bind[0].buffer = energy;
    res_bind[1].buffer = last_index;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(connection, statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(connection, statement);
    if (mysql_stmt_execute(statement)) return stmt_error(connection, statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(connection, statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
    {
        return stmt_error(connection, statement);
    }

    mysql_stmt_close(statement);
    mysql_close(connection);

    return EAR_SUCCESS;

}


int db_read_applications_query(application_t **apps, char *query)
{
    int num_apps = 0;
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

   	num_apps = mysql_retrieve_applications(connection, query, apps, 0);
    
   
  	if (num_apps == EAR_MYSQL_ERROR){
        verbose(VDBH, "Error retrieving information from database (%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
		return num_apps;
    }

    mysql_close(connection);

    return num_apps;
}

int db_run_query(char *query, char *user, char *passw)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        verbose(VDBH, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }

    if (db_config == NULL)
    {
        verbose(VDBH, "Database configuration not initialized.");
		return EAR_MYSQL_ERROR;
    }
    if (user == NULL)
        user = db_config->user;
    if (passw == NULL)
        passw = db_config->pass;

    if (!mysql_real_connect(connection, db_config->ip, user, passw, db_config->database, db_config->port, NULL, 0))
    {
        verbose(VDBH, "Error connecting to the database(%d):%s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
		return EAR_MYSQL_ERROR;
    }

    if (mysql_query(connection, query))
    {
        verbose(VDBH, "Error when executing query(%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        return EAR_MYSQL_ERROR;
    }

    mysql_close(connection);

    return EAR_SUCCESS;
}

MYSQL_RES *db_run_query_result(char *query)
{
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return NULL;
    }

    if (mysql_query(connection, query))
    {
        verbose(VDBH, "Error when executing query(%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        return NULL;
    }

    MYSQL_RES *result;
    result = mysql_store_result(connection);

    mysql_close(connection);

    return result;

}

int db_read_applications(application_t **apps,uint is_learning, int max_apps, char *node_name)
{
    int num_apps = 0;
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }

    if (max_apps < 1)
    {
        verbose(VDBH, "ERROR: querying less than 1 app is not possible (%d requested).\n", max_apps);
        return EAR_ERROR;
    }

    char query[512];
    if (is_learning && node_name != NULL)
    {
/*        sprintf(query,  "SELECT Learning_applications.* FROM Learning_applications INNER JOIN "\
                        "Learning_jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Learning_jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d AND node_id='%s' GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id, node_name); */
        sprintf(query,  "SELECT Learning_applications.* FROM Learning_applications WHERE (job_id > %d AND node_id='%s') OR "\
                        "(job_id = %d AND step_id > %d AND node_id = '%s') ORDER BY job_id LIMIT %d", 
                        current_job_id, node_name, current_job_id, current_step_id, node_name, max_apps);
    }
    else if (is_learning && node_name == NULL)
    {
        sprintf(query,  "SELECT Learning_applications.* FROM Learning_applications INNER JOIN "\
                        "Learning_jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Learning_jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id);
    }
    else if (!is_learning && node_name != NULL)
    {
/*        sprintf(query,  "SELECT Applications.* FROM Applications INNER JOIN "\
                        "Jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d AND node_id='%s' GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id, node_name); */
        sprintf(query,  "SELECT Applications.* FROM Applications INNER JOIN Signatures ON signature_id = Signatures.id WHERE (job_id > %d AND node_id='%s') OR "\
                        "(job_id = %d AND step_id > %d AND node_id = '%s') AND  "\
                        " time > 60 AND DC_power > 100 AND DC_power < 1000 ORDER BY job_id LIMIT %d", 
                        current_job_id, node_name, current_job_id, current_step_id, node_name, max_apps);
    }
    else
    {
        sprintf(query,  "SELECT Applications.* FROM Applications INNER JOIN "\
                        "Jobs ON job_id = id where job_id < (SELECT max(id) FROM (SELECT (id) FROM "\
                        "Jobs WHERE id > %d ORDER BY id asc limit %u) as t1)+1 and "\
                        "job_id > %d GROUP BY job_id, step_id", current_job_id, max_apps, current_job_id);
    }


   	num_apps = mysql_retrieve_applications(connection, query, apps, is_learning);
   
  	if (num_apps == EAR_MYSQL_ERROR){
        verbose(VDBH, "Error retrieving information from database (%d): %s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
		return num_apps;
    }
    if (num_apps > 0)
    {
        current_job_id = (*apps)[num_apps - 1].job.id;
        current_step_id = (*apps)[num_apps - 1].job.step_id;
    }
    else if (num_apps == 0)
        db_reset_counters();
    else
        verbose(VDBH, "EAR's mysql internal error: %d\n", num_apps);
    mysql_close(connection);
	return num_apps;
}

void db_reset_counters()
{
    current_job_id = 0;
    current_step_id = -1;
}

#define LEARNING_APPS_QUERY     "SELECT COUNT(*) FROM Learning_applications WHERE node_id = '%s'"
//#define LEARNING_APPS_ALL_QUERY "SELECT COUNT(*) FROM Learning_applications"
#define LEARNING_APPS_ALL_QUERY "SELECT COUNT(*) FROM (SELECT * FROM Learning_applications GROUP BY job_id, step_id) AS t1"
#define APPS_QUERY              "SELECT COUNT(*) FROM Applications INNER JOIN Signatures ON signature_id = Signatures.id WHERE node_id = '%s' AND "\
                                "time > 60 AND DC_power > 100 AND DC_power < 1000"
#define APPS_ALL_QUERY          "SELECT COUNT(*) FROM (SELECT * FROM Applications GROUP BY job_id, step_id) AS t1"
//#define APPS_ALL_QUERY          "SELECT COUNT(*) FROM Applications"

ulong get_num_applications(char is_learning, char *node_name)
{
    
	MYSQL *connection = mysql_create_connection();

    if (connection == NULL) {
        return EAR_ERROR;
    }
    char query[256];

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        verbose(VDBH, "Error creating statement (%d): %s\n", mysql_errno(connection),
                mysql_error(connection));
        mysql_close(connection);
        return -1;
    }

    if (is_learning && node_name != NULL)
        sprintf(query, LEARNING_APPS_QUERY, node_name);
    else if (!is_learning && node_name != NULL)
        sprintf(query, APPS_QUERY, node_name);
    else if (is_learning && node_name == NULL)
        sprintf(query, LEARNING_APPS_ALL_QUERY);
    else if (!is_learning && node_name == NULL)
        sprintf(query, APPS_ALL_QUERY);


    if (mysql_stmt_prepare(statement, query, strlen(query)))
                                      return mysql_statement_error(statement);
    //Result parameters
    MYSQL_BIND res_bind[1];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    ulong result = 0;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(connection, statement);
    if (mysql_stmt_execute(statement)) return stmt_error(connection, statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(connection, statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    mysql_stmt_close(statement);
    mysql_close(connection);

    return result;

}

