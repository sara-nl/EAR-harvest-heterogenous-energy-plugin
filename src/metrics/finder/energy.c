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

#include <pthread.h>
#include <common/includes.h>
#include <freeipmi/freeipmi.h>
#include <metrics/finder/energy.h>

#define SZ_FIELDS	64
#define SZ_AREA		IPMI_FRU_AREA_SIZE_MAX

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static int _finder_energy(char *name_manufacturer, char *name_product)
{
	ipmi_fru_field_t product_custom_fields[SZ_FIELDS];
	ipmi_fru_field_t product_serial_number;
	ipmi_fru_field_t product_part_model_number;
	ipmi_fru_field_t product_manufacturer_name;
	ipmi_fru_field_t product_fru_file_id;
	ipmi_fru_field_t product_asset_tag;
	ipmi_fru_field_t product_version;
	ipmi_fru_field_t product_name;
	ipmi_ctx_t       ipmi_ctx_pn      = NULL;
	ipmi_fru_ctx_t   fru_ctx_pn       = NULL;
	uint8_t          area_buf[SZ_AREA + 1];
	uint             area_length      = 0;
	uint             area_type        = 0;
	uint             workaround_flags = 0;
	uint8_t          language_code;
	uid_t            uid;
	int              ready            = 0;
	int              ret;

	// Creating the context
	if (!(ipmi_ctx_pn = ipmi_ctx_create())) {
		verbose(0, "error in ipmi_ctx_create for product name %s", strerror(errno));
		return 0;
	}

	// Checking for root
	uid = getuid();
	if (uid != 0) {
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		return 0;
	}

	// Creating the inband context
	if ((ret = ipmi_ctx_find_inband(ipmi_ctx_pn,
									NULL, // driver_type
									0, // disable_auto_probe
									0, // driver_address
									0, // register_spacing
									NULL, // driver_device
									workaround_flags,
									IPMI_FLAGS_DEFAULT)) < 0)
	{
		verbose(0, "ipmi_ctx_find_inband for product name: %s", ipmi_ctx_errormsg(ipmi_ctx_pn));
		// Close context
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		return 0;
	}

	// Creating a fru context
	if (!(fru_ctx_pn = ipmi_fru_ctx_create(ipmi_ctx_pn))) {
		verbose(0, "ipmi_fru_ctx_create for product name: %s", ipmi_ctx_errormsg(ipmi_ctx_pn));
		// Close context
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		return 0;
	}

	if (ipmi_fru_open_device_id(fru_ctx_pn, 0) < 0) {
		verbose(0, "ipmi_fru_open_device_id %s", ipmi_fru_ctx_errormsg(fru_ctx_pn));
		// Close context
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		return 0;
	}

	// FRU returns a list
	if (ipmi_fru_first(fru_ctx_pn) < 0) {
		verbose(0, "ipmi_fru_first %s", ipmi_fru_ctx_errormsg(fru_ctx_pn));
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		return 0;
	}

	// We iterate until PRODUCT info is processed
	do {
		memset(area_buf, '\0', SZ_AREA + 1);
		if (ipmi_fru_read_data_area(fru_ctx_pn, &area_type, &area_length, area_buf, SZ_AREA) < 0) {
			verbose(0, "ipmi_fru_read_data_area %s", ipmi_fru_ctx_errormsg(fru_ctx_pn));
			// Close context
			ipmi_ctx_close(ipmi_ctx_pn);
			ipmi_ctx_destroy(ipmi_ctx_pn);
			return 0;
		}

		ready = 0;
		if (area_length)
		{
			char strbuf[IPMI_FRU_AREA_STRING_MAX + 1];
			unsigned int strbuflen = IPMI_FRU_AREA_STRING_MAX;

			switch (area_type) {
				case IPMI_FRU_AREA_TYPE_PRODUCT_INFO_AREA:
					memset(&product_manufacturer_name, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_name, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_part_model_number, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_version, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_serial_number, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_asset_tag, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_fru_file_id, '\0', sizeof(ipmi_fru_field_t));
					memset(&product_custom_fields[0], '\0', sizeof(ipmi_fru_field_t) * SZ_FIELDS);

					if (ipmi_fru_product_info_area(fru_ctx_pn,
												   area_buf,
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
												   SZ_FIELDS) < 0)
					{
						verbose(0, "ipmi_fru_product_info_area %s", ipmi_fru_ctx_errormsg(fru_ctx_pn));
						ipmi_ctx_close(ipmi_ctx_pn);
						ipmi_ctx_destroy(ipmi_ctx_pn);
						return 0;
					}

					// Here we have data , just print it
					if (product_manufacturer_name.type_length_field_length > 0) {
						memset(strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);
						if (ipmi_fru_type_length_field_to_string(fru_ctx_pn,
																 product_manufacturer_name.type_length_field,
																 product_manufacturer_name.type_length_field_length,
																 language_code,
																 strbuf,
																 &strbuflen) < 0) {
							verbose(0, "ipmi_fru_type_length_field_to_string error");
						}
						if (strbuflen) {
							verbose(1, "FRU Product Manufacturer name: %s", strbuf);
							strcpy(name_manufacturer, strbuf);
						}
					} else {
						verbose(0, "ipmi_fru_product_info_area returns 0 size for product_manufacturer_name");
					}

					// product_name
					strbuflen = IPMI_FRU_AREA_STRING_MAX;
					if (product_name.type_length_field_length > 0) {
						memset(strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);
						if (ipmi_fru_type_length_field_to_string(fru_ctx_pn,
																 product_name.type_length_field,
																 product_name.type_length_field_length,
																 language_code,
																 strbuf,
																 &strbuflen) < 0) {
							verbose(0, "ipmi_fru_type_length_field_to_string error");
						}
						if (strbuflen) {
							verbose(1, "FRU Product name: %s", strbuf);
							strcpy(name_product, strbuf);
						}
					} else {
						verbose(0, "ipmi_fru_product_info_area returns 0 size for product_name");
					}
					ready = 1;
					break;
				default:
					break;
			}
		}
	} while ((!ready) && (ret = ipmi_fru_next(fru_ctx_pn)) == 1);

	// End product name
	ipmi_ctx_close(ipmi_ctx_pn);
	ipmi_ctx_destroy(ipmi_ctx_pn);

	return 1;
}

int finder_energy(char *name_manufacturer, char *name_product)
{
	int status;

	// Locking
	pthread_mutex_lock(&lock);
	status = _finder_energy(name_manufacturer, name_product);
	pthread_mutex_unlock(&lock);

	return status;
}
