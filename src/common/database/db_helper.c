/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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


#include <mysql.h>
#include <stdio.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/database/db_helper.h>
#include <common/database/mysql_io_functions.h>

static char *db_ip = NULL;
static char *db_user = NULL;
static char *db_pass = NULL;
static char *database = "Report";
static unsigned int db_port = 0;


int getenv_database()
{
    
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
            return EAR_ERROR;
        }
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, "Report", db_port, NULL, 0))
    {
        VERBOSE_N(0, "ERROR connecting to the database: %s", mysql_error(connection));
        mysql_close(connection);
        return EAR_ERROR;
    }

    if (mysql_insert_application(connection, application) < 0)
    {
        VERBOSE_N(0, "ERROR while writng signature to database.");
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
        VERBOSE_N(0, "ERROR while writng loop signature to database.");
        return EAR_ERROR;
    }

    mysql_close(connection);
    
    return EAR_SUCCESS;
}
