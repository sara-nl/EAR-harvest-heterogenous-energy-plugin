#include <nvml.h>

typedef unsigned int uint;

int main(int argc, char *argv[])
{
	nvmlEnableState_t is, isDefault;
	nvmlDevice_t device;
	uint mem_clock_list[200];
	uint clock_list[200];
	uint mem_clock;
	uint clock;
	int i;

	if ((r = nvmlDeviceGetHandleByIndex_v2(i, &device)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetHandleByIndex_v2 returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("device %d:", i);

	if ((r = nvmlDeviceSetApplicationsClocks(device, 0, 1380)) != NVML_SUCCESS) {
		debug("nvmlDeviceSetApplicationsClocks returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tset clocks to %d/%d", 0, 1380);

	if ((r = nvmlDeviceGetAutoBoostedClocksEnabled(device, &is, &isDefault)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetAutoBoostedClocksEnabled returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tboosted clocks %d/%d", is, isDefault);

	if ((r = nvmlDeviceGetApplicationsClock(device, NVML_CLOCK_GRAPHICS, &clock)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetApplicationsClock returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tapplication clock %d", clock);

	if ((r = nvmlDeviceGetDefaultApplicationsClock (device, NVML_CLOCK_GRAPHICS, &clock)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetDefaultApplicationsClock returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tapplication default clock %d", clock);

	if ((r = nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &clock)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetClockInfo returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tinfo current clock %d", clock);

	if ((r = nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_GRAPHICS, &clock)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetMaxClockInfo returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tinfo max clock %d", clock);

	if ((r = nvmlDeviceGetMaxCustomerBoostClock (device, NVML_CLOCK_GRAPHICS, &clock)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetMaxCustomerBoostClock  returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tmax customer boost clock %d", clock);


	if ((r = nvmlDeviceGetSupportedMemoryClocks (device, &cm, mem_clock_list)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetSupportedMemoryClocks  returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tsupported memory clocks %d (mem %d): ", c, mem_clock);
	for (i = 0; i < cm; ++i) {
		debug("\tP%d: %d", i, clock_list[i]);
	}


	if ((r = nvmlDeviceGetSupportedGraphicsClocks (device, &mem_clock, &c, clock_list)) != NVML_SUCCESS) {
		debug("nvmlDeviceGetSupportedGraphicsClocks  returned %d (%s)",
			  r, nvml.ErrorString(r));
		return 0;
	}
	debug("\tsupported graphics clocks %d (mem %d): ", c, mem_clock);
	for (i = 0; i < c; ++i) {
		debug("\tP%d: %d", i, clock_list[i]);
	}

	return 0;
}