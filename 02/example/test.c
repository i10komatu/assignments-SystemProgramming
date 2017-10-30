#include <stdio.h>

#include "alloc.h"

int main() {
	int i;
	char *s = (char *)alloc(sizeof(char) * 6);
	int *n = (int *)alloc(sizeof(int) * 4);

	s[0] = 'H';
	s[1] = 'e';
	s[2] = 'l';
	s[3] = 'l';
	s[4] = 'o';
	s[5] = '\0';

	printf("%p:%s\n", s, s);

	n[0] = 10;
	n[1] = 20;
	n[2] = 30;
	n[3] = 40;

	printf("%p:[%d,%d,%d,%d]\n", n, n[0], n[1], n[2], n[3]);

	afree(n);
	afree(s);

	return 0;
}
