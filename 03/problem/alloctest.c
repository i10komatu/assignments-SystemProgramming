#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "alloc2.h"

/* NUM_MAX * SIZE は ALLOCSIZE より小さく，ALLOCSIZEに近い値がよい */
#define NUM_MAX 100
#define SIZE 10000

#define REPEAT 100

void error(char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	exit(1);
}

/* tableに，割り付け済み領域へのポインタを格納する．
   それぞれの領域のサイズはSIZEバイト(ヘッダは含まない)
   table[0]..table[current_num - 1]までが使用中 */
void *table[NUM_MAX];
int current_num = 0;

/* table内のポインタ数をnum個に変化させる */
void alloc_to(int num, int size) {
	if (num < 0 || NUM_MAX < num) {
		error("illegal num %d\n", num);
	}
	/* もし num > current_num なら増やす */
	while (current_num < num) {
		table[current_num] = alloc2(size);
		if (table[current_num] == NULL) {
			error("allocation failed\n");
		}
		current_num++;
	}
	/* もし num < current_num なら減らす */
	while (current_num > num) {
		afree2(table[--current_num]);
	}
}

/* 0..(n-1)の一様乱数を返す */
int random_number_upto(int n) {
	return rand() / (1 + RAND_MAX / n);
}

/* table内をランダムに並べ替える */
void randomize(void) {
	int i;
	for (i = 0; i < current_num; i++) {
		int j = random_number_upto(current_num);
		void *p = table[i];
		table[i] = table[j];
		table[j] = p;
	}
}

/* 割り付け済みの領域に重なりがないかチェックする */
void check_overlap(void) {
	int i, j;
	for (i = 0; i < current_num; i++) {
		for (j = 0; j < current_num; j++) {
			if (i != j) {
				int distance = table[i] - table[j];
				if (distance < 0) {
					distance = -distance;
				}
				if (distance < SIZE) {
					error("overlap! distance(%d, %d) = %d\n", i, j, distance);
				}
			}
		}
	}
}

int main(void) {
	int i;
	void *p;

	/* テスト1: 上限を越えた割り付けを行なうと，ちゃんと失敗するか? */
	printf("test1: "); fflush(stdout);
	alloc_to(NUM_MAX, SIZE);
	if (alloc2(NUM_MAX * SIZE) != NULL) { /* 失敗するはず */
		error("over allocation\n");
	}
	printf("passed.\n");

	/* テスト2: ランダムな順序で割り付けと解放を繰り返す */
	printf("test2: "); fflush(stdout);
	for (i = 0; i < REPEAT; i++ ) {
		alloc_to(random_number_upto(NUM_MAX), SIZE);
		check_overlap();
		randomize();
		putchar('.'); fflush(stdout);
	}
	alloc_to(0, SIZE);
	printf("passed.\n");

	/* テスト3: 空き領域がすべて統合されているか試す */
	printf("test3: "); fflush(stdout);
	alloc_to(1, NUM_MAX * SIZE);
	printf("passed.\n");

	return 0;
}
