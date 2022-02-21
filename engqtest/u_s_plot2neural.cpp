// plotter2 runs in FPU control high precision, so you can use doubles correctly if necessary
// interactively test neural networks

#include <m_eng.h>
#include "u_plotter2.h"
#include "u_neuralNetwork.h"
#include "m_perf.h"

#if 1
#include <array>        // std::array
#include <chrono>       // std::chrono::system_clock
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#endif

using namespace u_plotter2;
// Keyboard shortcuts, 'r' to reset/randomize weights and biases

// lots of switches
// random only
//#define USE_TIMEB // randomize based on time

// sub switches, what to run in this state
//#define DO_NEURAL1 // Layers 1 (1 - 1), 2 vars, 2 costs, test 1 more, total 3
//#define DO_NEURAL2 // Layers 2 (1 - 1 - 1), 4 vars, 2 costs, test 1 more, total 3
//#define DO_NEURAL3 // Layers 2 (2 - 2 - 2), 12 vars, 4 costs, test 1 more, total 5
//#define DO_NEURAL4 // Layers 3 (3 - 2 - 3 - 2), 25 vars, 4 costs, test 2 more, total 6
//#define DO_NEURAL5 // Layers 3 (6 - 6 - 4), 70 vars, 60 costs, test 4 more costs for a total of 64 costs
#define DO_NEURAL6 // Layers 3 (784 - 16 - 16 - 10), 13,002 vars, 60,000 costs, test 10,000 more costs total 70,000
#define DO_GRAD_TEST // 1 var, minimize the adjustable quartic equation
//#define SHOW_SIGMOID
//#define SHOW_DESIREDS_OVER_OUTPUT // for DO_NEURAL6
#define DO_NORMALIZE // all data is normalized to mean = 0, standard deviation = 1

#ifdef USE_TIMEB
#include <sys/timeb.h>
#endif

namespace neuralPlot {
	// common data
	bool busy;
#ifdef SHOW_DESIREDS_OVER_OUTPUT
	const bool showDesireds = true;
#else
	const bool showDesireds = false;
#endif
	S32 runinbackgroundSave;
	const U32 uiCounter = 30; // for load and save models
// these are not on time
	U32 randomSeed = 456;
	U32 randomNext = 1;
	// for load and save weights and biases
	U32 loadSaveSlot = 0;
	// counters
	U32 loading = 0;
	U32 noLoad = 0;
	U32 yesLoad = 0;
	U32 saving = 0;
	// calc gradient descent
	double learn = 0.0; // .03125; // how fast to learn, too small too slow, too large too unstable
	S32 calcAmount = -1; // how many calcs to do, negative run forever, positive decrements every frame until 0 
	S32 calcSpeed = 1; // number of calculations per frame
	S32 runTestCount = 32;// 20; // how many frames to wait to run test and user
	S32 runTest = 0;

	const double LO = .1;
	const double HI = .9;

#ifdef SHOW_SIGMOID
	double sigmoidIn = 0.0;
	double sigmoidOut;
	double sigmoidOutPrime;
	double tanhOut;
	double tanhOutPrime;
#endif
#ifdef DO_GRAD_TEST
	// 4th order polynomial
	double Acoeff, Bcoeff, Ccoeff, Dcoeff, Ecoeff;
	double xVar, yVar;
	double dydx;
#endif
#ifdef DO_NEURAL1
	const string neuralName{ "Neural1" };
	vector<U32> aTesterTopology{ 1, 1 };
	// train
	// inputs, desires
	vector<vector<double>> inputTrain = {
		{.3},
		{.4},
	};
	vector<vector<double>> desiredTrain = {
		{.8},
		{.5},
	}; // simple linear
	// test
	// inputs, desires
	vector<vector<double>> inputTest = {
		{.35},
	};
	vector<vector<double>> desiredTest = {
		{.65},
	};
#endif
#ifdef DO_NEURAL2
	const string neuralName{ "Neural2" };
	vector<U32> aTesterTopology{ 1, 1, 1 };
	// train
	// inputs, desires
	vector<vector<double>> inputTrain = {
		{.3},
		{.4},
	};
	vector<vector<double>> desiredTrain = {
		{.8},
		{.5},
	}; // simple linear
	// test
	// inputs, desires
	vector<vector<double>> inputTest = {
		{.35},
	};
	vector<vector<double>> desiredTest = {
		{.65},
	};

#endif
#ifdef DO_NEURAL3
	const string neuralName{ "Neural3" };
	vector<U32> aTesterTopology{ 2, 2, 2 };
	// train
	// inputs, desires
	vector<vector<double>> inputTrain = {
		{.12, .16},
		{.13, .97},
		{.94, .18},
		{.95, .99}
	};
	vector<vector<double>> desiredTrain = {
		{.11, .12},
		{.93, .14},
		{.95, .16},
		{.97, .98}
	}; // for now try or and and gates
	// test
	// inputs, desires
	vector<vector<double>> inputTest = {
		{.1, .3},
		{.4, .7},
	};
	vector<vector<double>> desiredTest = {
		{.2, .21},
		{.93, .94},
	}; // for now try or and and gates
#endif
#ifdef DO_NEURAL4
	const string neuralName{ "Neural4" };
	vector<U32> aTesterTopology{ 3, 2, 3, 2 };
	// train
	// inputs, desires
	vector<vector<double>> inputTrain = {
		{.12, .16, .26},
		{.13, .97, .45},
		{.94, .18, .36},
		{.95, .54, .79}
	};
	vector<vector<double>> desiredTrain = {
		{.11, .12},
		{.93, .14},
		{.95, .16},
		{.97, .48}
	}; // for now try or and and gates
	// test
	// inputs, desires
	vector<vector<double>> inputTest = {
		{.123, .167, .265},
		{.134, .978, .456},
	};
	vector<vector<double>> desiredTest = {
		{.111, .123},
		{.933, .144},
	}; // for now try or and and gates
#endif
#ifdef DO_NEURAL5 // add 2 3 digit binary numbers to make 1 4 digit binary number
	const string neuralName{ "Neural5" };
	vector<U32> aTesterTopology{ 6, 6, 4 };
	// train
	// inputs, desires
	vector<vector<double>> inputTrain;
	vector<vector<double>> desiredTrain;
	// for now try or and and gates
	// test
	// inputs, desires
	vector<vector<double>> inputTest;
	vector<vector<double>> desiredTest;

	void nerual5init()
	{
		// add 2 3bit binary numbers to get a 4bit binary number
		const U32 doTrain = 60;
		const U32 doTest = 4;
		const U32 doTotal = doTrain + doTest;
		// input size 6
		// output size 4
		inputTrain.clear();
		desiredTrain.clear();
		inputTest.clear();
		desiredTest.clear();
		for (U32 k = 0; k < doTotal; ++k) {
			vector<double>in(aTesterTopology[0]);
			vector<double>des(aTesterTopology[aTesterTopology.size() - 1]);
			// convert k to binary 6bit
			U32 val = k;
			for (U32 i = 0; i < 6; ++i) {
				in[i] = LO + (HI - LO) * (val & 1);
				val >>= 1;
			}
			U32 add = (k & 7) + (k >> 3);
			val = add;
			for (U32 i = 0; i < 4; ++i) {
				des[i] = LO + (HI - LO) * (val & 1);
				val >>= 1;
			}
			if (k < doTrain) {
				inputTrain.push_back(in);
				desiredTrain.push_back(des);
			} else {
				inputTest.push_back(in);
				desiredTest.push_back(des);
			}
		}
	}
#endif

#ifdef DO_NEURAL6 // handwritten digit recognition 28 by 28 grid
	class idxFile {
		// input
		// for display
		vector<vector<vector<U8>>> rawInput; // 3D
		// for network
		vector<vector<double>> input; // 2D

		// desired
		// for display
		vector<U8> rawDesired; // 1D
		// for network
		vector<vector<double>> desired; // 2D
	public:
		idxFile(const C8* fNameInput, const C8* fNameDesired, U32 limit = 0);
		U32 getNumData();
		// for network
		vector<vector<double>>* getInput();
		vector<vector<double>>* getDesired();
		// for display
		U32 getOneDesired(U32 idx);
		vector<vector<U8>>* getOneImage(U32 idx);
	};

	void normalize(vector<double>& anInput, double& mean, double& stdDev)
	{
		mean = 0.0;
		U32 N = anInput.size();
		for (auto val : anInput) {
			mean += val;
		}
		mean /= N;
		double variance = 0.0;
		for (auto val : anInput) {
			double diff = val - mean;
			variance += diff * diff;
		}
		if (variance < EPSILON) {
			logger("no variance!\n");
			stdDev = 0.0;
			return; // no variance, don't divide by zero
		}
		variance /= N;
		stdDev = sqrt(variance);
		for (auto& v : anInput) {
			v = (v - mean) / stdDev;
		}
	}

	idxFile::idxFile(const C8* fNameInput, const C8* fNameDesired, U32 limit)
	{
		perf_start(READ_IDX1);
		pushandsetdir("neural");

		// input data
		FILE* fh = fopen2(fNameInput, "rb");
		if (!fh) {
			perf_end(READ_IDX1);
			return;
		}
		U32 magic = filereadU32BE(fh); // big endian file
		const U32 goodMagic3{ 0x803 }; // unsigned byte, 3 dimensions
		if (magic != goodMagic3) {
			logger("bad magic %08x, should be %08x\n", magic, goodMagic3);
			fclose(fh);
			perf_end(READ_IDX1);
			return;
		}
		logger("file = %s, magic = %08x\n", fNameInput, magic);
		U32 dataSize3 = filereadU32BE(fh); // how many bitmaps are there
		if (limit && dataSize3 > limit) {
			dataSize3 = limit;
		}
		U32 height = filereadU32BE(fh);
		U32 width = filereadU32BE(fh);
		U32 prod = width * height;

		string doubleName = string() + fNameInput + ".doubleRaw.bin";
		bool readRawDoubles = false;
		if (fileexist(doubleName.c_str())) {
			readRawDoubles = true;
		}
		readRawDoubles = false;

		FILE* fhd;
		if (readRawDoubles) { // cache normalized double data
			fhd = fopen2(doubleName.c_str(), "rb"); // read cached double data
		} else {
			fhd = fopen2(doubleName.c_str(), "wb"); // else calc and save them doubles
		}


		for (U32 k = 0; k < dataSize3; ++k) {
			vector<vector<U8>> abm(height, vector<U8>(width));
			for (U32 j = 0; j < height; ++j) {
				vector<U8>& arow = abm[j];
				fileread(fh, &arow[0], width);
			}
			rawInput.push_back(abm); // populate rawInput

									 
			// data for neuralNet
			// input image
			vector<double> anInput = vector<double>(prod);
			if (readRawDoubles) {
				fileread(fhd, &anInput[0], anInput.size() * sizeof anInput[0]);
			} else {
				double* dest = &anInput[0];
				for (U32 j = 0; j < height; ++j) {
					//vector<U8>& aRow = rawInput[k][j];
					vector<U8>& aRow = abm[j];
					for (U32 i = 0; i < width; ++i) {
						// 0 to 255 ==> LO to HI
						*dest++ = LO + (HI - LO) * aRow[i] / 255.0;
					}
				}
#ifdef DO_NORMALIZE
				// normalize data 0 mean, 1 standard deviation
				double mean;
				double stdDev;
				normalize(anInput, mean, stdDev);
				//logger("normalizing file data 1, mean = %f, stdDev = %f\n", mean, stdDev);
				//normalize(anInput, mean, stdDev); // check that mean = 0 and stdDev = 1
				//logger("normalizing file data 2, mean = %f, stdDev = %f\n", mean, stdDev);
#endif
				//filewrite(fhd, &anInput[0], anInput.size() * sizeof anInput[0]);
			}
			input.push_back(anInput); // populate input
		}

		fclose(fh);
		fclose(fhd);

		// desired data
		fh = fopen2(fNameDesired, "rb");
		if (!fh) {
			perf_end(READ_IDX1);
			return;
		}
		magic = filereadU32BE(fh); // big endian file
		const U32 goodMagic1{ 0x801 }; // unsigned byte, 1 dimension
		if (magic != goodMagic1) {
			logger("bad magic %08x, should be %08x\n", magic, goodMagic1);
			fclose(fh);
			perf_end(READ_IDX1);
			return;
		}
		logger("file = %s, magic = %08x\n", fNameDesired, magic);
		U32 dataSize1 = filereadU32BE(fh);
		if (limit && dataSize1 > limit) {
			dataSize1 = limit;
		}
		if (dataSize1 != dataSize3) {
			logger("mismatched dataSize!");
			perf_end(READ_IDX1);
			return;
		}
		rawDesired.resize(dataSize1);
		fileread(fh, &rawDesired[0], dataSize1); // populate rawDesired
		fclose(fh);
		popdir();
		for (U32 k = 0; k < dataSize1; ++k) {
				// desired output
			vector<double> aDesired = vector<double>(10); // number of digits
			for (U32 j = 0; j < 10; ++j) {
				aDesired[j] = j == rawDesired[k] ? HI : LO;
			}
			desired.push_back(aDesired); // populate desired
		}
		perf_end(READ_IDX1);
	}

	U32 idxFile::getNumData()
	{
		return rawDesired.size();
	}

	vector<vector<double>>* idxFile::getInput()
	{
		return &input;
	}

	vector<vector<double>>* idxFile::getDesired()
	{
		return &desired;
	}

	U32 idxFile::getOneDesired(U32 idx)
	{
		return rawDesired[idx];
	}

	vector<vector<U8>>* idxFile::getOneImage(U32 idx)
	{
		return &rawInput[idx];
	}

	idxFile* idxFileTrain;
	idxFile* idxFileTest;
	S32 idxTrain;
	S32 idxTest;
	const string neuralName{ "Neural6" };
	vector<U32> aTesterTopology{ 784, 16, 16, 10 }; // a big one!
	// train
	// inputs, desires
	vector<vector<double>>* inputTrain; // get these from idxFile
	vector<vector<double>>* desiredTrain;
	// test
	// inputs, desires
	vector<vector<double>>* inputTest;
	vector<vector<double>>* desiredTest;

	bitmap32* userBM; // hand draw numbers here
	bitmap32* trainBM;
	bitmap32* testBM;

	U32 MBUTuserBM;

	// helper
	bitmap32* makeBMfromImage(const vector<vector<U8>>& img)
	{
		U32 wid = img[0].size();
		U32 hit = img.size();
		bitmap32* ret = bitmap32alloc(wid, hit);
		U32 i, j;
		C32* data = ret->data;
		for (j = 0; j < hit; ++j) {
			const vector<U8>& row = img[j];
			for (i = 0; i < wid; ++i) {
				U8 rawVal = row[i];
				C32 c32Val = C32(rawVal, rawVal, rawVal);
				*data++ = c32Val;
			}
		}
		return ret;
	}

	bitmap32* scale8(bitmap32* in)
	{
		bitmap32* bm2 = bitmap32double(in);
		bitmap32* bm4 = bitmap32double(bm2);
		bitmap32* bm8 = bitmap32double(bm4);
		bitmap32free(bm2);
		bitmap32free(bm4);
		return bm8;
	}

	void nerual6init()
	{
		idxTrain = 0;
		idxTest = 0;
		// read MNIST data
#if 1
		const U32 trainSize = 100;
		const U32 testSize = 2;
#elif 0
		const U32 trainSize = 100;
		const U32 testSize = 2;
#else
		const U32 trainSize = 0;
		const U32 testSize = 0;
#endif
		idxFileTrain = new idxFile("train-images.idx3-ubyte.bin", "train-labels.idx1-ubyte.bin", trainSize);
		idxFileTest = new idxFile("t10k-images.idx3-ubyte.bin", "t10k-labels.idx1-ubyte.bin", testSize);
		// reference from idxFile to args for new neuralNet
		inputTrain = idxFileTrain->getInput();
		desiredTrain = idxFileTrain->getDesired();
		inputTest = idxFileTest->getInput();
		desiredTest = idxFileTest->getDesired();
		// create a user hand drawing bitmap
		vector<vector<U8>>* anImage = idxFileTrain->getOneImage(0);
		userBM = bitmap32alloc(anImage[0].size(), anImage->size(), C32BLACK);
		trainBM = bitmap32alloc(anImage[0].size(), anImage->size(), C32CYAN);
		testBM = bitmap32alloc(anImage[0].size(), anImage->size(), C32CYAN);
	}
#endif

	// one user set to run on runNetwork
	vector<double> userInputs(aTesterTopology[0]);
	vector<double> userDesireds(aTesterTopology[aTesterTopology.size() - 1]);
	vector<double> userOutputs(aTesterTopology[aTesterTopology.size() - 1]);
	double userCost = 0;

	// for debvars
	struct menuvar plot2neuralDeb[] = {
#ifdef DO_GRAD_TEST
		{"@lightblue@--- gradient descent test ---", NULL, D_VOID, 0},
		{"Ax^4 +", &Acoeff, D_DOUBLE, FLOATUP / 32},
		{"Bx^3 +", &Bcoeff, D_DOUBLE, FLOATUP / 32},
		{"Cx^2 +", &Ccoeff, D_DOUBLE, FLOATUP / 32},
		{"Dx +", &Dcoeff, D_DOUBLE, FLOATUP / 32},
		{"E", &Ecoeff, D_DOUBLE, FLOATUP / 32},
		{"X", &xVar, D_DOUBLE, FLOATUP / 32},
		{"Y", &yVar, D_DOUBLE | D_RDONLY},
		{"dY/dX", &dydx, D_DOUBLE | D_RDONLY},
#endif
#ifdef SHOW_SIGMOID
		{"@brown@--- sigmoid graph ---", NULL, D_VOID, 0},
		{"activation in", &sigmoidIn, D_DOUBLE, FLOATUP / 8},
		{"sigmoid out", &sigmoidOut, D_DOUBLEEXP | D_RDONLY},
		{"sigmoid out prime", &sigmoidOutPrime, D_DOUBLEEXP | D_RDONLY},
		{"tanh out", &tanhOut, D_DOUBLEEXP | D_RDONLY},
		{"tanh out prime", &tanhOutPrime, D_DOUBLEEXP | D_RDONLY},
#endif
		{"@yellow@--- neural network vars ---", NULL, D_VOID, 0},
		{"calcAmount", &calcAmount, D_INT, 1},
		{"calcSpeed", &calcSpeed, D_INT, 32},
		{"learn", &learn, D_DOUBLE, FLOATUP / 32},
		{"runTestcount", &runTestCount, D_INT, 32},
		// for now, hand code these
		{"@lightcyan@--- neural network user ---", NULL, D_VOID, 0},
#ifdef DO_NEURAL1
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL2
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL3
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userInput1", &userInputs[1], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL4
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userInput1", &userInputs[1], D_DOUBLE, FLOATUP / 4},
		{"userInput2", &userInputs[2], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL5
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userInput1", &userInputs[1], D_DOUBLE, FLOATUP / 4},
		{"userInput2", &userInputs[2], D_DOUBLE, FLOATUP / 4},
		{"userInput3", &userInputs[3], D_DOUBLE, FLOATUP / 4},
		{"userInput4", &userInputs[4], D_DOUBLE, FLOATUP / 4},
		{"userInput5", &userInputs[5], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
		{"userDesired2", &userDesireds[2], D_DOUBLE, FLOATUP / 4},
		{"userOutput2", &userOutputs[2], D_DOUBLE | D_RDONLY},
		{"userDesired3", &userDesireds[3], D_DOUBLE, FLOATUP / 4},
		{"userOutput3", &userOutputs[3], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL6
		// userInput is a 28 by 28 grid 784, too big for here, do it graphically instead
		{"@lightmagenta@idxTrain", &idxTrain, D_INT, 1},
		{"idxTest", &idxTest, D_INT, 1},
		{"@lightcyan@userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
		{"userDesired2", &userDesireds[2], D_DOUBLE, FLOATUP / 4},
		{"userOutput2", &userOutputs[2], D_DOUBLE | D_RDONLY},
		{"userDesired3", &userDesireds[3], D_DOUBLE, FLOATUP / 4},
		{"userOutput3", &userOutputs[3], D_DOUBLE | D_RDONLY},
		{"userDesired4", &userDesireds[4], D_DOUBLE, FLOATUP / 4},
		{"userOutput4", &userOutputs[4], D_DOUBLE | D_RDONLY},
		{"userDesired5", &userDesireds[5], D_DOUBLE, FLOATUP / 4},
		{"userOutput5", &userOutputs[5], D_DOUBLE | D_RDONLY},
		{"userDesired6", &userDesireds[6], D_DOUBLE, FLOATUP / 4},
		{"userOutput6", &userOutputs[6], D_DOUBLE | D_RDONLY},
		{"userDesired7", &userDesireds[7], D_DOUBLE, FLOATUP / 4},
		{"userOutput7", &userOutputs[7], D_DOUBLE | D_RDONLY},
		{"userDesired8", &userDesireds[8], D_DOUBLE, FLOATUP / 4},
		{"userOutput8", &userOutputs[8], D_DOUBLE | D_RDONLY},
		{"userDesired9", &userDesireds[9], D_DOUBLE, FLOATUP / 4},
		{"userOutput9", &userOutputs[9], D_DOUBLE | D_RDONLY},
#endif
		{"userCost", &userCost, D_DOUBLEEXP | D_RDONLY},
	};
	const int nplot2neuralDeb = NUMELEMENTS(plot2neuralDeb);

	// random stuff
	// time based or seed based
	void randomInit()
	{
		srand(randomSeed);
		// and mix it up
		rand();
		rand();
		rand();
	}

	void randomNextSeed()
	{
#ifdef USE_TIMEB
		struct timeb t;
		ftime(&t);
		randomSeed = static_cast<unsigned int>(t.millitm);
#else
		randomSeed += randomNext;
#endif
	}
	// end random stuff

	neuralNet* aNeuralNet;


#ifdef DO_GRAD_TEST
	void gradTestInit()
	{
		Acoeff = .03125;
		Bcoeff = .09375;
		Ccoeff = -.21875;
		Dcoeff = 0.0;
		Ecoeff = 0.0;
		xVar = 0.0;
	}

	double polyFunction(double x)
	{
		// y = Ax^4 + Bx^3 + Cx^2 + Dx + E
		return x * (x * (x * (x * Acoeff + Bcoeff) + Ccoeff) + Dcoeff) + Ecoeff;
	}

	double polyFunctionPrime(double x)
	{
		// y = 4Ax^3 + 3Bx^2 + 2Cx + D
		// y = x(4Ax2 + 3Bx + 2C) + D
		// y = x(x(4Ax + 3B) + 2C) + D
		return x * (x * (4.0 * Acoeff * x + 3.0 * Bcoeff) + 2.0 * Ccoeff) + Dcoeff;
	}

	void gradientDescentPoly()
	{
		double minYValue = -10.0;
		double maxYValue = 20.0;
		double minLearn = .001;
		double goSlower = .9; // oscillating when too high, learn slower
		double slope = polyFunctionPrime(xVar);
		double delX = -learn * slope;
		double xVarNew = xVar + delX;
		double yVarNew = polyFunction(xVarNew);
		if (yVarNew < minYValue) {
			return; // extreme negative, don't go any lower, don't do step
		}
		if (yVarNew > maxYValue) {
			if (learn > minLearn) {
				learn *= goSlower; // extreme, don't go any higher, probably learning too fast
			}
			return; // don't do step, set learn slower
		}
		xVar = xVarNew;
		yVar = yVarNew;
		yVar = polyFunction(xVar);
	}
#endif

#ifdef DO_NEURAL6
	void getUserInputsFromBM(const bitmap32* userBM, vector<double>& userInput)
	{
		U32 outIdx = 0;
		U32 prod = userBM->size.x * userBM->size.y;
		const C32* bmData = userBM->data;
		for (U32 i = 0; i < prod; ++i) {
			userInput[i] = LO + (HI - LO) * bmData[i].g / 255.0;
		}
#ifdef DO_NORMALIZE
		// normalize data 0 mean, 1 standard deviation
		double mean;
		double stdDev;
		normalize(userInput, mean, stdDev);
		logger("normalizing user input 1, mean = %f, stdDev = %f\n", mean, stdDev);
		//normalize(userInput, mean, stdDev);
		//logger("normalizing user input 2, mean = %f, stdDev = %f\n", mean, stdDev);
#endif
	}
#endif

	void commonProc()
	{
		// range check gradient descent
		busy = loading || saving || noLoad || yesLoad;
		if (!busy) {
			switch (KEY) {
				// re randomize weights and biases
			case 'r':
				randomInit();
				delete aNeuralNet;
				randomInit();
#ifdef DO_NEURAL6
				aNeuralNet = new neuralNet(neuralName, aTesterTopology, *inputTrain, *desiredTrain, *inputTest, *desiredTest);
#else
				aNeuralNet = new neuralNet(neuralName, aTesterTopology, inputTrain, desiredTrain, inputTest, desiredTest);
#endif
				randomNextSeed();
				break;
#ifdef DO_NEURAL6
			case 'c':
				clipclear32(userBM, C32BLACK);
				break;
			case 'd':
				clipblit32(trainBM, userBM, 0, 0, 0, 0, userBM->size.x, userBM->size.y);
				break;
			case 'e':
				clipblit32(testBM, userBM, 0, 0, 0, 0, userBM->size.x, userBM->size.y);
				break;
#endif
				// load
			case 'l':
				if (!busy) {
					loading = uiCounter;
				}
				break;
				// save
			case 's':
				if (loading == 0 && saving == 0 && noLoad == 0 && yesLoad == 0) {
					saving = uiCounter;
					aNeuralNet->saveNetwork(loadSaveSlot);
				}
				break;
			}
			// pick load save slot
			if (KEY >= '0' && KEY <= '9') {
				loadSaveSlot = KEY - '0';
			}
		}
		if (MBUTuserBM & M_MBUTTON) {
			clipclear32(userBM, C32BLACK);
		}
		learn = range(0.0, learn, 100.0);
		calcAmount = range(-1, calcAmount, 1000000);
		calcSpeed = range(1, calcSpeed, 10000);
		runTestCount = range(0, runTestCount, 1000);
#ifdef DO_NEURAL6
		// range check idx for showing data
		S32 nd = idxFileTrain->getNumData();
		idxTrain = range(0, idxTrain, nd - 1);
		nd = idxFileTest->getNumData();
		idxTest = range(0, idxTest, nd - 1);

#endif

		if (calcAmount != 0) {
			for (S32 step = 0; step < calcSpeed; ++step) {
#ifdef DO_GRAD_TEST
				gradientDescentPoly();
#endif
				aNeuralNet->gradientDescent(learn);
			}
			if (calcAmount > 0) {
				--calcAmount;
			}
		}
		// test neural network, no training here
		if (runTestCount > 0) {
			if (runTest == 0) {
				// test suite
				runTest = runTestCount;
				aNeuralNet->testNetwork();
				// run 1 user setting
#ifdef DO_NEURAL6
				getUserInputsFromBM(userBM, userInputs);
#endif
				userCost = aNeuralNet->runNetwork(userInputs, userDesireds, userOutputs);
			}
			--runTest;
		}
#ifdef DO_GRAD_TEST
		yVar = polyFunction(xVar);
		dydx = polyFunctionPrime(xVar);
#endif
#ifdef SHOW_SIGMOID
		// show a point on the sigmoid function
		sigmoidOut = neuralNet::sigmoid(sigmoidIn);
		sigmoidOutPrime = neuralNet::delSigmoid(sigmoidIn);
		tanhOut = neuralNet::tangentH(sigmoidIn);
		tanhOutPrime = neuralNet::delTangentH(sigmoidIn);
#endif
	}

	void showOutput(U32 yoffset, const vector<double>& desireds, const vector<double>& outputs, bool showDes, bool noCost = false)
	{
		double maxVal = -1.0;
		U32 maxValIdx = -1;
		const vector<double>& data = showDes ? desireds : outputs;
		for (U32 i = 0; i < 10; ++i) { // digits
			double val = data[i];
			if (val > maxVal) {
				maxVal = val;
				maxValIdx = i;
			}
		}
		double cost = 0.0;
		for (U32 i = 0; i < 10; ++i) { // digits
			double val = data[i];
			bool hilit = i == maxValIdx;
			MEDIUMFONT->outtextxybf32(B32, WX / 2 + 28, yoffset + i * 16, hilit ? C32WHITE : C32MAGENTA, C32BLACK, "D %d, V = %8.4f%%", i, 100 * val);
			double del = outputs[i] - desireds[i];
			cost += del * del;
		}
		if (!noCost) {
			MEDIUMFONT->outtextxybf32(B32, WX / 2 + 28, yoffset + 160, C32LIGHTMAGENTA, C32BLACK, "Cost = %f", cost);
		}
	}

#ifdef DO_NEURAL6
	void drawToUser(C32 drawColor)
	{
		const S32 xMin = 3 * WX / 4;
		const S32 yMin = WY / 4;
		const S32 nPix = 28;
		if (MX >= xMin && MX < xMin + nPix * 8 && MY >= yMin && MY < yMin + nPix * 8) {
			U32 x = (MX - xMin) / 8;
			U32 y = (MY - yMin) / 8;
			const U32 A = 255;
			const U32 B = 180;
			const U32 C = 92;
			const U32 D = 64;
			const U32 E = 32;
			const U32 F = 0;
			vector<vector<U32>> brush{
				{F,E,D,E,F},
				{E,C,B,C,E},
				{D,B,A,B,D},
				{E,C,B,C,E},
				{F,E,D,E,F},
			};
			if (drawColor == C32BLACK) {
				// erase: just clear to black if brush is non zero
				for (S32 j = 0; j < 5; ++j) {
					for (S32 i = 0; i < 5; ++i) {
						U32 brushVal = brush[j][i];
						if (brushVal) {
							S32 xi = x + i - 2;
							S32 yj = y + j - 2;
							clipputpixel32(userBM, xi, yj, drawColor);
						}

					}
				}
			} else { // 
				// draw: only when output would be greater then the input
				for (S32 j = 0; j < 5; ++j) {
					for (S32 i = 0; i < 5; ++i) {
						U32 brushVal = brush[j][i];
						if (brushVal) {
							S32 xi = x + i - 2;
							S32 yj = y + j - 2;
							C32 oldVal = clipgetpixel32(userBM, xi, yj);
							if (oldVal.g < brushVal) {
								clipputpixel32(userBM, xi, yj, C32(brushVal, brushVal, brushVal));
							}
						}
					}
				}
			}
		}
	}
#endif

	void testShuffle()
	{
		logger("TEST SHUFFLE!\n");
		// shuffle algorithm example
		vector<S32> foo{ 1, 2, 3, 4, 5};
		// use a fixed seed
		U32 rndSeed = 1257;
		logger("shuffled elements\n");
		auto eng = default_random_engine(rndSeed);
		for (auto i = 0; i < 100; ++i) {
			for (const S32 x : foo) {
				logger("%d ", x);
			}
			shuffle(foo.begin(), foo.end(), eng);
			logger("\n");
		}
	}

} // end namespace neuralPlot

using namespace neuralPlot;
void plot2neuralinit()
{
	testShuffle();
	runinbackgroundSave = wininfo.runinbackground;
	wininfo.runinbackground = 1;
	loading = 0;
	noLoad = 0;
	saving = 0;
	// initialize graph paper
	plotter2init();
	adddebvars("neural_network", plot2neuralDeb, nplot2neuralDeb);
#ifdef DO_GRAD_TEST
	gradTestInit();
#endif
#ifdef DO_NEURAL5
	nerual5init();
#endif
#ifdef DO_NEURAL6
	nerual6init();
#endif
	randomInit();
#ifdef DO_NEURAL6
	aNeuralNet = new neuralNet(neuralName, aTesterTopology, *inputTrain, *desiredTrain, *inputTest, *desiredTest);
#else
	aNeuralNet = new neuralNet(neuralName, aTesterTopology, inputTrain, desiredTrain, inputTest, desiredTest);
#endif
	randomNextSeed();
}

void plot2neuralproc()
{
	perf_start(TEST2);
#ifdef DO_NEURAL6
	MBUTuserBM = MBUT;
	MBUT = 0; // don't move graph paper when hand drawing numbers
#endif
	// interact with graph paper
	plotter2proc();
	// calc neural interactive
	commonProc();
	perf_end(TEST2);
}

void plot2neuraldraw2d()
{
	perf_start(TEST1);
	// draw graph paper
	plotter2draw2d();
	// test gradient descent
#ifdef DO_GRAD_TEST
	drawfunction(polyFunction, C32CYAN);
	drawfunction(polyFunctionPrime, C32RED);
	pointf2x polyPoint(static_cast<float>(xVar), static_cast<float>(yVar));
	drawfcirclef(polyPoint, C32BLACK, .0625f);
	float slope = static_cast<float>(polyFunctionPrime(xVar));
	float delX = static_cast<float>(-learn * slope);
	float xVarNext = static_cast<float>(xVar + delX);
	float yVarNext = static_cast<float>(polyFunction(xVarNext));
	float yVarLine = static_cast<float>(delX * slope + yVar);
	pointf2x curPoint(static_cast<float>(xVar), static_cast<float>(yVar));
	pointf2x pNextLine(xVarNext, yVarLine);
	pointf2x pNextPoint(xVarNext, yVarNext);
	drawflinec(curPoint, pNextLine, C32BLACK);
	drawfpoint(pNextPoint, C32GREEN);
#endif
#ifdef SHOW_SIGMOID
	drawfunction(neuralNet::sigmoid, C32LIGHTGREEN);
	drawfunction(neuralNet::delSigmoid, C32GREEN);
	drawfunction(neuralNet::tangentH, C32LIGHTRED);
	drawfunction(neuralNet::delTangentH, C32RED);
	pointf2x point(static_cast<float>(sigmoidIn), static_cast<float>(sigmoidOut));
	drawfcirclef(point, C32BLACK, .0625f);
	point = pointf2x(static_cast<float>(sigmoidIn), static_cast<float>(tanhOut));
	drawfcirclef(point, C32BLUE, .0625f);
#endif
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 3, C32LIGHTGREEN, C32BLACK, "'~': Control Menu");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 20, C32LIGHTGREEN, C32BLACK, "'r': Randomize Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 37, C32LIGHTGREEN, C32BLACK, "'l': Load Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 54, C32LIGHTGREEN, C32BLACK, "'s': Save Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 71, C32LIGHTGREEN, C32BLACK, "'0'-'9': Load/Save Slot = '%d'", loadSaveSlot);
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 105, C32GREEN, C32BLACK, "busy = %d", busy);
	if (loading > 0) {
		MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 88, C32LIGHTMAGENTA, C32BLACK, "Loading from Slot '%d'", loadSaveSlot);
		loading--;
		if (loading == 0) {
			if (aNeuralNet->loadNetwork(loadSaveSlot)) {
				yesLoad = uiCounter;
			} else {
				noLoad = uiCounter;
			}
		}
	}
	if (noLoad > 0) {
		MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 88, C32LIGHTRED, C32BLACK, " Slot '%d' NOT loaded", loadSaveSlot);
		noLoad--;
	}
	if (yesLoad > 0) {
		MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 88, C32WHITE, C32BLACK, " Slot '%d' loaded!", loadSaveSlot);
		yesLoad--;
	}
	if (saving > 0) {
		MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 88, C32LIGHTMAGENTA, C32BLACK, "Saving to Slot '%d'", loadSaveSlot);
		saving--;
	}
#ifdef DO_NEURAL6
	// draw images from mnist database
	// train
	bitmap32* abm = makeBMfromImage(*idxFileTrain->getOneImage(idxTrain));
	clipblit32(abm, trainBM, 0, 0, 0, 0, abm->size.x, abm->size.y);
	bitmap32* bigger = scale8(abm);
	clipblit32(bigger, B32, 0, 0, 3 * WX / 4, WY / 2, bigger->size.x, bigger->size.y);
	bitmap32free(abm);
	bitmap32free(bigger);
	U32 des = idxFileTrain->getOneDesired(idxTrain);
	LARGEFONT->outtextxybf32(B32, WX / 2 + 28, WY / 2, C32LIGHTMAGENTA, C32BLACK, "Train Desired '%d'", des);
	showOutput(WY / 2 + 40, aNeuralNet->getOneTrainDesired(idxTrain), aNeuralNet->getOneTrainOutput(idxTrain), showDesireds);

	// test
	abm = makeBMfromImage(*idxFileTest->getOneImage(idxTest));
	clipblit32(abm, testBM, 0, 0, 0, 0, abm->size.x, abm->size.y);
	bigger = scale8(abm);
	clipblit32(bigger, B32, 0, 0, 3 * WX / 4, 3 * WY / 4, bigger->size.x, bigger->size.y);
	bitmap32free(abm);
	bitmap32free(bigger);
	des = idxFileTest->getOneDesired(idxTest);
	LARGEFONT->outtextxybf32(B32, WX / 2 + 28, 3 * WY / 4, C32LIGHTMAGENTA, C32BLACK, " Test Desired '%d'", des);
	showOutput(3 * WY / 4 + 40, aNeuralNet->getOneTestDesired(idxTest), aNeuralNet->getOneTestOutput(idxTest), showDesireds);

	// user
	bigger = scale8(userBM);
	clipblit32(bigger, B32, 0, 0, 3 * WX / 4, WY / 4, bigger->size.x, bigger->size.y);
	bitmap32free(bigger);
	LARGEFONT->outtextxybf32(B32, WX / 2 + 28, WY / 4, C32LIGHTMAGENTA, C32BLACK, "User", des);
	showOutput(WY / 4 + 40, userDesireds, userOutputs, showDesireds, true);

	// instructions for user
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8 - 24, 134, C32YELLOW, C32BLACK, "'LMB': Draw Foreground (white)");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8 - 24, 154, C32YELLOW, C32BLACK, "'RMB': Draw Background (black)");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8 - 24, 174, C32YELLOW, C32BLACK, "'c' or 'MMB': Clear Image");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8 - 24, 194, C32YELLOW, C32BLACK, "'d': Copy image from train");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8 - 24, 214, C32YELLOW, C32BLACK, "'e': Copy image from test");

	// draw to userBM
	bool draw = false;
	C32 drawColor = C32BLACK;
	if (MBUTuserBM & M_LBUTTON) {
		draw = true;
		drawColor = C32WHITE;
	} else if (MBUTuserBM & M_RBUTTON) {
		draw = true;
		drawColor = C32BLACK;
	}
	if (draw) {
		drawToUser(drawColor);
	}
#endif
	perf_end(TEST1);
}

void plot2neuralexit()
{
	wininfo.runinbackground = runinbackgroundSave;
	// free graph paper, free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("neural_network");
	delete aNeuralNet;
#ifdef DO_NEURAL6
	delete idxFileTrain;
	delete idxFileTest;
	bitmap32free(userBM);
	bitmap32free(trainBM);
	bitmap32free(testBM);
#endif
	aNeuralNet = nullptr;
}
