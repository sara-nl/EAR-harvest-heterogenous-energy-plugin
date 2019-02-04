#ifndef EAR_DEBUG_H
#define EAR_DEBUG_H

#include <stdio.h>
#include <string.h>

int debug_channel	__attribute__((weak)) = 2;

#ifdef SHOW_DEBUGS
#define debug(...) \
        dprintf(debug_channel, "%s: ", __FUNCTION__); \
        dprintf(debug_channel, __VA_ARGS__); \
        dprintf(debug_channel, "\n");
#else
#define debug(...)
#endif

#endif //EAR_DEBUG_H
