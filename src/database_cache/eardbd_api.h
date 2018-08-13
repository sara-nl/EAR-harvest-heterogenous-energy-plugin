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

#ifndef EAR_EARDBD_API_H
#define EAR_EARDBD_API_H

#include <common/sockets.h>
#include <common/types/log.h>
#include <common/types/loop.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <common/types/periodic_metric.h>
#include <common/types/periodic_aggregation.h>
#include <common/types/configuration/cluster_conf.h>
#include <database_cache/eardbd.h>

#define verbose(...) \
	fprintf(stderr, "EARDBD, " __VA_ARGS__); \
	fprintf(stderr, "\n");

state_t eardbd_connect(cluster_conf_t *conf);
state_t eardbd_reconnect(cluster_conf_t *conf);
state_t eardbd_is_connected();
state_t eardbd_disconnect();

state_t eardbd_send_ping();
state_t eardbd_send_periodic_metric(periodic_metric_t *met);
state_t eardbd_send_application(application_t *app);
state_t eardbd_send_loop(loop_t *loop);
state_t eardbd_send_event(ear_event_t *eve);

#endif //EAR_EARDBD_API_H
