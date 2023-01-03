#pragma once

#include "board.h"

class Convolutional
{
public:
	int size;

	std::vector<std::vector<double>> weight;
	double bias;

	Convolutional();
	Convolutional(int _size);

	std::vector<double> forward(const std::vector<std::vector<int>>& input, double (*activation)(double)) const;
};

class Dense
{
public:
	int inputSize, outputSize;
	std::vector<std::vector<double>> weight;
	std::vector<double> bias;

	Dense();
	Dense(int input, int output);

	std::vector<double> forward(const std::vector<double>& input, double (*activation)(double)) const;
	std::vector<double> backward(const std::vector<double>& delta, const std::vector<double>& inputs, double (*activationDerivative)(double), double lr);
};

class Network
{
public:
	int convFilterSize;
	int denseNum;

	Convolutional conv;
	std::vector<Dense> dense;

	void initMemory(int _convFilterSize, int _denseNum);
	void randomize();

public:
	Network(int _convFilterSize, int _denseNum);
	Network(std::string networkFile);

	void saveToFile(std::string fileName) const;
	std::vector<double> evaluate(const Board& board, double (*activation)(double)) const;
	std::vector<double> evaluate(const std::vector<std::vector<int>>& board, int moveNum, double (*activation)(double)) const;

	void backPropagate(const std::vector<std::vector<int>>& initialInput, int moveNum, const std::vector<double>& target,
		double (*activation)(double), double(*activationDerivative)(double), double lr);

	void trainGame(const Board& finishedBoard, double (*activation)(double), double(*activationDerivative)(double), double lr);
};