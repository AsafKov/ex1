#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "headers/chessSystem.h"
#include "../map/headers/map.h"

struct chess_system_t {
    Map tournaments;//key is tournament_id
    Map players;
    int num_games;
    int num_valid_time_games;
    int sum_valid_time_games;
};

typedef struct chess_tournament_t {
    Map games;
    int tournament_id;
    const char *tournament_location;
    Winner tournament_winner;
    int max_games_per_player;
    bool has_ended;
    int longest_game_time;
    int num_valid_time_games;
    int sum_valid_time_games;
    int num_games;
    int num_players;
} *ChessTournament;

typedef struct chess_game_t {
    int game_id;
    int first_player;
    int second_player;
    Winner game_winner;
    int play_time;
} *ChessGame;

typedef struct player {
    int id;
    int num_games;
    int num_wins;
    int num_draws;
    int num_losses;
    bool has_been_removed;
} *Player;

// Functions
int comparePlayers(Player first, Player second);
double calculatePlayerLevel(Player player);
double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result);
static bool isPlayerInSystem(ChessSystem chess, int player_id);
void chessDestroy(ChessSystem chess);
static bool isValidID(int id);
static bool isValidLocation(const char *location);
ChessResult chessAddPlayers(ChessSystem chess, int player_id);
static bool isValidMaxGame(int num);
static bool isValidGameTime(int num);
static bool isTournamentEnded(ChessSystem chess, int tournament_id);
static bool isGameExist(ChessSystem chess, ChessTournament *tournament, int first_player, int second_player);
static bool isMaxExceeded(ChessSystem chess, int tournament_id, int first_player, int second_player);
static bool isPlayerInSystem(ChessSystem chess, int player_id);
ChessResult convertMapResultToChessResult(MapResult map_result);
Player createPlayer(int id);
void updateScores(ChessSystem chessSystem, ChessGame match, int first_player_id, int second_player_id);
ChessTournament createTournament(int tournament_id, int max_games_per_player, const char *tournament_location);
ChessGame createGame(int game_id, int first_player, int second_player, Winner winner, int play_time);

MapDataElement copyMapDataTournament(MapDataElement data) {
    if (data == NULL) {
        return NULL;
    }
    ChessTournament *current_tournament = (ChessTournament*)data;
    ChessTournament *copy = malloc(sizeof(struct chess_tournament_t));
    *copy = *current_tournament;
    if (copy == NULL) {
        return NULL;
    }
    Map games = (*(ChessTournament*)data)->games;
    if(games != NULL && mapGetSize(games) != 0) {
        (*copy)->games = mapCopy(games);
        if ((*copy)->games == NULL) {
            free(copy);
            return NULL;
        }
    }
    return (MapDataElement) copy;
}

void freeMapDataTournament(MapDataElement data){
    if(data == NULL){
        return;
    }
    ChessTournament *tournament = (ChessTournament*) data;
    mapDestroy((*tournament)->games);
    free(tournament);
}

MapDataElement copyMapKey(MapKeyElement key) {
    if (key == NULL){
        return NULL;
    }
    int *key_copy = malloc(sizeof(int));
    *key_copy = *((int*)key);
    return key_copy;
}

void freeMapKey(MapKeyElement key){
    free(key);
}

MapDataElement copyMapDataGame(MapDataElement data) {
    ChessGame copy = (ChessGame) malloc(sizeof(struct chess_game_t));
    if (data == NULL) {
        return NULL;
    }
    copy->game_id = ((ChessGame) data)->game_id;
    copy->first_player = ((ChessGame) data)->first_player;
    copy->second_player = ((ChessGame) data)->second_player;
    copy->game_winner = ((ChessGame) data)->game_winner;
    copy->play_time = ((ChessGame) data)->play_time;
    return copy;
}

MapDataElement copyMapDataPlayer(MapDataElement data) {
    Player *player_copy = malloc(sizeof(struct player));
    *player_copy = *(Player*)data;
    return player_copy;
}

void freeMapData(MapDataElement data){
    free(data);
}

int compareMapKeys(MapKeyElement key1, MapKeyElement key2){
    if(key1 == NULL) return -1;
    if(key2 == NULL) return 1;
    return *((int*)key1) - (*(int*)key2);
}

ChessSystem chessCreate() {
    ChessSystem Chess = (ChessSystem) malloc(sizeof(struct chess_system_t));
    if (Chess == NULL)
        return NULL;
    Chess->num_games = 0;
    Chess->num_valid_time_games = 0;
    Chess->sum_valid_time_games = 0;
    Chess->tournaments = mapCreate(copyMapDataTournament, copyMapKey, freeMapDataTournament, freeMapKey,
                                   compareMapKeys);
    Chess->players = mapCreate(copyMapDataGame, copyMapKey, freeMapData, freeMapKey,
                               compareMapKeys);
    return Chess;
}

//TODO: check what needs to be freed
void chessDestroy(ChessSystem chess) {
    if (chess == NULL)
        return;
    //free(chess)
}

//check if ID is a positive number
static bool isValidID(int id) {
    if (id < 0)
        return false;
    return true;
}

//check if location is valid string by specified conditions
static bool isValidLocation(const char *location) {
    if (location == NULL)
        return false;
    if (*location > 'Z' || *location < 'A')//check if first letter is capital
        return false;
    location++;
    while (*location) {
        if (*location != ' ' && (*location < 'a' || *location > 'z'))//check if remaining letters are a-z or space
            return false;
        location++;
    }
    return true;
}

//check if number of max games is a positive number
static bool isValidMaxGame(int num) {
    if (num < 0)
        return false;
    return true;
}

//check if game length in seconds is a positive number
static bool isValidGameTime(int num) {
    if (num < 0)
        return false;
    return true;
}

//check if tournament has ended
//TODO: check if we wanna use mapGet function instead (to find tournament ID)
static bool isTournamentEnded(ChessSystem chess, int tournament_id) {
    assert(chess != NULL);
    ChessTournament *current_tournament = (ChessTournament *) mapGet(chess->tournaments,
                                                                     (MapKeyElement) &tournament_id);
    if ((*current_tournament)->has_ended == true)
        return false;
    return true;
}

//check if 2 players already played together in this tournament.
// If so - game already happened
//TODO: check if we wanna use mapGet function instead (to find tournament ID)
static bool isGameExist(ChessSystem chess, ChessTournament *tournament, int first_player, int second_player) {
    assert(tournament != NULL && chess != NULL);
    int *key = (int *) mapGetFirst((*tournament)->games);
    ChessGame *current_game = (ChessGame *) mapGet((*tournament)->games, (MapKeyElement) key);
    while (current_game != NULL) {
        if ((*current_game)->first_player == first_player && (*current_game)->second_player == second_player)
            return true;
        if ((*current_game)->first_player == second_player && (*current_game)->second_player == first_player)
            return true;
        key = mapGetNext((*tournament)->games);
        current_game = (ChessGame *) mapGet((*tournament)->games, (MapKeyElement) key);
    }
    return false;
}

//check if one of the players already played the maximum amount of games allowed in tournament.
//TODO: check if we wanna use mapGet function instead (to find tournament ID)
static bool isMaxExceeded(ChessSystem chess, int tournament_id, int first_player, int second_player) {
    assert(chess != NULL);
    ChessTournament *current_tournament = (ChessTournament *) mapGet(chess->tournaments,
                                                                     (MapKeyElement) &tournament_id);
    if (current_tournament == NULL) {
        return false;
    }
    Player *current_player = (Player *) mapGet(chess->players, (MapKeyElement) &first_player);
    if ((*current_player)->num_games > (*current_tournament)->max_games_per_player)
        return true;
    current_player = (Player *) mapGet(chess->players, (MapKeyElement) &second_player);
    if ((*current_player)->num_games > (*current_tournament)->max_games_per_player)
        return true;
    return false;
}

//check if player ID is recognized to chess system
static bool isPlayerInSystem(ChessSystem chess, int player_id) {
    assert(chess != NULL);
    return mapContains(chess->players, (MapKeyElement) &player_id);
}

//converts map functions results to legitimate chess return values
ChessResult convertMapResultToChessResult(MapResult map_result) {
    if (map_result == MAP_NULL_ARGUMENT)
        return CHESS_NULL_ARGUMENT;
    if (map_result == MAP_OUT_OF_MEMORY)
        return CHESS_OUT_OF_MEMORY;
    return CHESS_SUCCESS; //only other option MAP_OUT_OF_MEMORY
}

//creates new tournament, to be added to chess system
ChessTournament createTournament(int tournament_id, int max_games_per_player, const char *tournament_location) {
    ChessTournament Tournament = (ChessTournament) malloc(sizeof(struct chess_tournament_t));
    if (Tournament == NULL)
        return NULL;
    Tournament->tournament_id = tournament_id;
    Tournament->tournament_location = tournament_location;
    Tournament->tournament_winner = DRAW;
    Tournament->max_games_per_player = max_games_per_player;
    Tournament->has_ended = false;
    Tournament->longest_game_time = -1;
    Tournament->num_valid_time_games = 0;
    Tournament->sum_valid_time_games = 0;
    Tournament->num_games = 0;
    Tournament->num_players = 0;
    Tournament->games = mapCreate(copyMapDataGame, copyMapKey, freeMapData, freeMapKey, compareMapKeys);
    return Tournament;
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char *tournament_location) {
    if (chess == NULL || tournament_location == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    if (!isValidLocation(tournament_location))
        return CHESS_INVALID_LOCATION;
    if (!isValidMaxGame(max_games_per_player))
        return CHESS_INVALID_MAX_GAMES;
    if (mapContains(chess->tournaments, (MapKeyElement)&tournament_id))
        return CHESS_TOURNAMENT_ALREADY_EXISTS;

    ChessTournament tournament = createTournament(tournament_id, max_games_per_player,
                                                  (const char *) &tournament_location);
    if (tournament == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }

    if(chess->tournaments == NULL){
        chess->tournaments = mapCreate(copyMapDataTournament, copyMapKey, freeMapDataTournament,
                                       freeMapKey, compareMapKeys);
    }
    MapResult map_result = mapPut(chess->tournaments, (MapKeyElement) &tournament_id,
                                  (MapDataElement)&tournament);//adding Tournament to chessSystem chess
    return convertMapResultToChessResult(map_result);
}

//creates new game, to be added to chess system
ChessGame createGame(int game_id, int first_player, int second_player, Winner winner, int play_time) {
    ChessGame Game = (ChessGame) malloc(sizeof(struct chess_game_t));
    if (Game == NULL)
        return NULL;
    Game->game_id = game_id;
    Game->first_player = first_player;
    Game->second_player = second_player;
    Game->game_winner = winner;
    Game->play_time = play_time;
    return Game;
}



//TODO:what to do to game when player is removed?
ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player,
                         Winner winner, int play_time) {
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }

    if (!isValidID(tournament_id) || first_player == second_player || !isValidID(first_player)
        || !isValidID(second_player)) {
        return CHESS_INVALID_ID;
    }

    ChessTournament *tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (tournament == NULL) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if ((*tournament)->has_ended) {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (isGameExist(chess, tournament, first_player, second_player)) {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (!isValidGameTime(play_time)) {
        return CHESS_INVALID_PLAY_TIME;
    }

    chessAddPlayers(chess, first_player);
    chessAddPlayers(chess, second_player);
    if (isMaxExceeded(chess, tournament_id, first_player, second_player)) {
        return CHESS_EXCEEDED_GAMES;
    }

    int game_id = (*tournament)->num_games + 1;

    ChessGame game = createGame(game_id, first_player, second_player, winner, play_time);
    if (game == NULL)
        return CHESS_OUT_OF_MEMORY;
    MapResult map_result = mapPut((*tournament)->games, (MapKeyElement) &game_id,
                                  (MapDataElement)&game);
    ChessResult result = convertMapResultToChessResult(map_result);
    if(result == CHESS_SUCCESS){
        updateScores(chess, game, first_player, second_player);
    }
    return result;
}

void updateScores(ChessSystem chessSystem,ChessGame match, int first_player_id, int second_player_id){
    Player *player1 = (Player*)mapGet(chessSystem->players, (MapKeyElement)&first_player_id);
    Player *player2 = (Player*)mapGet(chessSystem->players, (MapKeyElement)&second_player_id);
    if(match->game_winner == FIRST_PLAYER){
        (*player1)->num_wins++;
        (*player2)->num_losses--;
    } else {
        (*player1)->num_wins--;
        (*player2)->num_losses++;
    }
    (*player1)->num_draws++;
    (*player2)->num_draws++;
}

Player createPlayer(int id) {
    Player player = (Player) malloc(sizeof(Player));
    if (player == NULL)
        return NULL;
    player->id = id;
    player->num_games =0;
    player->num_wins=0;
    player->num_draws=0;
    player->num_losses=0;
    player->has_been_removed=false;
    return player;
}

ChessResult chessAddPlayers(ChessSystem chess, int player_id) {
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if(mapContains(chess->players, (MapKeyElement)&player_id)){
        return CHESS_SUCCESS;
    }
    if (!isValidID(player_id)){
        return CHESS_INVALID_ID;
    }
    Player player = createPlayer(player_id);
    if (player == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult map_result = mapPut(chess->players, (MapKeyElement) &player_id,
                                  (MapDataElement)&player);
    return convertMapResultToChessResult(map_result);
}

//TODO: TBD
ChessResult chessEndTournament(ChessSystem chess, int tournament_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    ChessTournament *tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (!mapContains(chess->tournaments, (MapKeyElement) &tournament_id))
        return CHESS_TOURNAMENT_NOT_EXIST;
    if (isTournamentEnded(chess, tournament_id))
        return CHESS_TOURNAMENT_ENDED;
    return CHESS_SUCCESS;
}

//TODO: TBD
ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    if (!mapContains(chess->tournaments, (MapKeyElement)&tournament_id))
        return CHESS_TOURNAMENT_NOT_EXIST;
    return CHESS_SUCCESS;
}

//TODO: TBD
//does not remove player from data, only flags player as removed
ChessResult chessRemovePlayer(ChessSystem chess, int player_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isPlayerInSystem(chess, player_id))
        return CHESS_PLAYER_NOT_EXIST;
    return CHESS_SUCCESS;
}

//TODO: TBD
double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result) {
    if (chess == NULL)
        *chess_result = CHESS_NULL_ARGUMENT;
    if (!isPlayerInSystem(chess, player_id))
        *chess_result = CHESS_PLAYER_NOT_EXIST;
    *chess_result = CHESS_SUCCESS;
    return 0;
}

double calculatePlayerLevel(Player player) {
    int wins = player->num_wins;
    int draws = player->num_draws;
    int losses = player->num_losses;
    double level = 6 * wins + 2 * draws - 10 * losses;
    return level;
}

int comparePlayers(Player first, Player second) {
    assert(first && second);
    if (calculatePlayerLevel(first) > calculatePlayerLevel(second))
        return 1;
    if (calculatePlayerLevel(first) < calculatePlayerLevel(second))
        return -1;

    //at this point both players levels are equal, and we order them by map order of their IDs
    if (first->id > second->id)
        return -1;
    if (first->id < second->id)
        return 1;

    //at this point both players levels are equal, and it is the same ID for some reason
    return 0;
}

//TODO: make sure working with file is OK
//TODO: check if we wanna use mapGet function instead (to find tournament ID)
//TODO: where to put comparePlayers?
//check if file is open and writable, otherwise chess_save_failure
ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (file == NULL)
        return CHESS_NULL_ARGUMENT;
    //make sure working with file is OK


    int *key = (int *) mapGetFirst(chess->players);
    Player *current_player = (Player *) mapGet(chess->players, (MapKeyElement) key);
//    while (current_player != NULL) {
//        assert(chess->num_games > 0); //valid because first player is not null, i.e. games were entered into system
//        if (!(*current_player)->has_been_removed)
//            fprintf(file, "%d, %.2lf\n", (*current_player)->id,
//                    calculatePlayerLevel(*current_player) / chess->num_games);
//        key = mapGetNext(chess->players);
//        current_player = (Player *) mapGet(chess->players, (MapKeyElement) key);
//    }
    return CHESS_SUCCESS;
}

//TODO: TBD
ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (path_file == NULL)
        return CHESS_SAVE_FAILURE;
    return CHESS_SUCCESS;
}


