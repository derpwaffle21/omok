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

Network::Network(int _convFilterSize, int _denseNum) : convFilterSize(_convFilterSize), denseNum(_denseNum)
{
	initMemory(convFilterSize, denseNum);
	randomize();
}

Network::Network(std::string networkFile)
{
	//TODO : network파일 읽어서 CNN과 dense에 값들을 집어넣는거
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
	for (int i = 0; i < BRD_LEN - convFilterSize + 1; i++)
	{
		for (int j = 0; j < BRD_LEN - convFilterSize + 1; j++)
		{
			for (int k = 0; k < convFilterSize; k++)
			{
				for (int l = 0; l < convFilterSize; l++)
				{
					conv[i][j].weight[k][l] = normal_dist_random();
					conv[i][j].bias[k][l] = 0;
				}
			}
		}
	}

	// dense
	for (int i = 0; i < denseNum; i++)
	{
		for (int j = 0; j < dense[i].inputSize; j++)
		{
			for (int k = 0; k < dense[i].outputSize; k++)
			{
				dense[i].weight[j][k] = normal_dist_random() * sqrt((double)2 / dense[i].inputSize);
				dense[i].bias[j][k] = 0;
			}
		}
	}
}

void Network::saveToFile(std::string fileName)
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

double Network::evaluate(const std::vector<std::vector<int>>& board, int moveNum)
{
	//TODO

	return 0;
}