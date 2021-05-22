#include <stdio.h>
#include <stdlib.h>
#include "chessSystem.h"

typedef struct game{
    int game_id;
    int player1_id;
    int player2_id;
    Winner winner;
    int time;
} Game;

typedef struct tournament{
    int tournament_id;
    //TODO: location
    //TODO: games-map
    int winner_id;
} Tournament;

typedef struct chess_system_t{
    //TODO: tournament map
};

/**
 * chessCreate: create an empty chess system.
 *
 * @return A new chess system in case of success, and NULL otherwise (e.g.
 *     in case of an allocation error)
 */
ChessSystem chessCreate(){
    ChessSystem chessSystem = malloc(sizeof(ChessSystem));
    return chessSystem;
}

