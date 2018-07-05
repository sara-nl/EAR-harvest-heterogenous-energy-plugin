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

static int _socket(char *host, unsigned int port, int protocol)
{
	static struct addrinfo hints;
	char c_port[16];
	int status;

	// Format
	sprintf(c_port, "%u", port);
	memset(&hints, 0, sizeof hints);

	hints.ai_socktype = protocol;
	hints.ai_family = AF_UNSPEC; // Don't care IPv4 or IPv6

	if ((status = getaddrinfo(host, c_port, &hints, &srv_info)) != 0)
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