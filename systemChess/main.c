#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "headers/chessGame.h"
#include "headers/chessTournament.h"
#include "headers/chessSystem.h"
#include "headers/player.h"
#include "../map/headers/map.h"

//Defines

// typedefs and structs //
struct chess_system_t {
    Map tournaments;//key is tournament_id
    Map players;
    int num_games;
    int num_valid_time_games;
    int sum_valid_time_games;
};

// Static Functions //
static bool isValidID(int id);
static bool isValidLocation(const char *location);
static bool isValidMaxGame(int num);
static bool isValidGameTime(int num);
static bool isTournamentEnded(ChessSystem chess, int tournament_id);
static bool doesGameExists(ChessSystem chess, ChessTournament tournament, int first_player, int second_player,
                           bool was_first_removed, bool was_second_removed);
static bool isMaxExceeded(ChessSystem chess, int tournament_id, int first_player, int second_player,
                          bool ignore_first_player_games, bool ignore_second_player_games);
static bool isPlayerInSystem(ChessSystem chess, int player_id);
void updateGameStatistics(ChessSystem chess, ChessGame game, int player_id);
int compareTournamentScores(Player current_player, int *current_highest, Player current_winner);
void preformSwitcharoo(int *first_id, int *second_id, double *first_score, double *second_score);
void maxSort(int *ids, double *scores, int size);
void printArrays(int size, int *ids, double *scores);


// Chess Functions //
ChessResult convertMapResultToChessResult(MapResult map_result);
ChessTournament createTournament(int tournament_id, int max_games_per_player, const char *tournament_location);
ChessResult chessRemovePlayerEffects(ChessSystem chess, Player player);
void updatePlayersStatistics(Map players, ChessGame match, int first_player_id, int second_player_id,
                             bool was_first_removed, bool was_second_removed);
Player createPlayer(int id);
ChessResult chessAddPlayer(ChessSystem chess, ChessTournament tournament, int player_id);
int comparePlayers(Player first, Player second);
ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file);

// mapCreate Functions //
int compareMapKeys(MapKeyElement key1, MapKeyElement key2);
void freeMapKey(MapKeyElement key);
void freeMapData(MapDataElement data);
MapDataElement copyMapKey(MapKeyElement key);
void freeMapDataTournament(MapDataElement data);
MapDataElement copyMapDataTournament(MapDataElement data);
MapDataElement copyMapDataGame(MapDataElement data);
MapDataElement copyMapDataPlayer(MapDataElement data);

int compareMapKeys(MapKeyElement key1, MapKeyElement key2) {
    if (key1 == NULL) return -1;
    if (key2 == NULL) return 1;
    return *((int *) key1) - (*(int *) key2);
}

void freeMapKey(MapKeyElement key) {
    free(key);
}
void freeMapData(MapDataElement data) {
    free(data);
}
MapDataElement copyMapKey(MapKeyElement key) {
    if (key == NULL) {
        return NULL;
    }
    int *key_copy = malloc(sizeof(int));
    *key_copy = *((int *) key);
    return key_copy;
}

void freeMapDataTournament(MapDataElement data) {
    freeTournament((ChessTournament) data);
}

MapDataElement copyMapDataTournament(MapDataElement data) {
    Map game_map = mapCreate(copyMapDataGame, copyMapKey, freeMapData, freeMapKey, compareMapKeys);
    Map players_map = mapCreate(copyMapDataPlayer, copyMapKey, freeMapData, freeMapKey,
                                compareMapKeys);
    return copyTournament((ChessTournament)data, game_map, players_map);
}

MapDataElement copyMapDataGame(MapDataElement data) {
    return copyGame((ChessGame) data);
}

MapDataElement copyMapDataPlayer(MapDataElement data) {
    return copyPlayer((Player) data);
}

//
//void freeMapDataTournament(ChessTournament data) {
//    if (data == NULL) {
//        return;
//    }
//    mapDestroy(getPlayers(data));
//    setPlayersMap(data, NULL);
//    mapDestroy(getGames(data));
//    setGamesMap(data, NULL);
//    free(data);
//    data = NULL;
//}
//void freeMapKey(MapKeyElement key) {
//    free(key);
//}
//void freeMapData(MapDataElement data) {
//    Player test = (Player )data;
//    free(data);
//}
//int compareMapKeys(MapKeyElement key1, MapKeyElement key2) {
//    if (key1 == NULL) return -1;
//    if (key2 == NULL) return 1;
//    return *((int *) key1) - (*(int *) key2);
//}
//MapDataElement copyMapKey(MapKeyElement key) {
//    if (key == NULL) {
//        return NULL;
//    }
//    int *key_copy = malloc(sizeof(int));
//    *key_copy = *((int *) key);
//    return key_copy;
//}
//MapDataElement copyMapDataTournament(MapDataElement data) {
//    if (data == NULL) {
//        return NULL;
//    }
//    ChessTournament tournament_copy = createEmptyTournament();
//    tournament_copy = (ChessTournament)data;
//    if (tournament_copy == NULL) {
//        return NULL;
//    }
//    Map games = getGames(tournament_copy);
//    if (games != NULL && mapGetSize(games) != 0) {
//        setGamesMap(tournament_copy, mapCopy(games));
//        if (getGames(tournament_copy) == NULL) {
//            free(tournament_copy);
//            return NULL;
//        }
//    }
//
//    Map players = getPlayers(tournament_copy);
//    if (players != NULL && mapGetSize(players) != 0) {
//        setPlayersMap(tournament_copy, mapCopy(players));
//        if (getPlayers(tournament_copy) == NULL && players != NULL) {
//            mapDestroy(getGames(tournament_copy));
//            free(tournament_copy);
//            return NULL;
//        }
//    }
//    return (MapDataElement) tournament_copy;
//}
//MapDataElement copyMapDataGame(MapDataElement data) {
//    ChessGame *copy = createEmptyChessGame();
//    if (copy == NULL) {
//        return NULL;
//    }
//    *copy = *(ChessGame *)data;
//    return copy;
//}
//MapDataElement copyMapDataPlayer(MapDataElement data) {
//    Player player_copy = createEmptyPlayer();
//
//    if(player_copy == NULL){
//        return NULL;
//    }
//    return (MapDataElement)player_copy;
//}




/*
MapDataElement copyMapDataPlayer(MapDataElement data) {
    Player original = (Player)data;
    Player player_copy = createEmptyPlayer();
    Player *original_pnt = original;
    Player *copy_pnt = &player_copy;
    *copy_pnt = *original_pnt;
    if(player_copy == NULL){
        return NULL;
    }
    return (MapDataElement)player_copy;
}
*/
// Static Functions //

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
static bool isTournamentEnded(ChessSystem chess, int tournament_id) {
    assert(chess != NULL);
    ChessTournament current_tournament = mapGet(chess->tournaments,(MapKeyElement) &tournament_id);
    return hasEnded(current_tournament);
}

//check if 2 players already played together in this tournament.
// If so - game already happened
static bool doesGameExists(ChessSystem chess, ChessTournament tournament, int first_player, int second_player,
                           bool was_first_removed, bool was_second_removed) {
    assert(tournament != NULL && chess != NULL);
    if(was_first_removed || was_second_removed){
        return false;
    }
    Map games = getGames(tournament);
    ChessGame *current_game = NULL;
    mapGetFirst(games);
    MAP_FOREACH(MapKeyElement, iterator, games){
        current_game = mapGet(games, iterator);
        freeMapKey(iterator);
        if(current_game == NULL){
            break;
        }
        if (getFirstPlayerId(*current_game) == first_player && getSecondPlayerId(*current_game) == second_player)
            return true;
        if (getFirstPlayerId(*current_game) == second_player && getSecondPlayerId(*current_game) == first_player)
            return true;
    }
    return false;
}

//check if one of the players already played the maximum amount of games allowed in tournament.
static bool isMaxExceeded(ChessSystem chess, int tournament_id, int first_player, int second_player,
                          bool ignore_first_player_games, bool ignore_second_player_games) {
    assert(chess != NULL);
    ChessTournament current_tournament = mapGet(chess->tournaments,(MapKeyElement) &tournament_id);
    if (current_tournament == NULL) {
        return false;
    }
    Player current_player = mapGet(getPlayers(current_tournament), (MapKeyElement) &first_player);
    if (getNumOfGames(current_player) >= getMaxGamesPerPlayer(current_tournament) && !ignore_first_player_games)
        return true;
    current_player = mapGet(getPlayers(current_tournament), (MapKeyElement) &second_player);
    if (getNumOfGames(current_player) >= getMaxGamesPerPlayer(current_tournament) && !ignore_second_player_games)
        return true;
    return false;
}

//check if player ID is recognized to chess system
static bool isPlayerInSystem(ChessSystem chess, int player_id) {
    assert(chess != NULL);
    return mapContains(chess->players, (MapKeyElement) &player_id);
}

// Chess Functions //

ChessSystem chessCreate() {
    ChessSystem Chess = (ChessSystem) malloc(sizeof(struct chess_system_t));
    if (Chess == NULL)
        return NULL;
    Chess->num_games = 0;
    Chess->num_valid_time_games = 0;
    Chess->sum_valid_time_games = 0;
    Chess->tournaments = mapCreate(copyMapDataTournament, copyMapKey, freeMapDataTournament, freeMapKey,
                                   compareMapKeys);
    Chess->players = mapCreate(copyMapDataPlayer, copyMapKey, freeMapData, freeMapKey,
                               compareMapKeys);
    return Chess;
}

//TODO: check what needs to be freed
void chessDestroy(ChessSystem chess) {
    if (chess == NULL)
        return;
    mapDestroy(chess->tournaments);
    mapDestroy(chess->players);
    free(chess);
    chess = NULL;
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
    ChessTournament tournament = createChessTournament(tournament_id, max_games_per_player, tournament_location);
    if(tournament == NULL){
        return NULL;
    }
    setGamesMap(tournament, mapCreate(copyMapDataGame, copyMapKey, freeMapData, freeMapKey, compareMapKeys));
    setPlayersMap(tournament, mapCreate(copyMapDataPlayer, copyMapKey, freeMapData, freeMapKey, compareMapKeys));
    return tournament;
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char *tournament_location) {
    if (chess == NULL || tournament_location == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    if (mapContains(chess->tournaments, (MapKeyElement) &tournament_id))
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    if (!isValidLocation(tournament_location))
        return CHESS_INVALID_LOCATION;
    if (!isValidMaxGame(max_games_per_player))
        return CHESS_INVALID_MAX_GAMES;

    ChessTournament tournament = createTournament(tournament_id, max_games_per_player,
                                                  (const char *) &tournament_location);
    if (tournament == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    if(chess->tournaments == NULL){
        chess->tournaments = mapCreate(copyMapDataTournament, copyMapKey, freeMapDataTournament,
                                       freeMapKey, compareMapKeys);
    }
    MapResult map_result = mapPut(chess->tournaments, (MapKeyElement)&tournament_id,
                                  (MapDataElement)tournament);//adding Tournament to chessSystem chess
    return convertMapResultToChessResult(map_result);
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

    ChessTournament tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    ChessResult result;
    if (tournament == NULL) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if (hasEnded(tournament)) {
        return CHESS_TOURNAMENT_ENDED;
    }

    bool reset_first_player = false, reset_second_player = false;
    Player first_player_profile = mapGet(getPlayers(tournament), &first_player);
    if(first_player_profile == NULL){
        result = chessAddPlayer(chess, tournament, first_player);
        if(result != CHESS_SUCCESS){
            return result;
        }
    } else{
        if(isRemoved(first_player_profile)){
            reset_first_player = true;
        }
    }
    Player second_player_profile = mapGet(getPlayers(tournament), &second_player);
    if(second_player_profile == NULL){
        result = chessAddPlayer(chess, tournament, second_player);
        if(result != CHESS_SUCCESS){
            return result;
        }
    } else{
        if(isRemoved(second_player_profile)){
            reset_second_player = true;
        }
    }

    if (doesGameExists(chess, tournament, first_player, second_player, reset_first_player, reset_second_player)) {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (!isValidGameTime(play_time)) {
        return CHESS_INVALID_PLAY_TIME;
    }



    //TODO: can we reset the games of a removed player regardless of the success of game creation?
    if (isMaxExceeded(chess, tournament_id, first_player, second_player, reset_first_player, reset_second_player)) {
        return CHESS_EXCEEDED_GAMES;
    }

    int game_id = getLastGameId(tournament);
    ChessGame game = createChessGame(game_id, first_player, second_player, winner, play_time);
    if (game == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult map_result = mapPut(getGames(tournament), (MapKeyElement) &game_id,
                                  (MapDataElement) &game);
    result = convertMapResultToChessResult(map_result);
    if (result == CHESS_SUCCESS) {
        //TODO: SOMEHOW THE SAME PROFILE GETS UPDATED BOTH TIMES
        updatePlayersStatistics(getPlayers(tournament), game, first_player, second_player, reset_first_player,
                                reset_second_player);
        updatePlayersStatistics(chess->players, game, first_player, second_player, reset_first_player,
                                reset_second_player);
    }
    return result;
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    ChessTournament tournament = mapGet(chess->tournaments, (MapKeyElement)&tournament_id);
    if(tournament == NULL){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Map players = getPlayers(tournament);
    Player tournament_profile = NULL;
    Player system_profile = NULL;
    MAP_FOREACH(MapKeyElement, iterator, players){
        tournament_profile = mapGet(players, (MapKeyElement)iterator);
        system_profile = mapGet(chess->players, (MapKeyElement)iterator);
        freeMapKey(iterator);
        if(tournament_profile == NULL || system_profile == NULL){
            break;
        }
        updateDraws(system_profile, -getNumOfDraws(tournament_profile));
        updateLosses(system_profile, -getNumOfLosses(tournament_profile));
        updateWins(system_profile, -getNumOfWins(tournament_profile));
        updatePlayerPlayTime(system_profile, -getPlayerPlayTime(tournament_profile));
    }
    mapRemove(chess->tournaments, (MapKeyElement)&tournament_id);
    return CHESS_SUCCESS;
}

void updateGameStatistics(ChessSystem chess, ChessGame game, int player_id){
    int first_player_id = getFirstPlayerId(game);
    int second_player_id = getSecondPlayerId(game);
    Player first_player = mapGet(chess->players, &first_player_id);
    Player second_player = mapGet(chess->players, &second_player_id);
    if(first_player == NULL || second_player == NULL){
        return;
    }
    if(first_player_id == player_id){
        if(isRemoved(second_player)){
            setGameWinner(game, DRAW);
        } else{
            setGameWinner(game, SECOND_PLAYER);
        }
    }
    if(second_player_id == player_id){
        if(isRemoved(first_player)){
            setGameWinner(game, DRAW);
        } else{
            setGameWinner(game, FIRST_PLAYER);
        }
    }
}

ChessResult chessRemovePlayerEffects(ChessSystem chess, Player player) {
    //assert(chess!=NULL);
    int player_id = getPlayerId(player);
    ChessGame current_game = NULL;
    ChessTournament current_tournament = NULL;
    Map games = NULL;
    Map players = NULL;
    Player tournament_profile;
    MAP_FOREACH(MapKeyElement, iterator, chess->tournaments){
        current_tournament = mapGet(chess->tournaments, (MapKeyElement) iterator);
        freeMapKey(iterator);
        if(current_tournament == NULL){
            break;
        }
        players = getPlayers(current_tournament);
        tournament_profile = mapGet(players, &player_id);
        if(tournament_profile != NULL){
            setIsRemoved(tournament_profile, true);
        }
        games = getGames(current_tournament);
        MAP_FOREACH(MapKeyElement, gamesIterator, games){
            current_game = mapGet(games, gamesIterator);
            freeMapKey(gamesIterator);
            if(current_game == NULL){
                break;
            }
            updateGameStatistics(chess, current_game, player_id);
        }
    }
    return CHESS_SUCCESS;
}

//does not remove player from data, only flags player as removed
ChessResult chessRemovePlayer(ChessSystem chess, int player_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(player_id))
        return CHESS_INVALID_ID;
    if (!isPlayerInSystem(chess, player_id)){
        return CHESS_PLAYER_NOT_EXIST;
    }
    Player player = mapGet(chess->players, (MapKeyElement)&player_id);
    ChessResult result = CHESS_SUCCESS;
    if(player != NULL){
        if(isRemoved(player)){
            return CHESS_PLAYER_NOT_EXIST;
        }
        setIsRemoved(player, true);
        result = chessRemovePlayerEffects(chess, player);
    }
    return result;
}

void updatePlayersStatistics(Map players, ChessGame game, int first_player, int second_player,
                             bool was_first_removed, bool was_second_removed) {
    //TODO: ALSO UPDATE GAME TIME
    Player first_player_profile = mapGet(players, (MapKeyElement) &first_player);
    Player second_player_profile = mapGet(players, (MapKeyElement) &second_player);
    if (getWinner(game) == FIRST_PLAYER) {
        updateWins(first_player_profile, 1);
        updateLosses(second_player_profile, 1);
    } else {
        if(getWinner(game) == SECOND_PLAYER){
            updateWins(second_player_profile, 1);
            updateLosses(first_player_profile, 1);
        } else{
            updateDraws(first_player_profile, 1);
            updateDraws(second_player_profile, 1);
        }
    }
    if(was_first_removed){
        resetGamesPlayed(first_player_profile);
        setIsRemoved(first_player_profile, false);
    }
    if(was_second_removed){
        resetGamesPlayed(second_player_profile);
        setIsRemoved(second_player_profile, false);
    }
    updateGamesPlayed(first_player_profile);
    updateGamesPlayed(second_player_profile);
    updatePlayerPlayTime(first_player_profile, getDuration(game));
    updatePlayerPlayTime(second_player_profile, getDuration(game));
}

ChessResult chessAddPlayer(ChessSystem  chess, ChessTournament tournament, int player_id) {
    Player player = createPlayer(player_id);
    if(player == NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    if(!mapContains(chess->players, &player_id)){
        mapPut(chess->players, (MapKeyElement) &player_id, (MapDataElement)player);
    }
    mapPut(getPlayers(tournament), (MapKeyElement) &player_id, (MapDataElement)player);
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (!isValidID(tournament_id))
        return CHESS_INVALID_ID;
    ChessTournament tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (!mapContains(chess->tournaments, (MapKeyElement) &tournament_id))
        return CHESS_TOURNAMENT_NOT_EXIST;
    if (isTournamentEnded(chess, tournament_id))
        return CHESS_TOURNAMENT_ENDED;
    setHasEnded(tournament, true);
    if (mapGetSize(getGames(tournament)) == 0) {
        return CHESS_NO_GAMES;
    }
    Map players = getPlayers(tournament);
    Player current_player = mapGet(players, mapGetFirst(players));
    if (current_player == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    Player current_winner = current_player;
    int *highest_score = malloc(sizeof(int));
    *highest_score = -1;
    MAP_FOREACH(MapKeyElement, playersIterator, players){
        current_player = mapGet(players, (MapKeyElement) playersIterator);
        if(current_player == NULL || isRemoved(current_player)){
            continue;
        }
        compareTournamentScores(current_player, highest_score, current_winner);
    }
    setTournamentWinner(tournament, getPlayerId(current_winner));
    free(highest_score);
    return CHESS_SUCCESS;
}

int compareTournamentScores(Player current_player, int *current_highest, Player current_winner){
    int current_score = (getNumOfWins(current_player)) * 2 + getNumOfDraws(current_player);
    if (current_score > *current_highest) {
        *current_highest = current_score;
        current_winner = current_player;
        return 1;
    }
    if (current_score == *current_highest) {
        if (getNumOfLosses(current_player) < getNumOfLosses(current_winner)) {
            current_winner = current_player;
            return 1;
        }
        if (getNumOfLosses(current_player) == getNumOfLosses(current_winner)) {
            if (getPlayerId(current_player) < getPlayerId(current_winner)) {
                current_winner = current_player;
                return 1;
            }
        }
    }
    return -1;
}

double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result) {
    if (chess == NULL) {
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0;
    }
    if(!isValidID(player_id)){
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }
    Player player = mapGet(chess->players, &player_id);
    if (!isPlayerInSystem(chess, player_id) || (player != NULL && isRemoved(player))) {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return 0;
    }
    if(player == NULL){
        *chess_result = CHESS_OUT_OF_MEMORY;
        return 0;
    }
    double games_played = getNumOfWins(player) + getNumOfDraws(player) + getNumOfLosses(player);
    double total_time = getPlayerPlayTime(player);
    double average_time = total_time/games_played;
    *chess_result = CHESS_SUCCESS;
    return average_time;
}

double calculatePlayerLevel(Player player) {
    double wins = getNumOfWins(player);
    double draws = getNumOfDraws(player);
    double losses = getNumOfLosses(player);
    double total_games = wins + draws + losses;
    double score = 6 * wins + 2 * draws - 10 * losses;
    return score/total_games;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file) {
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if(file == NULL){
        return CHESS_SAVE_FAILURE;
    }

    Map players = chess->players;
    Player current_player;

    int *ids = malloc(sizeof(int) * (unsigned int) mapGetSize(players));
    double *scores = malloc(sizeof(double) * (unsigned int) mapGetSize(players));
    int index = 0;
    double level;
    MAP_FOREACH(MapKeyElement, playerIterator, players){
        current_player = mapGet(players, playerIterator);
        freeMapKey(playerIterator);
        if(current_player == NULL || isRemoved(current_player)){
            continue;
        }
        level = calculatePlayerLevel(current_player);
        ids[index] = getPlayerId(current_player);
        scores[index] = level;
        index++;
    }
    if(index < mapGetSize(players)-1){
        ids[index] = -1;
    }
    maxSort(ids, scores, index);

    for(int i=0; i<index; i++){
        fprintf(file,"%d %.2f\n", ids[i], scores[i]);
    }

    return CHESS_SUCCESS;
}

//TODO: delete later
void printArrays(int size, int *ids, double *scores){
    printf("\n");
    for(int i=0; i<size; i++){
        printf("%d, %f\n", ids[i], scores[i]);
    }
}

void maxSort(int *ids, double *scores, int size){
    int sorted = 0;
    while(sorted < size && ids[sorted] != -1) {
        for (int i = 0; i < size - sorted -1 && ids[i+1] != -1; i++) {
            if (scores[i] < scores[i + 1]) {
                preformSwitcharoo(&ids[i], &ids[i + 1], &scores[i], &scores[i + 1]);
                continue;
            }
            if (scores[i] == scores[i + 1] && ids[i] > ids[i + 1]) {
                preformSwitcharoo(&ids[i], &ids[i + 1], &scores[i], &scores[i + 1]);
                continue;
            }
        }
        sorted++;
    }
}

void preformSwitcharoo(int *first_id, int *second_id, double *first_score, double *second_score){
    int dummy_id = *first_id;
    *first_id = *second_id;
    *second_id = dummy_id;

    double dummy_score = *first_score;
    *first_score = *second_score;
    *second_score = dummy_score;
}

//TODO: TBD
ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file) {
    if (chess == NULL)
        return CHESS_NULL_ARGUMENT;
    if (path_file == NULL)
        return CHESS_SAVE_FAILURE;
    return CHESS_SUCCESS;
}


