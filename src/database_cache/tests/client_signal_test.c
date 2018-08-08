#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

static void signal_handler(int signal)
{
	printf("Received signal %d\n", signal);

	sleep(10);

	printf("Exitting signal handler\n");

}

/*
 *
 */

int main(int argc, char **argv)
{
	struct sigaction action;
	sigset_t set;

	// Blocking all signals except PIPE, TERM, INT and HUP
	sigfillset(&set);
	sigdelset(&set, SIGPIPE);
	sigdelset(&set, SIGTERM);
	sigdelset(&set, SIGINT);
	sigdelset(&set, SIGHUP);

	sigprocmask(SIG_SETMASK, &set, NULL);

	// Editing signals individually
	//sigemptyset(&action.sa_mask);
	sigfillset(&action.sa_mask);
	action.sa_handler = signal_handler;
	action.sa_flags = 0;

	if (sigaction(SIGPIPE, &action, NULL) < 0) {
	}
	if (sigaction(SIGTERM, &action, NULL) < 0) {
	}
	if (sigaction(SIGINT, &action, NULL) < 0) {
	}
	if (sigaction(SIGHUP, &action, NULL) < 0) {
	}

	printf("Sleeping %d\n", getpid());
	sleep(120);

	return 0;
}
