#include "headers/chessTournament.h"

struct chess_tournament_t {
    Map games;
    Map players;
    int id;
    const char *tournament_location;
    int tournament_winner;
    int max_games_per_player;
    bool has_ended;
    int longest_game_time;
    int num_valid_time_games;
    int sum_valid_time_games;
    int last_game_id;
    int num_players;
};

// creates new empty tournament
ChessTournament createEmptyTournament(){
    ChessTournament tournament = (ChessTournament) malloc(sizeof(struct chess_tournament_t));
    return tournament;
}

//creates new tournament
ChessTournament createChessTournament(int tournament_id, int max_games_per_player, const char *tournament_location) {
    ChessTournament tournament = (ChessTournament) malloc(sizeof(*tournament));
    if (tournament == NULL)
        return NULL;
    tournament->id = tournament_id;
    tournament->tournament_location = tournament_location;
    tournament->tournament_winner = -1;
    tournament->max_games_per_player = max_games_per_player;
    tournament->has_ended = false;
    tournament->longest_game_time = -1;
    tournament->num_valid_time_games = 0;
    tournament->sum_valid_time_games = 0;
    tournament->last_game_id = 0;
    tournament->num_players = 0;

    return tournament;
}

Map getGames(ChessTournament tournament){
    return tournament->games;
}
Map getPlayers(ChessTournament tournament){
    return tournament->players;
}
int getTournamentId(ChessTournament tournament){
    return tournament->id;
}

const char* getTournamentLocation(ChessTournament tournament){
    return tournament->tournament_location;
}

int getLastGameId(ChessTournament tournament){
    return tournament->last_game_id++;
}
const char *getLocation(ChessTournament tournament){
    return tournament->tournament_location;
}
int getWinnerId(ChessTournament tournament){
    return tournament->tournament_winner;
}
int getMaxGamesPerPlayer(ChessTournament tournament){
    return tournament->max_games_per_player;
}
bool hasEnded(ChessTournament tournament){
    return tournament->has_ended;
}
int getLongestGameDuration(ChessTournament tournament){
    return tournament->longest_game_time;
}
int getValidTimeGames(ChessTournament tournament){
    return tournament->num_valid_time_games;
}
int getSumValidTimeGames(ChessTournament tournament){
    return tournament->sum_valid_time_games;
}
int getNumOfPlayers(ChessTournament tournament){
    return tournament->num_players;
}

void setHasEnded(ChessTournament tournament, bool hasEnded){
    tournament->has_ended = hasEnded;
}

void setLongestGameDuration(ChessTournament tournament, int time){
    tournament->longest_game_time = time;
}

void setGamesMap(ChessTournament tournament, Map games){
    tournament->games = games;
}
void setPlayersMap(ChessTournament tournament, Map players){
    tournament->players = players;
}

void setTournamentWinner(ChessTournament tournament, int winnerId){
    tournament->tournament_winner = winnerId;
}
