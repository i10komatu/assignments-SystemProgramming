#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	if (argc != 2) return 1;
	int n = atoi(argv[1]);
	for (int i = 0; i < n; i++) {
		int r = i % 3;
		int pid = fork();
		if (pid < 0) {
			fprintf(stderr, "%d: can't fork.\n", getpid());
			perror("fork");
			return 1;
		} else if (pid == 0) {
			printf("I'm child %d, sleeping %d seconds.\n", getpid(), r);
			sleep(r);  // r秒間待つ。
			return r;
		}
	}

	for (int i = 0; i < n; i++) {
		int pid;
		int status;
		if ((pid = wait(&status)) < 0) {
			perror("wait");
		} else {
			if (WIFEXITED(status)) {
				printf("child %d exited with status: %d\n", pid, WEXITSTATUS(status));
			}
		}
	}

	return 0;
}
