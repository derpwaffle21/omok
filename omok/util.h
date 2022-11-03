#pragma once

#include <utility>
#include <random>
#include <string>

#include "board.h"

//#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if (!(n)) \
{ \
printf("%s - Failed", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In File %s ", __FILE__); \
printf("At Line %d\n", __LINE__); \
exit(1); \
}
#endif

std::pair<int, int> idxToCoord(int idx);
int coordToIdx(std::pair<int, int> coord);	// y, x
bool outOfBounds(int idx);
void generateRandomGames(int gameNum, bool printBoard);