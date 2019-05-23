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



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <freeipmi/freeipmi.h>
#include <pthread.h>

#include <common/config.h>
#include <common/states.h>
#include <common/output/debug.h>
#include <common/output/error.h>
#include <common/output/verbose.h>
#include <metrics/custom/hardware_info.h>

#define IPMI_RAW_MAX_ARGS (1024)


#if 0
static ipmi_ctx_t ipmi_ctx = NULL;
#endif
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;
static pthread_mutex_t sd650_energy_node_lock = PTHREAD_MUTEX_INITIALIZER;


/* Specific functions for CPU XX PLATFORM YY */
/* Grants access to ipmi device */
int lenovo_wct_node_energy_init(ipmi_ctx_t *ipmi_ctx)
{
	uid_t uid;
	int ret=0;
	unsigned int workaround_flags = 0;
	debug("lenovo_SD650");
	pthread_mutex_lock(&sd650_energy_node_lock);
	//Creating the context
	if (!(*ipmi_ctx = ipmi_ctx_create ())){
    error("lenovo_SD650:Error in ipmi_ctx_create %s",strerror(errno));
		pthread_mutex_unlock(&sd650_energy_node_lock);
		return EAR_ERROR;
	}
	// Checking for root
	uid = getuid ();
	if (uid != 0){ 
		lenovo_wct_node_energy_dispose(ipmi_ctx);
		pthread_mutex_unlock(&sd650_energy_node_lock);
		error("lenovo_SD650: No root permissions");
		return EAR_ERROR;
	}
	// inband context
	if ((ret = ipmi_ctx_find_inband (*ipmi_ctx, 
					NULL, // driver_type
					0, //disable_auto_probe
					0, // driver_address
					0, // register_spacing
					NULL, // driver_device
                    workaround_flags,
                    IPMI_FLAGS_DEFAULT)) < 0) {
		error("lenovo_SD650: %s",ipmi_ctx_errormsg(*ipmi_ctx));
		lenovo_wct_node_energy_dispose(ipmi_ctx);
		pthread_mutex_unlock(&sd650_energy_node_lock);
		return EAR_ERROR;	
	}
	if (ret==0){
		lenovo_wct_node_energy_dispose(ipmi_ctx);
		pthread_mutex_unlock(&sd650_energy_node_lock);
		error("lenovo_SD650: Not inband device found");
		return EAR_ERROR;	
	}
	// This part is hardcoded since we are not supporting other commands rather than reading DC energy
	// To support a generic API with more ipmi commands we must create commands inside each call
	send_len=8;
	if (!(bytes_rq = calloc (send_len, sizeof (uint8_t))))
	{
		error("lenovo_SD650: Allocating memory for request %s",strerror(errno));
		lenovo_wct_node_energy_dispose(ipmi_ctx);
		pthread_mutex_unlock(&sd650_energy_node_lock);
		return EAR_ERROR;
	}
	if (!(bytes_rs = calloc (IPMI_RAW_MAX_ARGS, sizeof (uint8_t))))
	{
		error("lenovo_SD650: Allocating memory for recv data %s",strerror(errno));
		lenovo_wct_node_energy_dispose(ipmi_ctx);
		pthread_mutex_unlock(&sd650_energy_node_lock);
		return EAR_ERROR;
	}	
	// Robert Wolford provided command: ipmitool raw 0x3a 0x32 4 1 0 0 0 --> low frequency command
	// Robert Wolford provided command: ipmitool raw 0x3a 0x32 4 2 0 0 0 --> High frequency command : Energy (J,mJ) and Time (sec,ms)
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
#ifdef LOW_FREQ_ENERGY_READINGS
	bytes_rq[0]=(uint8_t)0x00;// lun
	bytes_rq[1]=(uint8_t)0x3a;// netfn
	bytes_rq[2]=(uint8_t)0x32;// cmd
	bytes_rq[3]=(uint8_t)0x4; // 5 args
	bytes_rq[4]=(uint8_t)0x1;
	bytes_rq[5]=(uint8_t)0x0;
	bytes_rq[6]=(uint8_t)0x0;
	bytes_rq[7]=(uint8_t)0x0;
#else
    bytes_rq[0]=(uint8_t)0x00;// lun
    bytes_rq[1]=(uint8_t)0x3a;// netfn
    bytes_rq[2]=(uint8_t)0x32;// cmd
    bytes_rq[3]=(uint8_t)0x4; // 5 args
    bytes_rq[4]=(uint8_t)0x2;
    bytes_rq[5]=(uint8_t)0x0;
    bytes_rq[6]=(uint8_t)0x0;
    bytes_rq[7]=(uint8_t)0x0;

#endif

	pthread_mutex_unlock(&sd650_energy_node_lock);
	return EAR_SUCCESS;	
		
}
int lenovo_wct_count_energy_data_length(ipmi_ctx_t ipmi_ctx)
{
	debug("lenovo_count_energy_data_length");
	return sizeof(unsigned long);
}
int lenovo_wct_read_dc_energy(ipmi_ctx_t ipmi_ctx,unsigned long *energy)
{
    uint32_t my_energy=0;
    uint16_t my_energy_mj=0;

    int rs_len;
    if (ipmi_ctx==NULL){
        error("lenovo_fast/accurate meter: IPMI context not initiallized");
        return EAR_ERROR;
    }
    debug("lenovo_read_dc_energy");
    // RAW CMD
    *energy=0;
		if (pthread_mutex_trylock(&sd650_energy_node_lock)) return EAR_BUSY;
    #define RAW_SIZE 14
    if ((rs_len = ipmi_cmd_raw (ipmi_ctx,
                              bytes_rq[0],
                              bytes_rq[1],
                              &bytes_rq[2],
                              send_len - 2,
                              bytes_rs,
                              IPMI_RAW_MAX_ARGS)) < 0)
    {
				pthread_mutex_unlock(&sd650_energy_node_lock);
        error("lenovo_fast/accurate meter: ipmi_cmd_raw fails %s",ipmi_ctx_errormsg(ipmi_ctx));
        return EAR_ERROR;
    }
    if (rs_len<(RAW_SIZE+2)){
			pthread_mutex_unlock(&sd650_energy_node_lock);
      error("lenovo_read_dc_energy ipmi_cmd_raw returns not valid answer %d",rs_len);
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
    my_energy=bytes_rs[7]<<24|bytes_rs[6]<<16|bytes_rs[5]<<8|bytes_rs[4];
    my_energy_mj=bytes_rs[9]<<8|bytes_rs[8];
    #if 0
    printf("fast/accurate meter_energy_reading(J):0x%02X%02X%02X%02X\n",bytes_rs[7],bytes_rs[6],bytes_rs[5],bytes_rs[4]);
    printf("fast/accurate meter_energy_reading(mJ):0x%02X%02X\n",bytes_rs[9],bytes_rs[8]);
    #endif

    // Energy values provided in this model are reported in Joules, the API
    // returns mJ (multiply by 1.000)
    *energy=(ulong)my_energy*1000+(ulong)my_energy_mj;

		pthread_mutex_unlock(&sd650_energy_node_lock);
    return EAR_SUCCESS;;
}

/* AC energy is not yet supported */
int lenovo_wct_read_ac_energy(ipmi_ctx_t ipmi_ctx,unsigned long *energy)
{
	*energy=0;
	return EAR_SUCCESS;
}
#if DEBUG_WCT
/** Energy and time is returned in a single ipmi command */
int lenovo_wct_read_dc_energy_and_time(ipmi_ctx_t ipmi_ctx,ulong *energy,ulong *energy_mj,uint8_t *raw_out,ulong *seconds,ulong *ms)
#else
/** Energy and time is returned in a single ipmi command */
int lenovo_wct_read_dc_energy_and_time(ipmi_ctx_t ipmi_ctx,ulong *energy,ulong *energy_mj,ulong *seconds,ulong *ms)
#endif
{
    uint32_t my_energy=0;
    uint16_t my_energy_mj=0;
    uint32_t my_seconds=0;
    uint16_t my_ms=0;

    int rs_len;
    if (ipmi_ctx==NULL){
        error("lenovo_fast/accurate meter: IPMI context not initiallized");
        return EAR_ERROR;
    }
    debug("lenovo_read_dc_energy");
    // RAW CMD
    *energy=0;
    *energy_mj=0;
    *seconds=0;
    *ms=0;
    #define RAW_SIZE 14
	#if DEBUG_WCT
    memset(raw_out,0,RAW_SIZE);
	#endif
		if (pthread_mutex_trylock(&sd650_energy_node_lock)) return EAR_BUSY;
    if ((rs_len = ipmi_cmd_raw (ipmi_ctx,
                              bytes_rq[0],
                              bytes_rq[1],
                              &bytes_rq[2],
                              send_len - 2,
                              bytes_rs,
                              IPMI_RAW_MAX_ARGS)) < 0)
    {
				pthread_mutex_unlock(&sd650_energy_node_lock);
        error("lenovo_fast/accurate meter: ipmi_cmd_raw fails %s",ipmi_ctx_errormsg(ipmi_ctx));
        return EAR_ERROR;
    }
		if (rs_len!=(RAW_SIZE+2)){
			pthread_mutex_unlock(&sd650_energy_node_lock);
      error("lenovo_read_dc_energy ipmi_cmd_raw returns not valid answer %d",rs_len);
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
    my_energy=bytes_rs[7]<<24|bytes_rs[6]<<16|bytes_rs[5]<<8|bytes_rs[4];
    my_energy_mj=bytes_rs[9]<<8|bytes_rs[8];
    my_seconds=bytes_rs[13]<<24|bytes_rs[12]<<16|bytes_rs[11]<<8|bytes_rs[10];
    my_ms=bytes_rs[15]<<8|bytes_rs[14];
	#if DEBUG_WCT
    raw_out[0]= bytes_rs[2];
    raw_out[1]= bytes_rs[3];
    raw_out[2]= bytes_rs[4];
    raw_out[3]= bytes_rs[5];
    raw_out[4]= bytes_rs[6];
    raw_out[5]= bytes_rs[7];
    raw_out[6]= bytes_rs[8];
    raw_out[7]= bytes_rs[9];
    raw_out[8]= bytes_rs[10];
    raw_out[9]= bytes_rs[11];
    raw_out[10]= bytes_rs[12];
    raw_out[11]= bytes_rs[13];
    raw_out[12]= bytes_rs[14];
    raw_out[13]= bytes_rs[15];
	#endif
    #if 0
    printf("fast/accurate meter_energy_reading(J):0x%02X%02X%02X%02X\n",bytes_rs[7],bytes_rs[6],bytes_rs[5],bytes_rs[4]);
    printf("fast/accurate meter_energy_reading(mJ):0x%02X%02X\n",bytes_rs[9],bytes_rs[8]);
    printf("fast/accurate meter_energy_reading(sec):0x%02X%02X%02X%02X\n",bytes_rs[13],bytes_rs[12],bytes_rs[11],bytes_rs[10]);
    printf("fast/accurate meter_energy_reading(ms):0x%02X%02X\n",bytes_rs[15],bytes_rs[14]);
    #endif

    // Energy values provided in this model are reported in Joules, the API
    // returns mJ (multiply by 1.000)
    //*energy=(unsigned long)my_energy*1000;
    *energy=(ulong)my_energy;
    *energy_mj=(ulong)my_energy_mj;
    *seconds=(ulong)my_seconds;
    *ms=(ulong)my_ms;

		pthread_mutex_unlock(&sd650_energy_node_lock);
    return EAR_SUCCESS;;
}

int lenovo_wct_read_dc_energy_time(ipmi_ctx_t ipmi_ctx,ulong *energy_mj,ulong *ms)
{
	ulong ej,emj,ts,tms;
	int ret;
	ret=lenovo_wct_read_dc_energy_and_time(ipmi_ctx,&ej,&emj,&ts,&tms);
	*energy_mj=ej*1000+emj;
	*ms=ts*1000+tms;
	return ret;
}


/* Release access to ipmi device */
int lenovo_wct_node_energy_dispose(ipmi_ctx_t *ipmi_ctx)
{
	debug("lenovo_node_energy_dispose");
	if (*ipmi_ctx==NULL){ 
		error("lenovo_water_cooling: IPMI context not initiallized");
		return EAR_ERROR;
	}
	// // Close context
	ipmi_ctx_close (*ipmi_ctx);
	// delete context
	ipmi_ctx_destroy (*ipmi_ctx);
	*ipmi_ctx=NULL;
	return EAR_SUCCESS;
}

