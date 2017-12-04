#include <papi.h>
#include <assert.h>

int main(void)
{
    int state = PAPI_library_init(PAPI_VER_CURRENT);
    assert(state == PAPI_VER_CURRENT);

    state = PAPI_is_initialized();
    assert(state == PAPI_LOW_LEVEL_INITED);

    return 0;
}
