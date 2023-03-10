#include <iostream>

#include "evaluate.h"
#include "util.h"
#include "board.h"
#include "search.h"
#include "network.h"
#include "train.h"
#include "test.h"

void playGame(int depth, Network& net)
{
	Board b;
	int playerColor, engineColor, ply = 0;
	bool playerTurn = false;
	int lastMove = coordToIdx(std::make_pair((BRD_LEN + 1) / 2, (BRD_LEN + 1) / 2));

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
			std::cout << std::endl << "input your y, x coord with a space between(example: 7 7), -1 to undo a move: ";
			std::cin >> y;

			if (y == -1)
			{
				b.undoMove(); b.undoMove();
				continue;
			}

			std::cin >> x;

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

			std::pair<int, int> searchResult = alphaBetaRoot(depth, b, net, info, engineColor, 0);
			int bestMove = searchResult.first;
			int score = searchResult.second;

			b.makeMove(bestMove);
			std::pair<int, int> coord = idxToCoord(bestMove);
			std::cout << "depth: " << depth << std::endl;
			std::cout << "move (" << coord.first << ", " << coord.second << ")" << std::endl;

			if (abs((double)score / 100) > 100)
			{
				if (((score > 0) && playerColor == WHITE) || ((score < 0) && playerColor == BLACK))
					std::cout << "BLACK WINS" << std::endl;
				else
					std::cout << "WHITE WINS" << std::endl;
			}
			else
			{
				std::cout << "eval: " << abs((double)score / 100) << "% for black" << std::endl;
			}

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
}

int main(void)
{
	Network nn("t3_180.nn");
	//playGame(4, nn);

	//nn.saveToFile("50.nn");

	for (int i = 181; i < 600; i++)
	{
		trainNetwork(nn, 2, 300, 5, 64, Sigmoid, SigmoidDerivative, 0.4, 16, 20);

		nn.saveToFile("t3_" + std::to_string(i) + ".nn");
	}

	return 0;
}
