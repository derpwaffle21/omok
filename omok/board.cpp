#include <iostream>
#include "board.h"
#include "util.h"

int Board::turn()
{
	return ply % 2; // BLACK = 0, WHITE = 1
}

void Board::printBoard(bool printInfo)
{
	if (printInfo)
	{
		std::cout << "===============================" << std::endl;
		std::cout << "ply: " << ply << std::endl;
		std::cout << "side to move: ";

		if (turn() == BLACK)
			std::cout << "black" << std::endl;
		else
			std::cout << "white" << std::endl;

		std::cout << "Board State: ";

		if (state == BoardState::UNF)
			std::cout << "Unfinished" << std::endl;
		else if (state == BoardState::W_WIN)
			std::cout << "White win" << std::endl;
		else if (state == BoardState::B_WIN)
			std::cout << "Black win" << std::endl;
		else
			std::cout << "Draw" << std::endl;

		std::cout << "eval: " << eval << std::endl;
	}

	for (int idx = 0; idx <= BRD_LEN; idx++)
		std::cout << idx % 10;
	
	for (int idx = BRD_LEN; idx < BRD_SQ_NUM - BRD_LEN - 2; idx++)
	{
		if (idx % (BRD_LEN + 2) == 0)
		{
			std::cout << std::endl;
			std::cout << (idx / (BRD_LEN + 2)) % 10;
		}

		if (outOfBounds(idx))
			continue;

		if (bb[WHITE].test(idx))
			std::cout << 'W';
		else if (bb[BLACK].test(idx))
			std::cout << 'B';
		else
			std::cout << '.';
	}

	std::cout << std::endl;
}

void Board::makeMove(int idx)
{
	ASSERT(!outOfBounds(idx));
	ASSERT(state == BoardState::UNF);
	ASSERT(!bb[BLACK].test(idx) && !bb[WHITE].test(idx));

	bb[turn()].set(idx);

	std::pair<bool, int> chkMove = checkMove(idx);
	bool gamestateChanged = chkMove.first;
	int evalChange = chkMove.second;

	if (gamestateChanged)
	{
		if (turn() == BLACK)	//side that moved won the game
			state = BoardState::B_WIN;
		else
			state = BoardState::W_WIN;
	}

	if (turn() == BLACK)
		eval += evalChange;
	else
		eval -= evalChange;

	hist.push_back(idx);
	ply++;

	if (ply == MAX_GAME_LEN && state == BoardState::UNF)	//if we filled the entire board and nobody won, the game is a draw
		state = BoardState::DRAW;
}

// return 0 if the side won, else return the eval change
std::pair<bool, int> Board::checkMove(int idx)
{
	ASSERT(getBoardElement(idx) != EMPTY);

	int vertNum = countContinous(idx, NORTH) + countContinous(idx, SOUTH) + 1;	// add 1 for the main(idx) stone
	int horiNum = countContinous(idx, EAST) + countContinous(idx, WEST) + 1;
	int	slashNum = countContinous(idx, NORTH_EAST) + countContinous(idx, SOUTH_WEST) + 1;
	int backslashNum = countContinous(idx, NORTH_WEST) + countContinous(idx, SOUTH_EAST) + 1;

	std::pair<int, int> coord = idxToCoord(idx);

	// mid = (4, 4) when BRD_LEN = 9, mid = (3, 3) when BRD_LEN = 8
	// give initiative to place the stone closer to the middle
	// because we want to treat (1, 1) and (BRD_LEN, BRD_LEN) the same, we add 1 to BRD_LEN
	
	int evalChange =
		(vertNum * vertNum) +
		(horiNum * horiNum) +
		(slashNum * slashNum) +
		(backslashNum * backslashNum) +
		std::min(coord.first, (BRD_LEN + 1) - coord.first) +
		std::min(coord.second, (BRD_LEN + 1) - coord.second);

	if (vertNum >= 5 || horiNum >= 5 || slashNum >= 5 || backslashNum >= 5)
		return std::make_pair(true, evalChange);
	else
		return std::make_pair(false, evalChange);
}

int Board::countContinous(int idx, int dir)
{
	int side = getBoardElement(idx);
	int cnt;

	for (cnt = 1; cnt <= 4; cnt++)
	{
		int newIdx = idx + IDX_CHANGE_DIR[dir] * cnt;

		if (outOfBounds(newIdx) || getBoardElement(newIdx) != side)
			break;
	}

	return cnt - 1;	// if cnt = 1, that means there is 0 continous stones in that direction
}

void Board::undoMove()
{
	ASSERT(ply != 0);
	ASSERT(bb[WHITE].test(hist.back()) || bb[BLACK].test(hist.back()));

	std::pair<int, int> chkMove = checkMove(hist.back());
	int evalChange = chkMove.second;

	// since the eval is in black's perspective, and evalChange is always > 0
	// turn == BLACK -> white played the last move
	if (turn() == BLACK)
		eval += evalChange;
	else
		eval -= evalChange;

	// we need to set the bit AFTER we undo our eval
	if (turn() == BLACK)	//black to move -> white played the last move
		bb[WHITE].reset(hist.back());
	else
		bb[BLACK].reset(hist.back());

	state = BoardState::UNF;	//since we can't make a move on a board that is finished, any undos will make it unfinished
	hist.pop_back();
	ply--;
}

void Board::clear()
{
	while (ply != 0)
		undoMove();

	ASSERT(eval == 0);
}

int Board::getBoardElement(int idx)
{
	if (bb[WHITE].test(idx))
		return WHITE;
	else if (bb[BLACK].test(idx))
		return BLACK;
	else
		return EMPTY;
}