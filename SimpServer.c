/*------------------------------
* SimpServer.c
* Description: A program that runs as an HTTP/1.0 server.
*
* By: Harrison Walters
-------------------------------*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_STR_LEN 120         /* maximum string length */

void cleanExit();

/*---------------------main() routine--------------------------*
 * tasks for main
 * generate socket and get socket id,
 * max number of connection is 3 (maximum length the queue of pending connections may grow to)
 * Accept request from client and generate new socket
 * Communicate with client and close new socket after done
 *---------------------------------------------------------------------------*/

main(int argc, char* argv[])
{
    	int newsockid, listen_fd;

	// Generate a "listening socket" to handle new connections.
    	struct sockaddr_in servaddr;
    	bzero( &servaddr, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
    	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    	servaddr.sin_port = htons(atoi(argv[1]));

	// Bind the "listening socket" to a new file desriptor, and enable listening.
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("---- ERROR ---- Unable to generate socket...\n");
		exit(EXIT_FAILURE);
	}
    	if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1){
		printf("---- ERROR ---- Unable to bind socket...\n");
		exit(EXIT_FAILURE);
	}
	if (listen(listen_fd, 3) == -1) {
		printf("---- ERROR ---- Unable to initiate listening...\n");
		exit(EXIT_FAILURE);
	}

	// Accept and processs incoming HTTP requests
    	while (1)
    	{
		if ((newsockid = accept(listen_fd, (struct sockaddr*) NULL, NULL)) == -1) {
			printf("---- ERROR ---- Request recieved, but unable to generate socket...\n");
			exit(EXIT_FAILURE);
		}
		perform_http(newsockid, argv[2]);
      		close(newsockid);
    	}
}

/*---------------------------------------------------------------------------*
 *
 * cleans up opened sockets when killed by a signal.
 *
 *---------------------------------------------------------------------------*/

void cleanExit()
{
    exit(0);
}

/*---------------------------------------------------------------------------*
 *
 * Accepts a request from "sockid" and sends a response to "sockid".
 *
 *---------------------------------------------------------------------------*/

perform_http(int sockid, char* dir)
{
	char request[4096];
	char response[4096];

	bzero( request, 4096);
	bzero( response, 4096);
        read(sockid,request,4096);

	if (strncmp(request, "GET", 3) == 0)
	{
		// Upon recieving an HTTP request, parse it and find the desired resource
		char *resIndex = strchr(request+((strncmp(request, "GET http://", 11) == 0) ? 11 : 0), '/');
		char *resIndexEnd = strchr(resIndex, ' ');

		char resourceName[4096];
		strncpy(resourceName, resIndex, resIndexEnd-resIndex);
		char rootDirectory[4096];
		strcpy(rootDirectory, dir);
		strcat(rootDirectory, resourceName);

		if( access( rootDirectory, F_OK ) != -1 )
		{
			// If the file has been found, respond with a 200 identifier
			// Open the target file, and write the information contained within it to response packet
 			strcpy(response, "HTTP/1.0 200 OK.\r\n\r\n");

       			char * line = NULL;
       			size_t len = 0;
       			ssize_t read;
       			FILE *fp = fopen(rootDirectory, "r");
			if (fp == NULL) {
				printf("---- ERROR ---- Unable to read file.\n");
			}
			else
			{
				while ((read = getline(&line, &len, fp)) != -1) {
           				strcat(response, line);
       				}
      				fclose(fp);
			}
		}
		else
		{
    			strcpy(response, "HTTP/1.0 404 Not Found.\r\n\r\n");
		}
	}
	else
	{
		strcpy(response, "HTTP/1.0 501 Not Implemented.\r\n\r\n");
	}

        write(sockid, response, strlen(response)+1);
}
