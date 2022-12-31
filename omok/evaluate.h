#pragma once

#include "board.h"
#include "network.h"

constexpr int TEMPO = 10;

double evaluate(const Board& _board, const Network& network);
