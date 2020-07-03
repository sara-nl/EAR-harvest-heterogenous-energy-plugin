#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <common/states.h>
#include <common/system/time.h>
#include <metrics/common/msr.h>
#include <metrics/common/topology.h>

// Up to 8 CCDs. But these data counters
// belongs to a Data Fabric, so are shared
// by all cores. So it isn't Just accesing
// the core 0 will be enough. Remember the
// 17h family have just 4 DF counters.
const ulong	cmd_ccd0 = 0x0000000000403807;
const ulong cmd_ccd1 = 0x0000000000403847;
const ulong cmd_ccd2 = 0x0000000000403887;
const ulong cmd_ccd3 = 0x00000000004038C7;
const ulong cmd_ccd4 = 0x0000000100403807;
const ulong cmd_ccd5 = 0x0000000100403847;
const ulong cmd_ccd6 = 0x0000000100403887;
const ulong cmd_ccd7 = 0x00000001004038C7;
const ulong df_ctl0  = 0x00000000c0010240;
const ulong df_ctl1  = 0x00000000c0010242;
const ulong df_ctl2  = 0x00000000c0010244;
const ulong df_ctl3  = 0x00000000c0010246;
const ulong df_ctr0  = 0x00000000c0010241;
const ulong df_ctr1  = 0x00000000c0010243;
const ulong df_ctr2  = 0x00000000c0010245;
const ulong df_ctr3  = 0x00000000c0010247;
// These L3 counters are shared by all CCX,
// and each CCX have 4 threads and 8 cores.
// It is required to get one core per CCX. 
const ulong cmd_l3   = 0xff0f000000400104;
const uint  ctl_l3   = 0x00000000c0010230;
const uint  ctr_l3   = 0x00000000c0010231;
// To stop all the counters. 
const ulong cmd_off  = 0x0000000000000000;

typedef struct bandwidth_s
{
	void *alloc;
} bandwidth_t;

typedef struct bandwidth_amd17_s
{
	topology_t tp;
	timestamp_t time;
	ulong *data_prev;
	ulong *data_curr;
	uint fd_count;
	uint filled;
} bandwidth_amd17_t;

state_t bandwidth_amd17_init(bandwidth_t *_bw, topology_t *tp)
{
	bandwidth_amd17_t *bw;
	state_t s;
	int i;
	
	// Initializing data
	_bw->alloc = calloc(1, sizeof(bandwidth_amd17_t));

	if (_bw->alloc == NULL) {
		return EAR_ERROR;
	}

	bw = (bandwidth_amd17_t *) _bw->alloc;
	uint ccx_count = tp->l3_count;
	uint ccd_count = ccx_count / 2;
	
	bw->fd_count  = ccx_count;
	bw->data_prev = calloc(ccx_count, sizeof(ulong) + 2);
	bw->data_curr = calloc(ccx_count, sizeof(ulong) + 2);

	// Getting the L3 groups
	topology_select(tp, &bw->tp, TPSelect.l3, TPGroup.merge, 0);
	
	for (i = 0; i < bw->fd_count; ++i)
	{
		s = msr_open(bw->tp.cpus[i].id);
	
		if (state_fail(s)) {
			return EAR_ERROR;
		}
	}

	return EAR_SUCCESS;
}

state_t bandwidth_amd17_close(bandwidth_t *_bw)
{
	bandwidth_amd17_t *bw = (bandwidth_amd17_t *) _bw;
	state_t s;
	int i;

	for (i = 0; i < bw->fd_count; ++i) {
        s = msr_close(bw->tp.cpus[i].id);
    }

	topology_close(&bw->tp);
	free(bw->data_prev);
	free(bw->data_curr);

	return EAR_SUCCESS;
}

state_t bandwidth_amd17_counters_start(bandwidth_t *_bw)
{
	return bandwidth_amd17_counters_reset(_bw);
}

state_t bandwidth_amd17_counters_stop(bandwidth_t *_bw)
{
	bandwidth_amd17_t *bw = (bandwidth_amd17_t *) _bw->alloc;
	state_t s;
	int i;

	// Two channels per CCD
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctl0);
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctl1);

	// One chunk of L3 per CCX	
	for (i = 0; i < bw->fd_count; ++i) {
		s = msr_write(bw->tp.cpus[i].id, &cmd_off, sizeof(ulong), ctl_l3);
	}
	
	return EAR_SUCCESS;
}

state_t bandwidth_amd17_counters_reset(bandwidth_t *_bw)
{
	bandwidth_amd17_t *bw = (bandwidth_amd17_t *) _bw->alloc;
	state_t s;
	int i;

	// Two channels per CCD
	s = msr_write(bw->tp.cpus[0].id, &cmd_ccd0, sizeof(ulong), df_ctl0);
	s = msr_write(bw->tp.cpus[0].id, &cmd_ccd1, sizeof(ulong), df_ctl1);

	// One chunk of L3 per CCX	
	for (i = 0; i < bw->fd_count; ++i) {
		s = msr_write(bw->tp.cpus[i].id, &cmd_l3, sizeof(ulong), ctl_l3);
	}
	
	return EAR_SUCCESS;
}

state_t bandwidth_amd17_counters_read(bandwidth_t *_bw, double *gbs)
{
	bandwidth_amd17_t *bw = (bandwidth_amd17_t *) _bw->alloc;
	state_t s;
	int i;

	// Timestamp	
	timestamp_t aux0;
	ulong secs;

	aux0 = bw->time;	
	timestamp_getfast(&bw->time);
	secs = timestamp_diff(&bw->time, &aux0, TIME_MSECS);

	// Reading the memory access channels of channels 0 and 1 of CCD0 and CCD1.
	int i0 = bw->fd_count+0;
	int i1 = bw->fd_count+1;
	
	msr_read(bw->tp.cpus[0].id, &bw->data_curr[i0], sizeof(ulong), df_ctr0);
	msr_read(bw->tp.cpus[0].id, &bw->data_curr[i1], sizeof(ulong), df_ctr1);

	// Reading the L3 counters of CCX0, CCX1, CCX2, CCX3
	msr_read(bw->tp.cpus[0].id, &bw->data_curr[0], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[1].id, &bw->data_curr[1], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[2].id, &bw->data_curr[2], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[3].id, &bw->data_curr[3], sizeof(ulong), ctr_l3);

	// Substracting data first (overflow safety).
	ulong aux_mem0 = bw->data_curr[i0] - bw->data_prev[i0];
	ulong aux_mem1 = bw->data_curr[i1] - bw->data_prev[i1];
	ulong aux_ccx0 = bw->data_curr[0]  - bw->data_prev[0];
	ulong aux_ccx1 = bw->data_curr[1]  - bw->data_prev[1];
	ulong aux_ccx2 = bw->data_curr[2]  - bw->data_prev[2];
	ulong aux_ccx3 = bw->data_curr[3]  - bw->data_prev[3];

	// Comping the percentage of L3 ticks respect the memory channel accesses
	double ccx_ticks = (double) (aux_ccx0 + aux_ccx1 + aux_ccx2 + aux_ccx3);
	double mem_ticks = (double) (aux_mem0 + aux_mem1);
	double ccx_coeff = (mem_ticks / ccx_ticks);

	// Saving the memory channel ticks for the next reading
	bw->data_prev[i0] = bw->data_curr[i0];
	bw->data_prev[i1] = bw->data_curr[i1];

	//
	double aux1;
	double aux2;
	*gbs = 0.0;

	for (i = 0; i < bw->fd_count; ++i)
	{
		// Reading each L3 miss counters
		msr_read(bw->tp.cpus[i].id, &bw->data_curr[i], sizeof(ulong), ctr_l3);

		// Passing the data to double
		aux2 = (double) (bw->data_curr[i]);
		aux1 = (double) (bw->data_prev[i]);
		aux2 = aux2 - aux1;

		// Doing the division first (overflow safety)
		aux2 = aux2 / (1000000000.0 * 4.0);
		aux2 = aux2 * 64.0 * ccx_coeff;
		*gbs = *gbs + aux2;

		// Saving the L3 counters for the next reading
		bw->data_prev[i] = bw->data_curr[i];
	}

	if (bw->filled == 0) {
		bw->filled = 1;
		*gbs = 0;
	}
	
	return EAR_SUCCESS;
}

int main(int argc,char *argv[])
{
	bandwidth_t band;
	topology_t topo;
	double gbs;
	
	topology_init(&topo);
	bandwidth_amd17_init(&band, &topo);
	bandwidth_amd17_counters_start(&band);
	bandwidth_amd17_counters_read(&band, &gbs);
	sleep(4);
	bandwidth_amd17_counters_read(&band, &gbs);
	fprintf(stderr, "Detected bandwidth: %.4lf GB/s\n", gbs);

	return 0;
}
