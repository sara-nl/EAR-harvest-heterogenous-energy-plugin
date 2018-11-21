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

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <database_cache/eardbd.h>

// Mirroring
extern pid_t server_pid;
extern pid_t mirror_pid;
extern pid_t others_pid;
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;

// State machine
extern int reconfiguring;
extern int listening;
extern int releasing;
extern int exitting;
extern int updating;
extern int waiting;
extern int forked;

// Strings
extern char *str_who[2];
extern int verbosity;

/*
 *
 *
 *
 */

void signal_handler(int signal, siginfo_t *info, void *context)
{
	int propagating = 0;

	if (signal == SIGUSR1)
	{
		verwho1("signal SIGUSR1 received on %s, switching verbosity", str_who[mirror_iam]);

		updating  = 1;
		verbosity = (verbosity + 1) % 4;
	}

	if (signal == SIGUSR2)
	{
		verwho1("signal SIGUSR2 received on %s, switching verbosity", str_who[mirror_iam]);
	}

	// Case exit
	if ((signal == SIGTERM || signal == SIGINT) && !exitting)
	{
		verwho1("signal SIGTERM/SIGINT received on %s, exitting", str_who[mirror_iam]);

		propagating = others_pid > 0 && info->si_pid != others_pid;
		listening   = 0;
		releasing   = 1;
		exitting    = 1;
	}

	// Case reconfigure
	if (signal == SIGHUP && !reconfiguring)
	{
		verwho1("signal SIGHUP received on %s, reconfiguring", str_who[mirror_iam]);

		propagating   = others_pid > 0 && info->si_pid != others_pid;
		listening     = 0;
		reconfiguring = server_iam;
		releasing     = 1;
		exitting      = mirror_iam;
	}

	// Propagate signals
	if (propagating)
	{
		kill(others_pid, signal);

		if (server_iam) {
			waitpid(mirror_pid, NULL, 0);
		}
	}
}
