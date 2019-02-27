#include "hardware_info.c"

int main(int argc, char *argv[])
{
	topology_t topo;

	hardware_topology_get(&topo);
	fprintf(stderr, "c%d t%d s%d\n", topo.cores, topo.threads, topo.sockets);

	return 0;
}