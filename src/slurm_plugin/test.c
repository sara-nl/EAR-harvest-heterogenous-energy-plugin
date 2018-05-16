//
// Created by xgomez on 16/05/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

char *strclean(char *string, char chr)
{
	char *index;

	if (string == NULL) {
		return NULL;
	}

	index = strchr(string, chr);

	if (index == NULL) {
		return NULL;
	}

	string[index - string] = '\0';

	return index;
}

int main(int argc, char **argv)
{
	char buffer1[PATH_MAX];
	char buffer2[PATH_MAX];
	char *pointer1;
	char *pointer2;

	sprintf(buffer1, "test.test.test.test");
	printf("%s\n", buffer1);
	strclean(buffer1, '.');
	printf("%s\n", buffer1);
	sprintf(buffer1, "test.test.test.test");
	printf("%s\n", buffer1);
	strclean(buffer1, '=');
	printf("%s\n", buffer1);
	buffer1[0] = '\0';
	printf("'%s'\n", buffer1);
	strclean(buffer1, '=');
	printf("test=", buffer1);
	strclean(buffer1, '=');
	printf("%s\n", buffer1);

	return 0;
}