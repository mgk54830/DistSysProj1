#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

#define BUFSIZE 256
#define MAX_CLIENTS 10

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("1 parameter: Port #");
		exit(1);
	}

	int portNum = stoi(argv[1]); // port number
	struct sockaddr_in address;
	char sendbuf[BUFSIZE], recvbuf[BUFSIZE];

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0) {
		printf("socket() failed");
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(portNum);

	if(bind(serverfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		printf("bind() failed");
		exit(1);
	}


	if(listen(serverfd, MAX_CLIENTS) < 0) {
		printf("listen() failed");
		exit(1);
	}

	while(1) { // loop to accept clients
		int clientfd;
		if((clientfd = accept(serverfd, NULL, NULL)) < 0) {
			printf("accept() failed");
			exit(1);
		}
	
		printf("Client connected\n");

		while(1) { // loop to receive commands
			if(recv(clientfd, recvbuf, BUFSIZE, 0) < 0) {
				printf("recv() failed");
				exit(1);
			}

			printf("Command received: %s", recvbuf);
			if(strncmp(recvbuf, "quit", 4) == 0) {
				printf("Client disconnecting\n");
				close(clientfd);
				break;
			} else if(strncmp(recvbuf, "get", 3) == 0) {
				strncpy(sendbuf, "Response for get", BUFSIZE);
			} else if(strncmp(recvbuf, "put", 3) == 0) {
				strncpy(sendbuf, "Response for put", BUFSIZE);
			} else if(strncmp(recvbuf, "delete", 6) == 0) {
				strncpy(sendbuf, "Response for delete", BUFSIZE);
			} else if(strncmp(recvbuf, "ls", 2) == 0) {
				strncpy(sendbuf, "Response for ls", BUFSIZE);
			} else if(strncmp(recvbuf, "cd", 2) == 0) {
				strncpy(sendbuf, "Response for cd", BUFSIZE);
			} else if(strncmp(recvbuf, "mkdir", 5) == 0) {
				strncpy(sendbuf, "Response for mkdir", BUFSIZE);
			} else if(strncmp(recvbuf, "pwd", 3) == 0) {
				strncpy(sendbuf, "Response for pwd", BUFSIZE);
			} else {
				strncpy(sendbuf, "No such command", BUFSIZE);
			}
			send(clientfd, sendbuf, BUFSIZE, 0);
		}
	}

	close(serverfd);

	return 0;
}
