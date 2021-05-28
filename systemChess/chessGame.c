#include "headers/chessGame.h"

struct chess_game_t {
    int game_id;
    int first_player;
    int second_player;
    Winner game_winner;
    int duration;
};

ChessGame createChessGame(int id, int first_player, int second_player, Winner winner, int duration){
    ChessGame game = (ChessGame)malloc(sizeof(struct chess_game_t));
    if(game == NULL){
        free(game);
        return NULL;
    }
    game->game_id = id;
    game->first_player = first_player;
    game->second_player = second_player;
    game->game_winner = winner;
    game->duration = duration;
    return game;
}

ChessGame createEmptyChessGame(){
    ChessGame game = (ChessGame)malloc(sizeof(struct chess_game_t));
    if(game == NULL){
        free(game);
        return NULL;
    }
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

ChessGame copyGame(ChessGame data) {
    if (data == NULL) {
        return NULL;
    }
    ChessGame game = createEmptyChessGame();
    if (game == NULL) {
        return NULL;
    }
    game->game_id = data->game_id;
    game->first_player = data->first_player;
    game->second_player = data->second_player;
    game->game_winner = data->game_winner;
    game->duration = data->duration;
    return game;
}
