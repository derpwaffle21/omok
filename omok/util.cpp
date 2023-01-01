#include <set>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

#include "util.h"
#include "types.h"
#include "board.h"

std::mt19937 rng = std::mt19937(std::random_device{}());;
std::normal_distribution<double> normalDist(0, 1);
std::uniform_int_distribution<long long> uniformDist(0, LONG_INF);

double normalDistributionRandom()
{
    return normalDist(rng);
}

// return a uniform random number from 0 ~ range
int randomInt(int range)
{
    return uniformDist(rng) % ((long long)range + 1);
}

// y, x
std::pair<int, int> idxToCoord(int idx)
{
    ASSERT(idx >= 0 && idx < BRD_SQ_NUM);

    return std::make_pair(idx / (BRD_LEN + 2), idx % (BRD_LEN + 2));
}

// y, x
int coordToIdx(std::pair<int, int> coord)
{
    ASSERT(coord.first >= 0 && coord.first < BRD_LEN + 2 && coord.second >= 0 && coord.second < BRD_LEN + 2);

    return coord.first * (BRD_LEN + 2) + coord.second;
}

bool outOfBounds(int idx)
{
    if (idx % (BRD_LEN + 2) == 0 || idx % (BRD_LEN + 2) == BRD_LEN + 1)
        return true;

    if (idx / (BRD_LEN + 2) == 0 || idx / (BRD_LEN + 2) == BRD_LEN + 1)
        return true;

    return false;
}

void saveGameToFile(const Board& board, std::string fileName)
{
    saveStringToFile(std::string(board), fileName);
}

void saveStringToFile(const std::string& gameString, std::string fileName)
{
    std::ofstream fout(fileName);

    fout << gameString;
    fout.close();
}

void generateRandomGame(Board& board, bool printBoard)
{
    std::vector<int> availableMoves;

    for (int idx = 0; idx < BRD_SQ_NUM; idx++)
    {
        if (!outOfBounds(idx))
            availableMoves.push_back(idx);
    }

    std::shuffle(availableMoves.begin(), availableMoves.end(), rng);

    for (auto& idx : availableMoves)
    {
        board.makeMove(idx);

        if (board.state != BoardState::UNF)
        {
            if (printBoard)
                board.printBoard(true);

            break;
        }

        if (printBoard)
            board.printBoard(true);
    }
}

double Linear(double x)
{
    return x;
}

double LinearDerivative(double x)
{
    return 1;
}

double ReLU(double x)
{
    if (x < 0)
        return 0;

    return x;
}

double Sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}

double SigmoidDerivative(double x)
{
    return x * (1 - x);
}

std::vector<double> Softmax(const std::vector<double>& x)
{
    double c = *std::max_element(x.begin(), x.end());
    double sum = 0;

    std::vector<double> probability(x.size());

    for (double val : x)
        sum += exp(val - c);

    for (int i = 0; i < x.size(); i++)
        probability[i] = exp(x[i] - c) / sum;

    return probability;
}

double MeanSquaredError(const std::vector<double>& targets, const std::vector<double>& values)
{
    ASSERT(targets.size() == values.size());

    double mse = 0;

    for (int i = 0; i < targets.size(); i++)
        mse += 0.5 * (targets[i] - values[i]) * (targets[i] - values[i]);   // faster than using pow

    return mse;
}

double CrossEntropyError(const std::vector<double>& targets, const std::vector<double>& values)
{
    ASSERT(targets.size() == values.size());

    double cse = 0;

    for (int i = 0; i < targets.size(); i++)
        cse += targets[i] * std::log(values[i] + 1e-7); // 1e-7: delta to avoid inf

    return -cse;
}
