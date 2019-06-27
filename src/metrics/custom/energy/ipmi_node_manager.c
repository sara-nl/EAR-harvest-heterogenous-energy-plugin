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
#include <metrics/api/energy.h>
#include <metrics/custom/hardware_info.h>

#define IPMI_RAW_MAX_ARGS (1024)

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;

static state_t _plug_energy_init(void **c)
{
	unsigned int workaround_flags = 0;
	int ret = 0;
	int rs_len;

	if (getuid() != 0) {
		state_return_msg(EAR_ERROR, 0, "the user is not root");
	}

	//
	*c = ipmi_ctx_create();
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) *c;

	// Creating the context
	if (!ipmi_ctx)
	{
		state_return_msg(EAR_ERROR, 0, "creating IPMI context");
	}

	// Inband context
	if ((ret = ipmi_ctx_find_inband(ipmi_ctx, NULL, 0, 0, 0, NULL,
			workaround_flags, IPMI_FLAGS_DEFAULT)) < 0)
	{
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx));
	}

	if (ret == 0) {
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx));
	}

	send_len = 11;

	if (!(bytes_rq = calloc(send_len, sizeof(uint8_t)))) {
		state_return_msg(EAR_ALLOC_ERROR, 0, strerror(errno));
	}

	if (!(bytes_rs = calloc(IPMI_RAW_MAX_ARGS, sizeof(uint8_t)))) {
		state_return_msg(EAR_ALLOC_ERROR, 0, strerror(errno));
	}

	bytes_rq[0] = (uint8_t) 0x00;
	bytes_rq[1] = (uint8_t) 0x2E;
	bytes_rq[2] = (uint8_t) 0x82;
	bytes_rq[3] = (uint8_t) 0x66;
	bytes_rq[4] = (uint8_t) 0x4a;
	bytes_rq[5] = (uint8_t) 0x00;
	bytes_rq[6] = (uint8_t) 0x00;
	bytes_rq[7] = (uint8_t) 0x00;
	bytes_rq[8] = (uint8_t) 0x01;

	if ((rs_len = ipmi_cmd_raw(ipmi_ctx, bytes_rq[0], bytes_rq[1], &bytes_rq[2],
	   7, bytes_rs, IPMI_RAW_MAX_ARGS)) < 0)
	{
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx));
	}

	if (bytes_rs[8] != 0x20) {
		bytes_rs[8] = 0x20;
	}

	bytes_rq[0]  = (uint8_t) 0x00;
	bytes_rq[1]  = (uint8_t) 0x2E;
	bytes_rq[2]  = (uint8_t) 0x81;
	bytes_rq[3]  = (uint8_t) 0x66;
	bytes_rq[4]  = (uint8_t) 0x4A;
	bytes_rq[5]  = (uint8_t) 0x00;
	bytes_rq[6]  =    bytes_rs[8];
	bytes_rq[7]  = (uint8_t) 0x01;
	bytes_rq[8]  = (uint8_t) 0x82;
	bytes_rq[9]  = (uint8_t) 0x00;
	bytes_rq[10] = (uint8_t) 0x08;

	return EAR_SUCCESS;
}

state_t plug_energy_init(void **c)
{
	state_t s;

	pthread_mutex_lock(&lock);
	s = _plug_energy_init(c);
	pthread_mutex_unlock(&lock);

	if (state_fail(s))
	{
		error("ipmi_node_manager returned error number '%d' ('%s')", s, intern_error_str);
		plug_energy_dispose(c);
	}

	return s;
}

state_t plug_energy_dispose(void **c)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) *c;

	if (ipmi_ctx == NULL) {
		return EAR_ERROR;
	}

	ipmi_ctx_close(ipmi_ctx);
	ipmi_ctx_destroy(ipmi_ctx);
	ipmi_ctx = NULL;

	return EAR_SUCCESS;
}

state_t plug_energy_getdata_length(void *c, size_t *size)
{
	*size = sizeof(unsigned long);
	return EAR_SUCCESS;
}

#if 0
state_t plug_energy_data_frequency_get(void *c, ulong *freq)
{
	*freq = 1000000;
	return EAR_SUCCESS;
}
#endif

static state_t _plug_energy_dc_read(void *c, ulong *emj)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	ulong *emjp;
	int rs_len;

	if (ipmi_ctx == NULL) {
		state_return_msg(EAR_ERROR, 0, "context is NULL");
	}

	rs_len = ipmi_cmd_raw(ipmi_ctx, bytes_rq[0], bytes_rq[1], &bytes_rq[2],
		  send_len - 2, bytes_rs, IPMI_RAW_MAX_ARGS);

	if (rs_len < 0) {
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx));
	}

	emjp = (unsigned long *) &bytes_rs[rs_len - 8];
	*emj = (unsigned long)    be64toh(*emjp);

	return EAR_SUCCESS;
}

state_t plug_energy_dc_read(void *c, ulong *emj)
{
	state_t s;

	*emj = 0;

	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	s = _plug_energy_dc_read(c, emj);
	pthread_mutex_unlock(&lock);

	if (state_fail(s)) {
		error("ipmi_node_manager returned error number '%d' ('%s')", s, intern_error_str);
	}

	return s;
}

state_t plug_energy_dc_time_read(void *c, ulong *emj, ulong *tms)
{
	timestamp ts;
	state_t s;

	s = plug_energy_dc_read(c, emj);

	timestamp_getprecise(&ts);
	*tms = timestamp_convert(&ts, TIME_MSECS);

	return s;
}

state_t plug_energy_ac_read(void *c, ulong *emj)
{
	*emj = 0;
	return EAR_SUCCESS;
}
