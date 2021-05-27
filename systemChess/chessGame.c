#include "headers/chessGame.h"

struct chess_game_t {
    int game_id;
    int first_player;
    int second_player;
    Winner game_winner;
    int duration;
};

ChessGame createChessGame(int id, int first_player, int second_player, Winner winner, int duration){
    ChessGame game = malloc(sizeof(*game));
    if(game == NULL){
        return NULL;
    }
    game->game_id = id;
    game->first_player = first_player;
    game->second_player = second_player;
    game->game_winner = winner;
    game->duration = duration;
    return game;
}

ChessGame* createEmptyChessGame(){
    ChessGame* game = malloc(sizeof(game));
    return game;
}

int getGameId(ChessGame game){
    return game->game_id;
}

int getFirstPlayerId(ChessGame game){
    return game->first_player;
}

int getSecondPlayerId(ChessGame game){
    return game->second_player;
}

Winner getWinner(ChessGame game){
    return game->game_winner;
}

int getDuration(ChessGame game){
    return game->duration;
}

void setGameWinner(ChessGame game, Winner game_winner){
    game->game_winner=game_winner;
}
