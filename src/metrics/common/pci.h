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

#ifndef EAR_PCI_H
#define EAR_PCI_H

typedef struct pci_s {
	uint bus;
	uint device;
	uint function;
	uint fd;
	uint init;
} pci_t;

/* Find a PCI ID in all PCI devices in the system and open it. */
state_t pci_find(uint id, pci_t *pci);

state_t pci_open(pci_t *pci, uint bus, uint device, uint function);

state_t pci_close(pci_t *pci);

state_t pci_read(pci_t *pci, void *buffer, size_t size, off_t offset);

state_t pci_write(pci_t *pci, const void *buffer, size_t size, off_t offset);

#endif //EAR_PCI_H