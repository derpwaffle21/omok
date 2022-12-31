#include <algorithm>
#include <iostream>

#include "search.h"

Network baseNetwork("initial_random.nn");

// when BRD_LEN % 2 == 1
std::vector<int> generateMoveSetByPos(const Board& _board, int color)
{
	std::vector<int> moves;
	std::pair<int, int> center = std::make_pair(ceil((float)BRD_LEN / 2), ceil((float)BRD_LEN / 2));

	for (int distToCenter = 0; distToCenter <= BRD_LEN - 1; distToCenter++)
	{
		for (int yOffset = 0; yOffset <= distToCenter; yOffset++)
		{
			int xOffset = distToCenter - yOffset;

			if (center.first + yOffset > BRD_LEN || center.second + xOffset > BRD_LEN)
				continue;

			if (_board.getBoardElement(coordToIdx(std::make_pair(center.first + yOffset, center.second + xOffset))) == EMPTY)
				moves.push_back(coordToIdx(std::make_pair(center.first + yOffset, center.second + xOffset)));

			if (_board.getBoardElement(coordToIdx(std::make_pair(center.first + yOffset, center.second - xOffset))) == EMPTY)
				moves.push_back(coordToIdx(std::make_pair(center.first + yOffset, center.second - xOffset)));

			if (_board.getBoardElement(coordToIdx(std::make_pair(center.first - yOffset, center.second + xOffset))) == EMPTY)
				moves.push_back(coordToIdx(std::make_pair(center.first - yOffset, center.second + xOffset)));

			if (_board.getBoardElement(coordToIdx(std::make_pair(center.first - yOffset, center.second - xOffset))) == EMPTY)
				moves.push_back(coordToIdx(std::make_pair(center.first - yOffset, center.second - xOffset)));
		}
	}

	return moves;
}

// using board's internal eval
std::vector<int> generateMoveSetByEval(Board& _board, int color)
{
	std::vector<std::pair<int, int>> scoreMove;	// score, move

	for (int idx = 0; idx < BRD_SQ_NUM; idx++)
	{
		if (!outOfBounds(idx) && _board.getBoardElement(idx) == EMPTY)
		{
			_board.makeMove(idx);

			//eval is always in black's perspective
			if (color == BLACK)
				scoreMove.push_back(std::make_pair(-_board.eval, idx));	// sort -> smallest is first -> BLACK_WIN = MAX_EVAL(positive)
			else
				scoreMove.push_back(std::make_pair(_board.eval, idx));

			_board.undoMove();
		}
	}

	std::sort(scoreMove.begin(), scoreMove.end());

	std::vector<int> ms;

	for (std::pair<int, int>& p : scoreMove)
		ms.push_back(p.second);

	return ms;
}

std::pair<int, int> alphaBetaRoot(int depth, Board& _board, SearchInfo& info, int color, int temp)
{
	ASSERT(_board.state == BoardState::UNF);
	int bestMove = -1;
	int alpha = -INF;
	int beta = INF;

	std::vector<int> moveSet = generateMoveSetByEval(_board, color);

	for (int& idx : moveSet)
	{
		if (_board.getBoardElement(idx) != EMPTY)
			continue;

		info.nodes++;

		_board.makeMove(idx);
		info.lastMove = idx;

		int score;
		if (color == BLACK)
			score = -alphaBeta(-beta, -alpha, depth, _board, info, WHITE, 1, temp);
		else
			score = -alphaBeta(-beta, -alpha, depth, _board, info, BLACK, 1, temp);

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

int alphaBeta(int alpha, int beta, int searchDepth, Board& _board, SearchInfo& info, int color, int depth, int temp)
{
	if (searchDepth == depth || _board.state != BoardState::UNF)
	{
		int changeByTemp = randomInt(temp * 2) - temp;

		if (color == BLACK)
			return (evaluate(_board, baseNetwork) * 10000) + changeByTemp;
		else
			return -(evaluate(_board, baseNetwork) * 10000) + changeByTemp;
	}

	std::vector<int> moveSet = generateMoveSetByEval(_board, color);

	for (int& idx : moveSet)
	{
		if (_board.getBoardElement(idx) != EMPTY)
			continue;

		info.nodes++;

		_board.makeMove(idx);
		info.lastMove = idx;

		int score;
		if (color == BLACK)
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, info, WHITE, depth + 1, temp);
		else
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, info, BLACK, depth + 1, temp);

		_board.undoMove();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}
