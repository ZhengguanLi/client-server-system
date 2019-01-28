#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

const int ROW = 6;
const int COLUMN = 3;
char* username[6];
char* password[6];
char* value[6];

int containsUsername(char* name){
	int index = -1; //-1 means no matching username; others means find it 
	for(int i = 0; i < ROW; i++){
		if(strcmp(username[i],name) == 0){
			index = i;			
			break;				
		}
	}
	return index;
}

int main(int argc, char *argv[]) {
	int simpleSocket = 0;
	int simplePort = 0;
	int returnStatus = 0;
	struct sockaddr_in simpleServer;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	// read stock file
	char buffer[256] = "";
	FILE* fp;
	fp = fopen("stockList.txt","r");
	if(fp == NULL){
		fprintf(stderr, "Can't open the file.\n");
	}
	for(int i = 0; i < ROW;i++){
		username[i]=(char*)malloc(256*sizeof(char));
		password[i]=(char*)malloc(256*sizeof(char));
		value[i]=(char*)malloc(256*sizeof(char));
	}
	for(int i = 0; i < ROW; i++){
		for(int j = 0; j < COLUMN; j++){
			fscanf(fp, "%s", buffer);
			if(j%3 == 0){
				strcpy(username[i],buffer);
			}
			else if(j%3 == 1){
				strcpy(password[i], buffer);
			}
			else if(j%3 == 2){
				strcpy(value[i], buffer);
			}
		}
	}
	fclose(fp);

   	simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (simpleSocket == -1) {
		fprintf(stderr, "Could not create a socket!\n");
		exit(1);
	}else {
		fprintf(stderr, "Socket created!\n");
   	}
    
	// set up the address structure
	// use INADDR_ANY to bind to all local addresses
	bzero(&simpleServer, sizeof(simpleServer));
	simplePort = atoi(argv[1]); // port number
	simpleServer.sin_family = AF_INET;
	simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
	simpleServer.sin_port = htons(simplePort);

	// bind to the address and port with our socket
	returnStatus = bind(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
	if (returnStatus == 0) {
		fprintf(stderr, "Bind completed!\n");
   	}else {
		fprintf(stderr, "Could not bind to address!\n");
		close(simpleSocket);
		exit(1);
   	}
	printf("--------------------------------\n");

	// listen on the socket for connections
	returnStatus = listen(simpleSocket, 30);
	if (returnStatus == -1) {
		fprintf(stderr, "Cannot listen on socket!\n");
		close(simpleSocket);
		exit(1);
  	}

   	while(1){	
		char buffer[256] = "";
		struct sockaddr_in clientName = {0};
		int simpleChildSocket = 0;
		int clientNameLength = sizeof(clientName);

		// wait for connection
		simpleChildSocket = accept(simpleSocket, (struct sockaddr *)&clientName, &clientNameLength);
		if (simpleChildSocket == -1) {
		    fprintf(stderr, "Cannot accept connections!\n");
		    close(simpleSocket);
		    exit(1);
		}

		// 30s waiting for username and password
		struct timeval timeout = {30, 0};
		setsockopt(simpleChildSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

		int index = -1;
		int bytes_sent = 0;
		int bytes_recv = 0;

		// read username
		bytes_recv = recv(simpleChildSocket, buffer, 256, 0);
		if(bytes_recv > 0){
			printf("Received username\n");
			// printf("Send acknowledgement\n");
			char acknowledge[256] = "acknowledgement";  
			bytes_sent = send(simpleChildSocket, acknowledge, strlen(acknowledge),0);
			index = containsUsername(buffer);
		}else{
			printf("Timing out for username!\n");
			close(simpleChildSocket);
		}
		
		// read password
		bzero(buffer, 256);
		bytes_recv = recv(simpleChildSocket, buffer, 256, 0);
		if(bytes_recv > 0){
			// authenticate with username and password
			printf("Received password\n");
			if(index != -1 && strcmp(password[index], buffer) == 0){
				bzero(buffer, 256);
				strcat(buffer, "The corresponding market value is: ");
				strcat(buffer, value[index]);		
				send(simpleChildSocket, buffer, strlen(buffer), 0);	
				printf("Verified, sent back result\n");
			}else{
				bzero(buffer, 256);
				strcat(buffer, "Username/password did not match!");
				send(simpleChildSocket, buffer, strlen(buffer), 0);
				printf("Username/password did not match\n");
			}
		}else {
			printf("Timing out for password!\n");
			close(simpleChildSocket);
	    }
	
		printf("--------------------------------\n");
    }

	// close socket
    close(simpleSocket);
    return 0;
}
