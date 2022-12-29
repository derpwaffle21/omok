#pragma once

#include "board.h"

double ReLU(double x);
double Sigmoid(double x);

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
	std::vector<std::vector<double>> weight;
	std::vector<std::vector<double>> bias;

	Dense();
	Dense(int input, int output);
};

class Network
{
private:
	int convSize;
	int denseNum;

	std::vector<std::vector<Convolutional>> conv;
	std::vector<Dense> dense;

public:
	Network(int convSize, int denseNum);
	Network(std::string networkFile);

	void init();
	void saveToFile(std::string fileName);
	double evaluate(const std::vector<std::vector<int>>& board, int moveNum);
};