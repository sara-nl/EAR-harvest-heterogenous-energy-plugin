#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <metrics/gpu/gpu.h>

#define ok(fun) \
    if (xtate_fail(s, fun)) { \
        printf("--------- calling " #fun " (%d, %s)\n", s, state_msg); \
    } else { \
    	printf("--------- calling " #fun " (%d)\n", s); \
    } 

#ifndef PLUGIN
int main(int argc, char *argv[])
{
	state_t s;

	ok(gpu_load(empty, none, empty));
	
	//load plugin
	void (*plugin) (void) = NULL;
	void *handler = NULL;
	
	handler = dlopen("./libplugin.so", RTLD_LAZY | RTLD_GLOBAL);
	
	if (handler == NULL) {
		printf("dlopen failed (%s)\n", dlerror());
		return 0;
	}
	
	plugin = dlsym(handler, "plugin");
	
	if (plugin == NULL) {
		printf("dlsym failed (%s)\n", dlerror());
		return 0;
	}

	plugin();

	return 0;
}
#else
void plugin()
{
	state_t s;
	printf("plugin called\n");
	ok(gpu_load(empty, none, empty));
}
#endif
