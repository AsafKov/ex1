#ifndef EX1_CHESSTOURNAMENT_H
#include "../../map/headers/map.h"
#include <stdlib.h>
#define EX1_CHESSTOURNAMENT_H

typedef struct chess_tournament_t *ChessTournament;

ChessTournament createChessTournament(int tournament_id, int max_games_per_player, const char *tournament_location);
ChessTournament createEmptyTournament();

Map getGames(ChessTournament tournament);
Map getPlayers(ChessTournament tournament);
int getTournamentId(ChessTournament tournament);
const char *getLocation(ChessTournament tournament);
int getWinnerId(ChessTournament tournament);
int getMaxGamesPerPlayer(ChessTournament tournament);
bool hasEnded(ChessTournament tournament);
int getLongestGameDuration(ChessTournament tournament);
int getValidTimeGames(ChessTournament tournament);
int getSumValidTimeGames(ChessTournament tournament);
int getNumOfPlayers(ChessTournament tournament);
int getLastGameId(ChessTournament tournament);

void setHasEnded(ChessTournament tournament, bool hasEnded);
void setLongestGameDuration(ChessTournament tournament, int time);
void setGamesMap(ChessTournament tournament, Map games);
void setPlayersMap(ChessTournament tournament, Map players);
void setTournamentWinner(ChessTournament tournament, int winnerId);
const char* getTournamentLocation(ChessTournament tournament);
void freeTournament(ChessTournament data) ;
ChessTournament copyTournament(ChessTournament data, Map games, Map players);


#endif //EX1_CHESSTOURNAMENT_H
