/* ocena kodu w obecnej formie: schludnie, choc nasrane */
#include "singleGameLogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>


#define true 1
#define false 0
#define maxsize 30

int get_message_size(int activeSocket) {
    char buf[1];
    int size[10];
    int iterator = 0;
    int multiplier = 1;
    while(true) {
        if (recv(activeSocket, buf, 1, 0) <= 0) { //zerwane polaczenie
            return -1;
        }
        else {
            if (strcmp(buf, "\n") == 0) //znak konca linii == koniec wiadomosci
                break;
            else {
                size[iterator] = atoi(buf); //kopiujemy znaki na dane miejsca w tablicy
                multiplier = multiplier * 10;
                iterator++;
            }
        }
    }

    multiplier = multiplier / 10;
    int message_size = 0;
    for (int i = 0; i < iterator; i++){
        message_size = message_size + (size[i] * multiplier);
        multiplier = multiplier / 10;
    }

    return message_size;
    
}

/* funkcja zamieniajaca pionki na krolowe */
int convertToQueens(int board[8][8]){
    int indexes;
    for (int i = 0; i < 8; i = i + 2)
        if(board[7][i] == 2) {
            board[7][i] = 22;
            indexes = 70 + i;
            return indexes;
        }
    for (int i = 1; i < 8; i = i + 2)
        if(board[0][i] == 1){
            board[0][i] = 11;
            indexes = 100 + i;
            return indexes;
        }
    return -1;

}

/* funkcja do sprawdzania czy dostepne jest bicie na planszy */
int checkJumps(int board[8][8], int playerMove, int lastJumpCoords[], int continuousJump){

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++){
            if (board[i][j] == playerMove || board[i][j] == playerMove * 11){
                if (playerMove == 1){
                    if (board[i][j] == 1 || board[i][j] == 11) { 
                        if (i - 2 > -1){
                            if (j + 2 < 8)
                                if (board[i-1][j+1] == 2 && board[i-2][j+2] == 0) //sprawdzamy czy jest to ustawienie do bicia
                                    if(continuousJump == true){ //kiedy sprawdzamy bicie konkretnym pionkiem
                                        if(lastJumpCoords[0] == i && lastJumpCoords[1] == j)
                                            return true;
                                    }
                                    else
                                        return true;
                            if (j - 2 > -1)
                                if (board[i-1][j-1] == 2 && board[i-2][j-2] == 0) //jw
                                    if(continuousJump == true){ //kiedy sprawdzamy bicie konkretnym pionkiem
                                        if(lastJumpCoords[0] == i && lastJumpCoords[1] == j)
                                            return true;
                                    }
                                    else
                                        return true;
                        }
                    if (board[i][j] == 11){ 
                        if (i + 2 < 8){
                            if (j + 2 < 8)
                                if (board[i+1][j+1] == 2 && board[i+2][j+2] == 0) //sprawdzenie czy jest to ustawienie pozwalajace na bicie
                                    if(continuousJump == true){ //kiedy sprawdzamy bicie konkretnym pionkiem
                                        if(lastJumpCoords[0] == i && lastJumpCoords[1] == j)
                                            return true;
                                    }
                                    else
                                        return true;
                            if (j - 2 > -1)
                                if (board[i+1][j-1] == 2 && board[i+2][j-2] == 0) //jw
                                    if(continuousJump == true){ //kiedy sprawdzamy bicie konkretnym pionkiem
                                        if(lastJumpCoords[0] == i && lastJumpCoords[1] == j)
                                            return true;
                                    }
                                    else
                                        return true;
                        }
                    }
                    } 
                }
                else{
                    if (board[i][j] == 2 || board[i][j] == 22){ //bicia dla czarnych dam i pionkow
                        if (i + 2 < 8){
                            if (j + 2 < 8)
                                if (board[i+1][j+1] == 1 && board[i+2][j+2] == 0)
                                    return true;
                            if (j - 2 > -1)
                                if (board[i+1][j-1] == 1 && board[i+2][j-2] == 0)
                                    return true;
                         }
                    if (board[i][j] == 22){ //brakujace kierunki dla czarnej krolowki
                        if (i - 2 > -1){
                            if (j + 2 < 8)
                                if (board[i-1][j+1] == 1 && board[i-2][j+2] == 0)
                                    return true;
                            if (j - 2 > -1)
                                if (board[i-1][j-1] == 1 && board[i-2][j-2] == 0)
                                    return true;
                         }
                    }
                    }
                }
            }
        }


    return false;
}

/* funkcja sprawdzajaca czy dany ruch jest prawidlowy */
int checkMove(int board[8][8], int playerMove, int continuousJump, int lastJumpCoords[], char move[]){
    //zamiana znakow z wiadomosci na inty
    int index0 = move[0] - '0'; 
    int index1 = move[1] - '0';
    int index2 = move[2] - '0';
    int index3 = move[3] - '0';

    if (index0 >= 0 && index1 >= 0 && index2 >= 0 && index3 >= 0 && index0 <= 7 && index1 <= 7 && index2 <= 7 && index3 <= 7){ //if zapobiegajacy wyjscie poza zakres tablicy board
        if (board[index0][index1] == playerMove || board[index0][index1] == playerMove * 11){ //sprawdzenie czy zawodnik stara sie poruszac swoja figura
            if (checkJumps(board, playerMove, lastJumpCoords, false) == true){ //kiedy dostepne sa bicia
                if (abs(index0 - index2) == 2 && abs(index1 - index3) == 2) //right distance
                    if (board[index2][index3] == 0 && board[(index0 + index2) / 2][(index1 + index3) / 2] != playerMove //sprawdzenie czy jest to sekwencja poprawna do bicia
                        && board[(index0 + index2) / 2][(index1 + index3) / 2] != playerMove * 11 && board[(index0 + index2) / 2][(index1 + index3) / 2] != 0)
                        if (continuousJump == true){
                            if(index0 == lastJumpCoords[0] && index1 == lastJumpCoords[1])
                                if (board[index0][index1] / 10 > 0) //czy figura to krolowa
                                    return true;
                                else if ( (playerMove == 1 && index0 > index2) || (playerMove == 2 && index0 < index2) ) //jesli nie, ruchy musza miec odpowiedni kierunek (gracz1 "do gory", gracz2 "na dol")
                                    return true;
                        }
                        else{
                            if (board[index0][index1] / 10 > 0) //czy figura to krolowa
                                return true;
                            else if ( (playerMove == 1 && index0 > index2) || (playerMove == 2 && index0 < index2) ) //jesli nie, ruchy musza miec odpowiedni kierunek (gracz1 "do gory", gracz2 "na dol")
                                return true;
                        } 
                    return false;
            return false;       
            }
            
            else{ //standardowy ruch
               if (abs(index0 - index2) == 1 && abs(index1 - index3) == 1 && board[index2][index3] == 0){ //sprawdzenie dystansu i czy pole na ktore idziemy jest wolne
                    if (board[index0][index1] / 10 > 0) //jesli krolowa
                        return true;
                    else if ( (playerMove == 1 && index0 > index2) || (playerMove == 2 && index0 < index2) )
                        return true;
               }
            }       
        }
        else
            return false;                                                                                                                      
    }

    return false;
}

int startNewGame(int player1Socket, int player2Socket){

    char msg1[]="91\n";
    char msg2[]="92\n";
    send(player1Socket, msg1, sizeof(msg1), 0); //wysylamy informacje kto jaki ma kolor
    send(player2Socket, msg2, sizeof(msg2), 0);

    

    int playerMove = 1; //ktory gracz ma wykonac ruch   
    int continuousJump = false; //flaga zapewniajaca wielokrotne bicie
    int lastJumpCoords[] = {5, 2}; //obecna pozycja figury ktora ma obecnie obowiazek ponownego bicia

    //tworzenie planszy startowej
    int board[8][8] = { { 0, 2, 0, 2, 0, 2, 0, 2 }, //0 - puste pole
                        { 2, 0, 2, 0, 2, 0, 2, 0 }, //1 - bialy pion
                        { 0, 2, 0, 2, 0, 2, 0, 2 }, //2 - czarny pion
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //11 - biala krolowa
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //22 - czarna krolowa
                        { 1, 0, 1, 0, 1, 0, 1, 0 },
                        { 0, 1, 0, 1, 0, 1, 0, 1 },
                        { 1, 0, 1, 0, 1, 0, 1, 0 } };
    

    int activeSocket; //socket, ktory ma prawo do ruchu
    int secondSocket; //socket, ktory czeka na drugiego gracza
    int nextPlayer; //gracz, ktory bedzie mial ruch w nastepnej iteracji petli
    
     //zakodowana wiadomosc
    /* petla odpowiedzialna za przetwarzanie rozgrywki */
    while(1) {
        
        char sendMessage[] = "1115041099099";
        
        if (playerMove == 1) { activeSocket = player1Socket; secondSocket = player2Socket; } //w zaleznosci czyj jest ruch ustawiamy aktywny socket
        else { activeSocket = player2Socket; secondSocket = player1Socket; }

        int rcv_message_size = get_message_size(activeSocket);
        if (rcv_message_size == -1){ //kiedy zerwane zostalo polaczenie
            sendMessage[0] = 0;
            write(secondSocket, sendMessage, sizeof(sendMessage)); //wysylamy informacje do drugiego o rozlaczeniu
            close(secondSocket); //zamknij polaczenie
            return -1;
        }
        char rcv_message[rcv_message_size];
        if (recv(activeSocket, rcv_message, rcv_message_size, 0) > 0) //odbieranie wiadomosci od aktywnego gracza
        {
        if(checkMove(board, playerMove, continuousJump, lastJumpCoords, rcv_message) == true) { //kiedy wyslany ruch jest prawidlowy
            int index0 = rcv_message[0] - '0'; int index1 = rcv_message[1] - '0'; //indeksy planszy
            int index2 = rcv_message[2] - '0'; int index3 = rcv_message[3] - '0';

            int convertPosition = convertToQueens(board);
            if (convertPosition != -1) { //jesli udalo sie skonwertowac na krolowe
                sendMessage[10] = 1 + '0';
                sendMessage[12] = convertPosition % 10 + '0';
                convertPosition = convertPosition / 10;
                sendMessage[11] = convertPosition % 10 + '0';
            }

            board[index2][index3] = board[index0][index1];
            board[index0][index1] = 0;

            /* kodowanie z ktorego pola na jaki idzie dana figura */
            sendMessage[3] = index0 + '0'; sendMessage[4] = index1 + '0';
            sendMessage[5] = index2 + '0'; sendMessage[6] = index3 + '0';

            if(abs(index0 - index2) == 2) { //upewniamy sie czy wykonany ruch byl biciem 
                board[(index0 + index2) / 2][(index1 + index3) / 2] = 0;
                lastJumpCoords[0] = index2;
                lastJumpCoords[1] = index3;
                
                /* ustawienie czy ma zdjac figure z planszy i jaka */
                sendMessage[7] = '1';
                sendMessage[8] = ((index0 + index2) / 2) + '0';
                sendMessage[9] = ((index1 + index3) / 2) + '0';

                if (checkJumps(board, playerMove, lastJumpCoords, true)) { // sprawdzamy czy jest kolejne bicie tym pionkiem
                    continuousJump = true;
                    sendMessage[1] = '1'; sendMessage[2] = '1';

                    write(activeSocket, sendMessage, sizeof(sendMessage)); //wysylamy informacje o poprawnosci ruchu i obowiazku kolejnego bicia
                    write(secondSocket, sendMessage, sizeof(sendMessage)); //informujemy drugiego gracza, jaki ruch wykonal przeciwnik aby zaktualizowal stan gry
                    nextPlayer = playerMove;
                    
                }
                else {
                    continuousJump = false;
                    sendMessage[1] = '1'; sendMessage[2] = '0';

                    write(activeSocket, sendMessage, sizeof(sendMessage)); //wysylamy informacje o ruchu i zmianie kolejki
                    write(secondSocket, sendMessage, sizeof(sendMessage)); //informujemy drugiego gracza, ze bedzie mogl wykonac swoj ruch i co przeciwnik zrobil
                    if (playerMove == 1) nextPlayer = 2;
                    else nextPlayer = 1;
                
                }
            }
            else {
                sendMessage[1] = '1'; sendMessage[2] = '0';

                write(activeSocket, sendMessage, sizeof(sendMessage)); //wysylamy informacje o ruchu i zmianie kolejki
                write(secondSocket, sendMessage, sizeof(sendMessage)); //informujemy drugiego gracza, ze bedzie mogl wykonac swoj ruch i co przeciwnik zrobil
                if (playerMove == 1) nextPlayer = 2;
                else nextPlayer = 1;
            
            }
            playerMove = nextPlayer;

        }
        else { //kiedy nie jest prawidlowy
            sendMessage[1] = '0'; sendMessage[2] = '0';
            write(activeSocket, sendMessage, sizeof(sendMessage)); //informujemy aktywnego gracza, ze ruch jest niepoprawny i aby wykonal go jeszcze raz
        }
        }

    }

    //int status = checkMove(board, playerMove, continuousJump, lastJumpCoords, msg);
}

