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

#include <fcntl.h>
#include <common/sizes.h>
#include <common/output/debug.h>
#include <metrics/common/pci.h>

state_t pci_find(uint id, pci_t *pci)
{
	// Iterate over all '/proc/bus/pci/' files, find the id value
	// in the position 0x00, and return a pci_t, filled with the
	// correct data and opened file descriptor.
	return EAR_SUCCESS;
}

state_t pci_open(pci_t *pci, uint bus, uint device, uint function)
{
	char path[SZ_BUFF_BIG];

	//
	if (pci->init == 1) {
		return_msg(EAR_INITIALIZED, Generr.api_initialized);
	}
	//
	sprintf(path, "/proc/bus/pci/%.2x/%.2x.%x", bus, device, function);
	debug("path is '%s'", path);
	//
	pci->bus      = bus;
	pci->device   = device;
	pci->function = function;
	pci->fd       = open(path, O_RDWR);
	pci->init     = pci->fd >= 0;
	//
	if (pci->fd == -1) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}

	return EAR_SUCCESS;
}

state_t pci_close(pci_t *pci)
{
	if (pci->init != 1) {
		return_msg(EAR_NOT_INITIALIZED, Generr.api_uninitialized);
	}
	close(pci->fd);
	pci->init =  0;
	pci->fd   = -1;
	return EAR_SUCCESS;
}

state_t pci_read(pci_t *pci, void *buffer, size_t size, off_t offset)
{
	if (pci->init != 1) {
		return_msg(EAR_NOT_INITIALIZED, Generr.api_uninitialized);
	}

	if (pread(pci->fd, buffer, size, offset) != size) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}

	return EAR_SUCCESS;
}

state_t pci_write(pci_t *pci, const void *buffer, size_t size, off_t offset)
{
	if (pci->init != 1) {
		return_msg(EAR_NOT_INITIALIZED, Generr.api_uninitialized);
	}

	if (pwrite(pci->fd, buffer, size, offset) != size) {
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}
