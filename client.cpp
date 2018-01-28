#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

#define BUFSIZE 256

int main(int argc, char* argv[]) {
	if(argc < 3) {
                printf("2 parameters: Machine name, Port #");
                exit(1);
        }

	// char* machine = argv[1]; // machine name
	int portNum = stoi(argv[2]); // port number
	struct sockaddr_in address;
	char sendbuf[BUFSIZE], recvbuf[BUFSIZE];

	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0) {
		printf("socket() failed");
		exit(1);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(portNum);

	if(connect(clientfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		printf("connect() failed");
		exit(1);
	}

	printf("Connected to FTP server\n");

	while(1) { // communicating with server
		printf("myftp>");
		fgets(sendbuf, BUFSIZE, stdin);	
		send(clientfd, sendbuf, BUFSIZE, 0);
		if(strncmp(sendbuf, "quit", 4) == 0) break; // quit command
		if(recv(clientfd, recvbuf, BUFSIZE, 0) < 0) {
			printf("recv() failed");
			close(clientfd);
			exit(1);
		}
		printf("Response received: %s\n", recvbuf);
	}

	printf("Disconnecting from FTP server\n");

	close(clientfd);

	return 0;
}
