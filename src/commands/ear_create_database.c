#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if DB_MYSQL
#include <mysql.h>
#endif

void usage(char *app)
{
	printf("Usage:%s MySQL_ip\n",app);
	exit(1);
}

#if DB_MYSQL
void execute_on_error(MYSQL *connection)
{
    fprintf(stderr, "Error: %s\n", mysql_error(connection));
    mysql_close(connection);
    exit(1);
}

void create_db(MYSQL *connection)
{
    if (mysql_query(connection, "CREATE DATABASE IF NOT EXISTS Report")) execute_on_error(connection);

    if (mysql_query(connection, "USE Report")) execute_on_error(connection);
}

void create_tables(MYSQL *connection)
{

    if (mysql_query(connection, "CREATE TABLE IF NOT EXISTS Applications (\
                            job_id INT unsigned NOT NULL, \
                            step_id INT unsigned NOT NULL, \
                            node_id VARCHAR(8), \
                            signature_id INT unsigned,\
                            PRIMARY KEY(job_id, step_id))")) execute_on_error(connection);

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
                            time DOUBLE,\
                            FLOPS1 BIGINT unsigned,\
                            FLOPS2 BIGINT unsigned,\
                            FLOPS3 BIGINT unsigned,\
                            FLOPS4 BIGINT unsigned,\
                            FLOPS5 BIGINT unsigned,\
                            FLOPS6 BIGINT unsigned,\
                            FLOPS7 BIGINT unsigned,\
                            FLOPS8 BIGINT unsigned,\
                            L1_misses BIGINT unsigned,\
                            L2_misses BIGINT unsigned,\
                            L3_misses BIGINT unsigned,\
                            instructions BIGINT unsigned, \
                            cycles BIGINT unsigned,\
                            avg_f INT unsigned,\
                            def_f INT unsigned, \
                            PRIMARY KEY (id))")) execute_on_error(connection);
}
#endif 

void main(int argc,char *argv[])
{
	if (argc != 2) usage(argv[0]);
#if DB_MYSQL
	
    MYSQL *connection = mysql_init(NULL); 

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }

    mysql_real_connect(connection, argv[1], "root", "", NULL, 0, NULL, 0);

    create_db(connection);

    create_tables(connection);

    mysql_close(connection);

	exit(1);
#endif	
}
