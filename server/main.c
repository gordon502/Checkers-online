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

#define queue 20 //maksymalny rozmiar kolejki
#define PORT 2223


struct Descriptors {
    int player1Socket;
    int player2Socket;
    int connectedInRow;
};

void * Thread(void *arg) {
    int player1Socket = ((struct Descriptors*)arg)->player1Socket;
    int player2Socket = ((struct Descriptors*)arg)->player2Socket;
    ((struct Descriptors*)arg) -> connectedInRow = 0; //zdjecie flagi odpowiedzialnej
                                                      //za przepisanie deskryptorow bez naruszenia
    startNewGame(player1Socket, player2Socket);
    pthread_exit(NULL); 
    
}



int main()
{
    struct Descriptors *players = (struct Descriptors *)malloc(sizeof(struct Descriptors));
    players -> connectedInRow = 0;

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    //Stworzenie socketu 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
   
    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");

    //Konfiguracja serwera
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (bind(serverSocket, (struct sockaddr *) &serverAddr,sizeof(serverAddr))<0) {    
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(serverSocket, queue) < 0) { 
        perror("Listening failed"); 
        exit(EXIT_FAILURE); 
    }

    //petla akceptujaca przychodzace polaczenia i laczaca graczy do gry
    while(1) {
        newSocket = accept(serverSocket, (struct sockaddr *) &serverAddr, &addr_size);
        
        while (players -> connectedInRow == 2) {} //semafor pozwalajacy na przepisanie deskryptorow tworzonej gry
        
        
        if(players -> connectedInRow == 0) { players -> player1Socket = newSocket; players -> connectedInRow++; }
        else if(players -> connectedInRow == 1) { players -> player2Socket = newSocket; players -> connectedInRow++; }

        if(players -> connectedInRow == 2) {
            pthread_t thread;
            int newGame = pthread_create(&thread, NULL, Thread, (void *) players);
            pthread_detach(newGame);
        }
    }

    close(serverSocket);

  return 0;
}
