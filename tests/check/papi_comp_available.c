#include <assert.h>
#include <papi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    const PAPI_component_info_t *info;
    int state = PAPI_library_init(PAPI_VER_CURRENT);
    assert(state == PAPI_VER_CURRENT);

    state = PAPI_get_component_index(argv[1]);
    assert(state >= 0);

    return 0;
}
