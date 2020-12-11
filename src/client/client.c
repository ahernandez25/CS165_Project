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
 */

/* client.c  - the "classic" example of a socket client */
#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../murmur/murmur3.c"
#include "../Rendezvous/rendezvous.c"

static void usage()
{
	extern char * __progname;
	fprintf(stderr, "usage: %s ipaddress portnumber\n", __progname);
	exit(1);
}

char PROXY_NAMES[][6] = {{"proxy1"}, {"proxy2"}, {"proxy3"},{"proxy4"}, {"proxy5"}, {"proxy6"}};





const char* PROXY_PORTS[6] = {"9991","9992","9993","9994","9995","9996"};

int main(int argc, char *argv[])
{
	struct sockaddr_in server_sa;
	char buffer[80], *ep;
	size_t maxread;
	ssize_t r, rc;
	u_short port;
	u_long p;
	int sd;


	char filename[80];
	/* gets filename from arguments passed in */
	strlcpy(filename, argv[1], sizeof(filename));

	/**************************************
	 * use rendevous hashing to find correct proxy ---
	 * 
	 * for now uses port 9991, PROXY_PORTS[0]
	 **************************************/
	char *proxyPort = Rendezvous(filename,6, PROXY_NAMES);
	
	int j,k;
	int selPort;
	
	
	//printf("\nchecking proxy name - %s\n", PROXY_NAMES[0][j]);
	if(proxyPort == "proxy1"){
		selPort = 0;
	}else if(proxyPort == "proxy2"){
		selPort = 1;
	}else if(proxyPort == "proxy3"){
                selPort = 2;
        }else if(proxyPort == "proxy4"){
                selPort = 3;
        }else if(proxyPort == "proxy5"){
		selPort = 4;
        }else {
                selPort = 5;
        }
	

	if (argc != 2)
		usage();

        p = strtoul(PROXY_PORTS[selPort], &ep, 10);
        

	if (PROXY_PORTS[selPort] == '\0' || *ep != '\0') {
		/* parameter wasn't a number, or was empty */
		fprintf(stderr, "%s - not a number\n", PROXY_PORTS[selPort]);
		usage();
	}


        if ((errno == ERANGE && p == ULONG_MAX) || (p > USHRT_MAX)) {
		/* It's a number, but it either can't fit in an unsigned
		 * long, or is too big for an unsigned short
		 */
		fprintf(stderr, "%s - value out of range\n", PROXY_PORTS[selPort]);
		usage();
	}
	
	
/* now safe to do this */
	port = p;
	/*
	 * first set up "server_sa" to be the location of the server
	 */
	memset(&server_sa, 0, sizeof(server_sa));
	server_sa.sin_family = AF_INET;
	server_sa.sin_port = htons(port);
	//server_sa.sin_addr.s_addr = inet_addr(argv[1]) /*reads IP in from console */
	server_sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (server_sa.sin_addr.s_addr == INADDR_NONE) {
		fprintf(stderr, "Invalid IP address %s\n", argv[1]);
		usage();
	}

	/* ok now get a socket. we don't care where... */
	if ((sd=socket(AF_INET,SOCK_STREAM,0)) == -1)
		err(1, "socket failed");

	/* connect the socket to the server described in "server_sa" */
	if (connect(sd, (struct sockaddr *)&server_sa, sizeof(server_sa))
	    == -1)
		err(1, "connect failed");


		
	ssize_t written, w;


	w = write(sd, filename, sizeof(filename));
	printf("\n\n file name sent to proxy: %s", filename);

	r = read(sd, buffer, sizeof(buffer));
	printf("\n\nServer sent:  %s\n",buffer);
	printf("\nclose client\n");
	close(sd);
	return(0);
}
