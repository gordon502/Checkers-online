#include <stdlib.h>
#include <stdio.h>
#include "singleGameLogic.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>


pthread_mutex_t copy_data_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Descriptors 
{
    int player1Socket;
    int player2Socket;
};

void * Thread(void *arg) 
{
    int player1Socket = ((struct Descriptors*)arg)->player1Socket;
    int player2Socket = ((struct Descriptors*)arg)->player2Socket;
    
    printf("elo");
    pthread_exit(NULL); 
    
}



int main()
{
    struct Descriptors *players = (struct Descriptors *)malloc(sizeof(struct Descriptors));
    int queue = 20; //maksimum size of queue
    int connectedInRow = 0; //how many players are waiting for new game

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    //Create the socket. 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
   
    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");

    //configure server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2222);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  //Set all bits of the padding field to 0 
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("Socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (bind(serverSocket, (struct sockaddr *) &serverAddr,sizeof(serverAddr))<0) 
    {    
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(serverSocket, queue) < 0) 
    { 
        perror("Listening failed"); 
        exit(EXIT_FAILURE); 
    }

    //loop handling connections
    while(1)
    {
        newSocket = accept(serverSocket, (struct sockaddr *) &serverAddr, &addr_size);
        
        if(connectedInRow == 0) { players -> player1Socket = newSocket; connectedInRow++; }
        else if(connectedInRow == 1) { players -> player2Socket = newSocket; connectedInRow++; }

        if(connectedInRow == 2)
        {
            pthread_t thread;
            connectedInRow = 0;
            pthread_create(&thread, NULL, Thread, (void *) players);
        }
    }

  return 0;
}
