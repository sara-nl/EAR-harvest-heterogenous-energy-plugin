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

//#define SHOW_DEBUGS 1

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <common/sizes.h>
#include <common/output/debug.h>
#include <management/cpufreq/drivers/linux_cpufreq.h>

#define N_FREQS 128
#define N_CPUS  4096

static topology_t tp;

typedef struct cpufreq_ctx_s
{
	ullong      freqs_current[N_CPUS];
	ullong      freqs_avail[N_FREQS];
	int         freqs_count;
	int         boost_enabled;
	char        freq_last[SZ_NAME_SHORT];
	char        govr_last[SZ_NAME_SHORT];
	char        freq_init[SZ_NAME_SHORT];
	char        govr_init[SZ_NAME_SHORT];
	int        *fds_freq;
	int        *fds_govr;
	uint        init;
} cpufreq_ctx_t;

// TODO:
// Reads a string from a fd with or without an lseek 0.
static state_t read_word(int fd, char *word, int reset_position)
{
	int i = 0;
	char c;
	if (reset_position) {
		if(lseek(fd, 0, SEEK_SET) < 0) {
			return_msg(EAR_ERROR, strerror(errno));
		}
	}
	// Does not read ' ' nor '\n'
	while((read(fd, &c, sizeof(char)) > 0) && !((c == (char) 10) || (c == (char) 32))) {
		word[i] = c;
		i++;
	}
	word[i] = '\0';
	debug("read the word '%s'", word);
	if (i == 0) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}

// Writes a string 'word' in fd, adding a line break or not.
static state_t write_word(int fd, char *word, int s, int line_break)
{
	int i, r, p;
	// Adding a line break
	if (line_break) {
		s += 1;
		word[s-1] = '\n';
		word[s-0] = '\0';
	}
	for (i = 0, r = 1, p = s; i < s && r > 0;) {
		r = pwrite(fd, (void*) &word[i], p, i);
		i = i + r;
		p = s - i;
	}
	if (r == -1) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}

// Writes a word in a list of file descriptors.
static state_t write_multi(int *fds, char *word, int line_break)
{
	state_t s = EAR_SUCCESS;
	int len = strlen(word)+1;
	int cpu;
	debug("Multiple writing a word '%s'", word);
	// Adding a line break
	if (line_break) {
		word[len-1] = '\n';
		word[len-0] = '\0';
	}
	for (cpu = 0; cpu < tp.cpu_count && state_ok(s); ++cpu) {
		s = write_word(fds[cpu], word, len, 0);
	}
	if (state_fail(s)) {
		return s;
	}
	return EAR_SUCCESS;
}

state_t cpufreq_linux_status(topology_t *_tp)
{
	char data[SZ_NAME_SHORT];
	state_t s;
	debug("testing ACPI-CPUFreq driver status");
	int fd = open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver", O_RDONLY);
	if (fd < 0) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	s = read_word(fd, data, 0);
	close(fd);
	if (state_fail(s)) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	if (strncmp(data, "acpi-cpufreq", 12) != 0) {
		return_msg(EAR_ERROR, "driver not found");
	}
	if (tp.cpu_count == 0) {
		if(xtate_fail(s, topology_copy(&tp, _tp))) {
			return EAR_ERROR;
		}
	}
	return EAR_SUCCESS;
}

static state_t static_init_test(ctx_t *c, cpufreq_ctx_t **f)
{
    if (c == NULL) {
        return_msg(EAR_ERROR, Generr.input_null);
    }
    if (((*f = (cpufreq_ctx_t *) c->context) == NULL) || !(*f)->init) {
        return_msg(EAR_ERROR, Generr.api_uninitialized);
    }
	return EAR_SUCCESS;
}

static state_t static_dispose(ctx_t *c, state_t s, char *msg)
{
	cpufreq_ctx_t *f;
	int cpu;	
	
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (f->fds_freq != NULL && f->fds_freq[cpu] >= 0) {
			//debug("closing fds_freq[%d]: %d", cpu, f->fds_freq[cpu]);
			close(f->fds_freq[cpu]);
		}
		if (f->fds_govr != NULL && f->fds_govr[cpu] >= 0) {
			//debug("closing fds_govr[%d]: %d", cpu, f->fds_govr[cpu]);
			close(f->fds_govr[cpu]);
		}
	}
	if (f->fds_freq != NULL)
		free(f->fds_freq);
	if (f->fds_govr != NULL)
		free(f->fds_govr);
	free(f);
	c->context = NULL;
	
	return_msg(s, msg);
}

static void cpufreq_linux_init_nominal(cpufreq_ctx_t *f)
{	
	char data[SZ_NAME_SHORT];
	int fd;

	if ((fd = open("/sys/devices/system/cpu/cpufreq/boost", O_RDONLY)) >= 0) {
		if (state_ok(read_word(fd, data, 0))) {
			f->boost_enabled = atoi(data);
		}
		close(fd);
	}
	if (tp.vendor == VENDOR_AMD) {
		if ((fd = open("/sys/devices/system/cpu/cpu0/cpufreq/cpb", O_RDONLY)) >= 0) {
			if (state_ok(read_word(fd, data, 0))) {
				f->boost_enabled = atoi(data);
			}
			close(fd);
		}
	}
}

state_t cpufreq_linux_init(ctx_t *c)
{
	char path[SZ_PATH];
	cpufreq_ctx_t *f;
	int cpu, aux, i;
	state_t s;

	//
	debug("initializing ACPI-CPUFreq driver");
	//
	if (state_ok(static_init_test(c, &f))) {
		return_msg(EAR_ERROR, Generr.api_initialized);
    }
	// Context
	if ((c->context = calloc(1, sizeof(cpufreq_ctx_t))) == NULL) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	f = (cpufreq_ctx_t *) c->context;
	// File descriptors allocation
	if ((f->fds_freq = (int *) malloc(tp.cpu_count * sizeof(int))) == NULL) {
		return static_dispose(c, EAR_ERROR, strerror(errno));
	}
	if ((f->fds_govr = (int *) malloc(tp.cpu_count * sizeof(int))) == NULL) {
		return static_dispose(c, EAR_ERROR, strerror(errno));
	}
	memset((void *) f->fds_freq, -1, tp.cpu_count * sizeof(int));
	memset((void *) f->fds_govr, -1, tp.cpu_count * sizeof(int));
	// Opening governors file (includes \n)
	for (cpu = 0, aux = O_RDWR; cpu < tp.cpu_count; ++cpu) {
		sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", cpu);
		if ((f->fds_govr[cpu] = open(path, aux)) == -1) {
			if (aux == O_RDONLY) {
				return static_dispose(c, EAR_ERROR, strerror(errno));
			}
			aux = O_RDONLY;
			cpu = -1;
		}
	}
	// Opening frequencies file (includes \n)
	for (cpu = 0, aux = O_RDWR; cpu < tp.cpu_count; ++cpu) {
		sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed", cpu);
		if ((f->fds_freq[cpu] = open(path, aux)) == -1) {
			if (aux == O_RDONLY) {
				return static_dispose(c, EAR_ERROR, strerror(errno));
			}
			aux = O_RDONLY;
			cpu = -1;
		}
	}
	// Loading available frequencies
	sprintf(path, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies");
	if ((aux = open(path, O_RDONLY)) == -1) {
		return static_dispose(c, EAR_ERROR, strerror(errno));
	}
	for(i = 0; i < N_FREQS; ++i, f->freqs_count = i) {
		if (state_fail(read_word(aux, path, 0))) {
			break;
		}
		f->freqs_avail[i] = (ullong) atoll(path);
	}
	close(aux);
	if (f->freqs_count == 0) {
		return static_dispose(c, EAR_ERROR, "no frequencies available");
	}
	// Saving init governor and frequency
	if (xtate_fail(s, read_word(f->fds_govr[0], f->govr_init, 1))) {
		return static_dispose(c, s, state_msg);
	}
	if (xtate_fail(s, read_word(f->fds_freq[0], f->freq_init, 1))) {
		return static_dispose(c, s, state_msg);
	}
	strncpy(f->govr_last, f->govr_init, SZ_NAME_SHORT);
	strncpy(f->freq_last, f->freq_init, SZ_NAME_SHORT);
	// Detecting nominal
	cpufreq_linux_init_nominal(f);
	// Finishing 
	debug ("init sum: %u available frequencies", f->freqs_count);
	debug ("init sum: first frequency '%s'", f->freq_init);
	debug ("init sum: first governor '%s'", f->govr_init);
	debug ("init sum: boost enabled '%d'", f->boost_enabled);
	f->init = 1;

	return EAR_SUCCESS;
}

state_t cpufreq_linux_dispose(ctx_t *c)
{
	return static_dispose(c, EAR_SUCCESS, "");
}

/* Getters */
state_t cpufreq_linux_get_available_list(ctx_t *c, const ullong **freq_list, uint *freq_count)
{
	cpufreq_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (freq_list  != NULL) *freq_list  = f->freqs_avail;
	if (freq_count != NULL) *freq_count = f->freqs_count;
	return EAR_SUCCESS;
}

state_t cpufreq_linux_get_current_list(ctx_t *c, const ullong **freq_list)
{
	char data[SZ_NAME_SHORT];
	state_t s2 = EAR_SUCCESS;
	state_t s1 = EAR_SUCCESS;
	cpufreq_ctx_t *f;
	state_t s;
	int cpu;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	*freq_list = f->freqs_current;
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (xtate_fail(s1, read_word(f->fds_freq[cpu], data, 1))) {
			f->freqs_current[cpu] = 0LLU;
			s2 = s1;
		} else {
			f->freqs_current[cpu] = (ullong) atoll(data);
		}
	}
	return s2;
}

state_t cpufreq_linux_get_boost(ctx_t *c, uint *boost_enabled)
{
	cpufreq_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (boost_enabled != NULL) {
		*boost_enabled = f->boost_enabled;
	}
	return EAR_SUCCESS;
}

state_t cpufreq_linux_get_governor(ctx_t *c, uint *governor)
{
	char data[SZ_NAME_SHORT];
	state_t s = EAR_SUCCESS;
	cpufreq_ctx_t *f;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, read_word(f->fds_govr[0], data, 1))) {
		*governor = Governor.other;
	} else if (xtate_fail(s, mgt_governor_toint(data, governor))) {
	}
	return s;
}

/** Setters */
state_t cpufreq_linux_set_current_list(ctx_t *c, uint *freqs_index)
{
	char data[SZ_NAME_SHORT];
	state_t s2 = EAR_SUCCESS;
	state_t s1 = EAR_SUCCESS;
	cpufreq_ctx_t *f;
	state_t s;
	int cpu;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	// Adding a line break
	for (cpu = 0; cpu < tp.cpu_count && state_ok(s); ++cpu)
	{
		if (freqs_index[cpu] > f->freqs_count) {
			freqs_index[cpu] = f->freqs_count-1;
		}
		sprintf(data, "%llu", f->freqs_avail[freqs_index[cpu]]);
		s1 = write_word(f->fds_freq[cpu], data, strlen(data), 1);
		//debug("writing a word '%s' returned %d", data, s1);

		if (state_fail(s1)) {
			s2 = s1;
		}
	}
	return s2;
}

state_t cpufreq_linux_set_current(ctx_t *c, uint freq_index, int cpu)
{
	char data[SZ_NAME_SHORT];
	cpufreq_ctx_t *f;
	state_t s;
	
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	//
	debug("setting frequency %u -> %llu", freq_index, f->freqs_avail[freq_index]);
	// Correcting invalid values 
	if (freq_index > f->freqs_count) {
		freq_index = f->freqs_count;
	}
	// If is one P_STATE for all CPUs
	if (cpu == all_cpus) {
		// Converting frequency to text
		sprintf(data, "%llu", f->freqs_avail[freq_index]);
		return write_multi(f->fds_freq, data, 1);
	}
	// If it is for a specified CPU
	if (cpu >= 0 && cpu < tp.cpu_count) {
		sprintf(data, "%llu", f->freqs_avail[freq_index]);
		debug("writing a word '%s'", data);
		return write_word(f->fds_freq[cpu], data, strlen(data), 1);
	}
	return_msg(EAR_ERROR, Generr.cpu_invalid);
}

state_t cpufreq_linux_set_governor(ctx_t *c, uint governor)
{
	cpufreq_ctx_t *f;
	char govr_aux[SZ_NAME_SHORT];
	char freq_aux[SZ_NAME_SHORT];
	uint set_freq = 0;
	uint current;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, cpufreq_linux_get_governor(c, &current))) {
		return s;
	}
	if (governor == current) {
		return EAR_SUCCESS;
	}
	//
	if (governor == Governor.last) {
		sprintf(govr_aux, "%s", f->govr_last);
		sprintf(freq_aux, "%s", f->freq_last);	
	}
	// Saving current governor
	s = read_word(f->fds_govr[0], f->govr_last, 1);
	s = read_word(f->fds_freq[0], f->freq_last, 1);
	//
	if (governor == Governor.init) {
		sprintf(govr_aux, "%s", f->govr_init);
		sprintf(freq_aux, "%s", f->freq_init);	
		set_freq = 1;
	} else if (governor == Governor.last) {
		set_freq = 1;
	} else {
		if (xtate_fail(s, mgt_governor_tostr(governor, govr_aux))) {
			return s;
		}
	}
	//
	debug("setting governor '%s'", govr_aux);
	debug("last governor is '%s'", f->govr_last);
	debug("last frequency is '%s'", f->freq_last);
	if (xtate_fail(s, write_multi(f->fds_govr, govr_aux, 1))) {
		return s;
	}
	if (!set_freq) {
		return EAR_SUCCESS;
	}
	return write_multi(f->fds_freq, freq_aux, 1);
}
