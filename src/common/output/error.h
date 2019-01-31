#ifndef EAR_ERROR_H
#define EAR_ERROR_H

#include <stdio.h>
#include <string.h>

int err_channel    __attribute__((weak))=2;
#if SHOW_ERRORS
#define error(...) dprintf(err_channel, __VA_ARGS__); 
#else
#define error(...)
#endif


#endif //EAR_ERROR_H
