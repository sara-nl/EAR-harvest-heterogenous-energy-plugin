#include <library/dynais/dynais.h>
#include <library/micro_library/micro.h>

void micro_init()
{
	fprintf(stderr, "MICRO INIT");
	dynais_init(100, 10);
}

void micro_call(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned long  event_l;
	unsigned short event_s;
	unsigned short result;
	unsigned short level;
	unsigned short size;

	event_l = (unsigned long) ((((buf >> 5) ^ dest) << 5) | call_type);
	event_s = dynais_sample_convert(event_l);
	result = dynais(event_s, &size, &level);

}

void micro_end()
{
	fprintf(stderr, "MICRO END");
}