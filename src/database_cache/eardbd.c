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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <database_cache/eardbd.h>
#include <database_cache/eardbd_api.h>

#include <common/types/configuration/cluster_conf.h>
#include <common/types/periodic_aggregation.h>
#include <common/types/periodic_metric.h>
#include <common/types/application.h>
#include <common/types/loop.h>
#include <common/types/log.h>
#include <common/states.h>

#define lops_len 128 * 512
#define mets_len 32 * 512
#define eves_len 32 * 512
#define apps_len 32 * 512

static periodic_aggregation_t aggr;
static application_t apps_mpi[apps_len];
static application_t apps_nor[apps_len];
static application_t apps_ler[apps_len];
static periodic_metric_t mets[mets_len];
static ear_event_t eves[apps_len];
static loop_t lops[apps_len];
static uint apps_mpi_i;
static uint apps_nor_i;
static uint apps_ler_i;
static uint mets_i;
static uint eves_i;
static uint lops_i;

static char buffer[4096];
static char buffer_aux[64];

int EAR_VERBOSE_LEVEL=1;

#define verbose(...) \
	update_time_buffer(); \
	fprintf(stderr, "%s message, ", buffer_aux); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n");

#define error(...) \
	update_time_buffer(); \
	fprintf(stderr, "%s ERROR, ", buffer_aux); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(1);

static void update_time_buffer()
{
	time_t timer = time(NULL);
	struct tm* tm_info = localtime(&timer);
	strftime(buffer_aux, 64, "%H:%M:%S (%d-%m-%y)", tm_info);
}

static void sprint_sockaddr(struct sockaddr *host_addr, char *buffer, size_t size)
{
	char *ip_version;
	void *address;
	int port;

	// IPv4
	if (host_addr->sa_family == AF_INET)
	{
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) host_addr;
		port = (int) ntohs(ipv4->sin_port);
		address = &(ipv4->sin_addr);
		ip_version = "IPv4";
	}
	// IPv6
	else
	{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) host_addr;
		port = (int) ntohs(ipv6->sin6_port);
		address = &(ipv6->sin6_addr);
		ip_version = "IPv6";
	}

	// convert the IP to a string and print it
	inet_ntop(host_addr->sa_family, address, buffer, size);

	if (buffer[0] != ':') {
		sprintf(buffer, "%s:%d", buffer, port);
	}
}

/*
static void print_addrinfo(struct addrinfo *host_info)
{
	struct addrinfo *p;

	for(p = host_info; p != NULL; p = p->ai_next)
	{
		sprint_sockaddr(p->ai_addr);
	}

	struct sockaddr cli_addr;
	socklen_t cli_size;
	char address[128];
	address[0] = '\0';
	getsockname(fd, &cli_addr, &cli_size);
	sprint_sockaddr(&cli_addr, address, cli_size);
}
 */

/*
 * Signal processing
 */

/*
 * Data storing/loading
 */

static void db_store_events()
{
	if (eves_i <= 0) {
		return;
	}

    verbose("Trying to insert in DB %d event samples", eves_i);
    db_batch_insert_ear_event(eves, eves_i);
}

static void db_store_loops()
{
   if (lops_i <= 0) {
        return; 
    }

    verbose("Trying to insert in DB %d loop samples", lops_i);
   //db_batch_insert_loops(lops, lops_i);
}

static void db_store_periodic_metrics()
{
	if (mets_i <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d periodic metric samples", mets_i);
	db_batch_insert_periodic_metrics(mets, mets_i);
}

static void db_store_periodic_aggregation()
{
	if (aggr.n_samples <= 0) {
		return;
	}

	verbose("Trying to insert in DB an aggregation of %d samples", aggr.n_samples);
	db_insert_periodic_aggregation(&aggr);
}

static void db_store_applications(application_t *apps, uint n_apps)
{
	if (n_apps == 0) {
		return;
	}

	verbose("Trying to insert in DB %d applications samples", n_apps);
	db_batch_insert_applications(apps, n_apps);
}

static void db_store_applications(application_t *apps, uint n_apps)
{
	if (n_apps == 0) {
		return;
	}

	verbose("Trying to insert in DB %d applications samples", n_apps);
	db_batch_insert_applications(apps, n_apps);
}

/*
 * Data processing
 */

static void make_periodic_aggregation(periodic_metric_t *met)
{
	add_periodic_aggregation(&aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_timeout_data()
{
	verbose("Finished aggregation, consumed %lu energy (?J) from %lu to %lu,",
			aggr.DC_energy, aggr.start_time, aggr.end_time);

	db_store_periodic_aggregation();
	init_periodic_aggregation(&aggr);

	db_store_periodic_metrics();
	mets_i = 0;

	db_store_applications();
	apps_i = 0;
	
	db_store_events();
	eves_i = 0;

	db_store_loops();
	lops_i = 0;
}

static void process_incoming_data(int fd, char *buffer, size_t size)
{
	char *type;

	if (size == sizeof(application_t))
	{
		type = "application_t";

		memcpy (&apps[apps_i], buffer, size);
		apps_i += 1;

		if (apps_i == apps_len)
		{
			db_store_applications();
			apps_i = 0;
		}
	}
	else if (size == sizeof(periodic_metric_t))
	{
		type = "periodic_metric_t";

		memcpy (&mets[mets_i], buffer, size);
		make_periodic_aggregation(&mets[mets_i]);
		mets_i += 1;

		if (mets_i == mets_len) {
			db_store_periodic_metrics();
			mets_i = 0;
		}
	} else if (size == sizeof(ear_event_t))
	{
		type = "ear_event_t";
		
		memcpy(&eves[eves_i], buffer, size);
		eves_i += 1;

		if (eves_i == eves_len) {
			db_store_events();
			eves_i = 0;
		}
	} else if (size = sizeof(loop_t))
	{
		type = "loop_t";

		memcpy(&lops[lops_i], buffer, size);
		lops_i += 1;

		if (lops_i == eves_len) {
			db_store_loops();
			lops_i = 0;
		}
	} else {
		type = "unknown";
	}

	verbose("received an object type '%s' from the socket %d", type, fd);
}

/*
 * Server managing
 */

static int _accept(int fd)
{
	char address[128];
	struct sockaddr_storage cli_addr;
	socklen_t size;
	int fd_cli;

	size = sizeof (cli_addr);
	fd_cli = accept(fd, (struct sockaddr *) &cli_addr, &size);

	if (fd_cli == -1)
	{
		error("on accept (%s)", strerror(errno));
		return EAR_ERROR;
	}

	verbose("accepted connection from socket %d", fd_cli);
	return fd_cli;
}

static ssize_t _receive(int fd)
{
	ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

	// Handle data from a client
	if (bytes <= 0)
	{
		if (bytes == 0) {
			verbose("disconnected from socket %d", fd);
		} else {
			error("on reception (%s)", strerror(errno));
		}

		return EAR_ERROR;
	}

	return bytes;
}

static int _socket(char *host, unsigned int port, int protocol, struct addrinfo **info)
{
	struct addrinfo *info_aux;
	struct addrinfo hints;

	char c_port[16];
	int status;
	int fd;

	// Format
	sprintf(c_port, "%u", port);
	memset(&hints, 0, sizeof (hints));
	
	hints.ai_socktype = protocol;	// TCP stream sockets
	hints.ai_family = AF_UNSPEC;	// Don't care IPv4 or IPv6
	hints.ai_flags = AI_PASSIVE;    // Fill in my IP for me

	if ((status = getaddrinfo(host, c_port, &hints, info)) != 0)
	{
		error("getaddrinfo error (%s)", gai_strerror(status));
		return EAR_ERROR;
	}

	//
	info_aux = *info;

	//
	fd = socket(info_aux->ai_family, info_aux->ai_socktype, info_aux->ai_protocol);

	if (fd < 0) {
		error("opening socket %d (%s)", fd, strerror(errno));
		return EAR_ERROR;
	}

	return fd;
}

static int _bind(int fd, struct addrinfo *info)
{
	// Assign to the socket the address and port
	int status = bind(fd, info->ai_addr, info->ai_addrlen);

	if (status < 0) {
		error("binding socket %d (%s)", fd, strerror(errno));
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

static int _listen(int fd, struct addrinfo *info)
{
	// Listening the port
	int status = listen(fd, BACKLOG);

	if (status < 0) {
		error("listening socket %d (%s)", fd, strerror(errno));
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

/*
 * Main
 */

void usage(int argc, char **argv)
{
	if (0)
	{
		printf("Usage: %s\n", argv[0]);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	struct timeval timeout;
	struct addrinfo *srv_info_tcp;
	struct addrinfo *srv_info_udp;

	char conf_path[PATH_MAX];
	cluster_conf_t conf_clus;

	long merge_time;
	float mb_apps;
	float mb_lops;
	float mb_eves;
	float mb_mets;

	fd_set fds_incoming;
	fd_set fds_active;
	int fd_srv_tcp;
	int fd_srv_udp;
	int fd_srv_max;
	int fd_cli;

	ssize_t size;
	int status_1;
	int status_2;
	int i;

	usage(argc, argv);

	//
	verbose("phase 1: starting");

	mb_apps = (double) (sizeof(application_t)     * apps_len) / 1000000.0;
	mb_mets = (double) (sizeof(periodic_metric_t) * mets_len) / 1000000.0;
	mb_lops = (double) (sizeof(loop_t)            * lops_len) / 1000000.0;
	mb_eves = (double) (sizeof(ear_event_t)       * mets_len) / 1000000.0;

	verbose("reserving %0.2f MBytes for applications", mb_apps);
	verbose("reserving %0.2f MBytes for power metrics", mb_mets);

	// Configuration file
	if (get_ear_conf_path(conf_path) == EAR_ERROR) {
		error("Error getting ear.conf path");
	}

	verbose("Reading '%s' configuration file", conf_path);
	read_cluster_conf(conf_path, &conf_clus);

	// Database
	init_db_helper(&conf_clus.database);

	// Format
	merge_time = (long) conf_clus.db_manager.aggr_time;

	timeout.tv_sec = merge_time;
	timeout.tv_usec = 0L;

	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	// Opening socket
	fd_srv_tcp = _socket(NULL, conf_clus.db_manager.tcp_port, TCP, &srv_info_tcp);
	fd_srv_udp = _socket(NULL, conf_clus.db_manager.udp_port, UDP, &srv_info_udp);

	verbose ("opened socket %d for TCP packets on port %u", fd_srv_tcp, conf_clus.db_manager.tcp_port);
	verbose ("opened socket %d for UDP packets on port %u", fd_srv_udp, conf_clus.db_manager.udp_port);

	if (fd_srv_tcp < 0 || fd_srv_udp < 0) {
		exit(1);
	}

	// Binding socket
	status_1 = _bind(fd_srv_tcp, srv_info_tcp);
	status_2 = _bind(fd_srv_udp, srv_info_udp);

	if (status_1 < 0 || status_2 < 0) {
		exit(1);
	}

	// Listening socket
	status_1 = _listen(fd_srv_tcp, srv_info_tcp);

	if (status_1 < 0) {
		exit(1);
	}

	// Add the listener to the ready set
	FD_SET(fd_srv_tcp, &fds_active);
	FD_SET(fd_srv_udp, &fds_active);

	// Keep track of the biggest file descriptor
	fd_srv_max = fd_srv_udp;

	if (fd_srv_tcp > fd_srv_udp) {
		fd_srv_max = fd_srv_tcp;
	}

	verbose("phase 2: listening (processing every %d s)", merge_time);

	while(1)
	{
		fds_incoming = fds_active;

		if (select(fd_srv_max + 1, &fds_incoming, NULL, NULL, &timeout) == -1) {
			error("select");
		}

		// If timeout, data processing
		if (timeout.tv_sec == 0 && timeout.tv_usec == 0)
		{
			process_timeout_data();
		
			timeout.tv_sec = merge_time;
			timeout.tv_usec = 0L;
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fd_srv_max; i++)
		{
			if (FD_ISSET(i, &fds_incoming)) // we got one!!
			{
				// Handle new connections
				if (i == fd_srv_tcp)
				{
					fd_cli = _accept(fd_srv_tcp);

					if (fd_cli != -1)
					{
						FD_SET(fd_cli, &fds_active);

						if (fd_cli > fd_srv_max) {
							fd_srv_max = fd_cli;
						}
					}
				} else {
					size = _receive(i);

					if (size >= 0) {
						process_incoming_data(i, buffer, size);
					} else {
						FD_CLR(i, &fds_active);
						close(i);
					}
				}
			}
		}
	}

	return 0;
}
