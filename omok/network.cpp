#include "network.h"
#include "util.h"

double ReLU(double x)
{
	if (x < 0)
		return 0;

	return x;
}

Convolutional::Convolutional(int _size) : size(_size)
{
	weight = std::vector<std::vector<double>>(size, std::vector<double>(size));
	bias = std::vector<std::vector<double>>(size, std::vector<double>(size));
}

Dense::Dense()
{
	inputSize = 0;
	outputSize = 0;
}

Dense::Dense(int input, int output) : inputSize(input), outputSize(output)
{
	weight = std::vector<std::vector<double>>(inputSize, std::vector<double>(outputSize));
	bias = std::vector<std::vector<double>>(inputSize, std::vector<double>(outputSize));
}

Network::Network(int _convSize, int _denseNum) : convSize(_convSize), denseNum(_denseNum)
{
	init();
}

Network::Network(std::string networkFile)
{
	init();
	//TODO
}

void Network::init()
{
	conv = std::vector<std::vector<Convolutional>>(BRD_LEN - convSize + 1,
		               std::vector<Convolutional>(BRD_LEN - convSize + 1,
												  Convolutional(convSize)));

	dense = std::vector<Dense>(denseNum);
	int denseSize = (BRD_LEN - convSize + 1) * (BRD_LEN - convSize + 1) + 1;		// CNN output + moveNum

	for (int i = 0; i < denseNum - 1; i++)
		dense[i] = Dense(denseSize, denseSize);

	dense[denseNum - 1] = Dense(denseSize, 3);
}

void Network::saveToFile(std::string fileName)
{
	std::string str;

	str += (std::to_string(convSize) + " " + std::to_string(denseNum) + "\n");

	for (int y = 1; y <= BRD_LEN - convSize + 1; y++)
	{
		for (int x = 1; x <= BRD_LEN - convSize + 1; x++)
		{
			str += ("c " + std::to_string(y) + " " + std::to_string(x) + " "); // conv: (y ~ y + convSize - 1) (x ~ x + convSize - 1)

			for (int cy = 0; cy < convSize; cy++)
			{
				for (int cx = 0; cx < convSize; cx++)
				{
					str += (std::to_string(conv[y - 1][x - 1].weight[cy][cx]) + " " +
							std::to_string(conv[y - 1][x - 1].bias  [cy][cx]) + " ");
				}
			}

			str += "\n";
		}
	}

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
	//todo

	return 0;
}