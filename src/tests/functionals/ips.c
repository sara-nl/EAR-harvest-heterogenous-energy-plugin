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
#include <unistd.h>

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

int global_num = 0;

void print_ips(ip_table_t *ips, int num_ips)
{
    int i;
    printf("%10s\t%10s\t%12s\t%10s\n", "hostname", "ip", "ip_int", "counter");

    for (i=0; i<num_ips; i++)
        printf("%10s\t%10s\t%12d\t%10d\n", ips[i].name, ips[i].ip, ips[i].ip_int, ips[i].counter); 
}

void fill_ip(char *buff, ip_table_t *table)
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
    freeaddrinfo(result);
}

ip_table_t *generate_node_names(cluster_conf_t my_cluster_conf, ip_table_t *ips)
{
    int i, j, k, rc; 
    char node_name[256];
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
     //           printf("node_name: %s\tip: %s\tip_int: %d\n", ips[num_ips].name, ips[num_ips].ip, ips[num_ips].ip_int); 
                num_ips++;
            }
        }
    }
  // print_ips(ips, num_ips);
    global_num = num_ips;
    return ips;
}

void visit_ip(int ip, int dist, ip_table_t *ips, int num_ips)
{
    int i;
    for (i=0; i < num_ips; i++)
        if (ips[i].ip_int == ip)
        {
            ips[i].counter++;
            break;
        }
    
    struct sockaddr_in saddr;
    //ina = saddr->sin_addr;    
    saddr.sin_addr.s_addr = ip;
    printf("current ip: %s\n", inet_ntoa(saddr.sin_addr));
    //reverse byte order for little-endian machines
    if (dist < 1) return;


    int ip1, ip2;
    ip1 = ip2 = htonl(ip);
    printf("new ip: %d\t old ip: %d\n", ip1, ip);
    ip1 -= dist;
    ip2 += dist;
    dist /= 2;
    visit_ip(ntohl(ip1), dist, ips, num_ips);
    visit_ip(ntohl(ip2), dist, ips, num_ips);

}

void main(int argc,char *argv[])
{
	cluster_conf_t my_cluster;
	char ear_path[256];
    int starter_ip = 0;
    int num_ips = 0;
    int initial_distance = 0;

    if (get_ear_conf_path(ear_path)==EAR_ERROR){
        printf("Error getting ear.conf path\n");
        exit(0);
    }
    ip_table_t *ips = NULL;
	read_cluster_conf(ear_path,&my_cluster);
    ips = generate_node_names(my_cluster, ips);
    num_ips = global_num;
    if (argc < 3)
    {
        ip_table_t temp;
        char buff[50];
        gethostname(buff, 50);
        fill_ip(buff, &temp);
        starter_ip = temp.ip_int;
        if (argc > 1)
            initial_distance = atoi(argv[1]);
        printf("No ip specified, taking default one (%s) and distance %d\n", temp.ip, initial_distance);
        visit_ip(starter_ip, initial_distance, ips, num_ips);
    }
    else
    {
        ip_table_t temp;
        int i;
        initial_distance = atoi(argv[argc-1]);
        for (i = 1; i < argc - 1; i++)
        {
            fill_ip(argv[i], &temp);
            starter_ip = temp.ip_int;
            visit_ip(starter_ip, initial_distance, ips, num_ips);
        }
    }
    print_ips(ips, num_ips);
    free_cluster_conf(&my_cluster);
    free(ips);
}
