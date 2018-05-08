#define _XOPEN_SOURCE 700 //to get rid of the warning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <common/config.h>

#if DB_MYSQL
#include <mysql.h>
#define SUM_QUERY   "SELECT SUM(dc_energy)/? FROM Report.Idle_energy WHERE start_time" \
                    ">= ? AND end_time <= ?"
#endif

void usage(char *app)
{
	printf("Usage: %s start_time end_time [options]\n", app);
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


void main(int argc,char *argv[])
{
    if (argc < 3) usage(argv[0]);

#if DB_MYSQL
    char *db_ip = "127.0.0.1";
    char *db_user = "root";
    char *db_pass = "";
    

    unsigned long long divisor = 1;
    int i;
    for (i = 3; i < argc -1; i++)
    {
        if (!strcmp(argv[i], "-u"))
            divisor = (atoi(argv[i+1]) > 0) ? atoi(argv[i+1]) : divisor;
    }

    struct tm *start={0}, *end={0}; 
    time_t start_time, end_time;
    
    start = getdate(argv[1]);
    if (start == NULL)
    {
     //   printf("getdate error %d\n", getdate_err);
        if (getdate_err == 7) printf("Unsupported date input.\n");
        exit(1);
    }
    start_time = mktime(start);
   
    end = getdate(argv[2]);
    end_time = mktime(end);
    
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
    
    long long result = get_sum(connection, start_time, end_time, divisor);
    
    printf("Total energy spent: %lli\n", result);
    
    mysql_close(connection);

#endif

    exit(1);
}




