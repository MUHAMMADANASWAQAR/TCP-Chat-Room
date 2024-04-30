#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
 
#define LENGTH 1024
 
// Global variables
int END_WHILE = 0;
int sockfd ;
char name[32];
 
void interupt(int sig) {
    END_WHILE = 1;
}
 
void send_message() {
char m[LENGTH+32];

   strcpy(m,name);
  strcat(m," has joined the chatroom\n");
	send(sockfd, m, 32, 0);
  char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};
  while(1) { 	
    read(0,message,sizeof(message));// read when you enter from screen by default it add neline operator
 
    if (strcmp(message, "exit\n") == 0) //that'swhy here i use exit\n
    {
    break;

    } else {
    strcpy(buffer,">");
        strcat(buffer,name);
    strcat(buffer,":");
    strcat(buffer," ");
    strcat(buffer,message);
      send(sockfd, buffer, strlen(buffer), 0);
    }
    bzero(message,LENGTH);
 
  }
         interupt(2);
}
 
void recieve_message() {
	char message[LENGTH] = {0};
  while (1) {
		int receive = recv(sockfd, message, LENGTH, 0);
      printf("%s", message);
     if (receive == 0) {
			break;
    } else {
			// -1
		}
   memset(message, 0, sizeof(message));
  }
}
 
int main(int argc, char **argv){
 
	char poort[3000];
   printf("eNTER THE PORT NUMBER "); 
   scanf("%s",poort);
	char *ip = "127.0.0.1";
	int port = atoi(poort);
 
	signal(SIGINT, interupt);
 
	printf("\nPlease enter your name: ");
       scanf("%s",name);

 
	struct sockaddr_in server_addr;
 
	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);
 
 
  // Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

 
	printf("\n\t          \"you are in the chatgroup NOW \" \n\n FOR LEAVING :\n>>>>         type  \"exit\" on the console screen \n\n\n ");
 
 
	pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) recieve_message, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}
		pthread_t send_msg_thread;
  if(pthread_create(&send_msg_thread, NULL, (void *) send_message, NULL) != 0){
		printf("ERROR: pthread\n");
    return EXIT_FAILURE;
	}
 
	while (1){
		if(END_WHILE){
			printf("\nGood bye and thanks for all the fish\n");
			break;
    }
	}
 
	close(sockfd);
 
	return EXIT_SUCCESS;
}
 

    listen(serversock, 5);
    perror("listen");
    int client;
    while( (client = accept(serversock, NULL, NULL)) >= 0) {
        pthread_t th;
        pthread_create(&th, 0, &serve, (void*)client);
    }

    return 0;
}