/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _EVENT_H
#define _EVENT_H

#include <time.h>
#include <common/config.h>
#include <common/types/generic.h>
#include <common/types/job.h>

typedef struct ear_event {
	job_id jid;
    job_id step_id;
	char node_id[GENERIC_NAME];
	uint event;
	ulong freq;
	time_t timestamp;
} ear_event_t;




#endif
