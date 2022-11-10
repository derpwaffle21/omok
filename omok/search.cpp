#include <algorithm>
#include <iostream>

#include "search.h"

std::vector<int> generateMoveSet(Board& _board, int color)
{
	std::vector<int> moves;

	for (int idx = 0; idx < BRD_SQ_NUM; idx++)
	{
		if (!outOfBounds(idx) && _board.getBoardElement(idx) == EMPTY)
			moves.push_back(idx);
	}

	return moves;
}

std::pair<int, int> alphaBetaRoot(int depth, Board& _board, SearchInfo& info, int color)
{
	ASSERT(_board.state == BoardState::UNF);
	int bestMove = -1;
	int alpha = -INF;
	int beta = INF;

	std::vector<int> moveSet = generateMoveSet(_board, color);

	for (int& idx : moveSet)
	{
		if (_board.getBoardElement(idx) != EMPTY)
			continue;

		info.nodes++;

		_board.makeMove(idx);
		info.lastMove = idx;

		int score;
		if (color == BLACK)
			score = -alphaBeta(-beta, -alpha, depth - 1, _board, info, WHITE);
		else
			score = -alphaBeta(-beta, -alpha, depth - 1, _board, info, BLACK);

		_board.undoMove();

		if (score > alpha)
		{
			std::pair<int, int> p = idxToCoord(idx);
			std::cout << "move: (" << p.first << ", " << p.second << "), score: " << score << std::endl;

			alpha = score;
			bestMove = idx;
		}
	}

	return std::make_pair(bestMove, alpha);
}

int alphaBeta(int alpha, int beta, int depthleft, Board& _board, SearchInfo& info, int color)
{
	if (depthleft == 0 || _board.state != BoardState::UNF)
	{
		if (color == BLACK)
			return evaluate(_board);
		else
			return -evaluate(_board);
	}

	std::vector<int> moveSet = generateMoveSet(_board, color);

	for (int& idx : moveSet)
	{
		if (_board.getBoardElement(idx) != EMPTY)
			continue;

		info.nodes++;

		_board.makeMove(idx);
		info.lastMove = idx;

		int score;
		if (color == BLACK)
			score = -alphaBeta(-beta, -alpha, depthleft - 1, _board, info, WHITE);
		else
			score = -alphaBeta(-beta, -alpha, depthleft - 1, _board, info, BLACK);

		_board.undoMove();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}
