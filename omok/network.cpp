#include <fstream>

#include "network.h"
#include "util.h"

Convolutional::Convolutional()
{
	bias = 0;
	size = 0;
}

Convolutional::Convolutional(int _size) : size(_size)
{
	weight = std::vector<std::vector<double>>(size, std::vector<double>(size));
	bias = 0;
}

Dense::Dense()
{
	inputSize = 0;
	outputSize = 0;
}

Dense::Dense(int input, int output) : inputSize(input), outputSize(output)
{
	weight = std::vector<std::vector<double>>(inputSize, std::vector<double>(outputSize));
	bias   = std::vector<double>(outputSize);
}

std::vector<double> Dense::forward(const std::vector<double>& input, double (*activation)(double)) const
{
	ASSERT(input.size() == inputSize);

	std::vector<double> out(outputSize);

	for (int i = 0; i < outputSize; i++)
	{
		for (int j = 0; j < inputSize; j++)
			out[i] += input[j] * weight[j][i];

		out[i] += bias[i];
	}

	for (int i = 0; i < outputSize; i++)
		out[i] = activation(out[i]);

	return out;
}

std::vector<double> Dense::backward(const std::vector<double>& delta, const std::vector<double>& inputs, double(*activationDerivative)(double)) const
{
	ASSERT(delta.size() == outputSize);
	ASSERT(inputs.size() == inputSize);
	
	std::vector<double> nextDelta(inputSize);

	for (int i = 0; i < inputSize; i++)
	{
		for (int j = 0; j < delta.size(); j++)	// delta.size() == outputSize
			nextDelta[i] += (delta[j] * weight[i][j]);

		nextDelta[i] *= activationDerivative(inputs[i]);
	}

	return nextDelta;
}

Network::Network(int _convFilterSize, int _denseNum) : convFilterSize(_convFilterSize), denseNum(_denseNum)
{
	initMemory(convFilterSize, denseNum);
	randomize();
}

Network::Network(std::string networkFile)
{
	std::ifstream file(networkFile);
	std::string word;

	file >> word;
	convFilterSize = std::stoi(word);

	file >> word;
	denseNum = std::stoi(word);

	initMemory(convFilterSize, denseNum);

	int cnnLen = BRD_LEN - convFilterSize + 1;

	file >> word;	// "c"
	ASSERT(word == "c");

	for (int cy = 0; cy < convFilterSize; cy++)
	{
		for (int cx = 0; cx < convFilterSize; cx++)
		{
			file >> word;
			conv.weight[cy][cx] = std::stod(word);
		}
	}

	file >> word;
	conv.bias = std::stod(word);

	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int output = 1; output <= dense[layer - 1].outputSize; output++)
		{
			file >> word;	// "d"
			ASSERT(word == "d");

			file >> word;
			ASSERT(std::stoi(word) == layer);

			file >> word;
			ASSERT(std::stoi(word) == output);

			for (int input = 1; input <= dense[layer - 1].inputSize; input++)
			{
				file >> word;
				dense[layer - 1].weight[input - 1][output - 1] = std::stod(word);
			}

			file >> word;
			dense[layer - 1].bias[output - 1] = std::stod(word);
		}
	}

	file.close();
}

void Network::initMemory(int _convFilterSize, int _denseNum)
{
	conv = Convolutional(_convFilterSize);
	dense = std::vector<Dense>(_denseNum);
	int denseSize = (BRD_LEN - _convFilterSize + 1) * (BRD_LEN - _convFilterSize + 1) + 1 + 1;		// CNN output + moveNum + sideToMove

	for (int i = 0; i < _denseNum - 1; i++)
		dense[i] = Dense(denseSize, denseSize);

	dense[_denseNum - 1] = Dense(denseSize, 3);
}

void Network::randomize()
{
	// He-et-al Initialization

	// conv
	for (int cy = 0; cy < convFilterSize; cy++)
		for (int cx = 0; cx < convFilterSize; cx++)
			conv.weight[cy][cx] = normalDistributionRandom();

	conv.bias = 0;

	// dense
	for (int layer = 0; layer < denseNum; layer++)
	{
		for (int output = 0; output < dense[layer].outputSize; output++)
		{
			for (int input = 0; input < dense[layer].inputSize; input++)
				dense[layer].weight[input][output] = normalDistributionRandom() * sqrt((double)2 / dense[layer].inputSize);

			dense[layer].bias[output] = 0;
		}
	}
}

void Network::saveToFile(std::string fileName) const
{
	std::string str;

	str += (std::to_string(convFilterSize) + " " + std::to_string(denseNum) + "\n");

	int cnnLen = BRD_LEN - convFilterSize + 1;

	//conv layer
	str += "c ";

	for (int cy = 0; cy < convFilterSize; cy++)
		for (int cx = 0; cx < convFilterSize; cx++)
			str += (doubleToString(conv.weight[cy][cx]) + " ");

	str += doubleToString(conv.bias);
	str += "\n";

	//dense layer(s)
	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int output = 1; output <= dense[layer - 1].outputSize; output++)
		{
			str += ("d " + std::to_string(layer) + " " + std::to_string(output) + " ");	// d + <layer> + <output_idx>
			
			for (int input = 1; input <= dense[layer - 1].inputSize; input++)
				str += (doubleToString(dense[layer - 1].weight[input - 1][output - 1]) + " ");

			str += doubleToString(dense[layer - 1].bias[output - 1]);
			str += "\n";
		}
	}

	saveStringToFile(str, fileName);
}

std::vector<double> Network::evaluate(const Board& board, double (*activation)(double)) const
{
	return evaluate(board.get2DVector(), board.getHist().size(), activation);
}

std::vector<double> Network::evaluate(const std::vector<std::vector<int>>& board, int moveNum, double (*activation)(double)) const
{
	std::vector<double> out;
	int cnnLen = BRD_LEN - convFilterSize + 1;

	// conv
	for (int y = 0; y < cnnLen; y++)
	{
		for (int x = 0; x < cnnLen; x++)
		{
			double filterOutput = 0;

			for (int cy = 0; cy < convFilterSize; cy++)
				for (int cx = 0; cx < convFilterSize; cx++)
					filterOutput += (board[y + cy][x + cx] * conv.weight[cy][cx]);

			filterOutput += conv.bias;
			out.push_back(activation(filterOutput));
		}
	}

	out.push_back(moveNum);
	
	// side to move
	if (moveNum % 2 == 0)	// BLACK
		out.push_back(1);
	else					// WHITE
		out.push_back(-1);

	ASSERT(out.size() == dense[0].inputSize);

	// dense
	// hidden layers
	for (int i = 0; i < denseNum - 1; i++)
		out = dense[i].forward(out, activation);

	// no activation function for last layer
	out = dense[denseNum - 1].forward(out, Linear);

	ASSERT(out.size() == 3);

	return out;
}

void Network::backPropagate(const std::vector<std::vector<int>>& initialInput, int moveNum, const std::vector<double>& target,
	double (*activation)(double), double(*activationDerivative)(double), double lr)
{
	// TODO: ¸ð¸àÅÒ(momentum) °è¼ö Ãß°¡

	std::vector<std::vector<double>> denseInput(denseNum);
	int cnnLen = BRD_LEN - convFilterSize + 1;

	//same as evalutate
	for (int y = 0; y < cnnLen; y++)
	{
		for (int x = 0; x < cnnLen; x++)
		{
			double filterOutput = 0;

			for (int cy = 0; cy < convFilterSize; cy++)
				for (int cx = 0; cx < convFilterSize; cx++)
					filterOutput += (initialInput[y + cy][x + cx] * conv.weight[cy][cx]);

			filterOutput += conv.bias;
			denseInput[0].push_back(activation(filterOutput));
		}
	}

	denseInput[0].push_back(moveNum);
	
	// side to move
	if (moveNum % 2 == 0)	// BLACK
		denseInput[0].push_back(1);
	else					// WHITE
		denseInput[0].push_back(-1);

	for (int i = 0; i < denseNum - 1; i++)
		denseInput[i + 1] = dense[i].forward(denseInput[i], activation);

	for (int i = 0; i < denseNum; i++)
		ASSERT(denseInput[i].size() == dense[i].inputSize);

	std::vector<double> networkOutput = dense[denseNum - 1].forward(denseInput[denseNum - 1], Linear);
	std::vector<double> prob = Softmax(networkOutput);

	ASSERT(prob.size() == 3);

	// (1 + denseNum) deltas for two 65 node layers and the output layer
	std::vector<std::vector<double>> delta(1 + denseNum, std::vector<double>());

	// for output layer
	delta[denseNum] = std::vector<double>(3);

	// derivative of Cross Entropy Error of Softmax = value - target
	for (int i = 0; i < 3; i++)
		delta[denseNum][i] = (prob[i] - target[i]);

	for (int i = denseNum - 1; i >= 0; i--)
		delta[i] = dense[i].backward(delta[i + 1], denseInput[i], activationDerivative);

	delta[0].pop_back();		// no use for delta of sideToMove
	delta[0].pop_back();		// same for moveNum

	ASSERT(delta[0].size() == (cnnLen * cnnLen));

	// use delta[1] ~ delta[denseNum], denseInput to update weights and bias of dense layers
	for (int i = denseNum - 1; i >= 0; i--)
	{
		for (int output = 0; output < dense[i].outputSize; output++)
		{
			for (int input = 0; input < dense[i].inputSize; input++)
			{
				double gradient = denseInput[i][input] * delta[i + 1][output];
				dense[i].weight[input][output] -= (lr * gradient);
			}

			dense[i].bias[output] -= (lr * delta[i + 1][output]);
		}
	}
	
	// use delta[0], initialInput(board) to update weights and bias of CNN
	for (int cy = 0; cy < convFilterSize; cy++)
	{
		for (int cx = 0; cx < convFilterSize; cx++)
		{
			double gradient = 0;

			for (int y = 0; y < cnnLen; y++)
				for (int x = 0; x < cnnLen; x++)
					gradient += (initialInput[y + cy][x + cx] * delta[0][y * cnnLen + x]);

			conv.weight[cy][cx] -= (lr * gradient);
		}
	}

	conv.bias -= (lr * sum(delta[0]));
}

void Network::trainGame(const Board& finishedBoard, double (*activation)(double), double(*activationDerivative)(double), double lr)
{
	ASSERT(finishedBoard.state != BoardState::UNF);

	Board board;
	const std::vector<int>& hist = finishedBoard.getHist();
	std::vector<double> target(3);

	// BLACK_WIN = { 1, 0, 0 }, DRAW = { 0, 1, 0 }, WHITE_WIN = { 0, 0, 1 }
	target[(int)finishedBoard.state] = 1;

	for (int moveNum = 0; moveNum < hist.size(); moveNum++)
	{
		backPropagate(board.get2DVector(), moveNum, target, activation, activationDerivative, lr);
		board.makeMove(hist[moveNum]);
	}

	backPropagate(board.get2DVector(), hist.size(), target, activation, activationDerivative, lr);
}
