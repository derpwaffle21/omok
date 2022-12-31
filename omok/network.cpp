#include <fstream>

#include "network.h"
#include "util.h"

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

	for (int y = 1; y <= BRD_LEN - convFilterSize + 1; y++)
	{
		for (int x = 1; x <= BRD_LEN - convFilterSize + 1; x++)
		{
			file >> word;	// "c"
			ASSERT(word == "c");

			file >> word;
			ASSERT(std::stoi(word) == y);

			file >> word;
			ASSERT(std::stoi(word) == x);

			for (int cy = 0; cy < convFilterSize; cy++)
			{
				for (int cx = 0; cx < convFilterSize; cx++)
				{
					file >> word;
					conv[y - 1][x - 1].weight[cy][cx] = std::stod(word);
				}
			}

			file >> word;
			conv[y - 1][x - 1].bias = std::stod(word);
		}
	}

	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int output = 1; output <= dense[layer - 1].outputSize; output++)
		{
			file >> word;	// "d"
			ASSERT(word == "d");

			file >> word;
			ASSERT(std::stoi(word) == layer);

			file >> word;
			ASSERT(std::stoi(word) == input);

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
	conv = std::vector<std::vector<Convolutional>>(BRD_LEN - _convFilterSize + 1,
						std::vector<Convolutional>(BRD_LEN - _convFilterSize + 1,
									 Convolutional(_convFilterSize)));

	dense = std::vector<Dense>(_denseNum);
	int denseSize = (BRD_LEN - _convFilterSize + 1) * (BRD_LEN - _convFilterSize + 1) + 1;		// CNN output + moveNum

	for (int i = 0; i < _denseNum - 1; i++)
		dense[i] = Dense(denseSize, denseSize);

	dense[_denseNum - 1] = Dense(denseSize, 3);
}

void Network::randomize()
{
	// He-et-al Initialization

	// conv
	for (int y = 0; y < BRD_LEN - convFilterSize + 1; y++)
	{
		for (int x = 0; x < BRD_LEN - convFilterSize + 1; x++)
		{
			for (int cy = 0; cy < convFilterSize; cy++)
				for (int cx = 0; cx < convFilterSize; cx++)
					conv[y][x].weight[cy][cx] = normalDistributionRandom();

			conv[y][x].bias = 0;
		}
	}

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

double Network::backPropagate(const std::vector<double>& target)
{
	return 0;
}

void Network::saveToFile(std::string fileName) const
{
	std::string str;

	str += (std::to_string(convFilterSize) + " " + std::to_string(denseNum) + "\n");

	//conv layer
	for (int y = 1; y <= BRD_LEN - convFilterSize + 1; y++)
	{
		for (int x = 1; x <= BRD_LEN - convFilterSize + 1; x++)
		{
			str += ("c " + std::to_string(y) + " " + std::to_string(x) + " "); // conv: (y ~ y + convSize - 1) (x ~ x + convSize - 1)

			for (int cy = 0; cy < convFilterSize; cy++)
				for (int cx = 0; cx < convFilterSize; cx++)
					str += (std::to_string(conv[y - 1][x - 1].weight[cy][cx]) + " ");

			str += std::to_string(conv[y - 1][x - 1].bias);
			str += "\n";
		}
	}

	//dense layer(s)
	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int output = 1; output <= dense[layer - 1].outputSize; output++)
		{
			str += ("d " + std::to_string(layer) + " " + std::to_string(output) + " ");	// d + <layer> + <output_idx>
			
			for (int input = 1; input <= dense[layer - 1].inputSize; input++)
				str += (std::to_string(dense[layer - 1].weight[input - 1][output - 1]) + " ");

			str += std::to_string(dense[layer - 1].bias[output - 1]);
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

	// conv
	for (int y = 0; y < BRD_LEN - convFilterSize + 1; y++)
	{
		for (int x = 0; x < BRD_LEN - convFilterSize + 1; x++)
		{
			double filterOutput = 0;

			for (int cy = 0; cy < convFilterSize; cy++)
				for (int cx = 0; cx < convFilterSize; cx++)
					filterOutput += (board[y + cy][x + cx] * conv[y][x].weight[cy][cx]);

			filterOutput += conv[y][x].bias;
			out.push_back(activation(filterOutput));
		}
	}

	out.push_back(moveNum);
	ASSERT(out.size() == dense[0].inputSize);

	// dense
	// hidden layers
	for (int i = 0; i < denseNum - 1; i++)
		out = dense[i].forward(out, activation);

	// no activation function for last layer
	out = dense[denseNum - 1].forward(out, Linear);

	return out;
}