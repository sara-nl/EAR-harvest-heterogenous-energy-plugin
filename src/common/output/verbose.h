#ifndef EAR_VERBOSE_H
#define EAR_VERBOSE_H

#include <stdio.h>
#include <string.h>
#include <common/config.h>

int verb_channel	__attribute__((weak)) = 2;
int warn_channel	__attribute__((weak)) = 2;
int verb_level		__attribute__((weak)) = 0;

#define verbose(v, ...) \
	if (v <= verb_level) { \
        dprintf(verb_channel, __VA_ARGS__); \
        dprintf(verb_channel, "\n"); \
	}

#define verb(v, ...) \
	verbose(v, __VA_ARGS__)

#if SHOW_WARNINGS0
#define warning(...) dprintf(warn_channel, __VA_ARGS__);
#else
#define warning(...)
#endif

#if SHOW_WARNINGS1
#define warning1(...) dprintf(warn_channel, __VA_ARGS__); 
#else
#define warning1(...)
#endif

#endif //EAR_VERBOSE_H
