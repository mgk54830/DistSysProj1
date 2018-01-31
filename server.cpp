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

#define IOBUFSIZE 256
#define MAX_CLIENTS 10
#define CHARBUF 256
int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("1 parameter: Port #");
		exit(1);
	}

	//this is for parser IMPLEMENT MORE OF THIS LATER
	const char div[3] = " \n";
	char * token;

	int portNum = stoi(argv[1]); // port number
	struct sockaddr_in address;
	char sendbuf[IOBUFSIZE], recvbuf[IOBUFSIZE], dirbuf[IOBUFSIZE];

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd < 0) {
		printf("socket() failed");
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(portNum);

	if(::bind(serverfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
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

		/**** Single client has connected *****/
		printf("Client connected\n");

		//File IO vars
		FILE* file = NULL;
		DIR* dir = NULL;
		struct dirent* entry;



		while(1) { // loop to receive commands
			memset(sendbuf, 0, IOBUFSIZE); // clear buffer
			//receive client commands

			if(recv(clientfd, recvbuf, IOBUFSIZE, 0) < 0) {
				printf("recv() failed");
				exit(1);
			}
			//test remove later
			printf("Command received: %s", recvbuf);
			//grab command token
			token = strtok(recvbuf, div);
			getcwd(dirbuf, IOBUFSIZE); // get current directory path

			// execute command
			if(strncmp(recvbuf, "quit", 4) == 0) {
				printf("Client disconnecting\n");
				close(clientfd);
				break;
			} else if(strncmp(recvbuf, "get", 3) == 0) {
				/*
			  //strncpy(sendbuf, "Response for get", IOBUFSIZE);
			  char filepath[CHARBUF];
			  strncpy(filepath, "./", 2);
			  char filename[CHARBUF] = NULL;
			  for(int i = 4; recvbuf[i] != ' '; ++i)

			  file = fopen(, "r");
			  if (file = NULL) {
			    printf("get failed\n");
			    exit(1);
			  }
			  //sends file name to client
			  size_t nbytes = 0;
			  int sent = 0;
			  while ((nbytes = fread(sendbuf, sizeof(char), CHARBUF, file)) > 0)
			    {
			      int offset = 0;
			      while ((sent = send(clientfd, sendbuf + offset, nbytes, 0)) > 0) {
								offset += sent;
				nbytes -= sent;
			      }
			    }
					*/
			} else if(strncmp(recvbuf, "put", 3) == 0) {
				strncpy(sendbuf, "Response for put", IOBUFSIZE);
			} else if(strncmp(recvbuf, "delete", 6) == 0) {
				strncpy(sendbuf, "Response for delete", IOBUFSIZE);
			} else if(strncmp(recvbuf, "ls", 2) == 0) {
				dir = opendir(dirbuf);
				if(dir == NULL) {
					printf("ls failed\n");
					exit(1);
				}
				while((entry = readdir(dir)) != NULL) {
					strncat(sendbuf, entry->d_name, IOBUFSIZE);
					strncat(sendbuf, "  ", 2);
				}
			} else if(strncmp(recvbuf, "cd", 2) == 0) {
				//get directory path name
				token = strtok(NULL, div);
				printf( "%s", token);
				//pass token to chdir
				if(chdir(token) < 0){
				  strncpy(sendbuf, "cd failed, invalid target", IOBUFSIZE);
				  perror("chdir() failed: ");
				} else {
				  strncpy(sendbuf, "Directory changed", IOBUFSIZE);
				}
			} else if(strncmp(recvbuf, "mkdir", 5) == 0) {
				strncpy(sendbuf, "Response for mkdir", IOBUFSIZE);
			} else if(strncmp(recvbuf, "pwd", 3) == 0) {
				strncpy(sendbuf, dirbuf, IOBUFSIZE);
			} else {
				strncpy(sendbuf, "No such command", IOBUFSIZE);
			}
				send(clientfd, sendbuf, IOBUFSIZE, 0); // send response string to client
			}
		}
	}

	close(serverfd);

	return 0;
}
