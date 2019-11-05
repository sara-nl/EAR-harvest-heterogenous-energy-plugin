/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * *
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * *
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING
 * */

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>


#include <common/states.h>
#include <pthread.h>
#include <metrics/api/energy.h>
#include <ipmi_nm.h>
#include <math.h>

#define SHOW_DEBUGS 1
#ifdef SHOW_DEBUGS
#define debug(...) fprintf(stderr, __VA_ARGS__);
#else
#define debug(...)
#endif

static pthread_mutex_t ompi_lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t cmd_arg;

int opendev(struct ipmi_intf *intf){
	intf->fd = open("/dev/ipmi0", O_RDWR);
	if (intf->fd < 0) {
		intf->fd = open("/dev/ipmi/0", O_RDWR);
		if (intf->fd < 0) {
			intf->fd = open("/dev/ipmidev/0", O_RDWR);
		};
	};
	return intf->fd;
};

void closedev(struct ipmi_intf * intf){
	if (intf->fd > 0){
		close(intf->fd);
		intf->fd = -1 ;
	};
};

static struct ipmi_rs * sendcmd(struct ipmi_intf * intf, struct ipmi_rq * req)
{
	struct ipmi_req _req;
	struct ipmi_recv recv;
	struct ipmi_addr addr;
	struct ipmi_system_interface_addr bmc_addr = {
			addr_type:	IPMI_SYSTEM_INTERFACE_ADDR_TYPE,
			channel:	IPMI_BMC_CHANNEL,
	};
	struct ipmi_ipmb_addr ipmb_addr = {
			addr_type:	IPMI_IPMB_ADDR_TYPE,
			channel:	intf->channel & 0x0f,
	};
	static struct ipmi_rs rsp;
	uint8_t * data = NULL;
	int data_len = 0;
	static int curr_seq = 0;
	fd_set rset;

	if (intf == NULL || req == NULL)
		return NULL;
	memset(&_req, 0, sizeof(struct ipmi_req));

	if (intf->addr != 0){
		ipmb_addr.slave_addr = intf->addr;
		ipmb_addr.lun = req->msg.lun;
		_req.addr = (unsigned char *) &ipmb_addr;
		_req.addr_len = sizeof(ipmb_addr);

	}else {
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

	if (select(intf->fd+1, &rset, NULL, NULL, NULL) < 0) {
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

	if( recv.msg.data[0] == 0 ) {

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
};//sendcmd


state_t nm_arg(struct ipmi_intf *intf, struct ipmi_data *out)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t msg_data[6];
	debug("getting nm_arg\n");
// ipmitool raw 0x2e 0x82 0x66 0x4a 0 0 0 1 --> Command to get the parameter (0x20 in Lenovo) bytes_rq[6]
// // sudo ./ipmi-raw 0x0 0x2e 0x82 0x66 0x4a 0 0 0 1
// // byte number 8 with ipmi-raw command

//// bytes_rq[3]=(uint8_t)0x66;
//// bytes_rq[4]=(uint8_t)0x4a;
//// bytes_rq[5]=(uint8_t)0x00;
//// bytes_rq[6]=(uint8_t)0x00;
//// bytes_rq[7]=(uint8_t)0x00;
//// bytes_rq[8]=(uint8_t)0x01;
//
	memset(&req, 0, sizeof(req));
	req.msg.netfn = NM_NETFN;
	req.msg.cmd = NM_CMD_GET_ARG;
	msg_data[0]=(uint8_t)0x66;
	msg_data[1]=(uint8_t)0x4a;
	msg_data[2]=(uint8_t)0x00;
	msg_data[3]=(uint8_t)0x00;
	msg_data[4]=(uint8_t)0x00;
	msg_data[5]=(uint8_t)0x01;
	intf->addr=0x0;
	req.msg.data = msg_data;
	req.msg.data_len = sizeof(msg_data);
	debug("sending command\n");
	rsp = sendcmd(intf, &req);
	if (rsp == NULL) {
		out->mode=-1;
		debug("Error rsp null\n");
		return EAR_ERROR;
	};
	if (rsp->ccode > 0) {
		out->mode=-1;
		debug("error code>0\n");
		return EAR_ERROR;
	};

	out->data_len=rsp->data_len;
	int i;
	for (i=0;i<rsp->data_len; i++) {
		out->data[i]=rsp->data[i];
		debug("cmd arg byte %d is %hu\n",i,out->data[i]);
	}
	debug("nm_arg ok\n");
	return EAR_SUCCESS;
}

state_t nm_ene(struct ipmi_intf *intf,struct ipmi_data * out)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t msg_data[8];
	if (pthread_mutex_trylock(&ompi_lock)) {
		return EAR_BUSY;
	}

//// bytes_rq[3]=(uint8_t)0x66;
//// bytes_rq[4]=(uint8_t)0x4A;
//// bytes_rq[5]=(uint8_t)0x00;
//// bytes_rq[6]=bytes_rs[8];
//// bytes_rq[7]=(uint8_t)0x01;
//// bytes_rq[8]=(uint8_t)0x82;
//// bytes_rq[9]=(uint8_t)0x00;
//// bytes_rq[10]=(uint8_t)0x08;

	memset(&req, 0, sizeof(req));
	req.msg.netfn = NM_NETFN;
	req.msg.cmd = NM_CMD_ENERGY;
	msg_data[0]=(uint8_t)0x66;
	msg_data[1]=(uint8_t)0x4A;
	msg_data[2]=(uint8_t)0x00;
	msg_data[3]=cmd_arg;
	msg_data[4]=(uint8_t)0x01;
	msg_data[5]=(uint8_t)0x82;
	msg_data[6]=(uint8_t)0x00;
	msg_data[7]=(uint8_t)0x08;
	intf->addr=0x0;
	req.msg.data = msg_data;
	req.msg.data_len = sizeof(msg_data);

	rsp = sendcmd(intf, &req);
	if (rsp == NULL) {
		out->mode=-1;
		pthread_mutex_unlock(&ompi_lock);
		return EAR_ERROR;
	};
	if (rsp->ccode > 0) {
		out->mode=-1;
		pthread_mutex_unlock(&ompi_lock);
		return EAR_ERROR;
	};

	out->data_len=rsp->data_len;
	int i;
	for (i=0;i<rsp->data_len; i++) {
		out->data[i]=rsp->data[i];
	}
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;
};

/*
 * MAIN FUNCTIONS
 */
//#define CMD_ARG_BYTE	8
#define CMD_ARG_BYTE	6
state_t energy_init(void **c)
{
	int ret;
	struct ipmi_data out;
	state_t st;
	if (c==NULL) return EAR_ERROR;
	*c=(struct ipmi_intf *)malloc(sizeof(struct ipmi_intf));
	if (*c==NULL) return EAR_ERROR;
	pthread_mutex_lock(&ompi_lock);
	debug("trying opendev\n");
	ret= opendev((struct ipmi_intf *)*c);
	if (ret<0){
		debug("opendev fails\n");
		pthread_mutex_unlock(&ompi_lock);
		return EAR_ERROR;
	}
	st=nm_arg((struct ipmi_intf *)*c,&out);
	if (st!=EAR_SUCCESS){
		debug("nm fails\n");
		pthread_mutex_unlock(&ompi_lock);
		return st;
	}
	cmd_arg=out.data[CMD_ARG_BYTE];
	debug("cmd arg is %hu\n",cmd_arg);
	pthread_mutex_unlock(&ompi_lock);
	return EAR_SUCCESS;
}
state_t energy_dispose(void **c)
{
	if ((c==NULL) || (*c==NULL)) return EAR_ERROR;
	pthread_mutex_lock(&ompi_lock);
	closedev((struct ipmi_intf *)*c);
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
state_t energy_frequency(ulong *freq_us)
{
	*freq_us=1000000;
	return EAR_SUCCESS;
}

state_t energy_dc_read(void *c, edata_t energy_mj)
{
	struct ipmi_data out;
	unsigned long aux_emj = 0,*energyp;
	uint8_t *bytes_rs;
	int FIRST_BYTE_EMJ;
	state_t st;
	ulong *penergy_mj=(ulong *)energy_mj;

	debug("energy_dc_read\n");

	*penergy_mj=0;
	st=nm_ene((struct ipmi_intf *)c,&out);
	if (st!=EAR_SUCCESS) return st;
	FIRST_BYTE_EMJ=out.data_len-8;
	bytes_rs=out.data;
	energyp=(unsigned long *)&bytes_rs[FIRST_BYTE_EMJ];
	*penergy_mj=(unsigned long)be64toh(*energyp);
	return EAR_SUCCESS;
}


state_t energy_dc_time_read(void *c, edata_t energy_mj, ulong *time_ms)
{
	struct ipmi_data out;
	unsigned long aux_emj = 0,*energy,*energyp;
	int FIRST_BYTE_EMJ;
	uint8_t *bytes_rs;
	state_t st;
	struct timeval t;
	ulong *penergy_mj=(ulong *)energy_mj;

	*penergy_mj=0;
	*time_ms=0;
	st=nm_ene((struct ipmi_intf *)c,&out);
	if (st!=EAR_SUCCESS) return st;
	bytes_rs=out.data;
	FIRST_BYTE_EMJ=out.data_len-8;
	energyp=(unsigned long *)&bytes_rs[FIRST_BYTE_EMJ];
	*penergy_mj=(unsigned long)be64toh(*energyp);
	gettimeofday(&t, NULL);
	*time_ms=t.tv_sec*1000+t.tv_usec/1000;
	return EAR_SUCCESS;
}
state_t energy_ac_read(void *c, edata_t energy_mj)
{
	ulong *penergy_mj=(ulong *)energy_mj;
	*penergy_mj=0;
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

state_t energy_to_str(char *str,edata_t e)
{
	ulong *pe=(ulong *)e;
	sprintf(str,"%lu",*pe);
	return EAR_SUCCESS;
}