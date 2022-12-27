#include <set>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

#include "util.h"
#include "types.h"
#include "board.h"

std::mt19937 rng = std::mt19937(std::random_device{}());;

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
