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

// Buffers
char *path_mast = "/tmp/erun.master.lock";
char *path_step = "/tmp/erun.step.lock";
char  path_jobs[SZ_PATH];

//
int _fd_mast = -1;
int _fd_step = -1;
int _fd_jobs = -1;

int lock_clean(int argc, char *argv[])
{
	plug_verbose(sp, 2, "function lock_clean");
	// 1. Clean erun.master.lock
	// 2. Clean erun.step.lock
	file_unlock_master(_fd_mast, path_mast);
	file_clean(path_step);
	return 0;
}

int lock_master(int argc, char *argv[])
{
	plug_verbose(sp, 2, "function lock_master");
	_fd_mast = file_lock_master(path_lock);

	if (_fd_mast > 0) {
		return 1;
	}

	_fd_mast = -1;
	return 0;
}

int lock_step(int argc, char *argv[], int job_id, int step_id)
{
	plug_verbose(sp, 2, "function lock_step (%d)", step_id);
	sprintf(path_jobs, "erun.%d.lock", job_id);

	// 1. Test erun.1394394.lock
	//		 SLURM_STEP_ID -> Set step_id to SLURM_STEP_ID
	//		!SLURM_STEP_ID
	// 			!file -> Set step_id to 0
	// 			 file -> Read the step_id + 1
	// 2. Clean erun.1394394.lock
	// 3. Create erun.1394394.lock with the step_id
	// 4. Return step_id
	if (step_id == 0) {
		if (!file_read(path_jobs, &step_id, sizeof(int))) {
			plug_verbose(sp, 2, "readed %d step_id in the file '%s'", path_jobs);
			step_id = -1;
		}
		step_id += 1;
	}

	file_clean(path_jobs);
	file_write(path_jobs, &step_id, sizeof(int));

	return step_id;
}

int unlock_step(int argc, char *argv[], int step_id)
{
	plug_verbose(sp, 2, "function unlock_step");
	// 1. Create and write erun.step.lock
	file_write(path_step, &step_id, sizeof(int));
	return step_id;
}

int spinlock_step(int argc, char *argv[], int step_id)
{
	plug_verbose(sp, 2, "function spinlock_step");
	// 1. While !erun.step.lock file
	// 2. Read erun.step.lock step_id
	// 3. Return step_id
	while (access(path_step, F_OK) != 0);

	file_read(path_step, &step_id, sizeof(int));
	plug_verbose(sp, 2, "readed %d step_id in the file '%s'", path_jobs);

	return step_id;
}