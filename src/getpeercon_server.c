/*
 * Simple INET/INET6/UNIX socket getpeercon() test server
 *
 * compile: gcc -o getpeercon_server -lselinux getpeercon_server.c
 *
 * Paul Moore <paul@paul-moore.com>
 *
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2008, 2010
 * (c) Copyright Red Hat, 2012, 2015
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <selinux/selinux.h>
#include <selinux/context.h>

#define UNIX_PATH_MAX 108
#define LISTEN_QUEUE 1
#define RECV_BUF_LEN 1024
#define ADDR_STR_LEN (INET6_ADDRSTRLEN + 1)

#define printf_out(...) \
	do { \
		if (log) { \
			fprintf(log, __VA_ARGS__); \
			fflush(log); \
		} \
		fprintf(stdout, __VA_ARGS__); \
		fflush(stdout); \
	} while(0);

#define printf_err(...) \
	do { \
		if (log) { \
			fprintf(log, __VA_ARGS__); \
			fflush(log); \
		} \
		fprintf(stderr, __VA_ARGS__); \
		fflush(stderr); \
	} while(0);

/**
 * usage
 */
void usage(char *prg)
{
	fprintf(stderr, "usage: %s [-h] [-l <logfile>] <port|path>\n", prg);
	exit(1);
}

/**
 * main
 */
int main(int argc, char *argv[])
{
	int rc;
	int arg_iter;
	int srv_sock, cli_sock;
	int family;
	int true = 1;
	struct sockaddr_storage cli_sock_saddr;
	struct sockaddr *cli_sock_addr;
	struct sockaddr_in *cli_sock_4addr;
	struct sockaddr_in6 *cli_sock_6addr;
	socklen_t cli_sock_addr_len;
	short srv_sock_port;
	char *srv_sock_path = NULL;
	char buffer[RECV_BUF_LEN];
	char cli_sock_addr_str[ADDR_STR_LEN];
	char *ctx;
	char *ctx_str;
	FILE *log = NULL;

	/* get the command line arguments */
	do {
		arg_iter = getopt(argc, argv, "hl:");
		switch (arg_iter) {
		case 'l':
			if (log)
				usage(argv[0]);
			log = fopen(optarg, "a");
			if (log == NULL) {
				printf_err("error: unable to open \"%s\"\n",
					   optarg);
				exit(1);
			}
			break;
		case 'h':
			/* help */
			usage(argv[0]);
			break;
		default:
			break;
		}
	} while (arg_iter > 0);
	if (optind >= argc)
		usage(argv[0]);
	srv_sock_port = atoi(argv[optind]);
	if (srv_sock_port == 0)
		srv_sock_path = argv[optind];

	rc = getcon(&ctx);
	if (rc < 0)
		ctx_str = strdup("NO_CONTEXT");
	else
		ctx_str = strdup(ctx);
	printf_err("-> running as %s\n", ctx_str);
	free(ctx_str);

	printf_err("-> creating socket ... ");
	if (srv_sock_path == NULL) {
		family = AF_INET6;
		srv_sock = socket(family, SOCK_STREAM, IPPROTO_TCP);
		if (srv_sock < 0) {
			family = AF_INET;
			srv_sock = socket(family, SOCK_STREAM, IPPROTO_TCP);
		}
	} else {
		family = AF_UNIX;
		srv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	if (srv_sock < 0) {
		printf_err("error: %d\n", srv_sock);
		return 1;
	}
	rc = setsockopt(srv_sock,
			SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true));
	if (rc < 0) {
		printf_err("error: %d\n", srv_sock);
		return 1;
	}
	printf_err("ok\n");

	if (srv_sock_path == NULL) {
		struct sockaddr_in6 srv_sock_addr;

		printf_err("-> listening on TCP port %hu ... ", srv_sock_port);
		memset(&srv_sock_addr, 0, sizeof(srv_sock_addr));
		srv_sock_addr.sin6_family = family;
		memcpy(&srv_sock_addr.sin6_addr, &in6addr_any,
		       sizeof(in6addr_any));
		srv_sock_addr.sin6_port = htons(srv_sock_port);
		rc = bind(srv_sock, (struct sockaddr *)&srv_sock_addr,
			  sizeof(srv_sock_addr));
	} else {
		struct sockaddr_un srv_sock_addr;

		printf_err("-> listening on UNIX socket %s ... ",
			   srv_sock_path);
		srv_sock_addr.sun_family = family;
		strncpy(srv_sock_addr.sun_path, srv_sock_path, UNIX_PATH_MAX);
		srv_sock_addr.sun_path[UNIX_PATH_MAX - 1] = '\0';
		rc = bind(srv_sock, (struct sockaddr *)&srv_sock_addr,
			  sizeof(srv_sock_addr));
	}
	if (rc < 0) {
		printf_err("bind error: %d\n", rc);
		return 1;
	}

	rc = listen(srv_sock, LISTEN_QUEUE);
	if (rc < 0) {
		printf_err("listen error: %d\n", rc);
		return 1;
	} else
		printf_err("ok\n");

	cli_sock_addr = (struct sockaddr *)&cli_sock_saddr;
	cli_sock_4addr = (struct sockaddr_in *)&cli_sock_saddr;
	cli_sock_6addr = (struct sockaddr_in6 *)&cli_sock_saddr;

	/* loop forever */
	for (;;) {
		printf_err("-> waiting ... ");
		memset(&cli_sock_saddr, 0, sizeof(cli_sock_saddr));
		cli_sock_addr_len = sizeof(cli_sock_saddr);
		cli_sock = accept(srv_sock, cli_sock_addr, &cli_sock_addr_len);
		if (cli_sock < 0) {
			printf_err("error: %d\n", cli_sock);
			continue;
		}
		rc = getpeercon(cli_sock, &ctx);
		if (rc < 0)
			ctx_str = strdup("NO_CONTEXT");
		else
			ctx_str = strdup(ctx);
		switch (cli_sock_addr->sa_family) {
		case AF_INET:
			inet_ntop(cli_sock_addr->sa_family,
				  (void *)&cli_sock_4addr->sin_addr,
				  cli_sock_addr_str, ADDR_STR_LEN);
			printf_err("connect(%s,%s)\n",
				   cli_sock_addr_str, ctx_str);
			break;
		case AF_INET6:
			if (IN6_IS_ADDR_V4MAPPED(&cli_sock_6addr->sin6_addr))
				inet_ntop(AF_INET,
				(void *)&cli_sock_6addr->sin6_addr.s6_addr32[3],
					  cli_sock_addr_str, ADDR_STR_LEN);
			else
				inet_ntop(cli_sock_addr->sa_family,
					  (void *)&cli_sock_6addr->sin6_addr,
					  cli_sock_addr_str, ADDR_STR_LEN);
			printf_err("connect(%s,%s)\n",
				   cli_sock_addr_str, ctx_str);
			break;
		case AF_UNIX:
			printf_err("connect(UNIX,%s)\n", ctx_str);
			break;
		default:
			printf_err("connect(%d,%s)\n",
				   cli_sock_addr->sa_family, ctx_str);
		}
		free(ctx_str);

		do {
			rc = recv(cli_sock, buffer, RECV_BUF_LEN, 0);
			if (rc < 0) {
				printf_err("error: %d\n", rc);
			} else {
				buffer[rc] = '\0';
				printf_out("%s", buffer);
			}
		} while (rc > 0);
		close(cli_sock);
		printf_err("-> connection closed\n");
	}
	if (log)
		fclose(log);

	return 0;
}
