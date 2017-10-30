#include "morecore.h"
#include <sys/mman.h>  /* for mmap(), PROT_*, MAP_* */
#include <stddef.h>

static int total = 0;

void *morecore(int nbytes, int *realbytes)
{
	void *cp;
	if (total > CORE_LIMIT) {
		/* Already allocated to maximum */
		return NULL;
	} 

	*realbytes = (nbytes + CORE_UNIT - 1) / CORE_UNIT * CORE_UNIT;

	cp = mmap(NULL, *realbytes, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
	if (cp == (void *)-1) {
		return NULL;
	}
	total += *realbytes;

	return cp;
}
