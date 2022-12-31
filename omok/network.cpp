#include <fstream>

#include "network.h"
#include "util.h"

Convolutional::Convolutional(int _size) : size(_size)
{
	weight = std::vector<std::vector<double>>(size, std::vector<double>(size));
	bias   = std::vector<std::vector<double>>(size, std::vector<double>(size));
}

Dense::Dense()
{
	inputSize = 0;
	outputSize = 0;
}

Dense::Dense(int input, int output) : inputSize(input), outputSize(output)
{
	weight = std::vector<std::vector<double>>(inputSize, std::vector<double>(outputSize));
	bias   = std::vector<std::vector<double>>(inputSize, std::vector<double>(outputSize));
}

std::vector<double> Dense::output(const std::vector<double>& input) const
{
	std::vector<double> out(outputSize);

	for (int i = 0; i < outputSize; i++)
		for (int j = 0; j < inputSize; j++)
			out[i] += (input[j] * weight[j][i] + bias[j][i]);

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

					file >> word;
					conv[y - 1][x - 1].bias[cy][cx] = std::stod(word);
				}
			}
		}
	}

	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int input = 1; input <= dense[layer - 1].inputSize; input++)
		{
			file >> word;	// "d"
			ASSERT(word == "d");

			file >> word;
			ASSERT(std::stoi(word) == layer);

			file >> word;
			ASSERT(std::stoi(word) == input);

			for (int output = 1; output <= dense[layer - 1].outputSize; output++)
			{
				file >> word;
				dense[layer - 1].weight[input - 1][output - 1] = std::stod(word);

				file >> word;
				dense[layer - 1].bias  [input - 1][output - 1] = std::stod(word);
			}
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
			{
				for (int cx = 0; cx < convFilterSize; cx++)
				{
					conv[y][x].weight[cy][cx] = normalDistributionRandom();
					conv[y][x].bias  [cy][cx] = 0;
				}
			}
		}
	}

	// dense
	for (int layer = 0; layer < denseNum; layer++)
	{
		for (int input = 0; input < dense[layer].inputSize; input++)
		{
			for (int output = 0; output < dense[layer].outputSize; output++)
			{
				dense[layer].weight[input][output] = normalDistributionRandom() * sqrt((double)2 / dense[layer].inputSize);
				dense[layer].bias  [input][output] = 0;
			}
		}
	}
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
			{
				for (int cx = 0; cx < convFilterSize; cx++)
				{
					str += (std::to_string(conv[y - 1][x - 1].weight[cy][cx]) + " " +
							std::to_string(conv[y - 1][x - 1].bias  [cy][cx]) + " ");
				}
			}

			str += "\n";
		}
	}

	//dense layer(s)
	for (int layer = 1; layer <= denseNum; layer++)
	{
		for (int input = 1; input <= dense[layer - 1].inputSize; input++)
		{
			str += ("d " + std::to_string(layer) + " " + std::to_string(input) + " ");	// d + <layer> + <input_idx>
			
			for (int output = 1; output <= dense[layer - 1].outputSize; output++)
			{
				str += (std::to_string(dense[layer - 1].weight[input - 1][output - 1]) + " " +
						std::to_string(dense[layer - 1].bias  [input - 1][output - 1]) + " ");
			}

			str += "\n";
		}
	}

	saveStringToFile(str, fileName);
}

std::vector<double> Network::evaluate(const Board& board) const
{
	return evaluate(board.get2DVector(), board.getHist().size());
}

std::vector<double> Network::evaluate(const std::vector<std::vector<int>>& board, int moveNum) const
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
					filterOutput += (board[y + cy][x + cx] * conv[y][x].weight[cy][cx] + conv[y][x].bias[cy][cx]);

			out.push_back(filterOutput);
		}
	}

	out.push_back(moveNum);
	ASSERT(out.size() == dense[0].inputSize);

	// dense
	for (int i = 0; i < denseNum; i++)
		out = dense[i].output(out);

	return out;
}