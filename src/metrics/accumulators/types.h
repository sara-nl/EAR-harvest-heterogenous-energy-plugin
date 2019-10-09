#ifndef EAR_PRIVATE_TYPES_H
#define EAR_PRIVATE_TYPES_H

#include <common/sizes.h>

typedef void *edata_t;

typedef struct ehandler {
	char manufacturer[SZ_NAME_MEDIUM];
	char product[SZ_NAME_MEDIUM];
	void *context;
} ehandler_t;

typedef long long rapl_data_t;
typedef edata_t node_data_t;

#endif //EAR_PRIVATE_TYPES_H

