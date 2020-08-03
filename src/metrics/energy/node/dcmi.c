/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <common/states.h> 
#include <common/types/generic.h> 
#include <common/math_operations.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <metrics/energy/node/dcmi.h>


static pthread_mutex_t ompi_lock = PTHREAD_MUTEX_INITIALIZER;
static ulong timeframe;

static int opendev(struct ipmi_intf *intf)
{
	intf->fd = open("/dev/ipmi0", O_RDWR);
	if (intf->fd < 0) {
		intf->fd = open("/dev/ipmi/0", O_RDWR);
		if (intf->fd < 0) {
			intf->fd = open("/dev/ipmidev/0", O_RDWR);
		};
	};
	return intf->fd;
};

static void closedev(struct ipmi_intf *intf)
{
	if (intf->fd > 0) {
		close(intf->fd);
		intf->fd = -1;
	};
};

static struct ipmi_rs *sendcmd(struct ipmi_intf *intf, struct ipmi_rq *req)
{
	struct ipmi_req _req;
	struct ipmi_recv recv;
	struct ipmi_addr addr;
	struct ipmi_system_interface_addr bmc_addr = {
			addr_type:    IPMI_SYSTEM_INTERFACE_ADDR_TYPE,
			channel:    IPMI_BMC_CHANNEL,
	};
	struct ipmi_ipmb_addr ipmb_addr = {
			addr_type:    IPMI_IPMB_ADDR_TYPE,
			channel:    intf->channel & 0x0f,
	};
	static struct ipmi_rs rsp;
	uint8_t *data = NULL;
	int data_len = 0;
	static int curr_seq = 0;
	fd_set rset;

	if (intf == NULL || req == NULL)
		return NULL;
	memset(&_req, 0, sizeof(struct ipmi_req));

	if (intf->addr != 0) {
		ipmb_addr.slave_addr = intf->addr;
		ipmb_addr.lun = req->msg.lun;
		_req.addr = (unsigned char *) &ipmb_addr;
		_req.addr_len = sizeof(ipmb_addr);

	} else {
		bmc_addr.lun = req->msg.lun;
		_req.addr = (unsigned char *) &bmc_addr;
		_req.addr_len = sizeof(bmc_addr);
	};
	_req.msgid = curr_seq++;

	_req.msg.data = req->msg.data;
	_req.msg.data_len = req->msg.data_len;
	_req.msg.netfn = req->msg.netfn;
	_req.msg.cmd = req->msg.cmd;

	if (ioctl(intf->fd, IPMICTL_SEND_COMMAND, &_req) < 0) {
		debug("Unable to send command\n");
		if (data != NULL)
			free(data);
		return NULL;
	};

	FD_ZERO(&rset);
	FD_SET(intf->fd, &rset);

	if (select(intf->fd + 1, &rset, NULL, NULL, NULL) < 0) {
		debug("I/O Error\n");
		if (data != NULL)
			free(data);
		return NULL;
	};
	if (FD_ISSET(intf->fd, &rset) == 0) {
		debug("No data available\n");
		if (data != NULL)
			free(data);
		return NULL;
	};

	recv.addr = (unsigned char *) &addr;
	recv.addr_len = sizeof(addr);
	recv.msg.data = rsp.data;
	recv.msg.data_len = sizeof(rsp.data);
	if (ioctl(intf->fd, IPMICTL_RECEIVE_MSG_TRUNC, &recv) < 0) {
		debug("Error receiving message\n");
		if (errno != EMSGSIZE) {
			if (data != NULL)
				free(data);
			return NULL;
		};
	};


	/* save completion code */
	rsp.ccode = recv.msg.data[0];
	rsp.data_len = recv.msg.data_len - 1;

	if (recv.msg.data[0] == 0) {

		/* save response data for caller */
		if (rsp.ccode == 0 && rsp.data_len > 0) {
			memmove(rsp.data, rsp.data + 1, rsp.data_len);
			rsp.data[recv.msg.data_len] = 0;
		};

		if (data != NULL)
			free(data);
		return &rsp;
	};
	rsp.ccode = recv.msg.data[0];
	rsp.data_len = recv.msg.data_len - 1;
	return &rsp;
} //sendcmd


state_t dcmi_get_capabilities_enh_power(struct ipmi_intf *intf, struct ipmi_data *out)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req;
	uint8_t msg_data[2];

	uint8_t num_rolling_avg, ra_time_period;
	
	int i;
	int s;

	if (pthread_mutex_trylock(&ompi_lock)) {
    return EAR_BUSY;
  }
  memset(&req, 0, sizeof(req));
  req.msg.netfn = DCMI_NETFN;
  req.msg.cmd = DCMI_CMD_GET_CAP;
  msg_data[0]=(uint8_t)DCMI_GROUP_EXT;
  msg_data[1]=(uint8_t)DCMI_PARAM_ENH_POWER;
  intf->addr=0x0;
  req.msg.data = msg_data;
  req.msg.data_len = sizeof(msg_data);

  rsp = sendcmd(intf, &req);
  if (rsp == NULL) {
        out->mode=-1;
				error("sendcmd returns NULL");
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
  };
  if (rsp->ccode > 0) {
				error("Get capability command returned with error 0x%02x",(int)rsp->ccode);
        out->mode=-1;
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
        };

  out->data_len=rsp->data_len;
  for (i=0;i<rsp->data_len; i++) {
  	out->data[i]=rsp->data[i];
		#if SHOW_DEBUGS
		printf("Byte %d: 0x%02x\n",i,rsp->data[i]);
		#endif
  }
	/* 0: Group extension
 *  * 1:2 DCMI Specification Conformance (Major/Minor)
 *   * 3 Parameter revisions
 *    * 6:N parameter data
 *     */
	/* Parameter data for capabilities
 *  * 0 -> 6 number of supported ra_time_periods 
 *  2:n ra_time_periods [7:6] duration unit [5:0] duration
 *    */
	num_rolling_avg=rsp->data[6];
	#if SHOW_DEBUGS
	printf("There are %u rolling averag periods\n",num_rolling_avg);
	#endif

	for (i=0;i<num_rolling_avg;i++){
		ra_time_period=rsp->data[7+i];
		printf("Units %u Time %u\n",ra_time_period&0xC0,ra_time_period&0x3F);
	}		
	
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;

}
state_t dcmi_get_capabilities(struct ipmi_intf *intf, struct ipmi_data *out)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req;
	uint8_t msg_data[2];
	uint8_t major,minor,revision,power_enabled;
	
	int i;
	int s;

	if (pthread_mutex_trylock(&ompi_lock)) {
    return EAR_BUSY;
  }
  memset(&req, 0, sizeof(req));
  req.msg.netfn = DCMI_NETFN;
  req.msg.cmd = DCMI_CMD_GET_CAP;
  msg_data[0]=(uint8_t)DCMI_GROUP_EXT;
  msg_data[1]=(uint8_t)DCMI_PARAM_SUP_CAP;
  intf->addr=0x0;
  req.msg.data = msg_data;
  req.msg.data_len = sizeof(msg_data);

  rsp = sendcmd(intf, &req);
  if (rsp == NULL) {
        out->mode=-1;
				error("sendcmd returns NULL");
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
  };
  if (rsp->ccode > 0) {
				error("Get capability command returned with error 0x%02x",(int)rsp->ccode);
        out->mode=-1;
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
        };

  out->data_len=rsp->data_len;
  for (i=0;i<rsp->data_len; i++) {
  	out->data[i]=rsp->data[i];
		#if 0
		printf("Byte %d: 0x%02x\n",i,rsp->data[i]);
		#endif
  }
	/* 0: Group extension
 * 1:2 DCMI Specification Conformance (Major/Minor)
 * 3 Parameter revisions
 * 6:N parameter data
 */
	/* Parameter data for capabilities
 * 0 -> 6 reserved 
 * 1 -> Platform capabilities. Bit 0 refers to Power management
 */
	major=rsp->data[1];
	minor=rsp->data[2];
	revision=rsp->data[3];
	power_enabled=rsp->data[5]&(uint8_t)0x01;
		
	#if SHOW_DEBUGS
	printf("Major %u minor %u revision %u power management enabled %u \n",(uint)major,(uint)minor,(uint)revision,(uint)power_enabled);
	#endif
	
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;

}


state_t dcmi_power_reading(struct ipmi_intf *intf, struct ipmi_data *out,power_data_t *cpower)
{
	struct ipmi_rs *rsp;
	struct ipmi_rq req;
	uint8_t msg_data[4];
	uint16_t current_power, min_power,max_power,avg_power;
	uint16_t *current_powerp, *min_powerp,*max_powerp,*avg_powerp;
	uint32_t timeframe,timestamp;
	uint32_t *timeframep,*timestampp;
	uint8_t power_state,*power_statep;
	int i;
	int s;

	if (pthread_mutex_trylock(&ompi_lock)) {
    return EAR_BUSY;
  }
/*
 * NETFN=0x2c 
 * COMMAND=0x02 
 * Group EXTENSSION=0xdc 
 * 0x01=SYTEMPOWER STATSTIC 
 * 0x00 0x00 = RESERVED
 *
 * OUTPUT
 * 1 -> COMP CODE
 * 2 -> GROUP ID =0xDC
 * 3:4 -> POwer in watts
 * 5:6 -> min power
 * 7:8 -> max power
 * 9:10 --> average power (*)
 * 11:14 IPMI Specification based Time Stamp
 * 15:18 Timeframe in milliseconds
 * 19 power reading state
 */
  memset(&req, 0, sizeof(req));
  req.msg.netfn = DCMI_NETFN;
  req.msg.cmd = DCMI_CMD_GET_POWER;
  msg_data[0]=(uint8_t)DCMI_GROUP_EXT;
  msg_data[1]=(uint8_t)0x01;
  msg_data[2]=(uint8_t)0x00;
  msg_data[3]=(uint8_t)0x00;
  intf->addr=0x0;
  req.msg.data = msg_data;
  req.msg.data_len = sizeof(msg_data);

  rsp = sendcmd(intf, &req);
  if (rsp == NULL) {
        out->mode=-1;
				error("sendcmd returns NULL");
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
  };
  if (rsp->ccode > 0) {
				error("Power reading command returned with error 0x%02x",(int)rsp->ccode);
        out->mode=-1;
				pthread_mutex_unlock(&ompi_lock);
        return EAR_ERROR;
        };

  out->data_len=rsp->data_len;
  for (i=0;i<rsp->data_len; i++) {
  	out->data[i]=rsp->data[i];
		#if 0
		printf("Byte %d: 0x%02x\n",i,rsp->data[i]);
		#endif
  }
	/*
 *   uint16_t current_power, min_power,max_power,avg_power;
 *     uint32_t timeframe;
 */
	/*
 *3:4 -> POwer in watts
 *5:6 -> min power
 *7:8 -> max power
 *9:10 --> average power (*)
 *11:14 IPMI Specification based Time Stamp
 *15:18 Timeframe in milliseconds
 */
	current_powerp=(uint16_t *)&rsp->data[1];
	current_power=*current_powerp;
	min_powerp=(uint16_t *)&rsp->data[3];
	max_powerp=(uint16_t *)&rsp->data[5];
	avg_powerp=(uint16_t *)&rsp->data[7];
	timestampp=(uint32_t *)&rsp->data[9];
	timeframep=(uint32_t *)&rsp->data[13];
	power_statep=(uint8_t *)&rsp->data[17];
	min_power=*min_powerp;
	max_power=*max_powerp;
	avg_power=*avg_powerp;
	timeframe=*timeframep;
	timestamp=*timestampp;
	power_state=((*power_statep&0x40) > 0);
	#if SHOW_DEBUGS
	printf("current power 0x%04x min power 0x%04x max power 0x%04x avg power 0x%04x\n",current_power,min_power,max_power,avg_power);
	printf("current power %u min power %u max power %u avg power %u\n",(uint)current_power,(uint)min_power,(uint)max_power,(uint)avg_power);
	printf("timeframe %u timestamp %u\n",timeframe,timestamp);
	printf("power state %u\n",(uint)power_state);
	#endif
	cpower->current_power=(ulong)current_power;
	cpower->min_power=(ulong)min_power;
	cpower->max_power=(ulong)max_power;
	cpower->avg_power=(ulong)avg_power;
	cpower->timeframe=(ulong)timeframe;
	cpower->timestamp=(ulong)timestamp;
	
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;
}



/*
 * MAIN FUNCTIONS
 */

state_t energy_init(void **c)
{
	struct ipmi_data out;
	state_t st;
	power_data_t my_power;
	int ret;

	if (c == NULL) {
		return EAR_ERROR;
	}

	*c = (struct ipmi_intf *) malloc(sizeof(struct ipmi_intf));
	if (*c == NULL) {
		return EAR_ERROR;
	}

	pthread_mutex_lock(&ompi_lock);
	debug("trying opendev\n");
	ret= opendev((struct ipmi_intf *)*c);
	if (ret<0){ 
		debug("opendev fails\n");
		pthread_mutex_unlock(&ompi_lock);
		return EAR_ERROR;
	}
	st=dcmi_power_reading(*c, &out,&my_power)
	timeframe = my_power.my_power * 1000;
	pthread_mutex_unlock(&ompi_lock);

	return EAR_SUCCESS;
}

state_t energy_dispose(void **c) {
	if ((c == NULL) || (*c == NULL)) return EAR_ERROR;
	pthread_mutex_lock(&ompi_lock);
	closedev((struct ipmi_intf *) *c);
	free(*c);
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;
}
state_t energy_datasize(size_t *size)
{
	debug("energy_datasize %lu\n",sizeof(unsigned long));
	*size=sizeof(unsigned long);
	return EAR_SUCCESS;
}

state_t energy_frequency(ulong *freq_us) {
	*freq_us = timeframe;
	return EAR_SUCCESS;
}

state_t energy_to_str(char *str, edata_t e) {
        ulong *pe = (ulong *) e;
        sprintf(str, "%lu", *pe);
        return EAR_SUCCESS;
}

unsigned long diff_node_energy(ulong init,ulong end)
{
  ulong ret=0;
  if (end>=init){
    ret=end-init;
  } else{
    ret=ulong_diff_overflow(init,end);
  }
  return ret;
}
state_t energy_units(uint *units) {
  *units = 1000;
  return EAR_SUCCESS;
}

state_t energy_accumulated(unsigned long *e, edata_t init, edata_t end) {
  int i;
  ulong *pinit = (ulong *) init, *pend = (ulong *) end;

  unsigned long total = diff_node_energy(*pinit, *pend);
  *e = total;
  return EAR_SUCCESS;
}



state_t energy_dc_read(void *c, edata_t energy_mj) {
	ulong *penergy_mj=(ulong *)energy_mj;

  debug("energy_dc_read\n");

  *penergy_mj=0;

	return EAR_SUCCESS;
}
