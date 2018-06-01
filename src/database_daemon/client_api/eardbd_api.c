#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <database_daemon/client_api/eardbd_api.h>
#include <common/types/periodic_metric.h>
#include <common/types/application.h>
#include <common/states.h>

static char buffer[4096];
static struct addrinfo *srv_info;
static int _protocol = -1;
static int connected = -1;
static int fd_srv    = -1;

#define verbose(...) \
	fprintf(stdout, "EARDBD_API, "); \
	fprintf(stdout, __VA_ARGS__); \
	fprintf(stdout, "\n");

#define error(...) \
	fprintf(stdout, "EARDBD_API ERROR, "); \
	fprintf(stdout, __VA_ARGS__); \
	fprintf(stdout, "\n"); \
	exit(1);

#define CONNECTION_TEST() \
	if (fd_srv == -1 || _protocol == -1 || (_protocol == TCP && connected == -1)) { \
		return EAR_ERROR; \
	}

/*
 * Generic functions
 */

static void print_sockaddr(struct sockaddr *host_addr)
{
	char *ip_version;
	void *address;
	int port;


	// IPv4
	if (host_addr->sa_family == AF_INET)
	{
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) host_addr;
		address = &(ipv4->sin_addr);
		port = (int) ntohs(ipv4->sin_port);
		ip_version = "IPv4";
	}
		// IPv6
	else
	{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) host_addr;
		address = &(ipv6->sin6_addr);
		ip_version = "IPv6";
	}

	// convert the IP to a string and print it
	inet_ntop(host_addr->sa_family, address, buffer, INET6_ADDRSTRLEN);

	if (buffer[0] != ':') {
		verbose("%s:%d", buffer, port);
	}
}

static int _send(void *object, size_t size)
{
	size_t bytes_left = size;
	size_t bytes_sent = 0;
	int n;

	while(bytes_sent < size)
	{
		if (_protocol == TCP) {
			verbose("sending by TCP an object of size %lu by socket %d", bytes_left, fd_srv);
			n = send(fd_srv, object + bytes_sent, bytes_left, 0);
		} else {
			verbose("sending UDP an object of size %lu using socket %d", bytes_left, fd_srv);
			n = sendto(fd_srv, object + bytes_sent, bytes_left, 0, srv_info->ai_addr, srv_info->ai_addrlen);
		}

		if (n == -1) {
			error("sending (%s)", strerror(errno));
			return EAR_ERROR;
		}
		verbose("sended");

		bytes_sent += n;
		bytes_left -= n;
	}

	return EAR_SUCCESS;
}

static int _socket(char *host, char *port, int protocol)
{
	static struct addrinfo hints;
	int status;

	// Format
	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = protocol;	// TCP stream sockets
	hints.ai_family = AF_UNSPEC;	// Don't care IPv4 or IPv6

	if ((status = getaddrinfo(host, port, &hints, &srv_info)) != 0)
	{
		error("getaddrinfo error (%s)", gai_strerror(status));
		return EAR_ERROR;
	}

	fd_srv = socket(srv_info->ai_family, srv_info->ai_socktype, srv_info->ai_protocol);

	if (fd_srv < 0) {
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

static int _disconnect()
{
	if (fd_srv > 0) {
		close(fd_srv);
	}

	connected = -1;
	fd_srv    = -1;

	return EAR_SUCCESS;
}

static int _connect(char *host)
{
	int status;

	status = _socket(host, PORT_TCP, TCP);

	if (status < 0) {
		error("opening socket");
		return EAR_ERROR;
	}

	verbose("openned socket");

	// Assign to the socket the address and port
	status = connect(fd_srv, srv_info->ai_addr, srv_info->ai_addrlen);

	if (status < 0) {
		error("conecting socket");
		return EAR_ERROR;
	}

	verbose("connected");

	freeaddrinfo(srv_info);
	connected = 1;

	print_sockaddr(srv_info->ai_addr);

	return EAR_SUCCESS;
}

/*
 * API
 */

int eardbd_send_application(void *app)
{
	CONNECTION_TEST();
	verbose("sending application");
	return _send((application_t *) app, sizeof(application_t));
}

int eardbd_send_periodic_metric(void *met)
{
	CONNECTION_TEST();
	verbose("sending application");
	return _send((periodic_metric_t *) met, sizeof(periodic_metric_t));
}

int eardbd_ping()
{
	CONNECTION_TEST();
	verbose("sending ping");
	return _send("EAR", 4);
}

int eardbd_is_connected()
{
	CONNECTION_TEST();
	return EAR_SUCCESS;
}

int eardbd_connect(char *host, int protocol)
{
	int status;

	if (protocol == TCP) {
		status = _connect(host);
	} else if (protocol == UDP) {
		status = _socket(host, PORT_UDP, UDP);
	} else {
		error("unknown protocol");
		return EAR_ERROR;
	}

	if (status < EAR_SUCCESS) {
		_disconnect();
		return EAR_ERROR;
	}

	_protocol = protocol;
	verbose("using %d fd", fd_srv);

	return EAR_SUCCESS;
}

int eardbd_disconnect()
{
	return _disconnect();
}