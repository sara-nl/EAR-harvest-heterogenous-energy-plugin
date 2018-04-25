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

#include <string.h>
#include <stdio.h>
#include <common/types/signature.h>
#include <common/math_operations.h>


void copy_signature(signature_t *destiny, signature_t *source)
{
    memcpy(destiny, source, sizeof(signature_t));
}

void init_signature(signature_t *sig)
{
    memset(sig, 0, sizeof(signature_t));
}

uint are_equal(signature_t *sig1, signature_t *sig2, double th)
{
    if (!equal_with_th(sig1->DC_power, sig2->DC_power, th)) return 0;
    if (!equal_with_th(sig1->DRAM_power, sig2->DRAM_power, th)) return 0;
    if (!equal_with_th(sig1->PCK_power, sig2->PCK_power, th)) return 0;
    if (!equal_with_th(sig1->EDP, sig2->EDP, th)) return 0;    
    if (!equal_with_th(sig1->GBS, sig2->GBS, th)) return 0;    
    if (!equal_with_th(sig1->TPI, sig2->TPI, th)) return 0;    
    if (!equal_with_th(sig1->CPI, sig2->CPI, th)) return 0;    
    if (!equal_with_th(sig1->Gflops, sig2->Gflops, th)) return 0;    
    return 1;
}

void print_signature_fd(int fd, signature_t *sig)
{
    /* print order: AVG.FREQ;DEF.FREQ;TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;
       PCK-POWER;CYCLES;INSTRUCTIONS;L1_MISSES;L2_MISSES;L3_MISSES;GFLOPS;
       SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;DP_512; */
    int i;
    
	dprintf(fd, "%lu;%lu;", sig->avg_f, sig->def_f);
	dprintf(fd, "%lf;%lf;%lf;%lf;", sig->time, sig->CPI, sig->TPI, sig->GBS);
	dprintf(fd, "%lf;%lf;%lf;", sig->DC_power, sig->DRAM_power, sig->PCK_power);
	dprintf(fd, "%llu;%llu;", sig->cycles, sig->instructions);
	dprintf(fd, "%llu;%llu;%llu;", sig->L1_misses, sig->L2_misses, sig->L3_misses);
	dprintf(fd, "%lf;%llu", sig->Gflops, sig->FLOPS[0]);

    for (i = 1; i < FLOPS_EVENTS; ++i) {
		dprintf(fd, ";%llu", sig->FLOPS[i]);
	}
}

#if DB_MYSQL
#include <mysql.h>

#define SIGNATURE_QUERY     "INSERT INTO Signatures (DC_power, DRAM_power, PCK_power, EDP,\
                            GBS, TPI, CPI, Gflops, time, FLOPS1, FLOPS2, FLOPS3, FLOPS4, \
                            FLOPS5, FLOPS6, FLOPS7, FLOPS8, L1_misses, L2_misses, L3_misses, \
                            instructions, cycles, avg_f, def_f) VALUES (?, ?, ?, ?, ?, ?, \
                            ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"


//returns id of the inserted signature
int mysql_insert_signature(MYSQL *connection, signature_t *sig)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 0;

    if (mysql_stmt_prepare(statement, SIGNATURE_QUERY, strlen(SIGNATURE_QUERY))) return 1;

    MYSQL_BIND bind[24];
    int i = 0;

    //double storage
    for (i = 0; i < 9; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
        bind[i].length = 0;
        bind[i].is_null = 0;
    }

    //unsigned long long storage
    for (i = 9; i < 22; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].length = 0;
        bind[i].is_null = 0;
        bind[i].is_unsigned = 1;
    }

    //unsigned long storage
    bind[22].buffer_type = bind[23].buffer_type = MYSQL_TYPE_LONG;
    bind[22].length = bind[23].length = 0;
    bind[22].is_null = bind[23].is_null = 0;
    bind[22].is_unsigned = bind[23].is_unsigned = 1;

    //storage variables assignation
    bind[0].buffer = (char *)&sig->DC_power;
    bind[1].buffer = (char *)&sig->DRAM_power;
    bind[2].buffer = (char *)&sig->PCK_power;
    bind[3].buffer = (char *)&sig->EDP;
    bind[4].buffer = (char *)&sig->GBS;
    bind[5].buffer = (char *)&sig->TPI;
    bind[6].buffer = (char *)&sig->CPI;
    bind[7].buffer = (char *)&sig->Gflops;
    bind[8].buffer = (char *)&sig->time;
    bind[9].buffer = (char *)&sig->FLOPS[0];
    bind[10].buffer = (char *)&sig->FLOPS[1];
    bind[11].buffer = (char *)&sig->FLOPS[2];
    bind[12].buffer = (char *)&sig->FLOPS[3];
    bind[13].buffer = (char *)&sig->FLOPS[4];
    bind[14].buffer = (char *)&sig->FLOPS[5];
    bind[15].buffer = (char *)&sig->FLOPS[6];
    bind[16].buffer = (char *)&sig->FLOPS[7];
    bind[17].buffer = (char *)&sig->L1_misses;
    bind[18].buffer = (char *)&sig->L2_misses;
    bind[19].buffer = (char *)&sig->L3_misses;
    bind[20].buffer = (char *)&sig->instructions;
    bind[21].buffer = (char *)&sig->cycles;
    bind[22].buffer = (char *)&sig->avg_f;
    bind[23].buffer = (char *)&sig->def_f;

    if (mysql_stmt_bind_param(statement, bind)) return 0;

    if (mysql_stmt_execute(statement)) return 0;

    int id = mysql_stmt_insert_id(statement);

    my_ulonglong affected_rows = mysql_stmt_affected_rows(statement);

    if (affected_rows != 1) printf("ERROR: inserting signature failed.\n");

    if (mysql_stmt_close(statement)) return 0;

    return id;
}

int mysql_retrieve_signatures(MYSQL *connection, char *query, signature_t **sigs)
{
    signature_t *sig_aux = calloc(1, sizeof(signature_t));
    signature_t *sigs_aux;
    int num_signatures;
    int i = 0;
    int status = 0;

    MYSQL_BIND bind[25];
    memset(bind, 0, sizeof(bind));
    
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;
    
    int id = 0;
    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    //id reciever
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer_length= 4;
    bind[0].is_null = 0;
    bind[0].is_unsigned = 1;

    //double recievers
    for (i = 1; i < 10; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
        bind[i].buffer_length = 8;
        bind[i].is_null = 0;
    }

    //unsigned long long recievers
    for (i = 9; i < 22; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].buffer_length = 8;
        bind[i].is_null = 0;
        bind[i].is_unsigned = 1;
    }

    //unsigned long recievers
    bind[23].buffer_type = bind[24].buffer_type = MYSQL_TYPE_LONG;
    bind[23].buffer_length = bind[24].buffer_length = 4;
    bind[23].is_null = bind[24].is_null = 0;
    bind[23].is_unsigned = bind[24].is_unsigned = 1;

    //reciever variables assignation
    bind[0].buffer = &id;
    bind[1].buffer = &sig_aux->DC_power;
    bind[2].buffer = &sig_aux->DRAM_power;
    bind[3].buffer = &sig_aux->PCK_power;
    bind[4].buffer = &sig_aux->EDP;
    bind[5].buffer = &sig_aux->GBS;
    bind[6].buffer = &sig_aux->TPI;
    bind[7].buffer = &sig_aux->CPI;
    bind[8].buffer = &sig_aux->Gflops;
    bind[9].buffer = &sig_aux->time;
    bind[10].buffer = &sig_aux->FLOPS[0];
    bind[11].buffer = &sig_aux->FLOPS[1];
    bind[12].buffer = &sig_aux->FLOPS[2];
    bind[13].buffer = &sig_aux->FLOPS[3];
    bind[14].buffer = &sig_aux->FLOPS[4];
    bind[15].buffer = &sig_aux->FLOPS[5];
    bind[16].buffer = &sig_aux->FLOPS[6];
    bind[17].buffer = &sig_aux->FLOPS[7];
    bind[18].buffer = &sig_aux->L1_misses;
    bind[19].buffer = &sig_aux->L2_misses;
    bind[20].buffer = &sig_aux->L3_misses;
    bind[21].buffer = &sig_aux->instructions;
    bind[22].buffer = &sig_aux->cycles;
    bind[23].buffer = &sig_aux->avg_f;
    bind[24].buffer = &sig_aux->def_f;

    if (mysql_stmt_bind_result(statement, bind)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    if (mysql_stmt_execute(statement)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    if (mysql_stmt_store_result(statement)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    num_signatures = mysql_stmt_num_rows(statement);

    if (num_signatures < 1)
    {
        mysql_stmt_close(statement);
        return 1;
    }

    sigs_aux = (signature_t*) calloc(num_signatures, sizeof(signature_t));
    i = 0; 
    
    //fetching and storing of signatures
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        copy_signature(&sigs_aux[i], sig_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }

    *sigs = sigs_aux;


    if (mysql_stmt_close(statement)) return 1;

    free(sig_aux);

    return (i == 1) ? 0: i;
}


#endif
