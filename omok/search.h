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

std::vector<int> generateMoveSet(board& _board, int color);
std::pair<int, int> alphaBetaRoot(int depth, board& _board, SearchInfo& info, int color);
int alphaBeta(int alpha, int beta, int depthleft, board& _board, SearchInfo& info, int color);