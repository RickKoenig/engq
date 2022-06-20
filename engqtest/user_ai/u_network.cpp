#include <m_eng.h>

#include "u_s_selfdriving.h"
#ifdef DONAMESPACE
namespace selfdriving {
#endif

#include "u_utils.h"
//#include "u_road.h"
//#include "u_controls.h"
//#include "u_sensor.h"
//#include "u_car.h"
#include "u_network.h"

Level::Level(U32 inputCount, U32 outputCount)
	: inputSize(inputCount)
	, outputs(outputCount)
	, biases(outputCount)
	, weights(inputCount)
{
	for (auto i = 0U; i < inputCount; ++i) {
		weights[i] = vector<float>(outputCount);
	}
	randomize();
}

Level::Level(FILE *fr, U32 inputCount, U32 outputCount)
	: inputSize(inputCount)
	, outputs(outputCount)
	, biases(outputCount)
	, weights(inputCount)
{
	for (auto i = 0U; i < inputCount; ++i) {
		weights[i] = vector<float>(outputs.size());
		for (auto j = 0U; j < outputs.size(); ++j) {
			fread(&weights[i][j], sizeof(float), 1, fr);
		}
	}
	for (auto j = 0U; j < biases.size(); ++j) {
		fread(&biases[j], sizeof(float), 1, fr);
	}
}
void Level::randomize()
{
	for (auto i = 0U; i < inputSize; ++i) {
		for (auto j = 0U; j < outputs.size(); ++j) {
			weights[i][j] = mt_frand() * 2 - 1;
		}
	}

	for (auto j = 0U; j < biases.size(); ++j) {
		biases[j] = mt_frand() * 2 - 1;
	}
}

void Level::save(FILE* fw)
{
	for (auto i = 0U; i < inputSize; ++i) {
		for (auto j = 0U; j < outputs.size(); ++j) {
			fwrite(&weights[i][j], sizeof(float), 1, fw);
		}
	}
	for (auto j = 0U; j < biases.size(); ++j) {
		fwrite(&biases[j], sizeof(float), 1, fw);
	}
}

void /*vector<float>*/ Level::feedForward(const vector<float>& givenInputs) {
	for (auto j = 0U; j < outputs.size(); ++j) {
		float sum = 0;
		for (auto i = 0U; i < inputSize; ++i) {
			sum += givenInputs[i] * weights[i][j];
		}
		if (sum > biases[j]) {
			outputs[j] = 1;
		} else {
			outputs[j] = 0;
		}
	}
	//return outputs;
}
// from topology
NeuralNetwork::NeuralNetwork(vector<U32> topology)
{
	for (auto k = 0U; k < topology.size() - 1; ++k) {
		levels.push_back(Level(topology[k], topology[k + 1]));
	}
}

// copy
NeuralNetwork::NeuralNetwork(const NeuralNetwork& rhs)
{
	for (auto k = 0U; k < rhs.levels.size(); ++k) {
		levels.push_back(rhs.levels[k]);
	}
}

// from a file
// TODO: remove hard coding, topology should be in the file
NeuralNetwork::NeuralNetwork(const C8* brainFile)
{
	vector<U32> topology{ 5, 6, 4 };
	NeuralNetwork fileBrain(topology);
	FILE* fr = fopen2(brainFile, "rb");
	*this = fileBrain;
	for (auto k = 0U; k < topology.size() - 1; ++k) {
		levels[k] = Level(fr, topology[k], topology[k + 1]);
	}
	fclose(fr);
}

void NeuralNetwork::save(const C8* brainFile)
{
	FILE* fw = fopen2(brainFile, "wb");
	for (auto k = 0U; k < levels.size(); ++k) {
		levels[k].save(fw);
	}
	fclose(fw);
}

const vector<float>& NeuralNetwork::feedForward(const vector<float>& givenInputs) {
//void NeuralNetwork::feedForward(const vector<float>& givenInputs) {
	//vector<float> outputs = levels[0].feedForward(givenInputs);
	levels[0].feedForward(givenInputs);
	for (auto k = 1U; k < levels.size(); ++k) {
		//outputs = levels[k].feedForward(outputs);
		levels[k].feedForward(levels[k - 1].outputs);
	}
	return levels[levels.size() - 1].outputs;
}

void NeuralNetwork::mutate(float amount) {
	for (auto& level : levels) {
		for (auto j = 0U; j < level.biases.size(); ++j) {
			level.biases[j] = lerp(
				level.biases[j],
				mt_frand() * 2 - 1,
				amount
			);
		}
		for (auto i = 0U; i < level.inputSize; ++i) {
			for (auto j = 0U; j < level.outputs.size(); ++j) {
				level.weights[i][j] = lerp(
					level.weights[i][j],
					mt_frand() * 2 - 1,
					amount
				);
			}
		}
	}
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
