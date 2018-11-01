//
// Created by xgomez on 29/10/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <common/string_enhanced.c>

int main(int argc, char *argv[])
{
	tprintf_init(stderr, STR_MODE_COL, "18 11");
	tprintf("Hola||Adios");
	tprintf("----||-----");
	tprintf("1980||--");
	tprintf("<red>1990||--");
	tprintf("2000||--");
	tprintf("2010||--");

	return 0;
}