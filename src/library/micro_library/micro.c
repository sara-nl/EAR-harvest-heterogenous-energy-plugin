#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <library/dynais/dynais.h>
#include <library/micro_library/micro.h>

int disabled;

void micro_init()
{
	char *var = getenv("MICRO_DISABLED");
	disabled = (var == NULL) || (var[0] == '1');
	dynais_init(100, 10);

	fprintf(stderr, "MICRO INIT, disabled '%u'\n", disabled);
}

void micro_call(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned long  event_l;
	unsigned short event_s;
	unsigned short result;
	unsigned short level;
	unsigned short size;

	if (disabled) {
		return;
	}

	event_l = (unsigned long) ((((buf >> 5) ^ dest) << 5) | call_type);
	event_s = dynais_sample_convert(event_l);
	result = dynais(event_s, &size, &level);
}

void micro_end()
{
	//fprintf(stderr, "MICRO END\n");
}
