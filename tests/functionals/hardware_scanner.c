#include <stdio.h>
#include <hardware.h>
//#include <config.h>

int EAR_VERBOSE_LEVEL = 4;

int main (int argc, char *argv[])
{
	char buffer[32];
	int value;

	get_vendor_id(buffer);
	printf("CPU vendor: %s\n", buffer);
	
	value = get_model();
	printf("CPU model: %d\n", value);

	value = get_family();
	printf("CPU family: %d\n", value);
	
	return 0;
}
