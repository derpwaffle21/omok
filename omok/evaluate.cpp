#include <iostream>

#include "util.h"
#include "network.h"
#include "evaluate.h"
#include "types.h"

double evaluate(const Board& _board, const Network& network)
{
	double eval;

	if (_board.state == BoardState::B_WIN)
		eval = MAX_EVAL;
	else if (_board.state == BoardState::W_WIN)
		eval = -MAX_EVAL;
	else if (_board.state == BoardState::DRAW)
		eval = 0;
	else
	{
		std::vector<double> prob = Softmax(network.evaluate(_board, Sigmoid));

		// prob[0] = BLACK_WIN, prob[1] = DRAW, prob[2] = WHITE_WIN
		eval = 1 * prob[0] + 0.5 * prob[1] + 0 * prob[2];
	}

	return eval;
}