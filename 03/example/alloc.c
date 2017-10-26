#include "alloc.h"

static char allocbuf[ALLOCSIZE];	/* Memory area for alloc */
static char *allocp = allocbuf;		/* Next free position */

/* Return pointer to n characters */
void *alloc(int n) {
	/* We have enough space */
	if (allocbuf + ALLOCSIZE - allocp >= n) {
		void *p = allocp;
		allocp += n;
		return p;
	} else
		return 0;
}

void afree(void *p) {
	if (p >= (void *)allocbuf && p < (void *)allocbuf + ALLOCSIZE)
		allocp = p;
}

