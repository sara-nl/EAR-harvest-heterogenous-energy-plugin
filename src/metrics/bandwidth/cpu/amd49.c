/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <common/system/time.h>
#include <common/output/debug.h>
#include <metrics/common/msr.h>
#include <metrics/bandwidth/cpu/amd49.h>

// Vendor	AuthenticAMD
// Family	17
// Model	X

// Up to 8 CCDs. But these data counters
// belongs to a Data Fabric, so are shared
// by all cores. So just accessing the
// core 0 will be enough. Remember the 49h
// (7742) model have just 4 DF counters.
const ulong	df_cmd0 = 0x0000000000403807; //CH0
const ulong df_cmd1 = 0x0000000000403847; //CH1
const ulong df_cmd2 = 0x0000000000403887; //CH2
const ulong df_cmd3 = 0x00000000004038C7; //CH3
const ulong df_cmd4 = 0x0000000100403807; //CH4
const ulong df_cmd5 = 0x0000000100403847; //CH5
const ulong df_cmd6 = 0x0000000100403887; //CH6
const ulong df_cmd7 = 0x00000001004038C7; //CH7
const ulong df_ctl0 = 0x00000000c0010240;
const ulong df_ctl1 = 0x00000000c0010242;
const ulong df_ctl2 = 0x00000000c0010244;
const ulong df_ctl3 = 0x00000000c0010246;
const ulong df_ctr0 = 0x00000000c0010241;
const ulong df_ctr1 = 0x00000000c0010243;
const ulong df_ctr2 = 0x00000000c0010245;
const ulong df_ctr3 = 0x00000000c0010247;
// These L3 counters are shared by all CCX,
// and each CCX have 4 threads and 8 cores.
// It is required to get one core per CCX. 
const ulong cmd_l3  = 0xff0f000000400104;
const uint  ctl_l3  = 0x00000000c0010230;
const uint  ctr_l3  = 0x00000000c0010231;
// To stop all the counters. 
const ulong cmd_off  = 0x0000000000000000;

typedef struct bwidth_amd49_s
{
	topology_t tp;
	timestamp_t time;
	ulong *data_curr;
	uint fd_count;
} bwidth_amd49_t;

static int initialized;

state_t bwidth_amd49_status(topology_t *tp)
{
	if (tp->vendor == VENDOR_AMD && tp->family >= FAMILY_ZEN){
		return EAR_SUCCESS;
	}
	return EAR_ERROR;
}

state_t bwidth_amd49_init(ctx_t *c, topology_t *tp)
{
	bwidth_amd49_t *bw;
	state_t s;
	int i;

	if (c == NULL) {
		debug("%s", Generr.context_null);
		return_msg(EAR_ERROR, Generr.context_null);
	}

	// Initializing data
	c->context = calloc(1, sizeof(bwidth_amd49_t));

	if (c->context == NULL) {
		debug("%s", Generr.alloc_error);
		return_msg(EAR_ERROR, Generr.alloc_error);
	}

	bw = (bwidth_amd49_t *) c->context;
	uint ccx_count = tp->l3_count;
	uint ccd_count = ccx_count / 2;

	bw->fd_count  = ccx_count;
	bw->data_curr = calloc(ccx_count, sizeof(ulong) + 2);

	// Getting the L3 groups
	topology_select(tp, &bw->tp, TPSelect.l3, TPGroup.merge, 0);

	for (i = 0; i < bw->fd_count; ++i)
	{
		debug("opening CPU %d (socket %d, L3 %d)",
			bw->tp.cpus[i].id, bw->tp.cpus[i].socket_id, bw->tp.cpus[i].l3_id);
		s = msr_open(bw->tp.cpus[i].id);

		if (state_fail(s)) {
			return EAR_ERROR;
		}
	}

	//
	debug("initialized");
	initialized = 1;

	return EAR_SUCCESS;
}

state_t bwidth_amd49_dispose(ctx_t *c)
{
	bwidth_amd49_t *bw = (bwidth_amd49_t *) c->context;
	state_t s;
	int i;
	
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}

	for (i = 0; i < bw->fd_count; ++i) {
		s = msr_close(bw->tp.cpus[i].id);
	}

	topology_close(&bw->tp);
	free(bw->data_curr);

	return EAR_SUCCESS;
}

state_t bwidth_amd49_count(ctx_t *c, uint *count)
{
	bwidth_amd49_t *bw = (bwidth_amd49_t *) c->context;
	
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	
	*count = bw->fd_count;
	
	return EAR_SUCCESS;
}

state_t bwidth_amd49_start(ctx_t *c)
{
	return bwidth_amd49_reset(c);
}

state_t bwidth_amd49_stop(ctx_t *c, ullong *cas)
{
	bwidth_amd49_t *bw = (bwidth_amd49_t *) c->context;
	state_t s;
	int i;
	
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}

	// Two channels per CCD
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctl0);
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctl1);

	// One chunk of L3 per CCX	
	for (i = 0; i < bw->fd_count; ++i) {
		s = msr_write(bw->tp.cpus[i].id, &cmd_off, sizeof(ulong), ctl_l3);
	}

	return bwidth_amd49_read(c, cas);
}

state_t bwidth_amd49_reset(ctx_t *c)
{
	bwidth_amd49_t *bw = (bwidth_amd49_t *) c->context;
	state_t s;
	int i;
	
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}

	// Two channels per CCD (more or less)
	s = msr_write(bw->tp.cpus[0].id, &df_cmd0, sizeof(ulong), df_ctl0);
	s = msr_write(bw->tp.cpus[0].id, &df_cmd1, sizeof(ulong), df_ctl1);
	
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctr0);
	s = msr_write(bw->tp.cpus[0].id, &cmd_off, sizeof(ulong), df_ctr1);

	// One chunk of L3 per CCX	
	for (i = 0; i < bw->fd_count; ++i) {
		s = msr_write(bw->tp.cpus[i].id, &cmd_l3, sizeof(ulong), ctl_l3);
	}

	return EAR_SUCCESS;
}

//state_t bwidth_amd49_counters_read(ctx_t *c, double *gbs)
state_t bwidth_amd49_read(ctx_t *c, ullong *cas)
{
	bwidth_amd49_t *bw = (bwidth_amd49_t *) c->context;
	state_t s;
	int i;
	
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}

	// Timestamp	
	timestamp_t aux0;
	ulong secs;

	aux0 = bw->time;
	timestamp_getfast(&bw->time);
	secs = timestamp_diff(&bw->time, &aux0, TIME_MSECS);

	// Reading the memory access channels of channels 0 and 1 of CCD0 and CCD1.
	// This is valid for CPUs with at least 2 CCDs.
	int i0 = bw->fd_count+0;
	int i1 = bw->fd_count+1;

	msr_read(bw->tp.cpus[0].id, &bw->data_curr[i0], sizeof(ulong), df_ctr0);
	msr_read(bw->tp.cpus[0].id, &bw->data_curr[i1], sizeof(ulong), df_ctr1);

	debug("CH0 counted %llu CAS", bw->data_curr[i0]);
	debug("CH1 counted %llu CAS", bw->data_curr[i1]);

	// Reading the L3 counters of CCX0, CCX1, CCX2, CCX3
	msr_read(bw->tp.cpus[0].id, &bw->data_curr[0], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[1].id, &bw->data_curr[1], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[2].id, &bw->data_curr[2], sizeof(ulong), ctr_l3);
	msr_read(bw->tp.cpus[3].id, &bw->data_curr[3], sizeof(ulong), ctr_l3);

	//
	ulong aux_mem0 = bw->data_curr[i0];
	ulong aux_mem1 = bw->data_curr[i1];
	ulong aux_ccx0 = bw->data_curr[0];
	ulong aux_ccx1 = bw->data_curr[1];
	ulong aux_ccx2 = bw->data_curr[2];
	ulong aux_ccx3 = bw->data_curr[3];
	
	if (cas == NULL) {
		return EAR_SUCCESS;
	}

	// Comparing the percentage of L3 ticks respect the memory channel accesses
	double ccx_ticks = (double) (aux_ccx0 + aux_ccx1 + aux_ccx2 + aux_ccx3);
	double mem_ticks = (double) (aux_mem0 + aux_mem1);
	double ccx_coeff = (mem_ticks / ccx_ticks);
	double aux;

	for (i = 0; i < bw->fd_count; ++i)
	{
		// Reading each L3 miss counters
		msr_read(bw->tp.cpus[i].id, &bw->data_curr[i], sizeof(ulong), ctr_l3);
		// Passing the data to double
		aux = ((double) bw->data_curr[i]) * ccx_coeff;
		// Deploying to CAS count
		cas[i] = (ullong) aux;
	}

	return EAR_SUCCESS;
}
