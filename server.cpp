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
		cout << "1 parameter: Port #" << endl;
		exit(1);
	}

	int portNum = stoi(argv[1]); // port number
	struct sockaddr_in address;
	// socklen_t addrlen = sizeof(address);
	// char buffer[BUFSIZE];
	char* message = "You have reached the ftp server";

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0) {
		cout << "socket() failed" << endl;
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(portNum);

	if(bind(serverfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		cout << "bind() failed" << endl;
		exit(1);
	}


	if(listen(serverfd, MAX_CLIENTS) < 0) {
		cout << "listen() failed" << endl;
		exit(1);
	}


	int clientfd;
	if((clientfd = accept(serverfd, NULL, NULL)) < 0) {
		cout << "accept() failed" << endl;
		exit(1);
	}

	send(clientfd, message, strlen(message), 0);

	close(clientfd);
	close(serverfd);

	return 0;
}
