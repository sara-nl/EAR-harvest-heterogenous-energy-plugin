#include <stdlib.h>
#include "loop.h"


loop_id_t *create_loop_id(ulong event, ulong size, ulong level)
{
    loop_id_t *loop_id = malloc(sizeof(loop_id_t));
    loop_id->event = event;
    loop_id->size = size;
    loop_id->level = level;

    return loop_id;
}

loop_t *create_loop(loop_id_t loop_id)
{
    loop_t *loop = calloc(1, sizeof(loop));
    loop->id = loop_id;
    return loop;
}

void loop_init(loop_t *loop)
{
    memset(loop, 0, sizeof(loop_t));
}


void add_loop_signature(loop_t *loop,  signature_t *sig)
{
    loop->signatures = *sig;
}

void end_loop(loop_t *loop, ulong iterations)
{
    loop->toal_iterations = iterations;
}

void copy_loop(loop_t *destiny, loop_t *source)
{
    memcpy(destiny, source, sizeof(loop_t));
}