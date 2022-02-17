// some switches
//#define SHOW_WEIGHT_BIAS
//#define SHOW_TRAINING_DATA
//#define SHOW_TESTING_DATA
//#define DO_BRUTE_FORCE_DERIVATIVES
//#define SHOW_DERIVATIVES

// try some stuff
//#define CLAMP_SIGMOID
#ifdef CLAMP_SIGMOID
#define CLAMP_AMOUNT 25.0
#endif

class neuralNet {
	string name;
	vector<U32> topo; // the structure of the network

	// a layer of the network, layer 0 is the input layer, no weights or biases on layer 0
	// the topo.size() - 1 layer is the output layer
	struct layer {
		vector<double> A; // layer 0 and layer nL - 1 don't have an A, instead it's input and output
		vector<double> Z; // layer 0 doesn't have a Z
		vector<vector<double>> W;
		vector<double> B;
		vector<vector<double>> dCdW_CR;
		vector<double> dCdB_CR;
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		vector<vector<double>> dCdW_BF;
		vector<double> dCdB_BF;
#endif
	};
	//temporaries, for optimization
	vector<double> DcostDZ;// (Ls); // same as DcostDBL, used for backtrace
	vector<double> DcostDA;// (Ls);

	// the network
	vector<layer> layers;

	// inputs and outputs
	vector<vector<double>>& inputsTrain;
	vector<vector<double>>& desiredsTrain;
	vector<vector<double>> outputsTrain;
	U32 nTrain;
	vector<vector<double>>& inputsTest;
	vector<vector<double>>& desiredsTest;
	vector<vector<double>> outputsTest;
	U32 nTest;

	double totalCost{};
	double totalCostTest{};
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
#if 0
	double sigmoid(double x)
	{
		return 1.0 / (1.0 + exp(-x));
	}
#endif

public:
	neuralNet(const string& name, const vector<U32>& topology
		, vector<vector<double>>& inTrain, vector<vector<double>>& desTrain
		, vector<vector<double>>& inTester, vector<vector<double>>& desTester);
	double runNetwork(const vector<double>& in, const vector<double>& des, vector<double>& out);
	void gradientDescent(double learn); // gradient descent training
	void testNetwork();
	bool loadNetwork(U32 slot); // true if loaded, false if not loaded
	void saveNetwork(U32 slot);
	vector<double>& getOneTrainOutput(U32 idx);
	vector<double>& getOneTestOutput(U32 idx);
	vector<double>& getOneTrainDesired(U32 idx);
	vector<double>& getOneTestDesired(U32 idx);
	~neuralNet();
	static double sigmoid(double in);
	static double delSigmoid(double in);
	static double delSigmoid2(double in);
};
