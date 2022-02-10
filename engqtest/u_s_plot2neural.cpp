// plotter2 runs in FPU control high precision, so you can use doubles correctly if necessary
// interactively test neural networks

#include <m_eng.h>
#include "u_plotter2.h"
#include "u_neuralNetwork.h"
#include "m_perf.h"

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
#define DO_NEURAL5 // Layers 3 (6 - 6 - 4), 70 vars, 60 costs, test 4 more costs for a total of 64 costs
//#define DO_NEURAL6 // NYI Layers 3 (784 - 16 - 16 - 10), 13,002 vars, 60,000 costs, test 10,000 more costs total 70,000
#define DO_GRAD_TEST // 1 var, minimize the adjustable quartic equation
#define SHOW_SIGMOID

#ifdef USE_TIMEB
#include <sys/timeb.h>
#endif

namespace neuralPlot {
	// common data
	const U32 uiCounter = 30; // for load and save models
// these are not on time
	U32 randomSeed = 123456;
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
	S32 runTestCount = 20; // how many frames to wait to run test and user
	S32 runTest = 0;

#ifdef SHOW_SIGMOID
	double sigmoidIn = 0.0;
	double sigmoidOut;
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
		const double LO = .1;
		const double HI = .9;
		// input size 6
		// output size 4
		inputTrain.clear();
		desiredTrain.clear();
		inputTest.clear();
		desiredTest.clear();
		for (U32 k = 0; k < doTotal; ++k) {
			vector<double>in(6);
			vector<double>des(4);
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
		{"sigmoid in", &sigmoidIn, D_DOUBLE, FLOATUP / 8},
		{"sigmoid out", &sigmoidOut, D_DOUBLEEXP | D_RDONLY},
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
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
#endif
#ifdef DO_NEURAL4
		{"userInput0", &userInputs[0], D_DOUBLE, FLOATUP / 4},
		{"userInput1", &userInputs[1], D_DOUBLE, FLOATUP / 4},
		{"userInput2", &userInputs[2], D_DOUBLE, FLOATUP / 4},
		{"userDesired0", &userDesireds[0], D_DOUBLE, FLOATUP / 4},
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
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
		{"userDesired1", &userDesireds[1], D_DOUBLE, FLOATUP / 4},
		{"userDesired2", &userDesireds[2], D_DOUBLE, FLOATUP / 4},
		{"userDesired3", &userDesireds[3], D_DOUBLE, FLOATUP / 4},
		{"userOutput0", &userOutputs[0], D_DOUBLE | D_RDONLY},
		{"userOutput1", &userOutputs[1], D_DOUBLE | D_RDONLY},
		{"userOutput2", &userOutputs[2], D_DOUBLE | D_RDONLY},
		{"userOutput3", &userOutputs[3], D_DOUBLE | D_RDONLY},
#endif
		{"userCost", &userCost, D_DOUBLEEXP | D_RDONLY},
	};
	const int nplot2neuralDeb = NUMELEMENTS(plot2neuralDeb);

	// common code
	double sigmoid(double x)
	{
		return 1.0 / (1.0 + exp(-x));
	}

	// derivative of sigmoid
	double delSigmoid(double x)
	{
		double s = sigmoid(x);
		return s * (1.0 - s);
	}

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

	void commonProc()
	{
		// range check gradient descent
		bool busy = loading || saving || noLoad || yesLoad;
		if (!busy) {
			switch (KEY) {
				// re randomize weights and biases
			case 'r':
				randomInit();
				delete aNeuralNet;
				randomInit();
				aNeuralNet = new neuralNet(neuralName, aTesterTopology, inputTrain, desiredTrain, inputTest, desiredTest);
				randomNextSeed();
				break;
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
		learn = range(0.0, learn, 100.0);
		calcAmount = range(-1, calcAmount, 1000000);
		calcSpeed = range(1, calcSpeed, 10000);
		runTestCount = range(0, runTestCount, 1000);
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
		sigmoidOut = sigmoid(sigmoidIn);
#endif
	}


} // end namespace neuralPlot

using namespace neuralPlot;

void plot2neuralinit()
{
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
	randomInit();
	aNeuralNet = new neuralNet(neuralName, aTesterTopology, inputTrain, desiredTrain, inputTest, desiredTest);
	randomNextSeed();
}

void plot2neuralproc()
{
	perf_start(TEST2);
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
	drawfunction(sigmoid, C32GREEN);
	drawfunction(delSigmoid, C32RED);
	pointf2x point(static_cast<float>(sigmoidIn), static_cast<float>(sigmoidOut));
	drawfcirclef(point, C32BLACK, .0625f);
#endif
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 3, C32LIGHTGREEN, C32BLACK, "'~': Control Menu");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 20, C32LIGHTGREEN, C32BLACK, "'r': Randomize Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 37, C32LIGHTGREEN, C32BLACK, "'l': Load Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 54, C32LIGHTGREEN, C32BLACK, "'s': Save Network");
	MEDIUMFONT->outtextxybf32(B32, 5 * WX / 8, 71, C32LIGHTGREEN, C32BLACK, "'0'-'9': Load/Save Slot = '%d'", loadSaveSlot);
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
	perf_end(TEST1);
}

void plot2neuralexit()
{
	// free graph paper, free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("neural_network");
	delete aNeuralNet;
	aNeuralNet = nullptr;
}
