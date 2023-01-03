#pragma once

#include "board.h"
#include "network.h"

void generateRandomGame(Board& board, bool printBoard = false);
void trainNetwork(Network& net, int depth, int temp, int iteration, int batchSize,
	double (*activation)(double), double(*activationDerivative)(double), double lr);