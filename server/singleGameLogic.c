#include "singleGameLogic.h"
#include <stdio.h>

#define true 1
#define false 0

//boolean function to check if is there any possible jump for active player, only pawns version
int checkJumps(int board[8][8], int playerMove){

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++){
            if (board[i][j] == playerMove){
                if (playerMove == 1){
                    if (i - 2 > -1){
                        if (j + 2 < 8)
                            if (board[i-1][j+1] == 2 && board[i-2][j+2] == 0)
                                return true;
                        if (j - 2 > -1)
                            if (board[i-1][j-1] == 2 && board[i-2][j-2] == 0)
                                return true;
                    }
                }
                else{
                    if (i + 2 < 8){
                        if (j + 2 < 8)
                            if (board[i+1][j+1] == 1 && board[i+2][j+2] == 0)
                                return true;
                        if (j - 2 > -1)
                            if (board[i+1][j-1] == 1 && board[i+2][j-2] == 0)
                                return true;
                    }
                }
            }
        }


    return false;
}

void startNewGame(){

    int playerMove = 1; //which player has to move    
    int continuousJump = false; //boolean flag to provide multiple jump move
    int lastJumpCoords[] = {99, 99}; //current piece indexes which jumped last move but still have available extra jump
    //fill board with start setup
    int board[8][8] = { { 0, 2, 0, 2, 0, 2, 0, 2 }, //0 - empty field
                        { 2, 0, 2, 0, 2, 0, 2, 0 }, //1 - white pawn
                        { 0, 2, 0, 2, 0, 2, 0, 2 }, //2 - black pawn
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //11 - white queen
                        { 0, 0, 0, 0, 0, 0, 0, 0 }, //22 - black queen
                        { 1, 0, 1, 0, 1, 0, 1, 0 },
                        { 0, 1, 0, 1, 0, 1, 0, 1 },
                        { 1, 0, 1, 0, 1, 0, 1, 0 } };
    int status = checkJumps(board, playerMove);
}

