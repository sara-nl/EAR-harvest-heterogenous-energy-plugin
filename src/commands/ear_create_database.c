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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <common/config.h>
#include <common/types/configuration/cluster_conf.h>

int EAR_VERBOSE_LEVEL = 1;

void usage(char *app)
{
	printf("Usage:%s MySQL_ip\n",app);
	exit(1);
}

void execute_on_error(MYSQL *connection)
{
    fprintf(stderr, "Error: %s\n", mysql_error(connection));
    mysql_close(connection);
    exit(1);
}

void create_user(MYSQL *connection, char *db_name, char *db_user)
{
    char query[256];
    sprintf(query, "GRANT INSERT, SELECT ON %s.* TO %s", db_name, db_user);
    if (mysql_query(connection, query)) execute_on_error(connection);
}

void create_db(MYSQL *connection, char *db_name)
{
    char query[256];

    sprintf(query, "CREATE DATABASE IF NOT EXISTS %s", db_name);
    if (mysql_query(connection, query)) execute_on_error(connection);

    sprintf(query, "USE %s", db_name);
    if (mysql_query(connection, query)) execute_on_error(connection);
}

void create_tables(MYSQL *connection)
{

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Applications (\
                            job_id INT unsigned NOT NULL, \
                            step_id INT unsigned NOT NULL, \
                            node_id VARCHAR(256), \
                            signature_id INT unsigned, \
                            power_signature_id INT unsigned, \
                            PRIMARY KEY(job_id, step_id, node_id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Loops ( \
                            event INT unsigned NOT NULL, \
                            size INT unsigned NOT NULL, \
                            level INT unsigned NOT NULL, \
                            job_id INT unsigned NOT NULL, \
                            step_id INT unsigned NOT NULL, \
                            node_id VARCHAR(8), \
                            total_iterations INT unsigned, \
                            signature_id INT unsigned)")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Jobs (\
                            id INT unsigned NOT NULL,\
                            step_id INT unsigned NOT NULL, \
                            user_id VARCHAR(256),\
                            app_id VARCHAR(256),\
                            start_time INT NOT NULL,\
                            end_time INT NOT NULL,\
                            start_mpi_time INT NOT NULL,\
                            end_mpi_time INT NOT NULL,\
                            policy VARCHAR(256) NOT NULL,\
                            threshold DOUBLE NOT NULL,\
                            procs INT unsigned NOT NULL,\
                            job_type SMALLINT unsigned NOT NULL,\
                            def_f INT unsigned, \
                            user_acc VARCHAR(256), \
                            user_group VARCHAR(256), \
                            e_tag VARCHAR(256), \
                            PRIMARY KEY(id, step_id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Signatures (\
                            id INT unsigned NOT NULL AUTO_INCREMENT,\
                            DC_power DOUBLE,\
                            DRAM_power DOUBLE,\
                            PCK_power DOUBLE,\
                            EDP DOUBLE,\
                            GBS DOUBLE,\
                            TPI DOUBLE,\
                            CPI DOUBLE,\
                            Gflops DOUBLE,\
                            time DOUBLE,"
#if !DB_SIMPLE
                            "FLOPS1 BIGINT unsigned,\
                            FLOPS2 BIGINT unsigned,\
                            FLOPS3 BIGINT unsigned,\
                            FLOPS4 BIGINT unsigned,\
                            FLOPS5 BIGINT unsigned,\
                            FLOPS6 BIGINT unsigned,\
                            FLOPS7 BIGINT unsigned,\
                            FLOPS8 BIGINT unsigned,\
                            instructions BIGINT unsigned, \
                            cycles BIGINT unsigned,"
#endif
                            "avg_f BIGINT unsigned,\
                            def_f BIGINT unsigned, \
                            PRIMARY KEY (id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Periodic_metrics ( \
                            id INT unsigned NOT NULL AUTO_INCREMENT, \
                            start_time BIGINT NOT NULL, \
                            end_time BIGINT NOT NULL, \
                            DC_energy BIGINT unsigned NOT NULL, \
                            node_id VARCHAR(256) NOT NULL, \
                            job_id BIGINT NOT NULL, \
                            step_id BIGINT NOT NULL, \
                            PRIMARY KEY (id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Power_signatures (  \
                            id INT unsigned NOT NULL AUTO_INCREMENT, \
                            DC_power DOUBLE NOT NULL, \
                            DRAM_power DOUBLE NOT NULL, \
                            PCK_power DOUBLE NOT NULL, \
                            EDP DOUBLE NOT NULL, \
                            max_DC_power DOUBLE NOT NULL, \
                            min_DC_power DOUBLE NOT NULL, \
                            time DOUBLE NOT NULL, \
                            avg_f BIGINT unsigned NOT NULL, \
                            def_f BIGINT unsigned NOT NULL, \
                            PRIMARY KEY (id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Events ( \
                            id INT unsigned NOT NULL AUTO_INCREMENT, \
                            timestamp INT NOT NULL, \
                            event_type INT NOT NULL, \
                            job_id INT NOT NULL, \
                            step_id INT NOT NULL, \
                            freq BIGINT unsigned NOT NULL, \
                            node_id VARCHAR(256), \
                            PRIMARY KEY (id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Warnings ( \
                            energy_percent DOUBLE, \
                            warning_level INT UNSIGNED NOT NULL, \
                            time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
                            inc_th DOUBLE, \
                            p_state INT, \
                            PRIMARY KEY (time))")) execute_on_error(connection);
                            

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Learning_applications (\
                            job_id INT unsigned NOT NULL, \
                            step_id INT unsigned NOT NULL, \
                            node_id VARCHAR(256), \
                            signature_id INT unsigned,\
                            power_signature_id INT unsigned, \
                            PRIMARY KEY(job_id, step_id, node_id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Learning_jobs (\
                            id INT unsigned NOT NULL,\
                            step_id INT unsigned NOT NULL, \
                            user_id VARCHAR(256),\
                            app_id VARCHAR(256),\
                            start_time INT NOT NULL,\
                            end_time INT NOT NULL,\
                            start_mpi_time INT NOT NULL,\
                            end_mpi_time INT NOT NULL,\
                            policy VARCHAR(256) NOT NULL,\
                            threshold DOUBLE NOT NULL,\
                            procs INT unsigned NOT NULL,\
                            job_type SMALLINT unsigned NOT NULL,\
                            def_f INT unsigned, \
                            user_acc VARCHAR(256) NOT NULL, \
                            PRIMARY KEY(id, step_id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Periodic_aggregations (\
                            id INT unsigned NOT NULL AUTO_INCREMENT,\
                            start_time BIGINT,\
                            end_time BIGINT,\
                            DC_energy BIGINT unsigned, \
                            PRIMARY KEY(id))")) execute_on_error(connection);

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Learning_signatures (\
                            id INT unsigned NOT NULL AUTO_INCREMENT,\
                            DC_power DOUBLE,\
                            max_DC_power DOUBLE, \
                            min_DC_power DOUBLE, \
                            DRAM_power DOUBLE,\
                            PCK_power DOUBLE,\
                            EDP DOUBLE,\
                            GBS DOUBLE,\
                            TPI DOUBLE,\
                            CPI DOUBLE,\
                            Gflops DOUBLE,\
                            time DOUBLE,"
#if !DB_SIMPLE
                            "FLOPS1 BIGINT unsigned,\
                            FLOPS2 BIGINT unsigned,\
                            FLOPS3 BIGINT unsigned,\
                            FLOPS4 BIGINT unsigned,\
                            FLOPS5 BIGINT unsigned,\
                            FLOPS6 BIGINT unsigned,\
                            FLOPS7 BIGINT unsigned,\
                            FLOPS8 BIGINT unsigned,\
                            instructions BIGINT unsigned, \
                            cycles BIGINT unsigned,"
#endif
                            "avg_f BIGINT unsigned,\
                            def_f BIGINT unsigned, \
                            PRIMARY KEY (id))")) execute_on_error(connection);

}

void main(int argc,char *argv[])
{
//    if (argc != 2) usage(argv[0]);
	
    MYSQL *connection = mysql_init(NULL); 

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }


    cluster_conf_t my_cluster;
    char ear_path[256];
    if (get_ear_conf_path(ear_path) == EAR_ERROR)
    {
        fprintf(stderr, "Error getting ear.conf path\n");
        exit(0);
    }
    read_cluster_conf(ear_path, &my_cluster);

    mysql_real_connect(connection, my_cluster.database.ip, "root", "", NULL, 0, NULL, 0);

    create_db(connection, my_cluster.database.database);

    create_user(connection, my_cluster.database.database, my_cluster.database.user);
    
    create_tables(connection);

    mysql_close(connection);

    printf("Database successfully created\n");

    exit(1);
}
