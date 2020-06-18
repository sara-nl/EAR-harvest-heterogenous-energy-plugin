/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

int lock_clean(char *path_tmp);

int lock_master(char *path_tmp);

int lock_step(char *path_tmp, int job_id, int step_id);

int unlock_step(char *path_tmp, int step_id);

int spinlock_step(char *path_tmp, int step_id);
