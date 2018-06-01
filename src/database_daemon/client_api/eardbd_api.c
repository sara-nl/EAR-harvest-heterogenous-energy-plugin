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
static int connected;
static int _protocol;
static int srv_fd;

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
	if (connected == 0) { \
		return EAR_ERROR; \
	}


/*
 * Generic functions
 */

static int _send(void *object, size_t size)
{
	size_t bytes_left = size;
	size_t bytes_sent = 0;
	int n;

	while(bytes_sent < size)
	{
		if (_protocol == TCP) {
			verbose("sending by TCP an object of size %lu by socket %d", bytes_left, srv_fd);

			n = send(srv_fd, object + bytes_sent, bytes_left, 0);
		} else {
			verbose("sending UDP an object of size %lu using socket %d", bytes_left, srv_fd);

			n = sendto(srv_fd, object + bytes_sent, bytes_left, 0, srv_info->ai_addr, srv_info->ai_addrlen);
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

	srv_fd = socket(srv_info->ai_family, srv_info->ai_socktype, srv_info->ai_protocol);

	if (srv_fd < 0) {
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

static int _disconnect()
{
	if (srv_fd > 0) {
		close(srv_fd);
	}

	connected = 0;
	srv_fd = 0;

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
	status = connect(srv_fd, srv_info->ai_addr, srv_info->ai_addrlen);

	if (status < 0) {
		error("conecting socket");
		return EAR_ERROR;
	}

	verbose("connected");

	freeaddrinfo(srv_info);
	connected = 1;

	return EAR_SUCCESS;
}

/*
 * API
 */

int eardbd_send_application(void *app)
{
	//CONNECTION_TEST();

	verbose("sending application");

	return _send((application_t *) app, sizeof(application_t));
}

int eardbd_send_periodic_metric(void *met)
{
	//CONNECTION_TEST();

	verbose("sending application");

	return _send((periodic_metric_t *) met, sizeof(periodic_metric_t));
}

int eardbd_ping()
{
	verbose("sending ping");

	return _send("EAR", 4);
}

int eardbd_start(char *host, int protocol)
{
	int status;

	_protocol = protocol;

	if (_protocol == TCP) {
		status = _connect(host);
	} else if (_protocol == UDP) {
		status = _socket(host, PORT_UDP, UDP);
	} else {
		error("unknown protocol");
		return EAR_ERROR;
	}

	if (srv_fd < 0) {
		_disconnect();
		return EAR_ERROR;
	}

	verbose("using %d fd", srv_fd);

	return EAR_SUCCESS;
}

int eardbd_dispose()
{
	return _disconnect();
}