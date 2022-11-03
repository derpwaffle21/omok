#include <iostream>

#include "evaluate.h"
#include "util.h"
#include "board.h"
#include "search.h"

void _test()
{
	board _board;

	for (int i = 0; i < BRD_LEN + 2; i++)
	{
		for (int j = 0; j < BRD_LEN + 2; j++)
		{
			int idx = coordToIdx(std::make_pair(i, j));
			if (outOfBounds(idx))
				std::cout << "X";
			else
				std::cout << "O";
		}

		std::cout << std::endl;
	}


	//int idx = coordToIdx(std::make_pair(5, 5));
	/*
	for (int i = 0; i < 8; i++)
	{
		_board.makeMove(idx + IDX_CHANGE_DIR[i]);
		_board.makeMove(idx + IDX_CHANGE_DIR[i] * 2);
	}

	_board.makeMove(coordToIdx(std::make_pair(15, 15)));
	_board.makeMove(coordToIdx(std::make_pair(1, 1)));
	_board.makeMove(coordToIdx(std::make_pair(16, 14)));
	_board.makeMove(coordToIdx(std::make_pair(2, 2)));
	_board.makeMove(coordToIdx(std::make_pair(17, 13)));
	_board.makeMove(coordToIdx(std::make_pair(1, 2)));
	_board.makeMove(coordToIdx(std::make_pair(18, 12)));
	_board.makeMove(coordToIdx(std::make_pair(2, 1)));
	_board.makeMove(coordToIdx(std::make_pair(20, 10)));
	_board.makeMove(coordToIdx(std::make_pair(2, 3)));
	_board.makeMove(coordToIdx(std::make_pair(19, 11)));
	_board.clear();
	_board.printBoard();*/

	while (true)
	{
		int y, x;

		std::cin >> y >> x;

		if (y < 0 || x < 0)
		{
			_board.undoMove();
			_board.printBoard(true);
			std::cout << "EVALUATION: " << evaluate(_board) << std::endl;
			continue;
		}

		_board.makeMove(coordToIdx(std::make_pair(y, x)));
		_board.printBoard(true);
		std::cout << "EVALUATION: " << evaluate(_board) << std::endl;
	}
}

void testDepth(int limit)
{
	board b;

	for (int depth = 2; depth < limit; depth++)
	{
		int lastMove = coordToIdx(std::make_pair((BRD_LEN + 1) / 2, (BRD_LEN + 1) / 2));
		SearchInfo info;

		info.nodes = 0;
		info.lastMove = lastMove;

		int eval = alphaBetaRoot(depth, b, info, BLACK).second;
		std::cout << "depth: " << depth << ", nodes: " << info.nodes << ", eval: " << eval << std::endl << std::endl;
	}
}

int main(void)
{
	board b;
	int playerColor, engineColor, ply = 0;
	bool playerTurn = false;
	int lastMove = coordToIdx(std::make_pair((BRD_LEN + 1) / 2, (BRD_LEN + 1) / 2));

	//testDepth(10);

	do
	{
		std::cout << "choose your color: black=0, white=1: ";
		std::cin >> playerColor;
	} while (playerColor != 0 && playerColor != 1);

	if (playerColor == BLACK)
	{
		playerTurn = true;
		engineColor = WHITE;
	}
	else
	{
		playerTurn = false;
		engineColor = BLACK;
	}

	while (b.state == BoardState::UNF)
	{
		if (playerTurn)
		{
			b.printBoard(false);

			std::cout << "your color: ";

			if (playerColor == BLACK)
				std::cout << "Black" << std::endl;
			else
				std::cout << "White" << std::endl;

			int x, y;
			std::cout << std::endl << "input your x, y coord with a space between(example: 7 7), -1 to undo a move: ";
			std::cin >> x;

			if (x == -1)
			{
				b.undoMove(); b.undoMove();
				continue;
			}

			std::cin >> y;

			if (x <= 0 || x > BRD_LEN || y <= 0 || y > BRD_LEN)
			{
				std::cout << "invalid move input" << std::endl;
				continue;
			}

			int moveIdx = coordToIdx(std::make_pair(y, x));

			if (outOfBounds(moveIdx) || b.getBoardElement(moveIdx) != EMPTY)
			{
				std::cout << "invalid move input" << std::endl;
				continue;
			}

			b.makeMove(moveIdx);
			lastMove = moveIdx;
			b.printBoard(false);
		}
		else
		{
			SearchInfo info;
			info.nodes = 0;
			info.lastMove = lastMove;
			int depth = 4;

			std::pair<int, int> searchResult = alphaBetaRoot(depth, b, info, engineColor);
			int bestMove = searchResult.first;
			int score = searchResult.second;

			b.makeMove(bestMove);
			std::pair<int, int> coord = idxToCoord(bestMove);
			std::cout << "depth: " << depth << std::endl;
			std::cout << "move (" << coord.second << ", " << coord.first << ")" << std::endl;
			std::cout << "eval: " << score << std::endl;
			std::cout << "nodes: " << info.nodes << std::endl;
		}

		playerTurn = !playerTurn;
		ply++;
	}

	b.printBoard(false);

	if (b.state == BoardState::DRAW)
		std::cout << "it's a draw!" << std::endl;
	else if ((b.state == BoardState::B_WIN && playerColor == BLACK) ||
			 (b.state == BoardState::W_WIN && playerColor == WHITE))
		std::cout << "you win!" << std::endl;
	else
		std::cout << "you lose!" << std::endl;

	return 0;
}
