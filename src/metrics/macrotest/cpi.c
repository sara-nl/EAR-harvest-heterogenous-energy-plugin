#include <stdio.h>
#include <unistd.h>
#include <metrics/cpi/cpi.h>

#define ret(fun) \
	fun;

void stress()
{
	int i = 0;
	for (i = 0; i < 5000; ++i) {
		// 1: cond
		// 1: add
		// 1: jmp
		// T: 15.000
	}
}

int main(int argc, char *argv[])
{
	ullong instructions1;
	ullong instructions2;
	ullong cycles1;
	ullong cycles2;
	int i;

	// Init también inicializa su propia topologia
	ret(init_basic_metrics());

	ret(reset_basic_metrics());
	ret(start_basic_metrics());

	ret(read_basic_metrics((ullong *) &cycles1, (ullong *) &instructions1));
	stress();
	ret(stop_basic_metrics((ullong *) &cycles2, (ullong *) &instructions2));

	fprintf(stderr, "instructions1 %llu\n", instructions1);
	fprintf(stderr, "instructions2 %llu\n", instructions2);
	fprintf(stderr, "instructionsT %llu\n", instructions2 - instructions1);
	fprintf(stderr, "cycles1 %llu\n", cycles1);
	fprintf(stderr, "cycles2 %llu\n", cycles2);
	fprintf(stderr, "cyclesT %llu\n", cycles2 - cycles1);

	return 0;
}
