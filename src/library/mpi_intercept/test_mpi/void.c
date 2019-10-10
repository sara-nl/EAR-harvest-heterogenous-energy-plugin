#include <stdio.h>

char *string = "caca";

typedef void *p1_t;

typedef struct p2t {
	int a;
	int b;
	void *c;
} p2_t;


void stringize(void **c) {
	printf("%p\n", c);
}

int main (int argc, char *argv[])
{
	p1_t p1;
	p2_t p2;
	p1 = (void *) string;
	p2.c = string;

	stringize(&p1);
	stringize(&p2.c);
	stringize(p1);

	return 0;
}
