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

extern std::mt19937 rng;
extern std::normal_distribution<double> normalDist;
extern std::uniform_int_distribution<long long> uniformDist;

double normalDistributionRandom();
int randomInt(int range);

std::pair<int, int> idxToCoord(int idx);
int coordToIdx(std::pair<int, int> coord);	// y, x
bool outOfBounds(int idx);

void saveGameToFile(const Board& board, std::string fileName);
void saveStringToFile(const std::string& gameString, std::string fileName);
std::string doubleToString(double x);

template <typename T>
void shuffleVector(std::vector<T>&v)
{
    std::shuffle(v.begin(), v.end(), rng);
}

double sum(const std::vector<double>& v);

double Linear(double x);
double LinearDerivative(double x);
double ReLU(double x);
double ReLUDerivative(double x);
double Sigmoid(double x);
double SigmoidDerivative(double x);
std::vector<double> Softmax(const std::vector<double>& x);

double MeanSquaredError(const std::vector<double>& targets, const std::vector<double>& values);
double CrossEntropyError(const std::vector<double>& targets, const std::vector<double>& values);