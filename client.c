#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    char buffer[256] = "";
    char acknowledge[256] = "";
    
    struct sockaddr_in simpleServer;
    if (argc != 3) {
       fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
	   exit(1);    
    }

    // create a streaming socket
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (simpleSocket == -1) {
		fprintf(stderr, "Could not create a socket!\n");
		exit(1);
    }else {
		fprintf(stderr, "Socket created!\n");
    }

	// set up the address structure
	// use the IP address argument for the server address
	simplePort=atoi(argv[2]); // port number
	bzero(&simpleServer, sizeof(simpleServer));
	simpleServer.sin_family = AF_INET;
	inet_addr(argv[1], &simpleServer.sin_addr.s_addr);
	simpleServer.sin_port = htons(simplePort);

	// connect to the address and port with our socket
	returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer,sizeof(simpleServer));
	if (returnStatus == 0) {
		fprintf(stderr, "Connection succeed!\n");
   	}else{
		fprintf(stderr, "Connection failed!\n");
		close(simpleSocket);
		exit(1);
   	}
	printf("--------------------------------\n");

	int bytes_sent = 0;
	int bytes_recv = 0;
	int temp = 0;

	// read username 
	printf("Please enter username: ");
	scanf("%s", buffer); 
	bytes_sent = send(simpleSocket, buffer, strlen(buffer), 0);

	// acknowledgement message
	bytes_recv = recv(simpleSocket, acknowledge, 256, 0);	
	if(bytes_recv > 0){
		if(strcmp("acknowledgement", buffer) == 0){
			printf("Acknowledgement received\n");		
		}
	}else{
		printf("Acknowledgement message unreceived!\n");
	}

	// read password 
	printf("Please enter password: ");
	scanf("%s", buffer);
	bytes_sent = send(simpleSocket, buffer, strlen(buffer), 0);

	// verification message
	bytes_recv = recv(simpleSocket, buffer, 256, 0);
	if(bytes_recv > 0){
		printf("%s\n", buffer);
	}else{
		printf("Verification message unreceived!\n");
	}	
	printf("--------------------------------\n");

	// close socket
 	close(simpleSocket);
    return 0;
}
