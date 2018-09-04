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
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <common/config.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>

#define IP_LENGTH 24

typedef struct ip_table
{
    int ip_int;
    char ip[IP_LENGTH];
    char name[IP_LENGTH];
    int counter;
} ip_table_t;

int EAR_VERBOSE_LEVEL=1;

void generate_node_names(cluster_conf_t my_cluster_conf)
{
    int i, j, k, rc; 
    char node_name[256];
    ip_table_t *ips = NULL;
    int num_ips = 0;
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {   
                if (k == -1) 
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10) 
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                }   
                ips = realloc(ips, sizeof(ip_table_t)*(num_ips+1));
                strcpy(ips[num_ips].name, node_name);
                fill_ip(node_name, &ips[num_ips]);
                printf("node_name: %s\tip: %s\tip_int: %d\n", ips[num_ips].name, ips[num_ips].ip, ips[num_ips].ip_int); 
                num_ips++;
            }
        }
    }
    free(ips);
}
int fill_ip(char *buff, ip_table_t *table)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

   	s = getaddrinfo(buff, NULL, &hints, &result);
    if (s != 0) {
		fprintf(stderr,"getaddrinfo fails for port %s (%s)\n",buff,strerror(errno));
		return;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            //ina = saddr->sin_addr;    
            table->ip_int = saddr->sin_addr.s_addr;
            strcpy(table->ip, inet_ntoa(saddr->sin_addr));
        }
    }
}
void main(int argc,char *argv[])
{
	cluster_conf_t my_cluster;
	char ear_path[256];
	if (argc>1){
		strcpy(ear_path,argv[1]);
	}else{
		if (get_ear_conf_path(ear_path)==EAR_ERROR){
			printf("Error getting ear.conf path\n");
			exit(0);
		}
	}
	read_cluster_conf(ear_path,&my_cluster);
    generate_node_names(my_cluster);
//	print_cluster_conf(&my_cluster);
    free_cluster_conf(&my_cluster);
}
