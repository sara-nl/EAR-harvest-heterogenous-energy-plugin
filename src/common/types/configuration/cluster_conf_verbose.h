/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _CLUSTER_CONF_VERBOSE
#define _CLUSTER_CONF_VERBOSE
void print_eard_conf(eard_conf_t *conf);
void print_eargm_conf(eargm_conf_t *conf);
void print_db_manager(eardb_conf_t *conf);
void print_database_conf(db_conf_t *conf);
void print_islands_conf(node_island_t *conf);
void print_tags_conf(tag_t *tag);
void print_energy_tag(energy_tag_t *etag);
void print_earlib_conf(earlib_conf_t *conf);
#endif

