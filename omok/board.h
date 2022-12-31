#pragma once

#include <bitset>
#include <vector>

#include "types.h"

class Board
{
private:
	std::bitset<BRD_SQ_NUM> bb[2];
	int ply;
	std::vector<int> hist;
	std::vector<std::vector<int>> board;

	std::pair<bool, int> checkMove(int idx) const;
	int countContinous(int idx, int dir) const;

public:
	Board();

	BoardState state;
	int eval;

	void printBoard(bool printInfo) const;
	void makeMove(int idx);
	void undoMove();
	void clear();

	int getBoardElement(int idx) const;
	int getTurn() const;

	const std::vector<int>& getHist() const;
	const std::vector<std::vector<int>>& get2DVector() const;

	operator std::string() const;
};
