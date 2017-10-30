/* redirect.c -- TCP/IP redirector
   usage: redirect [-p my_port] host port
          redirect [-p my_port]
   if my_port is not given, allocate arbitrary port number and print it.
 */

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include "netlib.h"

static int do_redirect(int in1, int out1, int in2, int out2);
static int copy(int src_sock, int dest_sock);

int main(int argc, char *argv[])
{
	int server_sock, src_sock;
	struct sockaddr_in addr, client_addr;
	unsigned int addr_len = sizeof(addr);
	int my_port = 0;
	int remote_sock;

	if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
		my_port = atoi(argv[2]);
		argc -= 2;
		argv += 2;
	}
	if (argc != 1 && argc != 3) {
		fprintf(stderr, "usage: %s [-p my_port] host dest_port\n"
				"       %s [-p my_port]\n", argv[0], argv[0]);
		return 1;
	}
	check((server_sock = make_server_sock(my_port)), "make_server_sock", 1);
	check(getsockname(server_sock, (struct sockaddr *)&addr, &addr_len), "getsockname", 1);
	show_addr("Waiting for connection at", &addr);
	if (my_port == 0) {
		printf("%u\n", ntohs(addr.sin_port));
	}

	check((src_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len)), "accept", 1);
	show_addr("Accepted connection from", &client_addr);
	close(server_sock);

	if (argc == 1) {
		return do_redirect(src_sock, 1, 0, src_sock);
	} else {
		check(remote_sock = connect_dest(argv[1], argv[2]), "connect_dest", 1);
		return do_redirect(src_sock, remote_sock, remote_sock, src_sock);
	}
}

int max(int x, int y) { return x > y ? x : y; }

static int do_redirect(int in1, int out1, int in2, int out2)
	/* in1 と in2 を多重化し，in1からout1へ，in2からout2へデータを転送する．
	   正常終了なら0を，エラーなら1を返す． */
{
	int epfd = epoll_create(2);
	int bytes;

	struct epoll_event ev[2];
	int in[2] = {in1, in2};
	int out[2] = {out1, out2};

	struct epoll_event events[2];

	for (int i = 0; i < 2; i++) {
		ev[i].events = EPOLLIN;
		ev[i].data.fd = in[i];
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, in[i], &ev[i]) < 0) {
			perror("epoll_ctl : EPOLL_CTL_ADD");
			exit(EXIT_FAILURE);
		}
	}

	while (1) {
		int nevents = epoll_wait(epfd, events, 2, -1);
		for (int i = 0; i < nevents; i++) {
			for (int j = 0; j < 2; j++) {
				if (events[i].data.fd == in[j]) {
					if ((bytes = copy(in[j], out[j])) <= 0) {
						break;
					}
				}
			}
		}
	}
	return bytes < 0;  /* bytesが0なら0を，負なら1を返す */
}

static int copy(int in, int out)
{
	char buf[BUFSIZ];
	int bytes = read(in, buf, sizeof(buf));
	int written = 0;
	while (written < bytes) {
		int b;
		if ((b = write(out, buf + written, bytes - written)) < 0) {
			return b;
		}
		written += b;
	}
	return bytes;
}
