/*------------------------------
* SimpClient.c
* Description: An program that runs as an HTTP/1.0 client.
*
* Written by: Harrison Walters, V00807994
-------------------------------*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "util.c"

#define MAX_STR_LEN 4096
#define MAX_RES_LEN 4096

/* --------- Main() routine ------------
 * three main task will be excuted:
 * accept the input URI and parse it into fragments for further operation
 * open socket connection with specified sockid ID
 * use the socket id to connect sopecified server
 */

main(int argc, char *argv)
{
	char uri[MAX_STR_LEN];
	char hostname[MAX_STR_LEN];
	char identifier[MAX_STR_LEN];
	int sockid, port;

	// Read input from the terminal, and extract information from URI for an HTTP request
    	printf("Open URI:  ");
    	scanf("%s", uri);
    	parse_URI(uri, hostname, &port, identifier);
	printf("\n--- REQUEST ---\n%s\nHost: %s\nPort: %d\n---------------\n", identifier, hostname, port);
	strcat(identifier,"\r\n\r\n");

	// Open socket connection, and connect to server
	sockid = open_connection(hostname, port);

	// Send an HTTP request to the server
    	perform_http(sockid, identifier);
}

/*---------------------------------------------------------------------------*
 * Parse an "uri" into "hostname" and resource "identifier"
 *---------------------------------------------------------------------------*/

parse_URI(char *uri, char *hostname, int *port, char *identifier)
{
	// Discover the indices of "http://", ":[port]", and "/resource" in the URI, for parsing
	char *hostIndex = uri+((strncmp(uri, "http://", 7) == 0) ? 7 : 0);
  	char *portIndex = strchr( uri,':');
	char *resIndex = strchr(hostIndex, '/');

	// Check the input URI for errors
	if (resIndex == NULL) {
		printf("---- ERROR ---- No resource specified...\n");
		exit(EXIT_FAILURE);
	}
	int j;

	// If a port is included in URI, extract it.
  	if (portIndex != NULL)
  	{
		j = (portIndex-hostIndex);
		portIndex++;
		int n = (resIndex-portIndex);
		char str[n];
   		strncpy(str, portIndex, n);
   		*port = atoi(str);
  	}
	else
	// If a port is not included in URI, use the default port of 80.
	{
		j = (resIndex-hostIndex);
		*port = 80;
	}

	strncpy ( hostname, hostIndex, j);
	strcat(strcat(strcat(identifier, "GET http://"), uri)," HTTP/1.0");
}

/*---------------------------------------------------------------------------*
 * open_conn() routine. It connects to a remote server on a specified port.
 *---------------------------------------------------------------------------*/

int open_connection(char *hostname, int port)
{
	// Generate a new socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	struct sockaddr_in servaddr;
    	bzero(&servaddr,sizeof servaddr);
    	servaddr.sin_family=AF_INET;
    	servaddr.sin_port=htons(port);

	// Determine the host address, given a charater string or an IP address
	struct hostent *server_ent;
	server_ent= gethostbyname(hostname);
	if (server_ent == NULL) {
		printf("---- ERROR ---- Unable to resolve URI...\n");
		exit(EXIT_FAILURE);
	}
	memcpy(&servaddr.sin_addr, server_ent->h_addr, server_ent->h_length);

    	if (connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1) {
		printf("---- ERROR ---- Unable to connect to host...\n");
		exit(EXIT_FAILURE);
	}
  	return sockfd;
}

/*------------------------------------*
* connect to a HTTP server using hostname and port,
* and get the resource specified by identifier
*--------------------------------------*/
perform_http(int sockid, char *identifier)
{
	// Send an HTTP request to the host
	printf("\nHTTP request sent, awaiting response...\n");
	writen(sockid, identifier, strlen(identifier)+1);

	// Upon recieving a response, break it down into header and body, and display it on the screen
	char recvline[MAX_RES_LEN];
        bzero( recvline, MAX_RES_LEN);
	if (read(sockid,recvline, MAX_RES_LEN) == -1) {
		printf("---- ERROR ---- No response recieved...\n");
		exit(EXIT_FAILURE);
	}
	char * body = strstr(recvline,"\r\n\r\n");
	char header[MAX_RES_LEN];
	strncpy(header, recvline, (body-recvline));
        printf("\n--- RESPONSE HEADER ---\n%s\n-----------------------\n",header);
	printf("\n--- RESPONSE BODY -----\n%s-----------------------\n",body+4);

   	close(sockid);
}
