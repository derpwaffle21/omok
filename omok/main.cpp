#include <iostream>

#include "evaluate.h"
#include "util.h"
#include "board.h"
#include "search.h"
#include "network.h"

void playGame(int depth)
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
			info.lastMove = lastMove;

			std::pair<int, int> searchResult = alphaBetaRoot(depth, b, info, engineColor);
			int bestMove = searchResult.first;
			int score = searchResult.second;

			b.makeMove(bestMove);
			std::pair<int, int> coord = idxToCoord(bestMove);
			std::cout << "depth: " << depth << std::endl;
			std::cout << "move (" << coord.second << ", " << coord.first << ")" << std::endl;

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
	/*
	Board b;
	std::string str;

	for (int i = 0; i < 500000; i++)
	{
		if (i % 5000 == 0)
			std::cout << "game: " << i << std::endl;

		b.clear();
		generateRandomGame(b);
		str += std::string(b);
	}*/

	
	Network nn("initial_random.nn");
	Board board;
	
	board.makeMove(coordToIdx(std::make_pair(7, 7)));
	

	std::vector<double> output;
	output = nn.evaluate(board.get2DVector(), board.getHist().size(), Sigmoid);

	std::cout << output.size() << std::endl;
	std::cout << "eval: " << output[0] << ", " << output[1] << ", " << output[2] << std::endl;

	std::vector<double> prob = Softmax(output);

	std::cout << prob.size() << std::endl;
	std::cout << "prob: " << prob[0] * 100 << "% Black win, " << prob[1] * 100 << "% Draw, " << prob[2] * 100 << "% White win" << std::endl;

	board.makeMove(coordToIdx(std::make_pair(7, 8)));
	//board.makeMove(coordToIdx(std::make_pair(7, 6)));
	//board.makeMove(coordToIdx(std::make_pair(6, 7)));

	output = nn.evaluate(board.get2DVector(), board.getHist().size(), Sigmoid);

	std::cout << output.size() << std::endl;
	std::cout << "eval: " << output[0] << ", " << output[1] << ", " << output[2] << std::endl;

	prob = Softmax(output);

	std::cout << prob.size() << std::endl;
	std::cout << "prob: " << prob[0] * 100 << "% Black win, " << prob[1] * 100 << "% Draw, " << prob[2] * 100 << "% White win" << std::endl;
	/*
	board.undoMove();

	for (int i = 1; i <= 13; i++)
	{
		for (int j = 1; j <= 13; j++)
		{
			board.makeMove(coordToIdx(std::make_pair(i, j)));
			output = nn.evaluate(board.get2DVector(), board.getHist().size(), Sigmoid);
			//board.printBoard(false);

			std::cout << output.size() << std::endl;
			std::cout << "eval: " << output[0] << ", " << output[1] << ", " << output[2] << std::endl;

			std::cout << prob.size() << std::endl;
			std::cout << "prob: " << prob[0] * 100 << "% Black win, " << prob[1] * 100 << "% Draw, " << prob[2] * 100 << "% White win" << std::endl;

			board.undoMove();
		}
	}*/
	bool train = false;

	//double prevCB, prevCW, prevDB

	if (train)
	{
		Board board;
		std::cout << std::fixed;

		for (int i = 0; i < 1000000; i++)
		{
			if (i % 10000 == 0)
			{
				//ASSERT(abs(nn.conv.bias))

				std::cout << "game: " << i << std::endl;
				std::cout << "conv bias: " << nn.conv.bias << std::endl;
				std::cout << "conv weight: " << nn.conv.weight[0][0] << std::endl;
				std::cout << "dense bias: " << nn.dense[0].bias[0] << std::endl;
			}

			generateRandomGame(board);

			std::vector<double> target(3);

			target[(int)board.state] = 1;

			nn.backPropagate(board.get2DVector(), board.getHist().size(), target, Sigmoid, SigmoidDerivative, 0.1);
			board.clear();
		}
	}

	//nn.saveToFile("random_10000.nn");

	playGame(4);

	return 0;
}
