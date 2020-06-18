/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

/** Creates a new /var/run/service.pid file with the pid of the calling process */
/* returns 1 if the service must recover the status because of failure */
int new_service(char *service);

/** Releases the /var/run/service.pid file */
void end_service(char *name);

