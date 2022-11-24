#pragma once

#define INF 987654321

constexpr int BRD_LEN = 7;
constexpr int BRD_SQ_NUM = (BRD_LEN + 2) * (BRD_LEN + 2);
constexpr int MAX_GAME_LEN = BRD_LEN * BRD_LEN;
constexpr int MAX_EVAL = BRD_LEN * BRD_LEN * BRD_LEN * BRD_LEN * BRD_LEN;	// BRD_LEN ^ 5

enum
{
	BLACK,
	WHITE,
	EMPTY
};

enum class BoardState
{
	B_WIN,
	DRAW,
	W_WIN,
	UNF,	//unfinished
};

enum DIR
{
	NORTH,
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH,
	SOUTH_WEST,
	WEST,
	NORTH_WEST
};

constexpr int IDX_CHANGE_DIR[8] = {
	-(BRD_LEN + 2),			// N
	-(BRD_LEN + 2) + 1,		// NE
	1,						// E
	(BRD_LEN + 2) + 1,		// SE
	(BRD_LEN + 2),			// S
	(BRD_LEN + 2) - 1,		// SW
	-1,						// W
	-(BRD_LEN + 2) - 1		// NW
};
