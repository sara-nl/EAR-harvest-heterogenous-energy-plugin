/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contwct Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contwct Luigi Brochard (lbrochard@lenovo.com)

*/

#include <intel_model_list.h>
#include <ear_verbose.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <errno.h>
#include <freeipmi/freeipmi.h>
#include <endian.h>
#define IPMI_RAW_MAX_ARGS (1024)


#define FUNCVERB(function)                               \
ear_debug(4, "ear_daemon(lenovo_wct) " function "\n");

static ipmi_ctx_t ipmi_ctx = NULL;
static struct ipmi_raw_arguments *args;
static uint8_t *bytes_rq = NULL;
static uint8_t *bytes_rs = NULL;
static unsigned int send_len;


/* Specific functions for CPU XX PLATFORM YY */
/* Grants access to ipmi device */
int lenovo_wct_node_energy_init()
{
	uid_t uid;
	int ret=0;
	int rs_len;
	unsigned int workaround_flags = 0;
	FUNCVERB("lenovo_node_energy_init");
	//Creating the context
	if (!(ipmi_ctx = ipmi_ctx_create ())){
        ear_verbose(0,"lenovo_water_cooling:Error in ipmi_ctx_create %s\n",strerror(errno));
		return -1;
	}
	// Checking for root
	uid = getuid ();
	if (uid != 0){ 
		ear_verbose(0,"lenovo_water_cooling: No root permissions\n");
		// Close context
		ipmi_ctx_close (ipmi_ctx);
		// delete context
		ipmi_ctx_destroy (ipmi_ctx);
		return -1;
	}
	// inband context
	if ((ret = ipmi_ctx_find_inband (ipmi_ctx, 
					NULL, // driver_type
					0, //disable_auto_probe
					0, // driver_address
					0, // register_spacing
					NULL, // driver_device
                    workaround_flags,
                    IPMI_FLAGS_DEFAULT)) < 0) {
		ear_verbose(0,"lenovo_water_cooling: %s\n",strerror(errno));
		// Close context
		ipmi_ctx_close (ipmi_ctx);
		// delete context
		ipmi_ctx_destroy (ipmi_ctx);
		return -1;	
	}
	if (ret==0){
		ear_verbose(0,"lenovo_water_cooling: Not inband device found\n");
		// Close context
		ipmi_ctx_close (ipmi_ctx);
		// delete context
		ipmi_ctx_destroy (ipmi_ctx);
		return -1;	
	}
	// This part is hardcoded since we are not supporting other commands rather than reading DC energy
	send_len=11;
	if (!(bytes_rq = calloc (send_len, sizeof (uint8_t))))
	{
		ear_verbose(0,"lenovo_water_cooling: Allocating memory for request %s\n",strerror(errno));
        // Close context
        ipmi_ctx_close (ipmi_ctx);
        // delete context
        ipmi_ctx_destroy (ipmi_ctx);
		return -1;
	}
	if (!(bytes_rs = calloc (IPMI_RAW_MAX_ARGS, sizeof (uint8_t))))
	{
		ear_verbose(0,"lenovo_water_cooling: Allocating memory for recv data %s\n",strerror(errno));
		// Close context
		ipmi_ctx_close (ipmi_ctx);
		// delete context
		ipmi_ctx_destroy (ipmi_ctx);
		return -1;
	}	
	// ipmitool raw 0x2e 0x82 0x66 0x4a 0 0 0 1 --> Command to get the parameter (0x20 in Lenovo) bytes_rq[6]
	// sudo ./ipmi-raw 0x0 0x2e 0x82 0x66 0x4a 0 0 0 1 
	// byte number 8 with ipmi-raw command
	bytes_rq[0]=(uint8_t)0x00;
    bytes_rq[1]=(uint8_t)0x2E;
    bytes_rq[2]=(uint8_t)0x82;
    bytes_rq[3]=(uint8_t)0x66;
    bytes_rq[4]=(uint8_t)0x4a;
    bytes_rq[5]=(uint8_t)0x00;
    bytes_rq[6]=(uint8_t)0x00;
    bytes_rq[7]=(uint8_t)0x00;
    bytes_rq[8]=(uint8_t)0x01;
    // RAW CMD to get the parameter
    if ((rs_len = ipmi_cmd_raw (ipmi_ctx,
                              bytes_rq[0],
                              bytes_rq[1],
                              &bytes_rq[2],
                              7,
                              bytes_rs,
                              IPMI_RAW_MAX_ARGS)) < 0)
    {
        ear_verbose(0,"lenovo_water_cooling: ipmi_cmd_raw fails\n");
        return -1;
    }
	// sudo ./ipmi-raw 0x0 0x2e 0x81 0x66 0x4a 0x00 0x20 0x01 0x82 0x0 0x08
	// Robert command: ipmitool -I lanplus -H 10.240.43.92 -U USERID -P PASSW0RD raw 0x3a 0x32 4 1 0 0 0
	// How that fits into ipmi-raw inband command???
	bytes_rq[0]=(uint8_t)0x00;
	bytes_rq[1]=(uint8_t)0x2E;
	bytes_rq[2]=(uint8_t)0x81;
	bytes_rq[3]=(uint8_t)0x66;
	bytes_rq[4]=(uint8_t)0x4A;
	bytes_rq[5]=(uint8_t)0x00;
	bytes_rq[6]=bytes_rs[8];
	bytes_rq[7]=(uint8_t)0x01;
	bytes_rq[8]=(uint8_t)0x82;
	bytes_rq[9]=(uint8_t)0x00;
	bytes_rq[10]=(uint8_t)0x08;

	return 0;	
		
}
int lenovo_wct_count_energy_data_length()
{
	int ret;
	FUNCVERB("lenovo_count_energy_data_length");
	return sizeof(unsigned long);
}
int lenovo_wct_read_dc_energy(unsigned long *energy)
{
	int ret;
	unsigned long *energyp;
	int rs_len;
	if (ipmi_ctx==NULL){ 
		ear_verbose(0,"lenovo_water_cooling: IPMI context not initiallized\n");
		return -1;
	}
	FUNCVERB("lenovo_read_dc_energy");
	ear_verbose(0,"lenovo_water_cooling not yet supported\n");
	// RAW CMD
	*energy=0;
	return 0;
	if ((rs_len = ipmi_cmd_raw (ipmi_ctx,
                              bytes_rq[0],
                              bytes_rq[1],
                              &bytes_rq[2],
                              send_len - 2,
                              bytes_rs,
                              IPMI_RAW_MAX_ARGS)) < 0)
    {
		ear_verbose(0,"lenovo_water_cooling: ipmi_cmd_raw fails\n");
		return -1;
	}
	energyp=(unsigned long *)&bytes_rs[rs_len-8];
	*energy=(unsigned long)be64toh(*energyp);
	return 0;
}
/* AC energy is not yet supported */
int lenovo_wct_read_ac_energy(unsigned long *energy)
{
	*energy=0;
	return 0;
}
/* Release access to ipmi device */
int lenovo_wct_node_energy_dispose()
{
	int ret;
	FUNCVERB("lenovo_node_energy_dispose");
	if (ipmi_ctx==NULL){ 
		ear_verbose(0,"lenovo_water_cooling: IPMI context not initiallized\n");
		return -1;
	}
	// // Close context
	ipmi_ctx_close (ipmi_ctx);
	// delete context
	ipmi_ctx_destroy (ipmi_ctx);
	return 0;
}

