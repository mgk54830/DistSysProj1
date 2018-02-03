#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

#define BUFSIZE 1024

int main(int argc, char* argv[]) {
	if(argc < 3) {
                printf("2 parameters: Machine name, Port #");
                exit(1);
        }

	//char* machine = argv[1]; // machine name
	int portNum = stoi(argv[2]); // port number
	struct sockaddr_in address;
	char sendbuf[BUFSIZE], recvbuf[BUFSIZE];
	const char div[3] = " \n"; // for tokenizing command
	char* token; // token variable
	FILE* file;

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

	// communicating with server
	while(1) {
		printf("myftp> ");
		fgets(sendbuf, BUFSIZE, stdin);	// get user input
		send(clientfd, sendbuf, BUFSIZE, 0); // send input
		token = strtok(sendbuf, div); // get command string
		if(strncmp(token, "quit", 4) == 0) break; // quit
		else if(strncmp(token, "get", 3) == 0) {
			recv(clientfd, recvbuf, BUFSIZE, 0);
			int filesize = stoi(recvbuf); // get file size
			if(filesize > 0) { // received file size, file exists
				token = strtok(NULL, div);
				file = fopen(token, "w");
				int written = 0; // bytes written
				int i = 0; // bytes received
				while(written < filesize) {
					bzero(recvbuf, BUFSIZE);
					i = recv(clientfd, recvbuf, BUFSIZE, 0);
					fwrite(recvbuf, 1, i, file);
					written += i;
				}
				printf("File %s received from remote server\n", token);
				fclose(file);
			} else { // received 0, file DNE
				printf("get failed: File %s does not exist on the remote server\n", token);
			}
		} else if(strncmp(token, "put", 3) == 0) {
			token = strtok(NULL, div);
			file = fopen(token, "r");
			if(file == NULL) {
				printf("File %s does not exist\n", token);
			} else {
				// get file size
				int filesize = 0;
				fseek(file, 0, SEEK_END);
				filesize = ftell(file);
				fseek(file, 0, SEEK_SET);
				sprintf(sendbuf, "%d", filesize);
				send(clientfd, sendbuf, BUFSIZE, 0);
				int i = 0;
				while((i = fread(sendbuf, 1, BUFSIZE, file)) > 0) {
					send(clientfd, sendbuf, i, 0);
					bzero(sendbuf, BUFSIZE);
				}
				fclose(file);
			}
		} else {
			if(recv(clientfd, recvbuf, BUFSIZE, 0) < 0) {
				printf("recv() failed");
				close(clientfd);
				exit(1);
			}
			printf("%s\n", recvbuf); // print response
		}	
	}

	printf("Disconnecting from FTP server\n");

	close(clientfd);

	return 0;
}
