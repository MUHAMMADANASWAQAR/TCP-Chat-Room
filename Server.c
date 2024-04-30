 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
 
#define MAX_CLIENTS 100
 
int serversock;
int clients[MAX_CLIENTS] = {};
 
pthread_mutex_t mtx;
 
void interrupt(int sig) {
    printf("Good bye and thanks for all the fish\n");
    close(serversock);
}
 
void sendAll(int client, char *data, int size) {
    //send the data
    char *tosend = data;
    while(size) {
        int sent = send(client, tosend, size, 0);
        if(sent <= 0) {
            perror("Send");
            exit(-5);
        }
        size -= sent;
        tosend += sent;
    }
}
 
void sendToAll(char *data, int size) {
    pthread_mutex_lock(&mtx);
    for(int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i] > 0) sendAll(clients[i], data, size);
    }
    pthread_mutex_unlock(&mtx);
}
 
void *serve(void *fd) {
    int client = (int)fd;
    pthread_mutex_lock(&mtx);
    for(int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i] == 0) {
            clients[i] = client;
            break;
        }
    }
    pthread_mutex_unlock(&mtx);
    char data[1024];
    int size;
    while( (size = recv(client, data, 1024, 0)) > 0) {
        sendToAll(data, size);
    }

    pthread_mutex_lock(&mtx);
    for(int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i] == client) {
            clients[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mtx);
    return NULL;
}
 
int main() {
    signal(SIGINT, &interrupt);
    pthread_mutex_init(&mtx, 0);
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    perror("socket");
    struct sockaddr_in server = {
        AF_INET, htons(2023), INADDR_ANY
    };
    if(bind(serversock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind");
        return 1;
    }