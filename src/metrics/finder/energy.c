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

static pthread_mutex_t lock   = PTHREAD_MUTEX_INITIALIZER;
static ipmi_ctx_t ipmi_ctx_pn = NULL;

static state_t _finder_energy(char *name_manufacturer, char *name_product)
{
	ipmi_fru_field_t pr_fields[SZ_FIELDS];
	ipmi_fru_field_t pr_fru_file;
	ipmi_fru_field_t pr_man_name;
	ipmi_fru_field_t pr_ser_num;
	ipmi_fru_field_t pr_mod_num;
	ipmi_fru_field_t pr_ase_tag;
	ipmi_fru_field_t pr_version;
	ipmi_fru_field_t pr_name;

	ipmi_fru_ctx_t   fru_ctx_pn       = NULL;
	uint8_t          area_buf[SZ_AREA + 1];
	uint             area_length      = 0;
	uint             area_type        = 0;
	uint             workaround_flags = 0;
	uint8_t          language_code    = 0;
	uid_t            uid              = 0;
	int              ready            = 0;
	int              ret              = 0;

	// Checking for root
	if (getuid() != 0) {
		state_return_msg(EAR_ERROR, 0, "the user is not root");
	}

	// Creating the context
	if (!(ipmi_ctx_pn = ipmi_ctx_create())) {
		state_return_msg(EAR_ERROR, 0, strerror(errno));
	}

	// Creating the inband context
	if ((ret = ipmi_ctx_find_inband(ipmi_ctx_pn, NULL, 0, 0, 0,
		NULL, workaround_flags, IPMI_FLAGS_DEFAULT)) < 0)
	{
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx_pn));
	}

	// Creating a fru context
	if (!(fru_ctx_pn = ipmi_fru_ctx_create(ipmi_ctx_pn))) {
		state_return_msg(EAR_ERROR, 0, ipmi_ctx_errormsg(ipmi_ctx_pn));
	}

	if (ipmi_fru_open_device_id(fru_ctx_pn, 0) < 0) {
		state_return_msg(EAR_ERROR, 0, ipmi_fru_ctx_errormsg(fru_ctx_pn));
	}

	// FRU returns a list
	if (ipmi_fru_first(fru_ctx_pn) < 0) {
		state_return_msg(EAR_ERROR, 0, ipmi_fru_ctx_errormsg(fru_ctx_pn));
	}

	// We iterate until PRODUCT info is processed
	do {
		memset(area_buf, '\0', SZ_AREA + 1);

		if (ipmi_fru_read_data_area(fru_ctx_pn, &area_type, &area_length, area_buf, SZ_AREA) < 0) {
			state_return_msg(EAR_ERROR, 0, ipmi_fru_ctx_errormsg(fru_ctx_pn));
		}

		ready = 0;
		if (area_length)
		{
			uint strbuflen = IPMI_FRU_AREA_STRING_MAX;
			char strbuf[IPMI_FRU_AREA_STRING_MAX + 1];

			switch (area_type)
			{
				case IPMI_FRU_AREA_TYPE_PRODUCT_INFO_AREA:
					memset(&pr_man_name,  '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_name,      '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_mod_num,   '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_version,   '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_ser_num,   '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_ase_tag,   '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_fru_file,  '\0', sizeof(ipmi_fru_field_t));
					memset(&pr_fields[0], '\0', sizeof(ipmi_fru_field_t) * SZ_FIELDS);

					if (ipmi_fru_product_info_area(fru_ctx_pn, area_buf, area_length,
						&language_code, &pr_man_name, &pr_name, &pr_mod_num, &pr_version,
						&pr_ser_num, &pr_ase_tag, &pr_fru_file, pr_fields, SZ_FIELDS) < 0)
					{
						state_return_msg(EAR_ERROR, 0, ipmi_fru_ctx_errormsg(fru_ctx_pn));
					}

					// Here we have data , just print it
					if (pr_man_name.type_length_field_length > 0)
					{
						memset(strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);

						if (ipmi_fru_type_length_field_to_string(fru_ctx_pn,
							pr_man_name.type_length_field,
							pr_man_name.type_length_field_length,
							language_code, strbuf, &strbuflen) < 0)
						{
							state_set_error("error during length to string");
						}

						if (strbuflen) {
							//verbose(1, "FRU Product Manufacturer name: %s", strbuf);
							strcpy(name_manufacturer, strbuf);
						}
					} else {
						state_set_error("product info returns 0 size");
					}

					// pr_name
					strbuflen = IPMI_FRU_AREA_STRING_MAX;
					if (pr_name.type_length_field_length > 0)
					{
						memset(strbuf, '\0', IPMI_FRU_AREA_STRING_MAX + 1);

						if (ipmi_fru_type_length_field_to_string(fru_ctx_pn,
							pr_name.type_length_field, pr_name.type_length_field_length,
							language_code, strbuf, &strbuflen) < 0)
						{
							state_set_error("error during length to string");
						}

						if (strbuflen) {
							//verbose(1, "FRU Product name: %s", strbuf);
							strcpy(name_product, strbuf);
						}
					} else {
						state_set_error("product info returns 0 size");
					}
					ready = 1;
					break;
				default:
					break;
			}
		}
	} while ((!ready) && (ret = ipmi_fru_next(fru_ctx_pn)) == 1);

	return EAR_SUCCESS;
}

int finder_energy(char *name_manufacturer, char *name_product)
{
	state_t s;

	sprintf(name_manufacturer, "unknown");
	sprintf(name_product, "unknown");

	// Locking
	pthread_mutex_lock(&lock);
	s = _finder_energy(name_manufacturer, name_product);

	if (ipmi_ctx_pn != NULL) {
		ipmi_ctx_close(ipmi_ctx_pn);
		ipmi_ctx_destroy(ipmi_ctx_pn);
		ipmi_ctx_pn = NULL;
	}

	pthread_mutex_unlock(&lock);

	if (state_fail(s)) {
		state_print_error("during energy product finding", s);
	}

	return state_ok(s);
}
