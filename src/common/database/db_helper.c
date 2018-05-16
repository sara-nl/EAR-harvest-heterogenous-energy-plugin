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

char *db_ip = NULL;
char *db_user = NULL;
char *db_pass = NULL;
char *database = "Report";
unsigned int db_port = 0;

int getenv_database()
{
    if (!getenv("DB_IP") || !getenv("DB_USER")) 
    {
        if (readconf_database() != EAR_SUCCESS)
        {
            VERBOSE_N(0, "Erro reading db config file.");
            return EAR_ERROR;
        }
    }

    db_ip = getenv("DB_IP");
    db_user = getenv("DB_USER");
    db_pass = (!getenv("DB_PASS")) ? "" : getenv("DB_PASS");
    database = (!getenv("DB_NAME")) ? database: getenv("DB_NAME");
    db_port = (!getenv("DB_PORT")) ? 0 : atoi(getenv("DB_PORT"));

    return EAR_SUCCESS;
}

int readconf_database()
{
    char conf_file_path[256], config[256];
    char *value;
    FILE *conf_file;
    
    strcpy(conf_file_path, EAR_INSTALL_PATH);
    strcat(conf_file_path, "/etc/sysconf/db.conf");

    conf_file = fopen(conf_file_path, "r");
    if (conf_file == NULL)
    {
        VERBOSE_N(0, "Error opening config file at \"$EAR_INSTALL_PATH/etc/sysconf/db.conf\".\n");
        VERBOSE_N(0, "Filepath: %s\n", conf_file_path);
        return EAR_ERROR;
    }

    while (fgets(config, 256, conf_file) != NULL)
    {
        if (strclean(config, '\n') && (value = strclean(config, '=')))
        {
            if (strlen(config) && strlen(++value))
            {
                strtoup(config);
                if (setenv(config, value, 0) == -1) return EAR_ERROR;
            }

        }

    }
    return EAR_SUCCESS;

}

int db_insert_application(application_t *application)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_ip == NULL || db_user == NULL || db_pass == NULL)
    {
        if (getenv_database() != EAR_SUCCESS)
        {
            mysql_close(connection);
            VERBOSE_N(0, "Error reading configuration file and setting environment variables.");
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, database, db_port, NULL, 0))
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

    if (db_ip == NULL || db_user == NULL || db_pass == NULL)
    {
        if (getenv_database() != EAR_SUCCESS)
        {
            mysql_close(connection);
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, "Report", db_port, NULL, 0))
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

#if SHARED_MEMORY
int db_insert_power_signature(power_signature_t *pow_sig)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_ip == NULL || db_user == NULL || db_pass == NULL)
    {
        if (getenv_database() != EAR_SUCCESS)
        {
            mysql_close(connection);
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, "Report", db_port, NULL, 0))
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

int db_insert_periodic_metric(periodic_metric_t *per_met)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_ip == NULL || db_user == NULL || db_pass == NULL)
    {
        if (getenv_database() != EAR_SUCCESS)
        {
            mysql_close(connection);
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, "Report", db_port, NULL, 0))
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
#endif

int db_insert_ear_event(ear_event_t *ear_ev)
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        VERBOSE_N(0, "ERROR creating MYSQL object.");
        return EAR_ERROR;
    }

    if (db_ip == NULL || db_user == NULL || db_pass == NULL)
    {
        if (getenv_database() != EAR_SUCCESS)
        {
            mysql_close(connection);
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, "Report", db_port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_ear_event(connection, ear_ev) < 0)
    {
        VERBOSE_N(0, "ERROR while writing periodic_metric to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}
