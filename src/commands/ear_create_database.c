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
#include <unistd.h>
#include <termios.h>
#include <mysql/mysql.h>
#include <common/user.h>
#include <common/config.h>
#include <common/database/db_helper.h>
#include <common/types/configuration/cluster_conf.h>

char print_out = 0;
cluster_conf_t my_cluster;

void usage(char *app)
{
	verbose(0, "Usage:%s [options]", app);
    verbose(0, "\t-p\t\tSpecify the password for MySQL's root user.");
    verbose(0, "\t-o\t\tOutputs the commands that would run.");
    verbose(0, "\t-r\t\tRuns the program. If '-o' this option will be override.");
    verbose(0, "\t-h\t\tShows this message.");
	exit(0);
}

void execute_on_error(MYSQL *connection)
{
    verbose(0, "Error: %s", mysql_error(connection)); //error
    return;
    //mysql_close(connection);
    //exit(1);
}

int get_num_indexes(char *table)
{
    init_db_helper(&my_cluster.database);
    char query[256];
    sprintf(query, "SELECT COUNT(1) IndexIsThere FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='%s'", table);
    //db_run_query(query, my_cluster.database.user, my_cluster.database.pass);
    MYSQL_RES *result = db_run_query_result(query);
    int num_indexes;
    if (result == NULL) {
        verbose(0, "Error while retrieving result");
    } else {
        MYSQL_ROW row;
        unsigned int num_fields;
        unsigned int i;

        num_fields = mysql_num_fields(result);
        while ((row = mysql_fetch_row(result)))
        {
            unsigned long *lengths;
            lengths = mysql_fetch_lengths(result);
            for(i = 0; i < num_fields; i++)
                num_indexes = atoi(row[i]);
        }
    }
    return num_indexes;
}

void run_query(MYSQL *connection, char *query)
{
    if (print_out) {
        verbose(0, "%s\n", query);
    } else {
        if (mysql_query(connection, query)) {
            execute_on_error(connection);
        }
    }
}

void create_users(MYSQL *connection, char *db_name, char *db_user, char *db_user_pass, char *eardbd_user, char *eardbd_pass)
{
    char query[256];
    if (strlen(db_user_pass) > 1)
        sprintf(query, "CREATE USER '%s'@'%%' IDENTIFIED BY %s", db_user, db_user_pass);
    else
        sprintf(query, "CREATE USER '%s'@'%%'", db_user);
    run_query(connection, query);

    sprintf(query, "GRANT INSERT ON %s.* TO '%s'@'%%'", db_name, db_user);
    run_query(connection, query);
    
    if (strlen(eardbd_pass) > 1)
        sprintf(query, "CREATE USER '%s'@'%%' IDENTIFIED BY '%s'", eardbd_user, eardbd_pass);
    else
        sprintf(query, "CREATE USER '%s'@'%%'", eardbd_user);
    run_query(connection, query);
    
    sprintf(query, "FLUSH PRIVILEGES");
    run_query(connection, query);
}

void create_db(MYSQL *connection, char *db_name)
{
    char query[256];

    sprintf(query, "CREATE DATABASE IF NOT EXISTS %s", db_name);
    run_query(connection, query);

    sprintf(query, "USE %s", db_name);
    run_query(connection, query);
}

void create_indexes(MYSQL *connection)
{
    char query[256];
    if (get_num_indexes("Periodic_metrics") < 3)
    {
        sprintf(query, "CREATE INDEX idx_end_time ON Periodic_metrics (end_time)");
        verbose(0, "Running query: %s", query);
        run_query(connection, query);

        sprintf(query, "CREATE INDEX idx_job_id ON Periodic_metrics (job_id)");
        verbose(0, "Running query: %s", query);
        run_query(connection, query);
    } else {
        verbose(0, "Periodic_metrics indexes already created, skipping...");
    }

    if (get_num_indexes("Jobs") < 3)
    {
        sprintf(query, "CREATE INDEX idx_user_id ON Jobs (user_id)");
        verbose(0, "Running query: %s", query);
        run_query(connection, query);
    }
    else {
        verbose(0, "Jobs indexes already created, skipping...");
    }
    
}

void create_tables(MYSQL *connection)
{
    char query[1024];
    sprintf(query, "CREATE TABLE IF NOT EXISTS Applications (\
job_id INT unsigned NOT NULL, \
step_id INT unsigned NOT NULL, \
node_id VARCHAR(128), \
signature_id INT unsigned, \
power_signature_id INT unsigned, \
PRIMARY KEY(job_id, step_id, node_id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Loops ( \
event INT unsigned NOT NULL, \
size INT unsigned NOT NULL, \
level INT unsigned NOT NULL, \
job_id INT unsigned NOT NULL, \
step_id INT unsigned NOT NULL, \
node_id VARCHAR(8), \
total_iterations INT unsigned, \
signature_id INT unsigned)");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Jobs (\
id INT unsigned NOT NULL,\
step_id INT unsigned NOT NULL, \
user_id VARCHAR(128),\
app_id VARCHAR(128),\
start_time INT NOT NULL,\
end_time INT NOT NULL,\
start_mpi_time INT NOT NULL,\
end_mpi_time INT NOT NULL,\
policy VARCHAR(256) NOT NULL,\
threshold FLOAT NOT NULL,\
procs INT unsigned NOT NULL,\
job_type SMALLINT unsigned NOT NULL,\
def_f INT unsigned, \
user_acc VARCHAR(256), \
user_group VARCHAR(256), \
e_tag VARCHAR(256), \
PRIMARY KEY(id, step_id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Signatures (\
id INT unsigned NOT NULL AUTO_INCREMENT,\
DC_power FLOAT,\
DRAM_power FLOAT,\
PCK_power FLOAT,\
EDP FLOAT,\
GBS FLOAT,\
TPI FLOAT,\
CPI FLOAT,\
Gflops FLOAT,\
time FLOAT,"
#if !DB_SIMPLE
"FLOPS1 INT unsigned,\
FLOPS2 INT unsigned,\
FLOPS3 INT unsigned,\
FLOPS4 INT unsigned,\
FLOPS5 INT unsigned,\
FLOPS6 INT unsigned,\
FLOPS7 INT unsigned,\
FLOPS8 INT unsigned,\
instructions INT unsigned, \
cycles INT unsigned,"
#endif
"avg_f INT unsigned,\
def_f INT unsigned, \
PRIMARY KEY (id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Periodic_metrics ( \
id INT unsigned NOT NULL AUTO_INCREMENT, \
start_time INT NOT NULL, \
end_time INT NOT NULL, \
DC_energy INT unsigned NOT NULL, \
node_id VARCHAR(256) NOT NULL, \
job_id INT NOT NULL, \
step_id INT NOT NULL, "
#if DEMO
"avg_f INT, "
#endif
                            "PRIMARY KEY (id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Power_signatures (  \
id INT unsigned NOT NULL AUTO_INCREMENT, \
DC_power FLOAT NOT NULL, \
DRAM_power FLOAT NOT NULL, \
PCK_power FLOAT NOT NULL, \
EDP FLOAT NOT NULL, \
max_DC_power FLOAT NOT NULL, \
min_DC_power FLOAT NOT NULL, \
time FLOAT NOT NULL, \
avg_f INT unsigned NOT NULL, \
def_f INT unsigned NOT NULL, \
PRIMARY KEY (id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Events ( \
id INT unsigned NOT NULL AUTO_INCREMENT, \
timestamp INT NOT NULL, \
event_type INT NOT NULL, \
job_id INT NOT NULL, \
step_id INT NOT NULL, \
freq INT unsigned NOT NULL, \
node_id VARCHAR(256), \
PRIMARY KEY (id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Warnings ( \
energy_percent FLOAT, \
warning_level INT UNSIGNED NOT NULL, \
time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
inc_th FLOAT, \
p_state INT, \
PRIMARY KEY (time))");
    run_query(connection, query);
                            

    sprintf(query, "CREATE TABLE IF NOT EXISTS Learning_applications (\
job_id INT unsigned NOT NULL, \
step_id INT unsigned NOT NULL, \
node_id VARCHAR(128), \
signature_id INT unsigned,\
power_signature_id INT unsigned, \
PRIMARY KEY(job_id, step_id, node_id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Learning_jobs (\
id INT unsigned NOT NULL,\
step_id INT unsigned NOT NULL, \
user_id VARCHAR(256),\
app_id VARCHAR(256),\
start_time INT NOT NULL,\
end_time INT NOT NULL,\
start_mpi_time INT NOT NULL,\
end_mpi_time INT NOT NULL,\
policy VARCHAR(256) NOT NULL,\
threshold FLOAT NOT NULL,\
procs INT unsigned NOT NULL,\
job_type SMALLINT unsigned NOT NULL,\
def_f INT unsigned, \
user_acc VARCHAR(256) NOT NULL, \
user_group VARCHAR(256), \
e_tag VARCHAR(256), \
PRIMARY KEY(id, step_id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Periodic_aggregations (\
id INT unsigned NOT NULL AUTO_INCREMENT,\
start_time INT,\
end_time INT,\
DC_energy INT unsigned, \
PRIMARY KEY(id))");
    run_query(connection, query);

    sprintf(query, "CREATE TABLE IF NOT EXISTS Learning_signatures (\
id INT unsigned NOT NULL AUTO_INCREMENT,\
DC_power FLOAT,\
DRAM_power FLOAT,\
PCK_power FLOAT,\
EDP FLOAT,\
GBS FLOAT,\
TPI FLOAT,\
CPI FLOAT,\
Gflops FLOAT,\
time FLOAT,"
#if !DB_SIMPLE
"FLOPS1 INT unsigned,\
FLOPS2 INT unsigned,\
FLOPS3 INT unsigned,\
FLOPS4 INT unsigned,\
FLOPS5 INT unsigned,\
FLOPS6 INT unsigned,\
FLOPS7 INT unsigned,\
FLOPS8 INT unsigned,\
instructions INT unsigned, \
cycles INT unsigned,"
#endif
"avg_f INT unsigned,\
def_f INT unsigned, \
PRIMARY KEY (id))");
    run_query(connection, query);

}


void main(int argc,char *argv[])
{
    int c;
    char passw[256];
    if (argc < 2) usage(argv[0]);
    
    strcpy(passw, "");

    struct termios t;
    while ((c = getopt(argc, argv, "phro")) != -1)
    {
        switch(c)
        {
            case 'h':
               usage(argv[0]);
               break;
            case 'p':
                tcgetattr(STDIN_FILENO, &t);
                t.c_lflag &= ~ECHO;
                tcsetattr(STDIN_FILENO, TCSANOW, &t);

                verbosen(0, "Introduce root's password:");
                fflush(stdout);
                fgets(passw, sizeof(passw), stdin);
                t.c_lflag |= ECHO;
                tcsetattr(STDIN_FILENO, TCSANOW, &t);
                strclean(passw, '\n');
                verbose(0, " ");
                break;
            case 'r':
                break;
            case 'o':
                print_out = 1;
                break;
        }
    }

    MYSQL *connection = mysql_init(NULL); 

    if (connection == NULL)
    {
        verbose(0, "Error creating MYSQL object: %s", mysql_error(connection)); //error
        exit(1);
    }


    //cluster_conf_t my_cluster;
    char ear_path[256];
    char eardbd_path[256];
    char eardbd_user[256];
    char eardbd_pass[256];

    if (get_ear_conf_path(ear_path) == EAR_ERROR)
    {
        verbose(0, "Error getting ear.conf path"); //error
        exit(0);
    }
    
    read_cluster_conf(ear_path, &my_cluster);

	print_database_conf(&my_cluster.database);

    if (get_eardbd_conf_path(eardbd_path) == EAR_ERROR)
    {
        verbose(0, "Error getting eardbd.conf path"); //error
        exit(0);
    }

    if (read_eardbd_conf(eardbd_path, eardbd_user, eardbd_pass) == EAR_ERROR)
    {
        verbose(0, "Error getting eardbd.conf path"); //error
        exit(0);
    }


    if (!print_out)
    {
        if (!mysql_real_connect(connection, my_cluster.database.ip, "root", passw, NULL, my_cluster.database.port, NULL, 0))
        {
            verbose(0, "Error connecting to database: %s", mysql_error(connection)); //error
            free_cluster_conf(&my_cluster);
            exit(0);
        }
    }

    create_db(connection, my_cluster.database.database);
    
    create_tables(connection);

    create_users(connection, my_cluster.database.database, my_cluster.database.user, my_cluster.database.pass, eardbd_user, eardbd_pass);

    create_indexes(connection);

    mysql_close(connection);

    free_cluster_conf(&my_cluster);

    verbose(0, "Database successfully created");

    exit(1);
}
