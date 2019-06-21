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
#include <metrics/custom/energy/ipmi_lenovo_sd650.h>

#define IPMI_RAW_MAX_ARGS	1024
#define RAW_SIZE 			14

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;

state_t plug_energy_init(void **c)
{
	uid_t uid;
	int ret = 0;
	unsigned int workaround_flags = 0;

	debug("lenovo_SD650");

	pthread_mutex_lock(&lock);

	//
	*c = ipmi_ctx_create();
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) * c;

	//Creating the context
	if (!ipmi_ctx) {
		error("lenovo_SD650:Error in ipmi_ctx_create %s", strerror(errno));
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	// Checking for root
	uid = getuid();
	if (uid != 0) {
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		error("lenovo_SD650: No root permissions");
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
		error("lenovo_SD650: %s", ipmi_ctx_errormsg(ipmi_ctx));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	if (ret == 0) {
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		error("lenovo_SD650: Not inband device found");
		return EAR_ERROR;
	}
	// This part is hardcoded since we are not supporting other commands rather than reading DC energy
	// To support a generic API with more ipmi commands we must create commands inside each call
	send_len = 8;

	if (!(bytes_rq = calloc(send_len, sizeof(uint8_t)))) {
		error("lenovo_SD650: Allocating memory for request %s", strerror(errno));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	if (!(bytes_rs = calloc(IPMI_RAW_MAX_ARGS, sizeof(uint8_t)))) {
		error("lenovo_SD650: Allocating memory for recv data %s", strerror(errno));
		plug_energy_dispose(c);
		pthread_mutex_unlock(&lock);
		return EAR_ERROR;
	}

	// Robert Wolford provided command: ipmitool raw 0x3a 0x32 4 1 0 0 0 --> low frequency command
	// Robert Wolford provided command: ipmitool raw 0x3a 0x32 4 2 0 0 0 --> High frequency command
	// 		: Energy (J,mJ) and Time (sec,ms)
	/*
	ipmitool raw 0x3a 0x32 4 2 0 0 0
	COMMENT: Add 2 bytes to this format
	the response format is (LSB first):
	Byte 0-1: the index/location of FPGA FIFO.
	Byte 2-5: energy reading(J)
	Byte 6-7: energy reading(mJ)
	Byte 8-11: the second info for timestamp
	Byte 12-13: the millisecond info for timestamp
	*/

	bytes_rq[0] = (uint8_t) 0x00;// lun
	bytes_rq[1] = (uint8_t) 0x3a;// netfn
	bytes_rq[2] = (uint8_t) 0x32;// cmd
	bytes_rq[3] = (uint8_t) 0x4; // 5 args
	bytes_rq[4] = (uint8_t) 0x2;
	bytes_rq[5] = (uint8_t) 0x0;
	bytes_rq[6] = (uint8_t) 0x0;
	bytes_rq[7] = (uint8_t) 0x0;

	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t plug_energy_dispose(void **c)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) *c;

	debug("lenovo_node_energy_dispose");

	if (ipmi_ctx == NULL) {
		error("lenovo_water_cooling: IPMI context not initiallized");
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
	uint32_t my_energy = 0;
	uint16_t my_energy_mj = 0;
	int rs_len;

	if (ipmi_ctx == NULL) {
		error("lenovo_fast/accurate meter: IPMI context not initiallized");
		return EAR_ERROR;
	}

	debug("lenovo_read_dc_energy");

	// RAW CMD
	*emj = 0;

	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	if ((rs_len = ipmi_cmd_raw(ipmi_ctx,
							   bytes_rq[0],
							   bytes_rq[1],
							   &bytes_rq[2],
							   send_len - 2,
							   bytes_rs,
							   IPMI_RAW_MAX_ARGS)) < 0) {
		pthread_mutex_unlock(&lock);
		error("lenovo_fast/accurate meter: ipmi_cmd_raw fails %s", ipmi_ctx_errormsg(ipmi_ctx));
		return EAR_ERROR;
	}

	if (rs_len < (RAW_SIZE + 2)) {
		pthread_mutex_unlock(&lock);
		error("lenovo_read_dc_energy ipmi_cmd_raw returns not valid answer %d", rs_len);
		return EAR_ERROR;
	}

	/*
	ipmitool raw 0x3a 0x32 4 2 0 0 0
	COMMENT: Add 2 bytes to this format
	the response format is (LSB first):
	Byte 0-1: the index/location of FPGA FIFO.
	Byte 2-5: energy reading(J)
	Byte 6-7: energy reading(mJ)
	Byte 8-11: the second info for timestamp
	Byte 12-13: the millisecond info for timestamp

	*/
	my_energy = bytes_rs[7] << 24 | bytes_rs[6] << 16 | bytes_rs[5] << 8 | bytes_rs[4];
	my_energy_mj = bytes_rs[9] << 8 | bytes_rs[8];

	// Energy values provided in this model are reported in Joules, the API
	// returns mJ (multiply by 1.000)
	*emj = (ulong) my_energy * 1000 + (ulong) my_energy_mj;

	pthread_mutex_unlock(&lock);
	return EAR_SUCCESS;
}

state_t plug_energy_dc_time_read(void *c, ulong *emj, ulong *tms)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	ulong ej, ts;
	int ret;

	ret = plug_energy_dc_time_debug(ipmi_ctx, &ej, emj, &ts, tms);
	*emj = ej * 1000 + *emj;
	*tms = ts * 1000 + *tms;

	return ret;
}

state_t plug_energy_dc_time_debug(void *c, ulong *ej, ulong *emj, ulong *ts, ulong *tms)
{
	ipmi_ctx_t ipmi_ctx = (ipmi_ctx_t) c;
	uint32_t my_energy = 0;
	uint16_t my_energy_mj = 0;
	uint32_t my_seconds = 0;
	uint16_t my_ms = 0;

	int rs_len;
	if (ipmi_ctx == NULL) {
		error("lenovo_fast/accurate meter: IPMI context not initiallized");
		return EAR_ERROR;
	}
	debug("lenovo_read_dc_energy");

	// RAW CMD
	*ej        = 0;
	*emj       = 0;
	*ts        = 0;
	*tms        = 0;

	if (pthread_mutex_trylock(&lock)) {
		return EAR_BUSY;
	}

	if ((rs_len = ipmi_cmd_raw(ipmi_ctx,
							   bytes_rq[0],
							   bytes_rq[1],
							   &bytes_rq[2],
							   send_len - 2,
							   bytes_rs,
							   IPMI_RAW_MAX_ARGS)) < 0)
	{
		pthread_mutex_unlock(&lock);
		error("lenovo_fast/accurate meter: ipmi_cmd_raw fails %s", ipmi_ctx_errormsg(ipmi_ctx));
		return EAR_ERROR;
	}

	if (rs_len != (RAW_SIZE + 2)) {
		pthread_mutex_unlock(&lock);
		error("lenovo_read_dc_energy ipmi_cmd_raw returns not valid answer %d", rs_len);
		return EAR_ERROR;
	}

	/*
	ipmitool raw 0x3a 0x32 4 2 0 0 0
	COMMENT: Add 2 bytes to this format
	the response format is (LSB first):
	Byte 0-1: the index/location of FPGA FIFO.
	Byte 2-5: energy reading(J)
	Byte 6-7: energy reading(mJ)
	Byte 8-11: the second info for timestamp
	Byte 12-13: the millisecond info for timestamp
	*/
	my_energy    = bytes_rs[7]  << 24 | bytes_rs[6]  << 16 | bytes_rs[5]  << 8 | bytes_rs[4];
	my_energy_mj = bytes_rs[9]  <<  8 | bytes_rs[8];
	my_seconds   = bytes_rs[13] << 24 | bytes_rs[12] << 16 | bytes_rs[11] << 8 | bytes_rs[10];
	my_ms        = bytes_rs[15] <<  8 | bytes_rs[14];

	// Energy values provided in this model are reported in Joules, the API
	// returns mJ (multiply by 1.000)
	//*energy=(unsigned long)my_energy*1000;

	*ej        = (ulong) my_energy;
	*emj       = (ulong) my_energy_mj;
	*ts        = (ulong) my_seconds;
	*tms       = (ulong) my_ms;

	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t plug_energy_ac_read(void *c, ulong *emj)
{
	*emj = 0;
	return EAR_SUCCESS;
}
