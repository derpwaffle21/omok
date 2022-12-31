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

double normalDistributionRandom();
int randomInt(int range);

std::pair<int, int> idxToCoord(int idx);
int coordToIdx(std::pair<int, int> coord);	// y, x
bool outOfBounds(int idx);

void saveGameToFile(const Board& board, std::string fileName);
void saveStringToFile(const std::string& gameString, std::string fileName);
void generateRandomGame(Board& board, bool printBoard = false);

double ReLU(double x);
double Sigmoid(double x);
std::vector<double> Softmax(const std::vector<double>& x);
double MeanSquaredError(const std::vector<double>&targets, const std::vector<double>&values);