// some switches
#if 0
#define SHOW_WEIGHT_BIAS
#define SHOW_TRAINING_DATA
#define SHOW_TESTING_DATA
#define DO_BRUTE_FORCE_DERIVATIVES
#define SHOW_DERIVATIVES
#endif

#define USE_TANH_HIDDEN // tanh is to be used in hidden layers instead of sigmoid

// try some adjustment stuff
// try to kill saturated neurons, used for both sigmoid and tanh
#define EXTRA_SLOPE
#ifdef EXTRA_SLOPE
#define EXTRA_SLOPE_AMOUNT .01
#endif

class neuralNet {
public:
	enum class costCorr {
		NONE,
		THRESHHOLD, // both < LO_THRESH or >= HI_THRESH
		DIGITS, // max matches desired digit
	};
private:
	// begin for correct guesses
	const double LO = .1; // TODO: many copies of LO and HI, should be just one
	const double HI = .9;
	const double THR = .5; // how close to go to LO and HI from middle
	const double AVG = (LO + HI) * .5;
	const double SPREAD = (HI - LO) * .5 * THR;
	const double LO_THRESH = AVG - SPREAD;
	const double HI_THRESH = AVG + SPREAD;
	// end for correct guesses
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
#ifdef DO_BRUTE_FORCE_DERIVATIVES
	const double epsilon = 1e-8;
#endif
	//temporaries, for optimization
	vector<double> DcostDZ;// (Ls); // same as DcostDBL, used for backtrace
	vector<double> DcostDA;// (Ls);

	// the network
	vector<layer> layers;

	// inputs and outputs
	vector<vector<double>>& inputsTrain;
	vector<vector<double>>& desiredsTrain;
	vector<vector<double>> outputsTrain;
	U32 nTrain; // maybe 100 or so, currently in use
	U32 nTotalTrain; // the full 60,000

// not used in training, but used for debprint menu and calculating costs
	vector<vector<double>>& inputsTest;
	vector<vector<double>>& desiredsTest;
	vector<vector<double>> outputsTest;
	U32 nTest;

	// cost of train and test, not used for training
	costCorr doCorrect{ costCorr::NONE};

	double totalCostTrain{};
	double avgCostTrain{};
	double minCostTrain{};
	double maxCostTrain{};
	U32 correctTrain{34}; // if doCorrect != NONE // TODO:: remove 34

	double totalCostTest{};
	double avgCostTest{};
	double minCostTest{};
	double maxCostTest{};
	U32 correctTest{56}; // if doCorrect != NONE // TODO:: remove 56

	// for debvars
	vector<menuvar> dbNeuralNet; // debprint menu for neuralNet

	// functions

	C8* copyStr(const C8* in); // make a copy of cstr, free with delete, all string names in dbNeuralNet are allocated and are to be freed

	// generate a random number in range [0 - 1)
	double frand()
	{
		return rand() / (RAND_MAX + 1.0);
	}

public:
	// constructor
	// tester is just for cost and display
	neuralNet(const string& name, const vector<U32>& topology
		, vector<vector<double>>& inTrain, vector<vector<double>>& desTrain, U32 trainLimit
		, vector<vector<double>>& inTester, vector<vector<double>>& desTester
		, costCorr corrCost);

	// run the network
	void runNetwork(const vector<double>& in, vector<double>& out);

	// next step in gradientDescent
	void gradientDescent(double learn); // gradient descent training

	// load and save weights and bias
	bool loadNetwork(U32 slot); // true if loaded, false if not loaded
	void saveNetwork(U32 slot);

	// getters for train data and test data
	vector<double>& getOneTrainOutput(U32 idx);
	vector<double>& getOneTrainDesired(U32 idx);
	vector<double>& getOneTestOutput(U32 idx);
	vector<double>& getOneTestDesired(U32 idx);

	// good for user calculations and brute force derivatives, fast
	static double calcOneCost(const vector<double>& des, const vector<double>& out);
	// array of costs, includes avg, total, min, max, and optional correct costs
	void calcCostArr(const vector<vector<double>>& inArr
		, const vector<vector<double>>& desArr
		, vector<vector<double>>& outArr
		, U32 nSteps
		, costCorr costCorrKind
		, double& totalCost, double& avgCost, double& minCost, double& maxCost, U32& correct);
	// update train and test outputs and costs
	void calcCostTrainAndTest();


	// clean up
	~neuralNet();

	// activation functions and their derivatives
	static double sigmoid(double in);
	static double delSigmoid(double in);
	static double tangentH(double in);
	static double delTangentH(double in);
};
