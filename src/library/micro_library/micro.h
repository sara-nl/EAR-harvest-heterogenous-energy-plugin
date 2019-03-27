#ifndef EAR_PRIVATE_MICRO_H
#define EAR_PRIVATE_MICRO_H

#include <library/micro_library/MPI_types.h>
#include <library/micro_library/MPI_calls_coded.h>

void micro_init();
void micro_call(mpi_call call_type, p2i buf, p2i dest);
void micro_end();

#endif //EAR_PRIVATE_MICRO_H
