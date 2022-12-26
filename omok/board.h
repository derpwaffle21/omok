#pragma once

#include <bitset>
#include <vector>

#include "types.h"

class Board
{
private:
	std::bitset<BRD_SQ_NUM> bb[2];
	int ply = 0;
	std::vector<int> hist = std::vector<int>();

	std::pair<bool, int> checkMove(int idx) const;
	int countContinous(int idx, int dir) const;

public:
	BoardState state = BoardState::UNF;
	int eval = 0;

	void printBoard(bool printInfo) const;
	void makeMove(int idx);
	void undoMove();
	void clear();
	int getBoardElement(int idx) const;
	int getTurn() const;
	const std::vector<int>& getHist() const;
};
