#pragma once

#include "types.h"
#include "network.h"

BoardState playTestGame(const Network& black, const Network& white, int depth, int temp, bool printInfo);
void testNet(const Network& newNet, const Network& oldNet, int gameNum, int depth, int temp);