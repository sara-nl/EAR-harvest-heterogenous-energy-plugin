#include <assert.h>
#include <papi.h>

int main(int argc, char *argv[])
{
    const PAPI_component_info_t *info;
    int state = PAPI_library_init(PAPI_VER_CURRENT);
    assert(state == PAPI_VER_CURRENT);

    state = PAPI_get_component_index(argv[1]);
    assert(state == PAPI_OK);

    info = PAPI_get_component_info(state);
    assert(!info->disabled);

    return 0;
}
