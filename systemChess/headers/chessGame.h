#ifndef EX1_CHESSGAME_H
#include "chessSystem.h"
#include <stdlib.h>
#define EX1_CHESSGAME_H

typedef struct chess_game_t *ChessGame;

ChessGame createChessGame(int id, int first_player, int second_player, Winner winner, int duration);
ChessGame createEmptyChessGame();
int getFirstPlayerId(ChessGame game);
int getSecondPlayerId(ChessGame game);
Winner getWinner(ChessGame game);
int getDuration(ChessGame game);

void setGameWinner(ChessGame game, Winner game_winner);
ChessGame copyGame(ChessGame data);

#endif //EX1_CHESSGAME_H
