#include <assert.h>
#include <metrics/custom/hardware_info.h>

int main ()
{
    int aperf = is_aperf_compatible();
    assert(aperf == 1);
    return 0;
}
