#include <iostream>
#include "evaluate.h"
#include "types.h"
#include "board.h"

int evaluate(Board& _board)
{
	int eval;

	if (_board.state == BoardState::B_WIN)
		eval = MAX_EVAL;
	else if (_board.state == BoardState::W_WIN)
		eval = -MAX_EVAL;
	else if (_board.state == BoardState::DRAW)
		eval = 0;
	else
	{
		eval = _board.eval;

		// tempo for the side to move
		if (_board.getTurn() == BLACK)
			eval += TEMPO;
		else
			eval -= TEMPO;
	}

	return eval;
}