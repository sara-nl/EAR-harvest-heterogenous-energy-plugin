#include <assert.h>
#include <hardware.h>

int main ()
{
    int aperf = is_aperf_compatible();
    assert(aperf == 1);
    return 0;
}
