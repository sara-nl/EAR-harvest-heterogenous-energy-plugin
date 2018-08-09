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
#ifndef EAR_EARDBD_H
#define EAR_EARDBD_H

#include <netdb.h>
#include <common/config.h>
#include <common/sockets.h>
#include <common/types/log.h>
#include <common/types/loop.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <common/types/periodic_metric.h>
#include <common/types/periodic_aggregation.h>
#include <common/types/configuration/cluster_conf.h>

#define CONTENT_TYPE_PIN	0
#define CONTENT_TYPE_PER	1
#define CONTENT_TYPE_APP	2
#define CONTENT_TYPE_LOO	3
#define CONTENT_TYPE_EVE	4
#define CONTENT_TYPE_QST	5
#define CONTENT_TYPE_ANS	6
#define SYNC_ENRGY 			0x01
#define SYNC_APPSM 			0x02
#define SYNC_APPSN			0x04
#define SYNC_APPSL 			0x08
#define SYNC_LOOPS			0x10
#define SYNC_EVNTS			0x20
#define SYNC_RESET			0x40
#define SYNC_ALL			0x7F
#define RES_SYNC			0
#define RES_TIME			1
#define RES_OVER			2
#define RES_FAIL			3

#define sync_option(option, type) \
	((option & type) > 0)

// Base macros
#define _MAX(X,Y) sizeof(X) > sizeof(Y) ? sizeof(X) : sizeof(Y)
#define _MMAAXX(W,X,Y,Z) _MAX(W,X) > _MAX(Y,Z) ? _MAX(W,X) : _MAX(Y,Z)

// Compile time macros
#define MAX_PACKET_SIZE() \
	(_MMAAXX(periodic_metric_t, application_t, ear_event_t, loop_t)) + sizeof(packet_header_t)

typedef struct sync_qst {
	uint sync_option;
} sync_qst_t;

typedef struct sync_ans {
	int answer;
} sync_ans_t;

#endif //EAR_EARDBD_H
