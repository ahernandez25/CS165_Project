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

#include "../Bloom_filters/bloom_filters.c"

char * blacklisted[10] = {"file10", "file20", "file30", "file40", "file50",
			  "file60", "file70", "file80", "file90", "file100"};

void initializeBlacklisted(){
	set_num_hashf(5);
	int i;

	for(i = 0; i < 10; i++){
		insert(blacklisted[i]);
	}	
}

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


char* getCache(char* port){
	char start[80];
	//printf("\n\n*%s*\n\n",port);

	strlcpy(start,"../../src/files/",sizeof(start));
	strcat(start,port);
	strcat(start,"/");
	return start;
}

int main(int argc,  char *argv[])
{
	struct sockaddr_in sockname, client, server;
	char inputFile[20], *ep, *ep2;
	struct sigaction sa;
	int sd, sdServer;
	socklen_t clientlen, serverLen;
	u_short port, portServer;
	pid_t pid;
	u_long p, p2;

	/* initializes bloom filter with blacklisted objects */
	initializeBlacklisted();
	
	char* filePath = getCache(argv[1]);
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
	char portNum[7];
	strlcpy(portNum, argv[1], sizeof(portNum));

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
                 * long, or is too big for an unsigned short             */
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
        		maxread = sizeof(inputFile) - 1; /* leave room for a 0 byte */
        		while ((r != 0) && rc < maxread) {
                		r = read(clientsd, inputFile + rc, maxread - rc);
                		if (r == -1) {
                        		if (errno != EINTR)
                                		err(1, "read failed");
                		} else
                        		rc += r;
        		}
			inputFile[rc] = '\0';

        		printf("client  sent:  %s",inputFile);

			
			/**************************************************
			 * use bloom filter function to see if blacklisted
			 **************************************************/
			
			int isBL = query(inputFile);
			if(isBL == 0){
	
				/*************************************
 			 	* if not blacklisted, checks local cache
 			 	*************************************/		
				FILE *filePointer;
				char readCacheFiles[60];
				int inCache = 0;
				char *fileCache;
				fileCache = getCache(argv[1]);

				filePointer = fopen("../../src/files/cache.txt", "r");
				
				if( filePointer == NULL){
					perror(filePointer);
				} else{
					while(fscanf(filePointer,"%s",readCacheFiles) == 1){
						if(inputFile == readCacheFiles){
							inCache = 1;
						}
					}

				fclose(filePointer);
				//printf("read from file, file now closed");
				}/* end read from file */


				/**********************************************
			 	* if in cache, sends file directly to client
			 	**********************************************/
				if(inCache == 1){
					/*send file from cache to client*/
				
				/*	strcat("../../src/files/",inputFile);
					filePointer = fopen("../../src/files/cache.txt", "r");
					if( filePointer == NULL){
                                        	perror(filePointer);
                                	} else{
                                        	while(fscanf(filePointer,"%s",readCacheFiles) == 1){
                                                	if(inputFile == readCacheFiles){
                                                        	inCache = 1;
                                                	}
                                        	}
					}*/

					ssize_t j;
					char proxyMsg[80];
					strlcpy(proxyMsg,"Proxy Sent from local cache: ",sizeof(proxyMsg)); 
					strcat(proxyMsg,inputFile);
					j = write(clientsd, proxyMsg, sizeof(proxyMsg));


				/**********************************************
			 	* if not in cache, asks server for file 
			 	**********************************************/
				}else {
				
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
					while (written < strlen(inputFile)) {
						w = write(sdServer, inputFile + written,
							strlen(inputFile) - written);
						if (w == -1) {
							if (errno != EINTR)
								err(1, "write failed");
						}
						else
							written += w;
					}
					//printf("\nwrote to server\n");
				
					char serverMsg[1024];

					w = read(sdServer, serverMsg, sizeof(serverMsg));
					printf("\n\nrecieved from server: [ %s ]\n", serverMsg);
					w = write(clientsd, serverMsg, sizeof(serverMsg));
					printf("\nwrote to client\n");
		

					filePointer = fopen("../../src/files/cache.txt", "a");

                                	if( filePointer == NULL){
                                        	perror(filePointer);
                                	} else{
                                        	fprintf(filePointer,"%s\n",inputFile);

                                		fclose(filePointer);
					}

					close(sdServer);
				}//checks if in cache
				
			} else {
				char msg[22];
				strlcpy(msg, "File is Blacklisted!\n", sizeof(msg)); 
				ssize_t w;

				w = write(clientsd, msg, sizeof(msg));
                                printf("\nwrote to client\n");

			}//end check if blacklisted
				
				close(clientsd);
				exit(0);
		}//end fork
			close(clientsd);
	}//end for
}
