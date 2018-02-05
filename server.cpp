#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
using namespace std;

#define BUFSIZE 1024
#define SMALL 256
#define MAX_CLIENTS 10
int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("1 parameter: Port #\n");
		exit(1);
	}

	//this is for parser IMPLEMENT MORE OF THIS LATER
	const char div[3] = " \n";
	char* token;

	// get directory where server resides
	char home[SMALL];
	getcwd(home, SMALL);

	int portNum = stoi(argv[1]); // port number
	struct sockaddr_in address;
	char sendbuf[BUFSIZE], recvbuf[BUFSIZE], dirbuf[BUFSIZE];

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0) {
		printf("socket() failed\n");
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(portNum);

	if(bind(serverfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		printf("bind() failed\n");
		exit(1);
	}


	if(listen(serverfd, MAX_CLIENTS) < 0) {
		printf("listen() failed\n");
		exit(1);
	}
	while(1) { // loop to accept clients
		int clientfd;
		if((clientfd = accept(serverfd, NULL, NULL)) < 0) {
			printf("accept() failed\n");
			exit(1);
		}

		/**** Single client has connected *****/
		printf("Client connected\n");

		//File IO vars
		FILE* file = NULL;
		DIR* dir = NULL;
		struct dirent* entry;

		while(1) { // loop to receive commands
			memset(sendbuf, 0, BUFSIZE); // clear buffer
			if(recv(clientfd, recvbuf, BUFSIZE, 0) < 0) {
				printf("recv() failed\n");
				exit(1);
			}

			printf("Command received: %s", recvbuf);
			getcwd(dirbuf, BUFSIZE); // get current directory path

			// determine command and execute
			token = strtok(recvbuf, div); // get command string
			if(strncmp(token, "quit", 4) == 0) {
				printf("Client disconnecting\n");
				close(clientfd);
				break;
			} else if(strncmp(token, "get", 3) == 0) {
				token = strtok(NULL, div); // get file name
				// check if file exists
				file = fopen(token, "r");
				if(file == NULL) { // file does not exist
					strncpy(sendbuf, "0", BUFSIZE); // send 0 to indicate file DNE
					send(clientfd, sendbuf, BUFSIZE, 0);
				} else { // file found
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
			} else if(strncmp(token, "put", 3) == 0) {
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
					printf("File %s received from client\n", token);
					fclose(file);
				}
			} else if(strncmp(token, "delete", 6) == 0) {
				token = strtok(NULL, div);
				if(remove(token) < 0) { // file DNE
					strncpy(sendbuf, "delete failed, file does not exist", BUFSIZE);
				} else { // file deleted
					strncpy(sendbuf, "File deleted", BUFSIZE);
				}
				send(clientfd, sendbuf, BUFSIZE, 0);
			} else if(strncmp(token, "ls", 2) == 0) {
				dir = opendir(dirbuf);
				if(dir == NULL) {
					printf("ls failed\n");
					exit(1);
				}
				while((entry = readdir(dir)) != NULL) {
					strncat(sendbuf, entry->d_name, BUFSIZE);
					strncat(sendbuf, "  ", 2);
				}
				send(clientfd, sendbuf, BUFSIZE, 0);
			} else if(strncmp(token, "cd", 2) == 0) {
				token = strtok(NULL, div); // get directory string
				if(chdir(token) < 0){ // change directory
				  strncpy(sendbuf, "cd failed, invalid target", BUFSIZE);
				  perror("chdir() failed: ");				  
				} else {
				  strncpy(sendbuf, "Directory changed", BUFSIZE);
				}
				send(clientfd, sendbuf, BUFSIZE, 0);
			} else if(strncmp(token, "mkdir", 5) == 0) {
				strncpy(sendbuf, "Response for mkdir", BUFSIZE);
				send(clientfd, sendbuf, BUFSIZE, 0);
			} else if(strncmp(token, "pwd", 3) == 0) {
				strncpy(sendbuf, dirbuf, BUFSIZE);
				send(clientfd, sendbuf, BUFSIZE, 0);
			} else {
				strncpy(sendbuf, "No such command", BUFSIZE);
				send(clientfd, sendbuf, BUFSIZE, 0);
			}
			chdir(home); // return to home
		}
	}
	close(serverfd);

	return 0;
}
