#ifndef EAR_VERBOSE_H
#define EAR_VERBOSE_H

#include <stdio.h>
#include <string.h>

int verb_channel	__attribute__((weak)) = STDERR_FILENO;
int verb_level		__attribute__((weak));

#define verbose(v, ...) \
	if (v <= verb_level) { \
        dprintf(verb_channel, __VA_ARGS__); \
        dprintf(verb_channel, "\n"); \
	}

#define verb(v, ...) \
	verbose(v, __VA_ARGS__)

#endif //EAR_VERBOSE_H
