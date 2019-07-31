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
#include <common/math_operations.h>


#define IPMI_RAW_MAX_ARGS (1024)
#define RAW_SIZE 14

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;

state_t energy_init(void **c);

static state_t _energy_init(void **c)
{
	unsigned int workaround_flags = 0;
	int ret = 0;

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
		state_return_msg(EAR_ERROR, 0, "not inband device found");
	}

	// First difference
	send_len = 8;

	if (!(bytes_rq = calloc(send_len, sizeof(uint8_t)))) {
		state_return_msg(EAR_ALLOC_ERROR, 0, strerror(errno));
	}

	if (!(bytes_rs = calloc(IPMI_RAW_MAX_ARGS, sizeof(uint8_t)))) {
		state_return_msg(EAR_ALLOC_ERROR, 0, strerror(errno));
	}

	// Second diference
	bytes_rq[0] = (uint8_t) 0x00;
	bytes_rq[1] = (uint8_t) 0x3a;
	bytes_rq[2] = (uint8_t) 0x32;
	bytes_rq[3] = (uint8_t) 0x4;
	bytes_rq[4] = (uint8_t) 0x2;
	bytes_rq[5] = (uint8_t) 0x0;
	bytes_rq[6] = (uint8_t) 0x0;
	bytes_rq[7] = (uint8_t) 0x0;

	return EAR_SUCCESS;
}

state_t energy_dispose(void **c);

state_t energy_init(void **c)
{
	state_t s;

	pthread_mutex_lock(&lock);
	s = _energy_init(c);
	pthread_mutex_unlock(&lock);

	if (state_fail(s))
	{
		error("ipmi_node_manager returned error number '%d' ('%s')", s, intern_error_str);
		energy_dispose(c);
	}

	return s;
}

state_t energy_dispose(void **c)
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

state_t energy_datasize(size_t *size)
{
	*size = sizeof(unsigned long);
	return EAR_SUCCESS;
}

state_t energy_frequency(ulong *freq)
{
	*freq = 10000;
	return EAR_SUCCESS;
}

static state_t _energy_dc_time_read(void *c, edata_t emj, ulong *tms, int get_time)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	uint16_t aux_emj = 0;
	uint16_t aux_tms = 0;
	uint32_t aux_ej = 0;
	uint32_t aux_ts = 0;
	int rs_len;
	ulong *pemj=(ulong *)emj;

	if (ipmi_ctx == NULL) {
		state_return_msg(EAR_ERROR, 0, "context is NULL");
	}

	rs_len = ipmi_cmd_raw(ipmi_ctx, bytes_rq[0], bytes_rq[1], &bytes_rq[2],
		  send_len - 2, bytes_rs, IPMI_RAW_MAX_ARGS);

	if (rs_len < 0) {
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx));
	}
	// Third difference
	if (rs_len < (RAW_SIZE + 2)) {
		state_return_msg(EAR_ERROR, 0, "IPMI command returned invalid answer");
	}

	//
	aux_ej  = (bytes_rs[7] << 24) | (bytes_rs[6] << 16) | (bytes_rs[5] << 8) | (bytes_rs[4]);
	aux_emj = (bytes_rs[9] << 8)  | (bytes_rs[8]);
	*pemj = (ulong) aux_ej * 1000  + (ulong) aux_emj;

	if (get_time) {
	aux_ts  = bytes_rs[13] << 24 | bytes_rs[12] << 16 | bytes_rs[11] << 8 | bytes_rs[10];
	aux_tms = bytes_rs[15] <<  8 | bytes_rs[14];
	*tms = (ulong) aux_ts * 1000 + (ulong) aux_tms;
	}

	return EAR_SUCCESS;
}

state_t energy_dc_read(void *c, edata_t emj)
{
	state_t s;
	ulong *pemj=(ulong *)emj;

	*pemj = 0;

	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	s = _energy_dc_time_read(c, emj, NULL, 0);
	pthread_mutex_unlock(&lock);

	if (state_fail(s)) {
		error("ipmi_node_manager returned error number '%d' ('%s')", s, intern_error_str);
	}

	return s;
}

state_t energy_dc_time_read(void *c, edata_t emj, ulong *tms)
{
	state_t s;
	ulong *pemj=(ulong *)emj;

	*pemj = 0;

	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	s = _energy_dc_time_read(c, emj, tms, 1);
	pthread_mutex_unlock(&lock);

	if (state_fail(s)) {
		error("ipmi_node_manager returned error number '%d' ('%s')", s, intern_error_str);
	}

	return s;
}

state_t energy_ac_read(void *c, edata_t emj)
{
	ulong *pemj=(ulong *)emj;
	*pemj = 0;
	return EAR_SUCCESS;
}

unsigned long diff_node_energy(ulong init,ulong end)
{
  ulong ret=0;
  if (end>init){
    ret=end-init;
  } else{
    ret=ulong_diff_overflow(init,end);
  }
  return ret;
}


state_t energy_units(uint *units)
{
  *units=1000;
  return EAR_SUCCESS;
}

state_t energy_accumulated(unsigned long *e,edata_t init,edata_t end)
{
  int i;
	ulong *pinit=(ulong *)init,*pend=(ulong *)end;
  unsigned long total=diff_node_energy(*pinit,*pend);
  *e=total;
  return EAR_SUCCESS;
}

