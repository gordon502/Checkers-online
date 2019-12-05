#include "singleGameLogic.h"
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

//boolean function to check if is there any possible jump for active player
int checkJumps(int board[8][8], int playerMove){

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++){
            if (board[i][j] == playerMove || board[i][j] == playerMove * 11){
                if (playerMove == 1){
                    if (board[i][j] == 1 || board[i][j] == 11) { 
                        if (i - 2 > -1){
                            if (j + 2 < 8)
                                if (board[i-1][j+1] == 2 && board[i-2][j+2] == 0) //checking if there is right sequence
                                    return true;
                            if (j - 2 > -1)
                                if (board[i-1][j-1] == 2 && board[i-2][j-2] == 0) //as above
                                    return true;
                            }
                        if (board[i][j] == 11){ 
                            if (i + 2 < 8){
                                if (j + 2 < 8)
                                    if (board[i+1][j+1] == 2 && board[i+2][j+2] == 0) //checking if there is right sequence
                                        return true;
                                if (j - 2 > -1)
                                    if (board[i+1][j-1] == 2 && board[i+2][j-2] == 0) //as above
                                        return true;
                            }
                        }
                    } 
                }
                else{
                    if (board[i][j] == 2 || board[i][j] == 22){ //jumps for black pawns and queens
                        if (i + 2 < 8){
                            if (j + 2 < 8)
                                if (board[i+1][j+1] == 1 && board[i+2][j+2] == 0)
                                    return true;
                            if (j - 2 > -1)
                                if (board[i+1][j-1] == 1 && board[i+2][j-2] == 0)
                                    return true;
                         }
                    if (board[i][j] == 22){ //missing directions for black queens
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

//function checking if move send by player is right and legal
int checkMove(int board[8][8], int playerMove, int continuousJump, int lastJumpCoords[], char move[]){
    //converting char digits from move message to int
    int index0 = move[0] - '0'; 
    int index1 = move[1] - '0';
    int index2 = move[2] - '0';
    int index3 = move[3] - '0';

    if (index0 >= 0 && index1 >= 0 && index2 >= 0 && index3 >= 0 && index0 <= 7 && index1 <= 7 && index2 <= 7 && index3 <= 7){ //if preventing getting out of range of board array
        if (board[index0][index1] == playerMove || board[index0][index1] == playerMove * 11){ //check if it is right piece to move
            if (checkJumps(board, playerMove) == true){ //if jumps are available
                if (abs(index0 - index2) == 2 && abs(index1 - index3) == 2) //right distance
                    if (board[index2][index3] == 0 && board[(index0 + index2) / 2][(index1 + index3) / 2] != playerMove //check if pieces are in right sequence
                        && board[(index0 + index2) / 2][(index1 + index3) / 2] != playerMove * 11 && board[(index0 + index2) / 2][(index1 + index3) / 2] != 0)
                        if (continuousJump == true){
                            if(index0 == lastJumpCoords[0] && index1 == lastJumpCoords[1])
                                if (board[index0][index1] / 10 > 0) //check if this figure is queen
                                    return true;
                                else if ( (playerMove == 1 && index0 > index2) || (playerMove == 2 && index0 < index2) ) //if not, moves must have proper direction (player1 up, player2 down)
                                    return true;
                        }
                        else{
                            if (board[index0][index1] / 10 > 0) //check if this figure is queen
                                return true;
                            else if ( (playerMove == 1 && index0 > index2) || (playerMove == 2 && index0 < index2) ) //if not, moves must have proper direction (player1 up, player2 down)
                                return true;
                        } 
                    return false;
            return false;       
            }
            
            else{ //standard move
               if (abs(index0 - index2) == 1 && abs(index1 - index3) == 1 && board[index2][index3] == 0){ //check distance and if second field is empty
                    if (board[index0][index1] / 10 > 0) //check if queen
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

void startNewGame(){

    int playerMove = 2; //which player has to move    
    int continuousJump = false; //boolean flag to provide multiple jump move
    int lastJumpCoords[] = {5, 2}; //current piece indexes who jumped last move but after move still have available jump

    //fill board with start setup
    int board[8][8] = { { 0, 2, 0, 2, 0, 2, 0, 2 }, //0 - empty field
                        { 2, 0, 2, 0, 2, 0, 2, 0 }, //1 - white pawn
                        { 0, 2, 0, 2, 0, 2, 0, 2 }, //2 - black pawn
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //11 - white queen
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //22 - black queen
                        { 1, 0, 1, 0, 1, 0, 1, 0 },
                        { 0, 1, 0, 1, 0, 1, 0, 1 },
                        { 1, 0, 1, 0, 1, 0, 1, 0 } };
    char msg[4] = "2132";
    int status = checkMove(board, playerMove, continuousJump, lastJumpCoords, msg);
}

