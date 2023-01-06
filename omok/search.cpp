#include <algorithm>
#include <iostream>

#include "search.h"

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

std::pair<int, int> alphaBetaRoot(int depth, Board& _board, const Network& net, SearchInfo& info, int color, int temp, bool output)
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
			score = -alphaBeta(-beta, -alpha, depth, _board, net, info, WHITE, 1, temp);
		else
			score = -alphaBeta(-beta, -alpha, depth, _board, net, info, BLACK, 1, temp);

		_board.undoMove();

		if (score > alpha)
		{
			if (output)
			{
				std::pair<int, int> p = idxToCoord(idx);
				std::cout << "move: (" << p.first << ", " << p.second << "), score: " << score << std::endl;
			}

			alpha = score;
			bestMove = idx;
		}
	}

	return std::make_pair(bestMove, alpha);
}

int alphaBeta(int alpha, int beta, int searchDepth, Board& _board, const Network& net, SearchInfo& info, int color, int depth, int temp)
{
	if (_board.state != BoardState::UNF)
	{
		if (color == BLACK)
			return evaluate(_board, net);		// MAX_EVAL * 10000 causes an overflow
		else
			return -evaluate(_board, net);
	}

	if (searchDepth == depth)
	{
		int eval;

		if (color == BLACK)
			eval = evaluate(_board, net) * 10000;
		else
			eval = -(evaluate(_board, net) * 10000);

		if (temp != 0)
		{
			int changeByTemp = randomInt(temp * 2) - temp;
			eval += changeByTemp;
		}

		return eval;
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
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, net, info, WHITE, depth + 1, temp);
		else
			score = -alphaBeta(-beta, -alpha, searchDepth, _board, net, info, BLACK, depth + 1, temp);

		_board.undoMove();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}