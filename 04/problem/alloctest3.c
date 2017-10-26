#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "alloc3.h"

#define NUM_MAX 1000
#define SIZE 10000

#define REPEAT 100

void error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	exit(1);
}

// tableに，割り付け済み領域へのポインタを格納する．
// それぞれの領域のサイズはSIZEバイト(ヘッダは含まない)
// table[0]..table[current_num - 1]までが使用中
char *table[NUM_MAX];
int current_num = 0;

// table内のポインタ数をnum個に変化させる
int alloc_to(int num, int size)
{
	if (num < 0 || NUM_MAX < num) {
		error("illegal num %d\n", num);
	}
	// もし num > current_num なら増やす
	while (current_num < num) {
		table[current_num] = alloc3(size);
		if (table[current_num] == NULL) {
			return -1;
		}
		current_num++;
	}
	// もし num < current_num なら減らす
	while (current_num > num) {
		afree3(table[--current_num]);
	}
	return 0;
}

// 0..(n-1)の一様乱数を返す
int random_number_upto(int n) {
	return rand() / (1 + RAND_MAX / n);
}

// table内をランダムに並べ替える
void randomize(void) {
	int i;
	for (i = 0; i < current_num; i++) {
		int j = random_number_upto(current_num);
		void *p = table[i];
		table[i] = table[j];
		table[j] = p;
	}
}

// 割り付け済みの領域に重なりがないかチェックする
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

// 割り付け済みの領域に書き込めるかチェックする
void check_writable(void) {
	int i, j;
	for (i = 0; i < current_num; i++) {
		for (j = 0; j < SIZE; j++) {
			table[i][j] = i + j;
		}
	}
}

int main(void)
{
	int pass;
	void *p;

	/* テスト: ランダムな順序で割り付けと解放を繰り返す
	   (途中で，重なりがないことをチェック) */
	printf("test: "); fflush(stdout);
	for (pass = 0; pass < REPEAT; pass++ ) {
		alloc_to(random_number_upto(NUM_MAX), SIZE);
		check_overlap();
		check_writable();
		randomize();
		putchar('.'); fflush(stdout);
	}
	printf("passed.\n");
	alloc_to(0, SIZE);

	return 0;
}
