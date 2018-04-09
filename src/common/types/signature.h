
#ifndef _EAR_TYPES_SIGNATURE
#define _EAR_TYPES_IGNATURE

#include "generic.h"

#define FLOPS_EVENTS 8

typedef struct signature
{
    double DC_power;
    double DRAM_power;
    double PCK_power;
    double EDP;
    double GBS;
    double TPI;
    double CPI;
    double Gflops;
    double time;
    ull FLOPS[FLOPS_EVENTS];
    ull L1_misses;
    ull L2_misses;
    ull L3_misses;
    ull instructions;
    ull cycles;
    ulong avg_f;
    ulong def_f;
} signature_t;


// Function declarations

//
void copy_signature(signature_t *destiny, signature_t *source);

// Resets values
void init_signature(signature_t *sig);

// returns true if basic values for sig1 and sig2 are equal with a maximum % of difference defined by th
uint are_equal(signature_t *sig1,signature_t *sig2,double th);

#endif