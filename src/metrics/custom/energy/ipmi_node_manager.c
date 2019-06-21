/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#include <endian.h>
#include <pthread.h>
#include <freeipmi/freeipmi.h>
#include <common/includes.h>
#include <metrics/custom/hardware_info.h>
#include <metrics/custom/energy/ipmi_lenovo_generic.h>

#define IPMI_RAW_MAX_ARGS (1024)

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;

state_t plug_energy_init(void **c)
{
	uid_t uid;
	int ret = 0;
	int rs_len;
	unsigned int workaround_flags = 0;

	debug("lenovo_node_energy_init");
	pthread_mutex_lock(&lock);

	//
	*c = ipmi_ctx_create();
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) * c;

	//Creating the context
	if (!ipmi_ctx) {
		pthread_mutex_unlock(&lock);
		error("lenovo_NM:Error in ipmi_ctx_create %s", strerror(errno));
		return EAR_ERROR;
	}

	// Checking for root
	uid = getuid();
	if (uid != 0) {
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		error("lenovo_NM: No root permissions");
		return EAR_ERROR;
	}

	// inband context
	if ((ret = ipmi_ctx_find_inband(ipmi_ctx,
									NULL, // driver_type
									0, //disable_auto_probe
									0, // driver_address
									0, // register_spacing
									NULL, // driver_device
									workaround_flags,
									IPMI_FLAGS_DEFAULT)) < 0) {
		error("lenovo_NM: %s", ipmi_ctx_errormsg(ipmi_ctx));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);

		return EAR_ERROR;
	}
	if (ret == 0) {
		error("lenovo_NM: Not inband device found %s", ipmi_ctx_errormsg(ipmi_ctx));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	// This part is hardcoded since we are not supporting other commands rather than reading DC energy
	send_len = 11;

	if (!(bytes_rq = calloc(send_len, sizeof(uint8_t)))) {
		error("lenovo_NM: Allocating memory for request %s", strerror(errno));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	if (!(bytes_rs = calloc(IPMI_RAW_MAX_ARGS, sizeof(uint8_t)))) {
		error("lenovo_NM: Allocating memory for recv data %s", strerror(errno));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}
	// ipmitool raw 0x2e 0x82 0x66 0x4a 0 0 0 1 --> Command to get the parameter (0x20 in Lenovo) bytes_rq[6]
	// sudo ./ipmi-raw 0x0 0x2e 0x82 0x66 0x4a 0 0 0 1
	// byte number 8 with ipmi-raw command
	bytes_rq[0] = (uint8_t) 0x00;
	bytes_rq[1] = (uint8_t) 0x2E;
	bytes_rq[2] = (uint8_t) 0x82;
	bytes_rq[3] = (uint8_t) 0x66;
	bytes_rq[4] = (uint8_t) 0x4a;
	bytes_rq[5] = (uint8_t) 0x00;
	bytes_rq[6] = (uint8_t) 0x00;
	bytes_rq[7] = (uint8_t) 0x00;
	bytes_rq[8] = (uint8_t) 0x01;

	// RAW CMD to get the parameter
	if ((rs_len = ipmi_cmd_raw(ipmi_ctx,
							   bytes_rq[0],
							   bytes_rq[1],
							   &bytes_rq[2],
							   7,
							   bytes_rs,
							   IPMI_RAW_MAX_ARGS)) < 0) {
		error("lenovo_NM: ipmi_cmd_raw fails when reading the parameter %s", ipmi_ctx_errormsg(ipmi_ctx));
		//return EAR_ERROR;
	}

	if (bytes_rs[8] != 0x20) {
		error("lenovo_NM warning raw argument != 0x20");
		bytes_rs[8] = 0x20;
	}

	bytes_rq[0] = (uint8_t) 0x00;
	bytes_rq[1] = (uint8_t) 0x2E;
	bytes_rq[2] = (uint8_t) 0x81;
	bytes_rq[3] = (uint8_t) 0x66;
	bytes_rq[4] = (uint8_t) 0x4A;
	bytes_rq[5] = (uint8_t) 0x00;
	bytes_rq[6] = bytes_rs[8];
	bytes_rq[7] = (uint8_t) 0x01;
	bytes_rq[8] = (uint8_t) 0x82;
	bytes_rq[9] = (uint8_t) 0x00;
	bytes_rq[10] = (uint8_t) 0x08;
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t plug_energy_dispose(void **c)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) *c;

	debug("lenovo_node_energy_dispose");
	if (ipmi_ctx == NULL) {
		error("lenovo_NM: IPMI context not initiallized");
		return EAR_ERROR;
	}
	// // Close context
	ipmi_ctx_close(ipmi_ctx);
	// delete context
	ipmi_ctx_destroy(ipmi_ctx);
	ipmi_ctx = NULL;
	return EAR_SUCCESS;
}

state_t plug_energy_getdata_length(void *c, size_t *size)
{
	debug("lenovo_count_energy_data_length");
	*size = sizeof(unsigned long);
	return EAR_SUCCESS;
}

state_t plug_energy_dc_read(void *c, ulong *emj)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	unsigned long *energyp;
	int rs_len;
	//int tries=0;

	verbose(0, "c '%p', ipmi_ctx '%p'", c, (void *) ipmi_ctx);

	if (ipmi_ctx == NULL) {
		error("lenovo_NM: IPMI context not initiallized");
		return EAR_ERROR;
	}
	debug("lenovo_read_dc_energy");
	// RAW CMD
	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	rs_len = ipmi_cmd_raw(ipmi_ctx,
						  bytes_rq[0],
						  bytes_rq[1],
						  &bytes_rq[2],
						  send_len - 2,
						  bytes_rs,
						  IPMI_RAW_MAX_ARGS);
	if (rs_len < 0) {
		error("lenovo_NM: ipmi_cmd_raw fails %s", ipmi_ctx_errormsg(ipmi_ctx));
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	energyp = (unsigned long *) &bytes_rs[rs_len - 8];
	*emj = (unsigned long) be64toh(*energyp);
	pthread_mutex_unlock(&lock);
	return EAR_SUCCESS;
}

state_t plug_energy_dc_time_read(void *c, ulong *emj, ulong *ms)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	int ret;
	struct timeval t;
	ret = plug_energy_dc_read(ipmi_ctx, emj);
	gettimeofday(&t, NULL);
	*ms = t.tv_sec * 1000 + t.tv_usec / 1000;
	return ret;
}

state_t plug_energy_dc_time_debug(void *c, ulong *ej, ulong *emj, ulong *ts, ulong *tms)
{
	*ej = 0;
	*ts = 0;
	return plug_energy_dc_time_read(c, emj, tms);
}

state_t plug_energy_ac_read(void *c, ulong *emj)
{
	*emj = 0;
	return EAR_SUCCESS;
}