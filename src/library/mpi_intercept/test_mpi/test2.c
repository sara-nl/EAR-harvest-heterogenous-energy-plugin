#include <stdio.h>
#include <dlfcn.h>

void foo() {
	void *symbol = dlsym(RTLD_NEXT, "foo");
	void (*function) (void) = (void (*)()) symbol;
	printf("foo de test2 %p\n", function);
	function();
}
