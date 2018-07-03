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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/string_enhanced.h>
#include <common/database/db_helper.h>
#include <common/database/mysql_io_functions.h>

static const char *__NAME__ = "db_helper";

db_conf_t *db_config = NULL;


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
        VERBOSE_N(0, "ERROR while writing signature to database.");
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

    if (mysql_batch_insert_periodic_metrics(connection, per_mets, num_mets) < 0)
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

#define SUM_QUERY   "SELECT SUM(dc_energy)/? FROM Report.Periodic_metrics WHERE start_time" \
                    ">= ? AND end_time <= ?"

ulong db_select_acum_energy(int start_time, int end_time, ulong  divisor)
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

