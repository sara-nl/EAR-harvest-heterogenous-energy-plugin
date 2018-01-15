/*
    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

/*
 * Reading performance monitor uncore counters.
 *
 * How it works?
 * - Opens /proc/bus/pci/ files to scan which ones are
 * the integrated memory controllers (IMCs) needed to
 * control and read the uncore counters.
 * - Every architecture has different devices, functions
 * and IMCs. When the cpu model is passed as an argument
 * to the init function, the pci files scanning starts.
 * - Once found, a small memory is allocated to hold
 * the number of file descriptors.
 * - Some commands are written in pci device files,
 * to start, stop and uncore counters.
 *
 * PCI architecture specifications:
 *
 *  Arch    | #IMCs   | #Channels/IMC | #Counters/Channel
 *  Haswell | Up to 2 | Up to 4       | 4 (+1)
 *  Skylake | Up to 2 | Up to 3       | 4 (+1)
 *
 * PCI devices and functions, identificatos and registers:
 *
 *  Memory/Channel | Haswell          | Skylake
 *  MC0 Ch0	   | ??:14.0 (D20:F0) | 02:0A.2 (B2:D10:F2)
 *  MC0 Ch1        | ??:14.1 (D20:F1) | 02:0A.6 (B2:D10:F6)
 *  MC0 Ch2        | ??:15.0 (D21:F0) | 02:0B.2 (B2:D11:F2)
 *  MC0 Ch3        | ??:15.1 (D21:F1) | -
 *  MC1 Ch0        | ??:17.0 (D23:F0) | 02:0C.2 (B2:D12:F2)
 *  MC1 Ch1        | ??:17.1 (D23:F1) | 02:0C.6 (B2:D12:F6)
 *  MC1 Ch2        | ??:18.0 (D24:F0) | 02:0D.2 (B2:D13:F2)
 *  MC1 Ch3        | ??:18.1 (D24:F1) | -
 *
 *  Memory/Channel | Haswell | Skylake
 *  MC0 Ch0        | 0x2FB4  | 0x2042
 *  MC0 Ch1        | 0x2FB5  | 0x2046
 *  MC0 Ch2        | 0x2FB0  | 0x204A
 *  MC0 Ch3        | 0x2FB1  | -
 *  MC1 Ch0        | 0x2FD4  | 0x2042
 *  MC1 Ch1        | 0x2FD5  | 0x2046
 *  MC1 Ch2        | 0x2FD0  | 0x204A
 *  MC1 Ch3        | 0x2FD1  | -
 *
 *	Register                   | Haswell | Skylake
 *	MC_CHy_PCI_PMON_BOX_STATUS | F8      | ?
 *	MC_CHy_PCI_PMON_BOX_CTL    | F4      | ?
 *	MC_CHy_PCI_PMON_FIXED_CTL  | F0      | F0
 *	MC_CHy_PCI_PMON_CTL0       | D8      | D8
 *	MC_CHy_PCI_PMON_CTL1       | DC      | DC
 *	MC_CHy_PCI_PMON_CTL2       | E0      | E0
 *	MC_CHy_PCI_PMON_CTL3       | E4      | E4 (?)
 *	MC_CHy_PCI_PMON_FIXED_CTR  | D4+D0   | D0
 *	MC_CHy_PCI_PMON_CTR0       | A4+A0   | A0
 *	MC_CHy_PCI_PMON_CTR1       | AC+A8   | A8
 *	MC_CHy_PCI_PMON_CTR2       | B4+B0   | B0
 *	MC_CHy_PCI_PMON_CTR3       | BC+B8   | B8 (?)
 *
 *	The registers are just offsets when reading a
 *	PCI device files.
 */

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ear_verbose.h>
#include <hardware.h>
#include <uncore_architectures/pci_uncores.h>
#include <config.h>

#define IJKFOR(i_len, j_len, k_len) \
    for(i = 0; i < i_len; i++)      \
    for(j = 0; j < j_len; j++)      \
    for(k = 0; k < k_len; k++)

static int n_buses;
static int n_functions;
static int *fd_functions;
static int _cpu_model;

// These macros are converted in a switch case. The
// purpose is, given a CPU architecture, offer an array
// of device, function, command and offset numbers
// needed to the correct reading and writing the
// the files of a set of integrated memory controllers.
#define pci_case(arch, retval)                          \
    case arch:                                          \
    *length = (int) sizeof(retval) / sizeof(retval[0]); \
    return retval;

static char* get_arch_devices(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_DEVICES);
        pci_case(CPU_BROADWELL_X, HASWELL_X_DEVICES);
        pci_case(CPU_SKYLAKE_X, SKYLAKE_X_DEVICES);
        default: *length = 0; return NULL;
    }
}

static char* get_arch_functions(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_FUNCTIONS);
        pci_case(CPU_BROADWELL_X, HASWELL_X_FUNCTIONS);
        pci_case(CPU_SKYLAKE_X, SKYLAKE_X_FUNCTIONS);
        default: *length = 0; return NULL;
    }
}

static char* get_arch_reset_controls(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_RST_CTL);
        pci_case(CPU_BROADWELL_X, HASWELL_X_RST_CTL);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_RST_CTL);
        default: *length = 0; return NULL;
    }
}

static int* get_arch_reset_commands(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_RST_CMD);
        pci_case(CPU_BROADWELL_X, HASWELL_X_RST_CMD);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_RST_CMD);
        default: *length = 0; return NULL;
    }
}

static int* get_arch_start_commands(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_STA_CMD);
        pci_case(CPU_BROADWELL_X, HASWELL_X_STA_CMD);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_STA_CMD);
        default: *length = 0; return NULL;
    }
}

static char* get_arch_start_controls(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_STA_CTL);
        pci_case(CPU_BROADWELL_X, HASWELL_X_STA_CTL);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_STA_CTL);
        default: *length = 0; return NULL;
    }
}

static int* get_arch_stop_commands(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_STO_CMD);
        pci_case(CPU_BROADWELL_X, HASWELL_X_STO_CMD);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_STO_CMD);
        default: *length = 0; return NULL;
    }
}

static char* get_arch_stop_controls(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_STO_CTL);
        pci_case(CPU_BROADWELL_X, HASWELL_X_STO_CTL);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_STO_CTL);
        default: *length = 0; return NULL;
    }
}

static char* get_arch_read_counters(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_RED_CTR);
        pci_case(CPU_BROADWELL_X, HASWELL_X_RED_CTR);
        pci_case(CPU_SKYLAKE_X, HASWELL_X_RED_CTR);
        default: *length = 0; return NULL;
    }
}

static short* get_arch_ids(int *length)
{
    switch (_cpu_model) {
        pci_case(CPU_HASWELL_X, HASWELL_X_IDS);
        pci_case(CPU_BROADWELL_X, HASWELL_X_IDS);
        pci_case(CPU_SKYLAKE_X, SKYLAKE_X_IDS);
        default: *length = 0; return NULL;
    }
}

static int get_cpu_n_functions()
{
    switch (_cpu_model) {
        case CPU_HASWELL_X: return HASWELL_X_N_FUNCTIONS;
        case CPU_BROADWELL_X: return HASWELL_X_N_FUNCTIONS;
        case CPU_SKYLAKE_X: return SKYLAKE_X_N_FUNCTIONS;
        default: return 0;
    }
}

// This function writes commands (bytes) into files
static int write_command(uchar *ctl, int *cmd, int n_ctl, int n_cmd)
{
    int i, j, k;
    int res;

    // writes the command array cmd[] in the file pointed
    // by fd_functions[] descriptors with ctl[] offsets
    IJKFOR(n_functions, n_ctl, n_cmd)
    {
        if (fd_functions[i] != -1)
        {
            res = pwrite(fd_functions[i], (void*) &cmd[k], sizeof(int), (int) ctl[j]);
            if (res == -1 || res != sizeof(int))
            {
                ear_verbose(0, "pci_uncores.c: writing file error (fd %i) (%s)\n",
                            fd_functions[i], strerror(errno));

                close(fd_functions[i]);
                fd_functions[i] = -1;
            }
        }
    }
    return -(n_functions == EAR_ERROR);
}

static void pci_scan_uncores()
{
    char path[PATH_MAX];
    uint read_tag, look_tag;
    int fd, bus, i, j, k, l;
    
    int n_devs, n_funcs, n_ids;
    uchar *devs, *funcs;
    short *ids;

    ids = get_arch_ids(&n_ids);
    devs = get_arch_devices(&n_devs);
    funcs = get_arch_functions(&n_funcs);
    fd_functions = malloc(sizeof(int) * 256);
    bus = -1;

    // Open pci files looking for an identification of intel (0x8086)
    // and also the respective architecture uncore counters.
    IJKFOR(256, n_devs, n_funcs)
    {
        sprintf(path, "/proc/bus/pci/%x/%0.2x.%x", i, devs[j], funcs[k]);
        fd = open(path, O_RDWR);
        if (fd == -1) continue;

        read_tag = 0;
        look_tag = 0;
        l = 0;

        pread(fd, &read_tag, 4, 0x00);

        do {
            look_tag = (ids[l++] << 16) | 0x8086;
        } while (l < n_ids && read_tag != look_tag);

        if (read_tag == look_tag)
        {
            ear_debug(1, "pci_uncores.c: uncore function found in %s\n", path);
            fd_functions[n_functions] = fd;
            n_functions = n_functions + 1;

            n_buses = n_buses + (i != bus);
            bus = i;
        } else {
            close(fd);
    	}
    }

    ear_debug(1, "pci_uncores.c: %i total uncore functions found\n", n_functions);
}

// Compares the supposed quantity of buses to be
// detected with the quantity of detected ones.
int pci_check_uncores()
{
    int supposed = n_buses * get_cpu_n_functions();
    ear_debug(1, "pci_uncores.c: %d detected functions versus %d supposed\n", n_functions, supposed);

    if (supposed != n_functions) return EAR_WARNING;
    if (supposed <= 0) return EAR_ERROR;
    return EAR_SUCCESS;
}

//Open PCI files and also allocates the memory needed
int pci_init_uncores(int cpu_model)
{
    _cpu_model = cpu_model;
    pci_scan_uncores();

    return pci_count_uncores();
}

// Get the number of the performance monitor uncore
// counters that will be needed. Needed when you
// stop the counters and pass a buffer to be filled
// with those values.
int pci_count_uncores()
{
    int n_ctrs;
    get_arch_read_counters(&n_ctrs);
    return n_functions * n_ctrs;
}

// Freezes and reset all uncore counters
int pci_reset_uncores()
{
    int n_cmd, n_ctl;
    int *cmd = get_arch_reset_commands(&n_cmd);
    char *ctl = get_arch_reset_controls(&n_ctl);
	return write_command(ctl, cmd, n_ctl, n_cmd);
}

// Unfreezes all uncore counters
int pci_start_uncores()
{
    int n_cmd, n_ctl;
    int *cmd = get_arch_start_commands(&n_cmd);
    char *ctl = get_arch_start_controls(&n_ctl);
    return write_command(ctl, cmd, n_ctl, n_cmd);
}

// Stop all uncore counters and also gets it's values
int pci_stop_uncores(ull *values)
{
    int n_cmd, n_ctl, res;
    int *cmd = get_arch_stop_commands(&n_cmd);
    char *ctl = get_arch_stop_controls(&n_ctl);
    res = write_command(ctl, cmd, n_ctl, n_cmd);
    pci_read_uncores(values);
    return res;
}

// Reads registers values.
int pci_read_uncores(ull *values)
{
    int i, j, k, res;
    int n_ctrs;
    ull aux;

    uchar *ctrs = get_arch_read_counters(&n_ctrs);

    for(i = k = 0; i < n_functions; i++)
    {
        for (j = 0; j < n_ctrs; ++j, ++k)
        {
            values[k] = 0;
            if (fd_functions[i] != -1)
            {
                res = pread(fd_functions[i], &values[k], sizeof(ull), ctrs[j]);

                if (res == -1 || res != sizeof(ull))
                {
                    ear_verbose(0, "pci_uncores.c: reading file error (%s)\n",
                                strerror(errno));
                    close(fd_functions[i]);
                    fd_functions[i] = -1;
                }
            }
        }
    }

    return  0;
}

// Just closes all file descriptors and frees it's
// allocated memory
int pci_dispose_uncores()
{
    int i;

    if(fd_functions == NULL) {
        return EAR_ERROR;
    }

    for(i = 0; i < n_functions; i++) {
        if (fd_functions[i] != -1) {
            close(fd_functions[i]);
        }
    }

    free(fd_functions);

    return 0;
}
