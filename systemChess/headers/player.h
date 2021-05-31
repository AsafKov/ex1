#ifndef EX1_PLAYER_H
#include <stdbool.h>
#include <stdlib.h>
#define EX1_PLAYER_H

typedef struct player *Player;

Player playerCreatePlayer(int id);

Player playerCreateEmptyPlayer();

int getPlayerId(Player player);

int getNumOfGames(Player player);

int getNumOfWins(Player player);

int getNumOfDraws(Player player);

int getNumOfLosses(Player player);

void resetRemovedPlayerStatistics(Player player);

bool isRemoved(Player player);

int getPlayerPlayTime(Player player);

void updateWins(Player player, int wins);

void updatePlayerPlayTime(Player player, int time);

void updateDraws(Player player, int draws);

void updateLosses(Player player, int losses);

void setIsRemoved(Player player, bool isRemoved);

void updateGamesPlayed(Player player);

void resetGamesPlayed(Player player);

void resetTimePlayed(Player player);

void setPlayerId(Player player, int id);

void setGamesPlayed(Player player, int gamesPlayed);

void setPlayerWins(Player player, int wins);

void setPlayerDraws(Player player, int draws);

void setPlayerLosses(Player player, int losses);

Player copyPlayer(Player data);


#endif //EX1_PLAYER_H
