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

void generateRandomGames(int gameNum, bool printBoard)
{
    std::vector<int> availableMoves;
    Board _board;

    for (int idx = 0; idx < BRD_SQ_NUM; idx++)
    {
        if (!outOfBounds(idx))
            availableMoves.push_back(idx);
    }

    while (gameNum--)
    {
        std::shuffle(availableMoves.begin(), availableMoves.end(), rng);

        for (auto& idx : availableMoves)
        {
            _board.makeMove(idx);

            if (_board.state != BoardState::UNF)
            {
                if (printBoard)
                    _board.printBoard(true);

                break;
            }

            if (printBoard)
                _board.printBoard(true);
        }

        _board.clear();
    }
}
