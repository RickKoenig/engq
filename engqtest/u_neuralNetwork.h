// some switches
#define SHOW_WEIGHT_BIAS
#define SHOW_TRAINING_DATA
#define SHOW_TESTING_DATA
#define DO_BRUTE_FORCE_DERIVATIVES
#define SHOW_DERIVATIVES

class neuralNet {
	vector<U32> topo; // the structure of the network

	// a layer of the network, layer 0 is the input layer, no weights or biases on layer 0
	// the topo.size() - 1 layer is the output layer
	struct layer {
		vector<double> A;
		vector<vector<double>> W;
		vector<double> B;
		vector<vector<double>> dCdW_CR;
		vector<double> dCdB_CR;
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		vector<vector<double>> dCdW_BF;
		vector<double> dCdB_BF;
#endif
	};
	// the network
	vector<layer> layers;

	// inputs and outputs
	vector<vector<double>>& inputs;
	vector<vector<double>>& desireds;
	vector<vector<double>> outputs;
	U32 nTrain;
	vector<vector<double>>& inputsTest;
	vector<vector<double>>& desiredsTest;
	vector<vector<double>> outputsTest;
	U32 nTest;
	vector<double> Y;

	double avgCost{};
	double avgCostTest{};

	// for debvars
	vector<menuvar> dbNeuralNet; // debprint menu
	const double epsilon = 1e-8; // for brute force derivatives

	C8* copyStr(const C8* in); // free with delete, all string names in dbNeuralNet are allocated and are to be freed

	// generate a random number in range [0 - 1)
    double frand()
	{
		return rand() / (RAND_MAX + 1.0);
	}

	double sigmoid(double x)
	{
		return 1.0 / (1.0 + exp(-x));
	}

	double runNetwork();

public:
	neuralNet(const vector<U32>& topology
		, vector<vector<double>>& inTrain, vector<vector<double>>& desTrain
		, vector<vector<double>>& inTester, vector<vector<double>>& desTester);
	void gradientDescent(double learn); // gradient descent training
	void testNetwork();
	~neuralNet();
};
