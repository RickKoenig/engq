class Level {
	vector<float> inputs;
	vector<float> outputs;
	vector<float> biases;
	vector<vector<float>> weights;
public:
	Level(U32 inputCount, U32 outputCount)
		: inputs (inputCount)
		, outputs (outputCount)
		, biases (outputCount)
		, weights (inputCount)
	{
		for (auto i = 0U; i < inputCount; ++i) {
			weights[i] = vector<float>(outputCount);
		}
		randomize();
	}
private:
	void randomize()
	{
		for (auto i = 0U; i < inputs.size(); ++i) {
			for (auto j = 0U; j < outputs.size(); ++j) {
				weights[i][j] = mt_frand() * 2 - 1;
			}
		}

		for (auto i = 0U; i < biases.size(); ++i) {
			biases[i] = mt_frand() * 2 - 1;
		}
	}

	const vector<float>& feedForward(const vector<float>& givenInputs) {
		for (auto i = 0U; i < inputs.size(); ++i) {
			inputs[i] = givenInputs[i];
		}
		for (auto i = 0U; i < outputs.size(); ++i) {
			float sum = 0;
			for (auto j = 0U; j < inputs.size(); ++j) {
				sum += inputs[j] * weights[j][i];
			}
			if (sum > biases[i]) {
				outputs[i] = 1;
			} else {
				outputs[i] = 0;
			}
		}
		return outputs;
	}
	friend class NeuralNetwork;
};

class NeuralNetwork {
public:
	vector<Level> levels;
	NeuralNetwork(vector<U32> neuronCounts)
	{
		for (auto i = 0U; i < neuronCounts.size() - 1; ++i) {
			levels.push_back(Level(neuronCounts[i], neuronCounts[i + 1]));
		}
	}

	vector<float> feedForward(const vector<float>& givenInputs) {
		vector<float> outputs = levels[0].feedForward(givenInputs);
		for (auto i = 1U; i < levels.size(); ++i) {
			outputs = levels[i].feedForward(outputs);
		}
		return outputs;
	}
	void mutate(float amount = 1) {
		for (auto& level : levels) {
			for (auto i = 0U; i < level.biases.size(); ++i) {
				level.biases[i] = lerp(
					level.biases[i],
					mt_frand() * 2 - 1,
					amount
				);
			}
			for (auto i = 0U; i < level.weights.size(); ++i) {
				for (auto j = 0U; j < level.weights[i].size(); ++j) {
					level.weights[i][j] = lerp(
						level.weights[i][j],
						mt_frand() * 2 - 1,
						amount
					);
				}
			}
		}
	}
};
