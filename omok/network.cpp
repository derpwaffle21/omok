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

std::vector<double> Network::evaluate(const std::vector<std::vector<int>>& board, int moveNum)
{
	//1. Conv Layer
	constexpr int output_size = BRD_LEN - convFilterSize + 1;
	std::vector<std::vector<double>> conv_output(output_size, std::vector<double>(output_size));
	for (int loc_y = 0; loc_y < output_size; loc_y++) 
	{
		for (int loc_x = 0; loc_x < output_size; loc_x++) 
		{
			Convolutional current_conv = conv[loc_y][loc_x];
			double temp = 0; //value of current output
			for (int conv_y = 0; conv_y < convFilterSize; conv_y++) 
			{
				for (int conv_x = 0; conv_x < convFilterSize; conv_x++)
				{
					int board_y = conv_y + loc_y;
					int board_x = conv_x + loc_x;
					int board_value = board[conv_y][conv_x];
					int multiple_value; // change BLACK/WHITE/EMPTY to -1/1/0
					switch (board_value) 
					{
					case BLACK:
						multiple_value = -1;
						break;
					case WHITE:
						multiple_value = 1;
						break;
					case EMPTY:
						multiple_value = 0;
						break;
					default:
						errorMessage("Unknown Board Value Detected");
					}				
					temp += (current_conv.weight[conv_y][conv_x] * multiple_value
						   + current_conv.bias[conv_y][conv_x]);

				}
			}
			conv_output[loc_y][loc_x] = temp;
		}
	}
	//2. Dense Layer(s)
	bool saving_index = 1;
	std::vector<double> hiddens[2];
	hiddens[0].resize(output_size * output_size + 1);
	int idx = 0;
	//comment : 이 부분 살짝 비효율적임(hiddens를 위로 보내고 이 코드를 없애도 됨)
	for (int hidden_y = 0; hidden_y < conv_output.size(); hidden_y++) 
	{
		for (int hidden_x = 0; hidden_x < conv_output.size(); hidden_x++) 
		{
			hiddens[0][idx] = conv_output[hidden_y][hidden_x];
			idx++;
		}
	}
	hiddens[0][idx] = moveNum;
	for (int layer = 0; layer < denseNum; layer++) 
	{
		Dense current_layer = dense[layer];
		hiddens[saving_index].resize(current_layer.outputSize, 0);
		//Error detecting code
		if (current_layer.inputSize != hiddens[!saving_index].size()) 
		{
			errorMessage("Size of Dense Layer is Invalid");
		}
		for (int outputidx = 0; outputidx < current_layer.outputSize; outputidx++) 
		{
			for (int inputidx = 0; inputidx < current_layer.inputSize; inputidx++) 
			{
				hiddens[saving_index][outputidx] +=
					hiddens[!saving_index][inputidx] * current_layer.weight[inputidx][outputidx]
				  + current_layer.bias[inputidx][outputidx];
			}
		}
		saving_index = !saving_index;
	}
	//Last saved index = !saving_index
	return Softmax(hiddens[!saving_index]);
}