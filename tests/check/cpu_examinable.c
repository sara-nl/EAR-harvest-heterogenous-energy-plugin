#include <assert.h>
#include <stdio.h>
#include <hardware.h>
#include <states.h>

int main ()
{
    int examinable = is_cpu_examinable();
    printf("%d\n", examinable);
    assert(examinable == EAR_SUCCESS);
    return 0;
}
