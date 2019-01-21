#ifndef EAR_DEBUG_H
#define EAR_DEBUG_H

#include <stdio.h>
#include <string.h>

#define DEBUG_CHANNEL stderr

#ifdef DEBUG_EAR
#define debug(...) \
        fprintf(DEBUG_CHANNEL, "%s: ", __FUNCTION__); \
        fprintf(DEBUG_CHANNEL, __VA_ARGS__); \
        fprintf(DEBUG_CHANNEL, "\n");
#else
#define debug(...)
#endif

#endif //EAR_DEBUG_H
