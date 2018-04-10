#include <string.h>
#include "signature.h"


void copy_signature(signature_t *destiny, signature_t *source)
{
    memcpy(destiny, source, sizeof(signature_t));
}

void init_signature(signature_t *sig)
{
    memset(sig, 0, sizeof(signature_t));
}

static uint equal_with_th(double a, double b, double th)
{
	int eq;
	if (a > b) {
		if (a < (b * (1 + th))) eq = 1;
		else eq = 0;
	} else {
		if ((a * (1 + th)) > b) eq = 1;
		else eq = 0;
	}
	return eq;
}

uint are_equal(signature_t *sig1, signature_t *sig2, double th)
{
    if (!equal_with_th(sig1->DC_power, sig2->DC_power, th)) return 0;
    if (!equal_with_th(sig1->DRAM_power, sig2->DRAM_power, th)) return 0;
    if (!equal_with_th(sig1->PCK_power, sig2->PCK_power, th)) return 0;
    if (!equal_with_th(sig1->EDP, sig2->EDP, th)) return 0;    
    if (!equal_with_th(sig1->GBS, sig2->GBS, th)) return 0;    
    if (!equal_with_th(sig1->TPI, sig2->TPI, th)) return 0;    
    if (!equal_with_th(sig1->CPI, sig2->CPI, th)) return 0;    
    if (!equal_with_th(sig1->Gflops, sig2->Gflops, th)) return 0;    
    return 1;
}

void print_signature_fd(int fd, signature_t *sig)
{
    //print order: AVG.FREQ;DEF.FREQ;" \
        "TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;CYCLES;INSTRUCTIONS;L1_MISSES;" \
        "L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;" \
        "DP_512;
    int i;
    
	dprintf(fd, "%u;%u;", sig->avg_f, sig->def_f);
	dprintf(fd, "%lf;%lf;%lf;%lf;", sig->time, sig->CPI, sig->TPI, sig->GBS);
	dprintf(fd, "%lf;%lf;%lf;", sig->DC_power, sig->DRAM_power, sig->PCK_power);
	dprintf(fd, "%llu;%llu;", sig->cycles, sig->instructions);
	dprintf(fd, "%llu;%llu;%llu;", sig->L1_misses, sig->L2_misses, sig->L3_misses);
	dprintf(fd, "%lf;%llu", sig->Gflops, sig->FLOPS[0]);

    for (i = 1; i < FLOPS_EVENTS; ++i) {
		dprintf(fd, ";%llu", sig->FLOPS[i]);
	}
}