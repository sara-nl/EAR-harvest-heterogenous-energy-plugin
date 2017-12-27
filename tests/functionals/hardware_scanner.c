#include <stdio.h>
#include <hardware.h>
#include <string_enhanced.h>
#include <config.h>

int EAR_VERBOSE_LEVEL = 4;

int main (int argc, char *argv[])
{
	char buffer[32];
	int value;

	print_spacing_digits(30);

	get_vendor_id(buffer);
	print_spacing_string("CPU vendor id");
	print_spacing_string(buffer);
	printf("\n");

	value = get_model();
	print_spacing_string("CPU model id");
	print_spacing_int(value);
	printf("\n");

	value = get_family();
	print_spacing_string("CPU family id");
	print_spacing_int(value);
	printf("\n");

	value = get_cache_line_size();
	print_spacing_string("CPU cache line size (bytes)");
	print_spacing_int(value);
	printf("\n");

	value = is_cpu_hyperthreading_capable();
	print_spacing_string("CPU HyperThreading capable");
	print_spacing_int(value);
	printf("\n");

	value = is_aperf_compatible();
	print_spacing_string("CPU APERF compatible");
	print_spacing_int(value);
	printf("\n");

	return 0;
}
