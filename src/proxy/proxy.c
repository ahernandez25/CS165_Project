/*
 * Copyright (c) 2008 Bob Beck <beck@obtuse.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *               */

/* server.c  - the "classic" example of a socket server */

/*
 * compile with gcc -o server server.c
 * or if you are on a crappy version of linux without strlcpy
 * thanks to the bozos who do glibc, do
 * gcc -c strlcpy.c
 * gcc -o server server.c strlcpy.        */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage()
{
	extern char * __progname;
	fprintf(stderr, "usage: %s portnumber\n", __progname);
	exit(1);
}

static void kidhandler(int signum) {
	/* signal handler for SIGCHLD */
	waitpid(WAIT_ANY, NULL, WNOHANG);
}


int main(int argc,  char *argv[])
{
	struct sockaddr_in sockname, client, server;
	char buffer[80], *ep, *ep2;
	struct sigaction sa;
	int sd, sdServer;
	socklen_t clientlen, serverLen;
	u_short port, portServer;
	pid_t pid;
	u_long p, p2;

	/*
 	 * first, figure out what port we will listen on - it should
 	 * be our first parameter.	 	 */

	if (argc != 3)
		usage();
		errno = 0;
        p = strtoul(argv[1], &ep, 10);
        if (*argv[1] == '\0' || *ep != '\0') {
		/* parameter wasn't a number, or was empty */
		fprintf(stderr, "%s - not a number\n", argv[1]);
		usage();
	}
        if ((errno == ERANGE && p == ULONG_MAX) || (p > USHRT_MAX)) {
		/* It's a number, but it either can't fit in an unsigned
 		 * long, or is too big for an unsigned short		 */
		fprintf(stderr, "%s - value out of range\n", argv[1]);
		usage();
	}
	/* now safe to do this */
	port = p;


	/*
 	 * makes sure port entered for server is valid               */

        if (argc != 3)
                usage();
                errno = 0;
        p2 = strtoul(argv[2], &ep2, 10);
        if (*argv[2] == '\0' || *ep2 != '\0') {
                /* parameter wasn't a number, or was empty */
                fprintf(stderr, "%s - not a number\n", argv[2]);
                usage();
        }
        if ((errno == ERANGE && p2 == ULONG_MAX) || (p2 > USHRT_MAX)) {
                /* It's a number, but it either can't fit in an unsigned
 *                  * long, or is too big for an unsigned short             */
                fprintf(stderr, "%s - value out of range\n", argv[2]);
                usage();
        }
        /* now safe to do this */
        portServer = p2;


	memset(&sockname, 0, sizeof(sockname));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(port);
	sockname.sin_addr.s_addr = htonl(INADDR_ANY);
	sd=socket(AF_INET,SOCK_STREAM,0);
	if ( sd == -1)
		err(1, "socket failed");

	if (bind(sd, (struct sockaddr *) &sockname, sizeof(sockname)) == -1)
		err(1, "bind failed");

	if (listen(sd,3) == -1)
		err(1, "listen failed");

	/*
 	 * we're now bound, and listening for connections on "sd" -
 	 * each call to "accept" will return us a descriptor talking to
 	 * a connected client */


	/*
 	* first, let's make sure we can have children without leaving
 	* zombies around when they die - we can do this by catching
 	* SIGCHLD. 	 	 	 */
	sa.sa_handler = kidhandler;
        sigemptyset(&sa.sa_mask);
	/*
 	* we want to allow system calls like accept to be restarted if they
 	* get interrupted by a SIGCHLD */
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
                err(1, "sigaction failed");

	/*
 	 * finally - the main loop.  accept connections and deal with 'em
	 * 	 	 */
	printf("Server up and listening for connections on port %u\n", port);
	for(;;) {
		int clientsd;
		clientlen = sizeof(&client);
		clientsd = accept(sd, (struct sockaddr *)&client, &clientlen);
		if (clientsd == -1)
			err(1, "accept failed");
		/*
 		* We fork child to deal with each connection, this way more
 		* than one client can connect to us and get served at any one
 		* time.		 		 */

		pid = fork();
		if (pid == -1)
		     err(1, "fork failed");

		if(pid == 0) {
			
			ssize_t r, rc;
			size_t maxread;

			r = -1;
        		rc = 0;
        		maxread = sizeof(buffer) - 1; /* leave room for a 0 byte */
        		while ((r != 0) && rc < maxread) {
                		r = read(clientsd, buffer + rc, maxread - rc);
                		if (r == -1) {
                        		if (errno != EINTR)
                                		err(1, "read failed");
                		} else
                        		rc += r;
        		}
			buffer[rc] = '\0';

        		printf("client  sent:  %s",buffer);

			memset(&server, 0, sizeof(server));
			server.sin_family = AF_INET;
			server.sin_port = htons(portServer);
			server.sin_addr.s_addr = inet_addr("127.0.0.2");
			if(server.sin_addr.s_addr == INADDR_NONE) {
				fprintf(stderr, "Invalid IP address \n");
				usage();
			}

			/* ok now get a socket. we don't care where... */
			if ((sdServer=socket(AF_INET,SOCK_STREAM,0)) == -1)
				err(1, "socket failed");

			/* connect the socket to the server described in "server_sa" */
			if (connect(sdServer, (struct sockaddr *)&server, sizeof(server)) == -1)
				err(1, "connect failed");


			ssize_t written, w;
			w = 0;
			written = 0;
			while (written < strlen(buffer)) {
				w = write(sdServer, buffer + written,
				    strlen(buffer) - written);
				if (w == -1) {
					if (errno != EINTR)
						err(1, "write failed");
				}
				else
					written += w;
			}
			printf("\nwrote to server\n");
			
			w = read(sdServer, buffer, sizeof(buffer));
			printf("\n\nrecieved from server: [ %s ]\n", buffer);
			w = write(clientsd, buffer, sizeof(buffer));
			printf("\nwrote to client\n");
	
			close(sdServer);
			close(clientsd);
			exit(0);
		}
		close(clientsd);
	}
}
