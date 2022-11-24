#include <algorithm>
#include <iostream>

#include "search.h"

// when BRD_LEN % 2 == 1
std::vector<int> generateMoveSet(Board& _board, int color)
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
			score = -alphaBeta(-beta, -alpha, depth, _board, info, WHITE, 1);
		else
			score = -alphaBeta(-beta, -alpha, depth, _board, info, BLACK, 1);

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

int alphaBeta(int alpha, int beta, int searchDepth, Board& _board, SearchInfo& info, int color, int depth)
{
	if (searchDepth == depth || _board.state != BoardState::UNF)
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
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, info, WHITE, depth + 1);
		else
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, info, BLACK, depth + 1);

		_board.undoMove();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}
