class Level {
public:
	//vector<float> inputs;
	U32 inputSize;
	vector<float> outputs;
	vector<float> biases;
	vector<vector<float>> weights;
	Level(U32 inputCount, U32 outputCount);
	Level(FILE *fr, U32 inputCount, U32 outputCount);
private:
	void randomize();
	void save(FILE* fw);
	//const vector<float> feedForward(const vector<float>& givenInputs);
	void feedForward(const vector<float>& givenInputs);
	friend class NeuralNetwork;
};

class NeuralNetwork {
public:
	vector<Level> levels;

	// from topology
	NeuralNetwork(vector<U32> topology);
	// copy
	NeuralNetwork(const NeuralNetwork& rhs);

	// from a file
	// TODO: remove hard coding, topology should be in the file
	NeuralNetwork(const C8* brainFile);
	void save(const C8* brainFile);
	const vector<float>& feedForward(const vector<float>& givenInputs);
	//void feedForward(const vector<float>& givenInputs);
	void mutate(float amount = 1);
};
