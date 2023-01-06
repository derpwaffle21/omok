#include <iostream>

#include "test.h"
#include "search.h"

BoardState playTestGame(const Network& black, const Network& white, int depth, int temp, bool printInfo)
{
	Board board;

	while (board.state == BoardState::UNF)
	{
		std::pair<int, int> info;
		int bestMove;
		int eval;
		SearchInfo sInfo;

		if (board.getTurn() == BLACK)
			info = alphaBetaRoot(depth, board, black, sInfo, BLACK, temp, false);
		else				// WHITE
			info = alphaBetaRoot(depth, board, white, sInfo, WHITE, temp, false);

		bestMove = info.first;
		eval = info.second;

		board.makeMove(bestMove);

		if (printInfo)
		{
			board.printBoard(false);
			std::cout << "eval: " << eval << std::endl;
		}
	}

	if (printInfo)
	{
		if (board.state == BoardState::B_WIN)
			std::cout << "Black wins!" << std::endl;
		else if (board.state == BoardState::W_WIN)
			std::cout << "White wins!" << std::endl;
		else
			std::cout << "Draw!" << std::endl;
	}

	return board.state;
}

void testNet(const Network& newNet, const Network& oldNet, int gameNum, int depth, int temp)
{
	if (gameNum % 2 == 1)
		std::cout << "It is advised to set your testing game number to a multiple of two." << std::endl;

	int newWin = 0, oldWin = 0, draw = 0;

	for (int game = 1; game <= gameNum; game++)
	{
		BoardState result;
		int newColor = game % 2;

		if (newColor == BLACK)
			result = playTestGame(newNet, oldNet, depth, temp, false);
		else
			result = playTestGame(oldNet, newNet, depth, temp, false);

		if (result == BoardState::DRAW)
		{
			draw++;
		}
		else
		{
			if ((result == BoardState::B_WIN && newColor == BLACK) || (result == BoardState::W_WIN && newColor == WHITE))
				newWin++;
			else
				oldWin++;
		}
	}

	std::cout << "new net vs old net result: " << newWin << " - " << draw << " - " << oldWin << std::endl;
}
