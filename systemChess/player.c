#include "headers/player.h"

struct player {
    int id;
    int games_played;
    int wins;
    int draws;
    int losses;
    int play_time;
    bool is_removed;
};

Player playerCreatePlayer(int id){
    Player player = malloc(sizeof(*player));
    if(player == NULL){
        free(player);
        return NULL;
    }
    player->id = id;
    player->games_played = 0;
    player->wins = 0;
    player->draws = 0;
    player->losses = 0;
    player->is_removed = false;
    player->play_time = 0;
    return player;
}

Player playerCreateEmptyPlayer(){
    Player player = (Player) malloc(sizeof(struct player));
    if(player == NULL){
        free(player);
        return NULL;
    }
    return player;
}

int getPlayerId(Player player){
    return player->id;
}

int getNumOfGames(Player player){
    return player->games_played;
}

int getNumOfWins(Player player){
    return player->wins;
}

int getNumOfDraws(Player player){
    return player->draws;
}

int getNumOfLosses(Player player){
    return player->losses;
}

bool isRemoved(Player player){
    return player->is_removed;
}

int getPlayerPlayTime(Player player){
    return player->play_time;
}

void updateWins(Player player, int wins){
    player->wins += wins;
}

void updateDraws(Player player, int draws){
    player->draws += draws;
}

void updateLosses(Player player, int losses){
    player->losses += losses;
}

void updateGamesPlayed(Player player){
    player->games_played++;
}

void setIsRemoved(Player player, bool isRemoved){
    player->is_removed = isRemoved;
}

void resetGamesPlayed(Player player){
    player->games_played = 0;
}

//TODO: needed?
void resetTimePlayed(Player player){
    player->play_time = 0;
}

void updatePlayerPlayTime(Player player, int time){
    player->play_time += time;
}

Player copyPlayer(Player data) {
    if (data == NULL) {
        return NULL;
    }
    Player player = playerCreateEmptyPlayer();
    if (player == NULL) {
        free(player);
        return NULL;
    }
    player->id = data->id;
    player->games_played = data->games_played;
    player->wins = data->wins;
    player->draws = data->draws;
    player->losses = data->losses;
    player->is_removed = data->is_removed;
    player->play_time = data->play_time;
    return player;
}
