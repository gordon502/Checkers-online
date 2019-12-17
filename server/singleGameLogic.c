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

/* funkcja zamieniajaca pionki na krolowe */
void convertToQueens(int board[8][8]){
    for (int i = 0; i < 8; i = i + 2)
        if(board[7][i] == 2)
            board[7][i] = 22;
    for (int i = 1; i < 8; i = i + 2)
        if(board[0][i] == 1)
            board[0][i] = 11;
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

void startNewGame(int player1Socket, int player2Socket){

    char msg1[]="1\n";
    char msg2[]="2\n";
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
    char message[maxsize];

    /* petla odpowiedzialna za przetwarzanie rozgrywki */
    while(1) {

        char sendMessage[] = "115041099\n"; //zakodowana wiadomosc
        
        if (playerMove == 1) { activeSocket = player1Socket; secondSocket = player2Socket; } //w zaleznosci czyj jest ruch ustawiamy aktywny socket
        else { activeSocket = player2Socket; secondSocket = player1Socket; }

        recv(activeSocket, message, sizeof(message), 0); //odbieranie wiadomosci od aktywnego gracza

        if(checkMove(board, playerMove, continuousJump, lastJumpCoords, message) == true) { //kiedy wyslany ruch jest prawidlowy
            int index0 = message[0] - '0'; int index1 = message[1] - '0'; //indeksy planszy
            int index2 = message[2] - '0'; int index3 = message[3] - '0';

            board[index2][index3] = board[index0][index1];
            board[index0][index1] = 0;

            /* kodowanie z ktorego pola na jaki idzie dana figura */
            sendMessage[2] = index0 + '0'; sendMessage[3] = index1 + '0';
            sendMessage[4] = index2 + '0'; sendMessage[5] = index3 + '0';

            if(abs(index0 - index2) == 2) { //upewniamy sie czy wykonany ruch byl biciem 
                board[(index0 + index2) / 2][(index1 + index3) / 2] = 0;
                lastJumpCoords[0] = index2;
                lastJumpCoords[1] = index3;
                
                /* ustawienie czy ma zdjac figure z planszy i jaka */
                sendMessage[6] = '1';
                sendMessage[7] = ((index0 + index2) / 2) + '0';
                sendMessage[8] = ((index1 + index3) / 2) + '0';

                if (checkJumps(board, playerMove, lastJumpCoords, true)) { // sprawdzamy czy jest kolejne bicie tym pionkiem
                    continuousJump = true;
                    sendMessage[0] = '1'; sendMessage[1] = '1';

                    send(activeSocket, sendMessage, sizeof(sendMessage), 0); //wysylamy informacje o poprawnosci ruchu i obowiazku kolejnego bicia
                    send(secondSocket, sendMessage, sizeof(sendMessage), 0); //informujemy drugiego gracza, jaki ruch wykonal przeciwnik aby zaktualizowal stan gry
                    nextPlayer = playerMove;
                    
                }
                else {
                    continuousJump = false;
                    sendMessage[0] = '1'; sendMessage[1] = '0';

                    send(activeSocket, sendMessage, sizeof(sendMessage), 0); //wysylamy informacje o ruchu i zmianie kolejki
                    send(secondSocket, sendMessage, sizeof(sendMessage), 0); //informujemy drugiego gracza, ze bedzie mogl wykonac swoj ruch i co przeciwnik zrobil
                    if (playerMove == 1) nextPlayer = 2;
                    else nextPlayer = 1;
                
                }
            }
            else {
                sendMessage[0] = '1'; sendMessage[1] = '0';

                send(activeSocket, sendMessage, sizeof(sendMessage), 0); //wysylamy informacje o ruchu i zmianie kolejki
                send(secondSocket, sendMessage, sizeof(sendMessage), 0); //informujemy drugiego gracza, ze bedzie mogl wykonac swoj ruch i co przeciwnik zrobil
                if (playerMove == 1) nextPlayer = 2;
                else nextPlayer = 1;
            
            }

            convertToQueens(board);
            playerMove = nextPlayer;

        }
        else { //kiedy nie jest prawidlowy
            sendMessage[0] = '0'; sendMessage[1] = '0';
            send(activeSocket, sendMessage, sizeof(sendMessage), 0); //informujemy aktywnego gracza, ze ruch jest niepoprawny i aby wykonal go jeszcze raz
        }

    }

    //int status = checkMove(board, playerMove, continuousJump, lastJumpCoords, msg);
}

