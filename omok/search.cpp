#include <algorithm>
#include <iostream>

#include "search.h"

std::vector<int> generateMoveSet(board& _board, int color)
{
	std::vector<std::pair<int, int>> scoreMove;	// score, move

	for (int idx = 0; idx < BRD_SQ_NUM; idx++)
	{
		if (!outOfBounds(idx) && _board.getBoardElement(idx) == EMPTY)
		{
			_board.makeMove(idx);

			if (color == BLACK)
				scoreMove.push_back(std::make_pair(-evaluate(_board), idx));	// sort->오름차순이므로
			else
				scoreMove.push_back(std::make_pair(evaluate(_board), idx));

			_board.undoMove();
		}
	}

	std::sort(scoreMove.begin(), scoreMove.end());

	std::vector<int> ms;

	for (std::pair<int, int>& p : scoreMove)
		ms.push_back(p.second);

	return ms;
}

std::pair<int, int> alphaBetaRoot(int depth, board& _board, SearchInfo& info, int color)
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

int alphaBeta(int alpha, int beta, int depthleft, board& _board, SearchInfo& info, int color)
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
