/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <freeipmi/freeipmi.h>

#include <metrics/custom/hardware_info.h>
#include <metrics/ipmi/energy_node/lenovo_act.h>
#include <metrics/ipmi/energy_node/lenovo_wct.h>
#include <metrics/ipmi/energy_node/ibm.h>
#include <common/ear_verbose.h>

#define FUNCVERB(function)                               \
    ear_debug(4, "ear_daemon(node_energy) " function "\n");

struct node_energy_op
{
    int (*node_energy_init) ();
	int (*count_energy_data_length)();
    int (*read_dc_energy) (unsigned long *energy);
    int (*read_ac_energy) (unsigned long *energy);
    int (*node_energy_dispose) ();
} node_energy_ops;

// FRU data for Product name
#define IPMI_FRU_CUSTOM_FIELDS 64
uint8_t areabuf[IPMI_FRU_AREA_SIZE_MAX+1];
unsigned int area_type = 0;
unsigned int area_length = 0;
char my_p_manufacturer_name[IPMI_FRU_AREA_STRING_MAX+1];
char my_p_name[IPMI_FRU_AREA_STRING_MAX+1];
// End FRU data

// This functios checks the product name using ipmi interface
int ipmi_get_product_name(char *my_product_manufacturer_name,char *my_product_name)
{
	ipmi_ctx_t ipmi_ctx_pn = NULL;
	ipmi_fru_ctx_t fru_ctx_pn=NULL;
	ipmi_sdr_ctx_t sdr_ctx_pn=NULL;
	unsigned int workaround_flags = 0;
	uid_t uid;
	int i;
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
        ear_verbose(0,"ear_daemon Error in ipmi_ctx_create for product name %s\n",strerror(errno));
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
        ear_verbose(0,"ear_daemon: ipmi_ctx_find_inband for product name: %s\n", ipmi_ctx_errormsg (ipmi_ctx_pn));
        // Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// We have here a context
	// Creating a fru context
	if (!(fru_ctx_pn=ipmi_fru_ctx_create(ipmi_ctx_pn))){
        ear_verbose(0,"ear_daemon: ipmi_fru_ctx_create for product name: %s\n", ipmi_ctx_errormsg (ipmi_ctx_pn));
        // Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	if (ipmi_fru_open_device_id (fru_ctx_pn, 0) < 0){
    	ear_verbose(0,"ear_daemon: ipmi_fru_open_device_id %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
        	// Close context
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// FRU returns a list 
	if (ipmi_fru_first(fru_ctx_pn)<0){
    	ear_verbose(0,"ear_daemon: ipmi_fru_first %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
        ipmi_ctx_close (ipmi_ctx_pn);
        ipmi_ctx_destroy (ipmi_ctx_pn);
		return -1;
	}
	// We iterate until PRODUCT info is processed	
	do {
	memset(areabuf, '\0', IPMI_FRU_AREA_SIZE_MAX + 1);
	if (ipmi_fru_read_data_area(fru_ctx_pn,&area_type,&area_length,areabuf,IPMI_FRU_AREA_SIZE_MAX) < 0){
    	ear_verbose(0,"ear_daemon: ipmi_fru_read_data_area %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
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
            	ear_verbose(0,"ear_daemon: ipmi_fru_product_info_area %s\n",ipmi_fru_ctx_errormsg(fru_ctx_pn));
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
                	ear_verbose(0,"ear_daemon: ipmi_fru_type_length_field_to_string error\n");
            	}
            	if (strbuflen){ 
					ear_verbose(1,"ear_daemon: FRU Product Manufacturer name: %s\n",strbuf);
					strcpy(my_product_manufacturer_name,strbuf);
				}
	
        	}else{
            	ear_verbose(0,"ear_daemon: ipmi_fru_product_info_area returns 0 size for product_manufacturer_name\n");
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
               		ear_verbose(0,"ear_daemon: ipmi_fru_type_length_field_to_string error\n");
                }
                if (strbuflen){ 
					ear_verbose(1,"ear_daemon: FRU Product name: %s\n",strbuf);
					strcpy(my_product_name,strbuf);
				}
           }else{
           		ear_verbose(0,"ear_daemon:ipmi_fru_product_info_area returns 0 size for product_name\n");
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



int node_energy_init()
{
	int cpu_model;
	int ret;

    FUNCVERB("node_energy_init");
	if ((ret=ipmi_get_product_name(my_p_manufacturer_name,my_p_name))<0){
		return -1;
	}
	
	cpu_model=get_model();
	switch (cpu_model) {
    case CPU_HASWELL_X:
    case CPU_BROADWELL_X:
		// Should we check the product name??
		if (strstr(my_p_name,"nx360")!=NULL){
			ear_verbose(0,"ear_daemon: Product name %s detected \n",my_p_name);
		} else{
			ear_verbose(0,"ear_daemon: Product name %s detected (Not known. default applied) \n",my_p_name);
		}
		node_energy_ops.node_energy_init=ibm_node_energy_init;
		node_energy_ops.count_energy_data_length=ibm_count_energy_data_length;
		node_energy_ops.read_dc_energy=ibm_read_dc_energy;
		node_energy_ops.read_ac_energy=ibm_read_ac_energy;
		node_energy_ops.node_energy_dispose=ibm_node_energy_dispose;
		break;	
    case CPU_SKYLAKE_X:
		// we should check for specific models 
		if (strstr(my_p_name,"SD530")!=NULL){
			// It is an air colling system
			ear_verbose(0,"ear_daemon: Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_act_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_act_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_act_read_dc_energy;
			node_energy_ops.read_ac_energy=lenovo_act_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_act_node_energy_dispose;
		}else if (strstr(my_p_name,"SR650")!=NULL){
			// It is an air colling system
			ear_verbose(0,"ear_daemon: Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_act_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_act_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_act_read_dc_energy;
			node_energy_ops.read_ac_energy=lenovo_act_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_act_node_energy_dispose;
		}else if (strstr(my_p_name,"SD650")!=NULL){
			//OceanCat
			ear_verbose(0,"ear_daemon: Product name %s detected \n",my_p_name);
			node_energy_ops.node_energy_init=lenovo_wct_node_energy_init;
			node_energy_ops.count_energy_data_length=lenovo_wct_count_energy_data_length;
			node_energy_ops.read_dc_energy=lenovo_wct_read_dc_energy;
			node_energy_ops.read_ac_energy=lenovo_wct_read_ac_energy;
			node_energy_ops.node_energy_dispose=lenovo_wct_node_energy_dispose;
		}else{
			ear_verbose(0,"ear_daemon: Product name %s detected (Not supported) \n",my_p_name);
			node_energy_ops.node_energy_init=NULL;
			node_energy_ops.count_energy_data_length=NULL;
			node_energy_ops.read_dc_energy=NULL;
			node_energy_ops.read_ac_energy=NULL;
			node_energy_ops.node_energy_dispose=NULL;
		}
		break;
	default:
		break;
	}
    return node_energy_ops.node_energy_init();
}
int count_energy_data_length()
{
	FUNCVERB("count_energy_data_length");
	if (node_energy_ops.count_energy_data_length!=NULL) return node_energy_ops.count_energy_data_length();
	else return 0;
}
int read_dc_energy(unsigned long *energy)
{
	FUNCVERB("read_dc_energy");
	if (node_energy_ops.read_dc_energy!=NULL) return node_energy_ops.read_dc_energy(energy);
	else{	
		*energy=0;
		return -1;
	}
}
int read_ac_energy(unsigned long *energy)
{
	FUNCVERB("read_ac_energy");
	if (node_energy_ops.read_ac_energy!=NULL) return node_energy_ops.read_ac_energy(energy);
	else{
		*energy=0;
		return -1;
	}
}
int node_energy_dispose()
{
	FUNCVERB("node_energy_dispose");
	if (node_energy_ops.node_energy_dispose!=NULL) return node_energy_ops.node_energy_dispose();
	else return -1;
}

unsigned long node_energy_frequency()
{
	unsigned long init, end,min_interval;
	struct timeval begin_time,end_time;
	int intents = 0;

	if (node_energy_ops.read_dc_energy != NULL)
	{
		node_energy_ops.read_dc_energy(&init);

		do {
			node_energy_ops.read_dc_energy(&end);
			intents++;
		} while((init == end) && (intents < 5000));

		if (intents == 5000) return 0;

		gettimeofday(&begin_time,NULL);
		init = end;

		do{
			node_energy_ops.read_dc_energy(&end);
		} while(init == end);

		gettimeofday(&end_time,NULL);
		min_interval  = (end_time.tv_sec * 1000000 + end_time.tv_usec);
		min_interval -= (begin_time.tv_sec *1000000 + begin_time.tv_usec);

		return min_interval;
	}
	return 0;
}
