
#include "memleak.h"

#include <stdio.h>
#include <stdlib.h>

int main() {

	init();

	void *p1 = malloc(5); 
	void *p2 = malloc(10);
	void *p3 = malloc(15);

	free(p1);
	free(p3);

    return 0;
}


