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
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <freeipmi/freeipmi.h>
#include <pthread.h>

#include <common/config.h>
#include <common/output/debug.h>
#include <common/output/verbose.h>
#include <metrics/custom/hardware_info.h>
#include <metrics/ipmi/energy_node.h>
#include <metrics/ipmi/energy_node/ibm.h>
#include <metrics/ipmi/energy_node/lenovo_nm.h>
#include <metrics/ipmi/energy_node/lenovo_sd650.h>

#if 0
static int ear_energy_node_connected=0;
static int ear_energy_node_status=0;
#endif
static pthread_mutex_t generic_energy_node_lock = PTHREAD_MUTEX_INITIALIZER;

#define NODE_MANAGER	0
#define SD650_HFR			1

static int energy_interface=NODE_MANAGER;
struct node_energy_op
{
	int (*node_energy_init) (ipmi_ctx_t *ctx);
	int (*count_energy_data_length)(ipmi_ctx_t ctx);
	int (*read_dc_energy) (ipmi_ctx_t ctx,ulong *energy);
	int (*read_dc_energy_time) (ipmi_ctx_t ctx,ulong *energy,ulong *time);
	int (*read_dc_energy_and_time) (ipmi_ctx_t ctx,ulong *energy_j,ulong *energy_mj,ulong *time_s,ulong *time_ms);
	int (*read_ac_energy) (ipmi_ctx_t ctx,ulong *energy);
	int (*node_energy_dispose) (ipmi_ctx_t *ctx);
} node_energy_ops;

// FRU data for Product name
#define IPMI_FRU_CUSTOM_FIELDS 64
static uint8_t areabuf[IPMI_FRU_AREA_SIZE_MAX+1];
static unsigned int area_type = 0;
static unsigned int area_length = 0;
static char my_p_manufacturer_name[IPMI_FRU_AREA_STRING_MAX+1];
static char my_p_name[IPMI_FRU_AREA_STRING_MAX+1];
// End FRU data

// This functios checks the product name using ipmi interface
int ipmi_get_product_name(char *my_product_manufacturer_name,char *my_product_name)
{
	ipmi_ctx_t ipmi_ctx_pn = NULL;
	ipmi_fru_ctx_t fru_ctx_pn=NULL;
	unsigned int workaround_flags = 0;
	uid_t uid;
	int ret;
	int ready=0;
	uint8_t language_code;
	ipmi_fru_field_t product_manufacturer_name;
	ipmi_fru_field_t product_name;
	ipmi_fru_field_t product_part_model_number;
	ipmi_fru_field_t product_version;
	ipmi_fru_field_t product_serial_number;
	ipmi_fru_field_t product_asset_tag;
	ipmi_fru_field_t product_fru_file_id;
	ipmi_fru_field_t product_custom_fields[IPMI_FRU_CUSTOM_FIELDS];
	
	
	//Creating the context
	if (!(ipmi_ctx_pn = ipmi_ctx_create ())){
        verbose(0,"ear_daemon Error in ipmi_ctx_create for product name %s\n",strerror(errno));
		return -1;
	}
	
	// Checking for root
	uid = getuid ();
	if (uid != 0){
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	
	// creating the inband context
	
	if ((ret = ipmi_ctx_find_inband (ipmi_ctx_pn,
                                        	NULL, // driver_type
                                        	0, //disable_auto_probe
                                        	0, // driver_address
                                        	0, // register_spacing
                                        	NULL, // driver_device
                                           	workaround_flags,
                                           	IPMI_FLAGS_DEFAULT)) < 0) {
        verbose(0,"ipmi_ctx_find_inband for product name: %s\n", ipmi_ctx_errormsg (ipmi_ctx_pn));
        // Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// We have here a context
	// Creating a fru context
	if (!(fru_ctx_pn=ipmi_fru_ctx_create(ipmi_ctx_pn))){
        verbose(0,"ipmi_fru_ctx_create for product name: %s\n", ipmi_ctx_errormsg (ipmi_ctx_pn));
        // Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	if (ipmi_fru_open_device_id (fru_ctx_pn, 0) < 0){
    	verbose(0,"ipmi_fru_open_device_id %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
        	// Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// FRU returns a list 
	if (ipmi_fru_first(fru_ctx_pn)<0){
    	verbose(0,"ipmi_fru_first %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// We iterate until PRODUCT info is processed	
	do {
	memset(areabuf, '\0', IPMI_FRU_AREA_SIZE_MAX + 1);
	if (ipmi_fru_read_data_area(fru_ctx_pn,&area_type,&area_length,areabuf,IPMI_FRU_AREA_SIZE_MAX) < 0){
    	verbose(0,"ipmi_fru_read_data_area %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
        	// Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	ready=0;
	if (area_length){
    	switch(area_type){
    	case IPMI_FRU_AREA_TYPE_PRODUCT_INFO_AREA:
        	memset (&product_manufacturer_name, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_name, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_part_model_number, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_version, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_serial_number, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_asset_tag, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_fru_file_id, '\0', sizeof (ipmi_fru_field_t));
        	memset (&product_custom_fields[0],
                	'\0',
                	sizeof (ipmi_fru_field_t) * IPMI_FRU_CUSTOM_FIELDS);
          	if (ipmi_fru_product_info_area (fru_ctx_pn,
                                  	areabuf,
                                  	area_length,
                                  	&language_code,
                                  	&product_manufacturer_name,
                                  	&product_name,
                                  	&product_part_model_number,
                                  	&product_version,
                                  	&product_serial_number,
                                  	&product_asset_tag,
                                  	&product_fru_file_id,
                                  	product_custom_fields,
                                  	IPMI_FRU_CUSTOM_FIELDS) < 0){
            	verbose(0,"ipmi_fru_product_info_area %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
                ipmi_ctx_close (ipmi_ctx_pn);
                ipmi_ctx_destroy (ipmi_ctx_pn);
				return -1;
        	}
           	// Here we have data , just print it 
        	char strbuf[IPMI_FRU_AREA_STRING_MAX + 1];
        	unsigned int strbuflen = IPMI_FRU_AREA_STRING_MAX;
        	// product_manufacturer_name
        	if (product_manufacturer_name.type_length_field_length>0){
            	memset (strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);
            	if (ipmi_fru_type_length_field_to_string (fru_ctx_pn,
                                            	product_manufacturer_name.type_length_field,
                                            	product_manufacturer_name.type_length_field_length,
                                            	language_code,
                                            	strbuf,
                                            	&strbuflen) < 0){
                	verbose(0,"ipmi_fru_type_length_field_to_string error\n");
            	}
            	if (strbuflen){ 
					verbose(1,"FRU Product Manufacturer name: %s\n",strbuf);
					strcpy(my_product_manufacturer_name,strbuf);
				}
	
        	}else{
            	verbose(0,"ipmi_fru_product_info_area returns 0 size for product_manufacturer_name\n");
        	}
        	// product_name
        	strbuflen = IPMI_FRU_AREA_STRING_MAX;
            if (product_name.type_length_field_length>0){
            	memset (strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);
               	if (ipmi_fru_type_length_field_to_string (fru_ctx_pn,
                                            	product_name.type_length_field,
                                            	product_name.type_length_field_length,
                                            	language_code,
                                            	strbuf,
                                            	&strbuflen) < 0){
               		verbose(0,"ipmi_fru_type_length_field_to_string error\n");
                }
                if (strbuflen){ 
					verbose(1,"FRU Product name: %s\n",strbuf);
					strcpy(my_product_name,strbuf);
				}
           }else{
           		verbose(0,"ipmi_fru_product_info_area returns 0 size for product_name\n");
           }
        	ready=1;
        	break;
    	default:
        	break;
    	}
	}
	}while((!ready) && (ret = ipmi_fru_next (fru_ctx_pn)) == 1);
	// END PRODUCT NAME
	ipmi_ctx_close (ipmi_ctx_pn);
	ipmi_ctx_destroy (ipmi_ctx_pn);
	return 0;
}



int node_energy_init(energy_handler_t *eh)
{
	int cpu_model;
	int ret;

	if (eh->ear_energy_node_connected) return eh->ear_energy_node_status;

    debug("node_energy_init");
	pthread_mutex_lock(&generic_energy_node_lock);
	if ((ret=ipmi_get_product_name(my_p_manufacturer_name,my_p_name))<0){
		node_energy_ops.node_energy_init=NULL;
		node_energy_ops.count_energy_data_length=NULL;
		node_energy_ops.read_dc_energy=NULL;
		node_energy_ops.read_dc_energy_time=NULL;
		node_energy_ops.read_ac_energy=NULL;
		node_energy_ops.node_energy_dispose=NULL;
		pthread_mutex_unlock(&generic_energy_node_lock);
		
		return -1;
	}
	
	cpu_model=get_model();
	switch (cpu_model) {
    case CPU_HASWELL_X:
    case CPU_BROADWELL_X:
		// Should we check the product name??
		if (strstr(my_p_name,"nx360")!=NULL){
			verbose(0,"Product name %s detected \n",my_p_name);
		} else{
			verbose(0,"Product name %s detected (Not known. default applied) \n",my_p_name);
		}
		node_energy_ops.node_energy_init=ibm_node_energy_init;
		node_energy_ops.count_energy_data_length=ibm_count_energy_data_length;
		node_energy_ops.read_dc_energy=ibm_read_dc_energy;
		node_energy_ops.read_dc_energy_time=ibm_read_dc_energy_time;
		node_energy_ops.read_dc_energy_and_time=ibm_read_dc_energy_and_time;
		node_energy_ops.read_ac_energy=ibm_read_ac_energy;
		node_energy_ops.node_energy_dispose=ibm_node_energy_dispose;
		break;	
    case CPU_SKYLAKE_X:
		// we should check for specific models 
		if (strstr(my_p_name,"SD530")!=NULL){
			// It is an air colling system
			verbose(0,"Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_act_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_act_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_act_read_dc_energy;
			node_energy_ops.read_dc_energy_time=lenovo_act_read_dc_energy_time;
			node_energy_ops.read_dc_energy_and_time=lenovo_act_read_dc_energy_and_time;
			node_energy_ops.read_ac_energy=lenovo_act_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_act_node_energy_dispose;
		}else if (strstr(my_p_name,"SR650")!=NULL){
			// It is an air colling system
			verbose(0,"Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_act_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_act_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_act_read_dc_energy;
			node_energy_ops.read_dc_energy_time=lenovo_act_read_dc_energy_time;
			node_energy_ops.read_dc_energy_and_time=lenovo_act_read_dc_energy_and_time;
			node_energy_ops.read_ac_energy=lenovo_act_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_act_node_energy_dispose;
		}else if (strstr(my_p_name,"SD650")!=NULL){
			//OceanCat
			energy_interface=SD650_HFR;
			verbose(0,"Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_wct_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_wct_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_wct_read_dc_energy;
			node_energy_ops.read_dc_energy_time=lenovo_wct_read_dc_energy_time;
			node_energy_ops.read_dc_energy_and_time=lenovo_wct_read_dc_energy_and_time;
			node_energy_ops.read_ac_energy=lenovo_wct_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_wct_node_energy_dispose;
		}else{
			verbose(0,"Product name %s detected (Not Known, using default) \n",my_p_name);
            node_energy_ops.node_energy_init=lenovo_act_node_energy_init;
            node_energy_ops.count_energy_data_length=lenovo_act_count_energy_data_length;
            node_energy_ops.read_dc_energy=lenovo_act_read_dc_energy;
            node_energy_ops.read_dc_energy_time=lenovo_act_read_dc_energy_time;
			node_energy_ops.read_dc_energy_and_time=lenovo_act_read_dc_energy_and_time;
            node_energy_ops.read_ac_energy=lenovo_act_read_ac_energy;
            node_energy_ops.node_energy_dispose=lenovo_act_node_energy_dispose;
		}
		break;
	default:
		break;
	}
	eh->ear_energy_node_status=node_energy_ops.node_energy_init(&eh->ctx);
	eh->ear_energy_node_connected=1;
	pthread_mutex_unlock(&generic_energy_node_lock);
  return eh->ear_energy_node_status;
}
int count_energy_data_length(energy_handler_t *eh)
{
	debug("count_energy_data_length");
	if (node_energy_ops.count_energy_data_length!=NULL) return node_energy_ops.count_energy_data_length(eh->ctx);
	else return 0;
}
int read_dc_energy(energy_handler_t *eh,unsigned long *energy)
{
	debug("read_dc_energy");
	if (node_energy_ops.read_dc_energy!=NULL) return node_energy_ops.read_dc_energy(eh->ctx,energy);
	else{	
		*energy=0;
		return -1;
	}
}
int read_dc_energy_try(energy_handler_t *eh,ulong *energy)
{
	int tries=0;
	int ret;
	*energy=0;
	do{
		ret=read_dc_energy(eh,energy);
		tries++;
	}while((ret==EAR_BUSY) && (tries<MAX_NODE_ENERGY_LOCK_TRIES));
	return ret;
}

int read_dc_energy_time(energy_handler_t *eh,ulong *energy,ulong *time_ms)
{
    debug("read_dc_energy_time");
    if (node_energy_ops.read_dc_energy_time!=NULL) return node_energy_ops.read_dc_energy_time(eh->ctx,energy,time_ms);
    else{
        *energy=0;
				*time_ms=0;
        return -1;
    }
}

int read_dc_energy_time_try(energy_handler_t *eh,ulong *energy,ulong *time_ms)
{
	int tries=0;
	int ret;
	*energy=0;*time_ms=0;
	do{
		ret=read_dc_energy_time(eh,energy,time_ms);
		tries++;
	}while((ret==EAR_BUSY) && (tries<MAX_NODE_ENERGY_LOCK_TRIES));
	return ret;
}

int read_dc_energy_time_debug(energy_handler_t *eh,ulong *energy_j,ulong *energy_mj,ulong *time_sec,ulong *time_ms)
{
    debug("read_dc_energy_time_debug");
    if (node_energy_ops.read_dc_energy_and_time!=NULL) return node_energy_ops.read_dc_energy_and_time(eh->ctx,energy_j,energy_mj,time_sec,time_ms);
    else{
        *energy_j=0;
        *energy_mj=0;
				*time_sec=0;
        *time_ms=0;
        return -1;
    }

}

int read_dc_energy_time_debug_try(energy_handler_t *eh,ulong *energy_j,ulong *energy_mj,ulong *time_sec,ulong *time_ms)
{
	int tries=0;
	int ret;
  *energy_j=0; *energy_mj=0; *time_sec=0; *time_ms=0;
	do{
		ret=read_dc_energy_time_debug(eh,energy_j,energy_mj,time_sec,time_ms);
		tries++;
	}while((ret==EAR_BUSY) && (tries<MAX_NODE_ENERGY_LOCK_TRIES));
	return ret;
}


int read_ac_energy(energy_handler_t *eh,unsigned long *energy)
{
	debug("read_ac_energy");
	if (node_energy_ops.read_ac_energy!=NULL) return node_energy_ops.read_ac_energy(eh->ctx,energy);
	else{
		*energy=0;
		return -1;
	}
}
int node_energy_dispose(energy_handler_t *eh)
{
	debug("node_energy_dispose");
	eh->ear_energy_node_connected=0;
	if (node_energy_ops.node_energy_dispose!=NULL) return node_energy_ops.node_energy_dispose(&eh->ctx);
	else return -1;
}

unsigned long node_energy_frequency(energy_handler_t *eh)
{
	unsigned long init, end,min_interval;
	struct timeval begin_time,end_time;
	int intents = 0;


	if (node_energy_ops.read_dc_energy != NULL)
	{
		node_energy_ops.read_dc_energy(eh->ctx,&init);

		do {
			node_energy_ops.read_dc_energy(eh->ctx,&end);
			intents++;
		} while((init == end) && (intents < 5000));

		if (intents == 5000) return 0;

		gettimeofday(&begin_time,NULL);
		init = end;

		do{
			node_energy_ops.read_dc_energy(eh->ctx,&end);
		} while(init == end);

		gettimeofday(&end_time,NULL);
		min_interval  = (end_time.tv_sec * 1000000 + end_time.tv_usec);
		min_interval -= (begin_time.tv_sec *1000000 + begin_time.tv_usec);
		switch (energy_interface){
		case NODE_MANAGER:
			min_interval=1000000;
			break;
		case SD650_HFR:
			min_interval=10000;
			break;
		}		
		min_interval = (min_interval / 2) / MAX_POWER_ERROR;


		return min_interval;
	}
	return 0;
}
