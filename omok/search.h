#pragma once

#include "board.h"
#include "evaluate.h"
#include "util.h"

class SearchInfo
{
public:
	int nodes = 0;
	int lastMove = coordToIdx(std::make_pair((BRD_LEN - 1) / 2, (BRD_LEN - 1) / 2));
};

std::vector<int> generateMoveSetByPos(const Board& _board, int color);
std::vector<int> generateMoveSetByEval(Board& _board, int color);
std::pair<int, int> alphaBetaRoot(int depth, Board& _board, Network& net, SearchInfo& info, int color, int temp = 0, bool output = true);
int alphaBeta(int alpha, int beta, int searchDepth, Board& _board, Network& net, SearchInfo& info, int color, int depth, int temp = 0);