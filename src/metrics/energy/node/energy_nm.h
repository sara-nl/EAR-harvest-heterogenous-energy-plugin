/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef METRICS_NODE_IPMI_NM_H
#define METRICS_NODE_IPMI_NM_H

#define IPMI_BUF_SIZE	1024
#define IPMI_SYSTEM_INTERFACE_ADDR_TYPE	0x0c

// ipmitool raw 0x2e 0x82 0x66 0x4a 0 0 0 1 --> Command to get the parameter (0x20 in Lenovo) bytes_rq[6]
// sudo ./ipmi-raw 0x0 0x2e 0x82 0x66 0x4a 0 0 0 1 
// byte number 8 with ipmi-raw command
// bytes_rq[0]=(uint8_t)0x00; LUN
// bytes_rq[1]=(uint8_t)0x2E; NETFN
// bytes_rq[2]=(uint8_t)0x82; CMD
// bytes_rq[3]=(uint8_t)0x66;
// bytes_rq[4]=(uint8_t)0x4a;
// bytes_rq[5]=(uint8_t)0x00;
// bytes_rq[6]=(uint8_t)0x00;
// bytes_rq[7]=(uint8_t)0x00;
// bytes_rq[8]=(uint8_t)0x01;
// bytes_rq[0]=(uint8_t)0x00; LUN
// bytes_rq[1]=(uint8_t)0x2E; NETFN
// bytes_rq[2]=(uint8_t)0x81; CMD
// bytes_rq[3]=(uint8_t)0x66;
// bytes_rq[4]=(uint8_t)0x4A;
// bytes_rq[5]=(uint8_t)0x00;
// bytes_rq[6]=bytes_rs[8];
// bytes_rq[7]=(uint8_t)0x01;
// bytes_rq[8]=(uint8_t)0x82;
// bytes_rq[9]=(uint8_t)0x00;
// bytes_rq[10]=(uint8_t)0x08;
// energyp=(unsigned long *)&bytes_rs[rs_len-8];
// *energy=(unsigned long)be64toh(*energyp);

#define NM_NETFN	      0x2E
#define NM_CMD_GET_ARG	0x82
#define NM_CMD_ENERGY		0x81

struct ipmi_rq {
	struct {
		uint8_t netfn:6;
		uint8_t lun:2;
		uint8_t cmd;
		uint8_t target_cmd;
		uint16_t data_len;
		uint8_t *data;
	} msg;
};

struct ipmi_rs {
	int data_len;

	uint8_t ccode;
	uint8_t data[IPMI_BUF_SIZE];

	/*
	 * Looks like this is the length of the entire packet, including the RMCP
	 * stuff, then modified to be the length of the extra IPMI message data
	 */
//	int data_len;

	struct {
		uint8_t netfn;
		uint8_t cmd;
		uint8_t seq;
		uint8_t lun;
	} msg;

	struct {
		uint8_t authtype;
		uint32_t seq;
		uint32_t id;
		uint8_t bEncrypted;	/* IPMI v2 only */
		uint8_t bAuthenticated;	/* IPMI v2 only */
		uint8_t payloadtype;	/* IPMI v2 only */
		/* This is the total length of the payload or
		   IPMI message.  IPMI v2.0 requires this to
		   be 2 bytes.  Not really used for much. */
		uint16_t msglen;
	} session;

	/*
	 * A union of the different possible payload meta-data
	 */
	union {
		struct {
			uint8_t rq_addr;
			uint8_t netfn;
			uint8_t rq_lun;
			uint8_t rs_addr;
			uint8_t rq_seq;
			uint8_t rs_lun;
			uint8_t cmd;
		} ipmi_response;
		struct {
			uint8_t message_tag;
			uint8_t rakp_return_code;
			uint8_t max_priv_level;
			uint32_t console_id;
			uint32_t bmc_id;
			uint8_t auth_alg;
			uint8_t integrity_alg;
			uint8_t crypt_alg;
		} open_session_response;
		struct {
			uint8_t message_tag;
			uint8_t rakp_return_code;
			uint32_t console_id;
			uint8_t bmc_rand[16];	/* Random number generated by the BMC */
			uint8_t bmc_guid[16];
			uint8_t key_exchange_auth_code[20];
		} rakp2_message;
		struct {
			uint8_t message_tag;
			uint8_t rakp_return_code;
			uint32_t console_id;
			uint8_t integrity_check_value[20];
		} rakp4_message;
		struct {
			uint8_t packet_sequence_number;
			uint8_t acked_packet_number;
			uint8_t accepted_character_count;
			uint8_t is_nack;	/* bool */
			uint8_t transfer_unavailable;	/* bool */
			uint8_t sol_inactive;	/* bool */
			uint8_t transmit_overrun;	/* bool */
			uint8_t break_detected;	/* bool */
		} sol_packet;

	} payload;
};
struct valstr {
	uint16_t val;
	const char * str;
};

//#endif

//======for open and send=================================
#define IPMI_MAX_ADDR_SIZE    0x20
#define IPMI_BMC_CHANNEL    0x0f
#define IPMI_NUM_CHANNELS   0x10
#define IPMI_IPMB_ADDR_TYPE   0x01
#define IPMI_IPMB_BROADCAST_ADDR_TYPE 0x41

struct ipmi_system_interface_addr {
	int addr_type;
	short channel;
	unsigned char lun;
};

struct ipmi_ipmb_addr {
	int addr_type;
	short channel;
	unsigned char slave_addr;
	unsigned char lun;
};

struct ipmi_addr {
	int addr_type;
	short channel;
	char data[IPMI_MAX_ADDR_SIZE];
};

struct ipmi_msg {
	unsigned char netfn;
        unsigned char cmd;
        unsigned short data_len;
        unsigned char *data;
};

struct ipmi_req {
	unsigned char *addr;
	unsigned int addr_len;
	long msgid;
	struct ipmi_msg msg;
};

struct ipmi_recv {
	int recv_type;
	unsigned char *addr;
	unsigned int addr_len;
	long msgid;
	struct ipmi_msg msg;
};


#define IPMICTL_RECEIVE_MSG_TRUNC	_IOWR('i', 11, struct ipmi_recv)
#define IPMICTL_SEND_COMMAND            _IOR('i', 13, struct ipmi_req)
#define IPMICTL_RECEIVE_MSG		_IOWR('i', 12, struct ipmi_recv)
#define IPMICTL_SET_MY_ADDRESS_CMD	_IOR('i', 17, unsigned int)

struct ipmi_data{
	int mode;
	uint8_t data[256];
	int data_len;
};
struct ipmi_intf{
int fd;
uint8_t addr;
uint8_t channel;
};

#endif
