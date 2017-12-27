#include <assert.h>
#include <hardware.h>
#include <config.h>
#include <stdio.h>

int main ()
{
    int examinable = is_cpu_examinable();
    printf("%d\n", examinable);
    assert(examinable == EAR_SUCCESS);
    return 0;
}
