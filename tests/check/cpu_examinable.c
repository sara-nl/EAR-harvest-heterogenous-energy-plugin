#include <stdio.h>
#include <assert.h>
#include <metrics/custom/hardware_info.h>
#include <common/states.h>

int main ()
{
    int examinable = is_cpu_examinable();
    printf("%d\n", examinable);
    assert(examinable == EAR_SUCCESS);
    return 0;
}
