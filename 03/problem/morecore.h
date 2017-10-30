#ifndef MORECORE_H
#define MORECORE_H

#define CORE_LIMIT (64 * 1024 * 1024) /* OSに要求するバイト数の上限 */

#define CORE_UNIT (64 * 1024) /* OSに要求する単位 */

extern void *morecore(int nbytes, int *realbytes);

#endif
