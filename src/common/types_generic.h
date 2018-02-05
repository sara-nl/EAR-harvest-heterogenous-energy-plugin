#ifndef _EAR_TYPES_GENERIC
#define _EAR_TYPES_GENERIC

#include <linux/limits.h>

#define PERMISSION_RWRR S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS_WCTA O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

typedef unsigned char uchar;
typedef unsigned long long ull;
typedef unsigned long ulong;
typedef unsigned int uint;

#endif
