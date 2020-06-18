/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef _APP_MGT_H
#define _APP_MGT_H

typedef struct app_management_data{
  uint master_rank;
  uint ppn;
  uint nodes;
  uint total_processes;
  uint max_ppn;
}app_mgt_t;


void app_mgt_new_job(app_mgt_t *a);
void app_mgt_end_job(app_mgt_t *a);
void print_app_mgt_data(app_mgt_t *a);
uint is_app_master(app_mgt_t *a);

#endif


