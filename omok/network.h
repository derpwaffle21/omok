#pragma once

#include "board.h"

class Convolutional
{
public:
	int size;
	std::vector<std::vector<double>> weight;
	std::vector<std::vector<double>> bias;

	Convolutional(int _size);
};

class Dense
{
public:
	int inputSize, outputSize;
	std::vector<std::vector<double>> weight; // [inputidx][outputidx]
	std::vector<std::vector<double>> bias;	 // [inputidx][outputidx]

	Dense();
	Dense(int input, int output);
};

class Network
{
private:
	int convFilterSize;
	int denseNum;

	std::vector<std::vector<Convolutional>> conv;
	std::vector<Dense> dense;

	void initMemory(int _convFilterSize, int _denseNum);
	void randomize();

public:
	Network(int _convFilterSize, int _denseNum);
	Network(std::string networkFile);

	void saveToFile(std::string fileName);
	std::vector<double> evaluate(const std::vector<std::vector<int>>& board, int moveNum);
};