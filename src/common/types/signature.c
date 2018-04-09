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
