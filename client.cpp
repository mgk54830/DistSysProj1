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
                cout << "2 parameters: Machine name, Port #" << endl;
                exit(1);
        }

	// char* machine = argv[1]; // machine name
	int portNum = stoi(argv[2]); // port number
	struct sockaddr_in address;
	// socklen_t addrlen = sizeof(address);
	char sendbuf[BUFSIZE], recvbuf[BUFSIZE];

	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0) {
		cout << "socket() failed" << endl;
		exit(1);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(portNum);

	if(connect(clientfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		cout << "connect() failed" << endl;
		exit(1);
	}

	if(read(clientfd, recvbuf, BUFSIZE) < 0) {
		cout << "recv() failed" << endl;
		exit(1);
	}

	printf("Message received: %s\n", recvbuf);

	close(clientfd);

	return 0;
}
