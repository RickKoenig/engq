// plotter2 runs in FPU control high precision, so you can use doubles correctly if necessary
// interactively test neural networks
#include <iomanip>      // std::setw

#include <m_eng.h>
#include "u_plotter2.h"

using namespace u_plotter2;
// Keyboard shortcuts, 'r' to reset/randomize weights and biases

// lots of switches
// random only
//#define USE_TIMEB // randomize based on time
// these are not on time
U32 randomSeed = 123456;
U32 randomNext = 1;

// sub switches, what to run in this state
#define DO_NEURAL4 // Layers 3 (3 - 2 - 3 - 2), 25 vars, 4 costs
#define NEURAL4_VERBOSE // show neural 4 derivatives
//#define DO_NEURAL5 // NYI Layers 3 (6 - 6 - 4), 70 vars, 60 costs // TODO: check 4 more costs for a total of 64 states
//#define DO_NEURAL6 // NYI Layers 3 (784 - 16 - 16 - 10), 13,002 vars, 60,000 costs // TODO: check 10,000 more costs
//#define DO_GRAD_TEST // 1 var, minimize the adjustable quartic equation
//#define SHOW_SIGMOID
#define TESTER // general purpose neural network class

#ifdef USE_TIMEB
#include <sys/timeb.h>
#endif

namespace neural {
	// common data
	// calc gradient descent
	double learn = 0.0; // .03125;
	S32 calcAmount = 1; // negative run forever
	S32 calcSpeed = 1;
	// for brute force derivatives
	const double epsilon = 1e-8;

#ifdef DO_NEURAL4
	// (3 - 2 - 3 - 2) // for now hard coded, but using arrays
	// network
	double weight1[2][3];
	double bias1[2];
	double weight2[3][2];
	double bias2[3];
	double weight3[2][3];
	double bias3[2];
	// per cost
	double al0[3]; // current input
	double al1[2]; // current hidden layer
	double al2[3]; // current another hidden layer
	double al3[2]; // current output
	double Y[2]; // current desired output

	// inputs, desires, train/cost
	const S32 nTrain = 4;
	double input[nTrain][3] = {
		{.12, .16, .26},
		{.13, .97, .45},
		{.94, .18, .36},
		{.95, .54, .79} 
	};
	double output[nTrain][2];
	double desired[nTrain][2] = { 
		{.11, .12},
		{.93, .14}, 
		{.95, .16}, 
		{.97, .48} 
	}; // for now try or and and gates
	double avgCost;

	// cost derivatives for nTrain
	// chain rule
	double DcostDweight1CR[2][3];
	double DcostDbias1CR[2];
	double DcostDweight2CR[3][2];
	double DcostDbias2CR[3];
	double DcostDweight3CR[2][3];
	double DcostDbias3CR[2];
	// brute force
	double DcostDweight1BF[2][3];
	double DcostDbias1BF[2];
	double DcostDweight2BF[3][2];
	double DcostDbias2BF[3];
	double DcostDweight3BF[2][3];
	double DcostDbias3BF[2];
#endif
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
#ifdef TESTER
	vector<S32> aTesterTopology {3, 2, 3, 2};
	// inputs, desires
	vector<vector<double>> inputTester = {
		{.12, .16, .26},
		{.13, .97, .45},
		{.94, .18, .36},
		{.95, .54, .79}
	};
	vector<vector<double>> desiredTester = {
		{.11, .12},
		{.93, .14},
		{.95, .16},
		{.97, .48}
	}; // for now try or and and gates
#endif
	// for debvars
	struct menuvar plot2neuralDeb[] = {
		{"@yellow@--- neural network vars ---", NULL, D_VOID, 0},
		{"calcAmount", &calcAmount, D_INT, 1},
		{"calcSpeed", &calcSpeed, D_INT, 32},
		{"learn", &learn, D_DOUBLE, FLOATUP / 32},
#ifdef DO_NEURAL4
		// network
		{"@lightgreen@--- neural4 ---", NULL, D_VOID, 0},
		{ "weight1_00", &weight1[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight1_01", &weight1[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight1_02", &weight1[0][2], D_DOUBLE, FLOATUP / 8 },
		{ "weight1_10", &weight1[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight1_11", &weight1[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight1_12", &weight1[1][2], D_DOUBLE, FLOATUP / 8 },
		{ "bias1_0", &bias1[0], D_DOUBLE, FLOATUP / 8 },
		{ "bias1_1", &bias1[1], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_00", &weight2[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_01", &weight2[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_10", &weight2[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_11", &weight2[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_20", &weight2[2][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight2_21", &weight2[2][1], D_DOUBLE, FLOATUP / 8 },
		{ "bias2_0", &bias2[0], D_DOUBLE, FLOATUP / 8 },
		{ "bias2_1", &bias2[1], D_DOUBLE, FLOATUP / 8 },
		{ "bias2_2", &bias2[2], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_00", &weight3[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_01", &weight3[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_02", &weight3[0][2], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_10", &weight3[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_11", &weight3[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "weight3_12", &weight3[1][2], D_DOUBLE, FLOATUP / 8 },
		{ "bias3_0", &bias3[0], D_DOUBLE, FLOATUP / 8 },
		{ "bias3_1", &bias3[1], D_DOUBLE, FLOATUP / 8 },
		// inputs, desires, outputs, cost
		{ "input0_0", &input[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "input0_1", &input[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "input0_2", &input[0][2], D_DOUBLE, FLOATUP / 8 },
		{ "input1_0", &input[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "input1_1", &input[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "input1_2", &input[1][2], D_DOUBLE, FLOATUP / 8 },
		{ "input2_0", &input[2][0], D_DOUBLE, FLOATUP / 8 },
		{ "input2_1", &input[2][1], D_DOUBLE, FLOATUP / 8 },
		{ "input2_2", &input[2][2], D_DOUBLE, FLOATUP / 8 },
		{ "input3_0", &input[3][0], D_DOUBLE, FLOATUP / 8 },
		{ "input3_1", &input[3][1], D_DOUBLE, FLOATUP / 8 },
		{ "input3_2", &input[3][2], D_DOUBLE, FLOATUP / 8 },
		{ "desired0_0", &desired[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "output 0_0", &output[0][0], D_DOUBLE | D_RDONLY },
		{ "desired0_1", &desired[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "output 0_1", &output[0][1], D_DOUBLE | D_RDONLY },
		{ "desired1_0", &desired[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "output 1_0", &output[1][0], D_DOUBLE | D_RDONLY },
		{ "desired1_1", &desired[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "output 1_1", &output[1][1], D_DOUBLE | D_RDONLY },
		{ "desired2_0", &desired[2][0], D_DOUBLE, FLOATUP / 8 },
		{ "output 2_0", &output[2][0], D_DOUBLE | D_RDONLY },
		{ "desired2_1", &desired[2][1], D_DOUBLE, FLOATUP / 8 },
		{ "output 2_1", &output[2][1], D_DOUBLE | D_RDONLY },
		{ "desired3_0", &desired[3][0], D_DOUBLE, FLOATUP / 8 },
		{ "output 3_0", &output[3][0], D_DOUBLE | D_RDONLY },
		{ "desired3_1", &desired[3][1], D_DOUBLE, FLOATUP / 8 },
		{ "output 3_1", &output[3][1], D_DOUBLE | D_RDONLY },
		{ "costAverage", &avgCost, D_DOUBLEEXP | D_RDONLY },
#ifdef NEURAL4_VERBOSE
		// cost derivatives
		// chain rule and brute force
		{ "DcostDweight1_00_CR", &DcostDweight1CR[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_00_BF", &DcostDweight1BF[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_01_CR", &DcostDweight1CR[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_01_BF", &DcostDweight1BF[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_02_CR", &DcostDweight1CR[0][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_02_BF", &DcostDweight1BF[0][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_10_CR", &DcostDweight1CR[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_10_BF", &DcostDweight1BF[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_11_CR", &DcostDweight1CR[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_11_BF", &DcostDweight1BF[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_12_CR", &DcostDweight1CR[1][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_12_BF", &DcostDweight1BF[1][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias1_0_CR"   , &DcostDbias1CR[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias1_0_BF"   , &DcostDbias1BF[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias1_1_CR"   , &DcostDbias1CR[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias1_1_BF"   , &DcostDbias1BF[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_00_CR", &DcostDweight2CR[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_00_BF", &DcostDweight2BF[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_01_CR", &DcostDweight2CR[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_01_BF", &DcostDweight2BF[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_10_CR", &DcostDweight2CR[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_10_BF", &DcostDweight2BF[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_11_CR", &DcostDweight2CR[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_11_BF", &DcostDweight2BF[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_20_CR", &DcostDweight2CR[2][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_20_BF", &DcostDweight2BF[2][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_21_CR", &DcostDweight2CR[2][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight2_21_BF", &DcostDweight2BF[2][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_0_CR"   , &DcostDbias2CR[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_0_BF"   , &DcostDbias2BF[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_1_CR"   , &DcostDbias2CR[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_1_BF"   , &DcostDbias2BF[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_2_CR"   , &DcostDbias2CR[2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_2_BF"   , &DcostDbias2BF[2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_00_CR", &DcostDweight3CR[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_00_BF", &DcostDweight3BF[0][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_01_CR", &DcostDweight3CR[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_01_BF", &DcostDweight3BF[0][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_02_CR", &DcostDweight3CR[0][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_02_BF", &DcostDweight3BF[0][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_10_CR", &DcostDweight3CR[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_10_BF", &DcostDweight3BF[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_11_CR", &DcostDweight3CR[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_11_BF", &DcostDweight3BF[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_12_CR", &DcostDweight3CR[1][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight3_12_BF", &DcostDweight3BF[1][2], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias3_0_CR"   , &DcostDbias3CR[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias3_0_BF"   , &DcostDbias3BF[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias3_1_CR"   , &DcostDbias3CR[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias3_1_BF"   , &DcostDbias3BF[1], D_DOUBLEEXP | D_RDONLY },
#endif
#endif
#ifdef DO_GRAD_TEST
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
		{"sigmoid in", &sigmoidIn, D_DOUBLE, FLOATUP / 8},
		{"sigmoid out", &sigmoidOut, D_DOUBLEEXP | D_RDONLY},
#endif
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

	// generate a random number in range [0 - 1)
	double frand()
	{
		return rand() / (RAND_MAX + 1.0);
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

#ifdef TESTER
	class tester {
		vector<S32> topo; // the structure of the network

		// a layer of the network, layer 0 is the input layer, no weights or biases on layer 0
		// the topo.size() - 1 layer is the output layer
		struct layer {
			vector<double> A;
			vector<vector<double>> W;
			vector<double> B;
			vector<vector<double>> dCdW_CR;
			vector<double> dCdB_CR;
			vector<vector<double>> dCdW_BF;
			vector<double> dCdB_BF;
		};
		// the network
		vector<layer> layers;

		// inputs and outputs
		vector<vector<double>>& inputs;
		vector<vector<double>>& desireds;
		vector<vector<double>> outputs;
		U32 nTrain;
		vector<double> Y;

		double avgCost{};

		// for debvars
		vector<menuvar> dbTester; // debprint menu

		C8* copyStr(const C8* in) // free with delete, all string names in dbTester are allocated and are to be freed
		{
			C8* ret = new C8[strlen(in) + 1];
			strcpy(ret, in);
			return ret;
		}
	public:
		tester(const vector<S32>& topology, vector<vector<double>>& in, vector<vector<double>>& des) :
			inputs(in), desireds(des), nTrain(des.size())
		{
			topo = topology;
			menuvar mv{ copyStr("@green@--- TESTER ---"), NULL, D_VOID, 0 };
			dbTester.push_back(mv);
			S32 outputSize = desireds[0].size();
			outputs = vector<vector<double>>(nTrain, vector<double> (outputSize));
			Y = vector<double>(outputSize);
			// build a random network
			U32 i, j, k;
			for (k = 0; k < topology.size(); ++k) {
				S32 cols = topology[k];
				layer aLayer;
				aLayer.A = vector<double>(cols);
				if (k > 0) {
					S32 rows = topology[k - 1];
					aLayer.B = vector<double>(cols);
					aLayer.W = vector<vector<double>>(cols, vector<double>(rows));
					aLayer.dCdB_CR = vector<double>(cols);
					aLayer.dCdW_CR = vector<vector<double>>(cols, vector<double>(rows));
					aLayer.dCdB_BF = vector<double>(cols);
					aLayer.dCdW_BF = vector<vector<double>>(cols, vector<double>(rows));
				}
				layers.push_back(aLayer);
			}
			// add network to debprint menu
			for (k = 1; k < layers.size(); ++k) {
				layer& lay = layers[k];
				U32 rowsW = lay.W.size();
				U32 colsW = lay.W[0].size();
				for (j = 0; j < rowsW; ++j) {
					vector<double>& aRow = lay.W[j];
					for (i = 0; i < colsW; ++i) {
						aRow[i] = frand();
						stringstream ssW;
						ssW << "weight" << k << "_" << j << i;
						C8* name = copyStr(ssW.str().c_str());
						mv = { name, &aRow[i], D_DOUBLE, FLOATUP / 8 };
						dbTester.push_back(mv);
					}
				}
				for (j = 0; j < rowsW; ++j) {
					lay.B[j] = frand();
					stringstream ssB;
					ssB << "bias" << k << "_" << j;
					C8* name = copyStr(ssB.str().c_str());
					mv = { name, &lay.B[j], D_DOUBLE, FLOATUP / 8 };
					dbTester.push_back(mv);
				}
			}
			// add inputs to debprint menu
			for (j = 0; j < nTrain; ++j) {
				vector<double>& anInput = inputs[j];
				for (i = 0; i < anInput.size(); ++i) {
					stringstream ssInput;
					ssInput << "input" << j << "_" << i;
					C8* name = copyStr(ssInput.str().c_str());
					mv = { name, &anInput[i], D_DOUBLE, FLOATUP / 8 };
					dbTester.push_back(mv);
				}
			}
			// add desireds and outputs to debprint menu
			for (j = 0; j < nTrain; ++j) {
				vector<double>& aDesired = desireds[j];
				vector<double>& anOutput = outputs[j];
				for (i = 0; i < aDesired.size(); ++i) {
					stringstream ssDesired;
					ssDesired << "desired" << j << "_" << i;
					C8* name = copyStr(ssDesired.str().c_str());
					mv = { name, &aDesired[i], D_DOUBLE, FLOATUP / 8 };
					dbTester.push_back(mv);
					stringstream ssOutput;
					ssOutput << "output " << j << "_" << i;
					name = copyStr(ssOutput.str().c_str());
					mv = { name, &anOutput[i], D_DOUBLE | D_RDONLY};
					dbTester.push_back(mv);
				}
			}
			mv = { copyStr("costAverage"), &avgCost, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
			dbTester.push_back(mv);
			// add derivatives to debprint menu
			for (k = 1; k < layers.size(); ++k) {
				layer& lay = layers[k];
				U32 rowsW = lay.W.size();
				U32 colsW = lay.W[0].size();
				for (j = 0; j < rowsW; ++j) {
					vector<double>& aRowCR = lay.dCdW_CR[j];
					vector<double>& aRowBF = lay.dCdW_BF[j];
					for (i = 0; i < colsW; ++i) {
						stringstream ssW;
						ssW << "DcostDweight" << k << "_" << j << i << "_CR";
						C8* name = copyStr(ssW.str().c_str());
						mv = { name, &aRowCR[i], D_DOUBLEEXP | D_RDONLY};
						dbTester.push_back(mv);
						ssW.str(string()); // clear
						ssW << "DcostDweight" << k << "_" << j << i << "_BF";
						name = copyStr(ssW.str().c_str());
						mv = { name, &aRowBF[i], D_DOUBLEEXP | D_RDONLY };
						dbTester.push_back(mv);
					}
				}
				for (j = 0; j < rowsW; ++j) {
					stringstream ssB;
					ssB << "DcostDbias" << k << "_" << j << "_CR";
					C8* name = copyStr(ssB.str().c_str());
					mv = { name, &lay.dCdB_CR[j], D_DOUBLEEXP | D_RDONLY };
					dbTester.push_back(mv);
					ssB.str(string()); // clear
					ssB << "DcostDbias" << k << "_" << j << "_BF";
					name = copyStr(ssB.str().c_str());
					mv = { name, &lay.dCdB_BF[j], D_DOUBLEEXP | D_RDONLY };
					dbTester.push_back(mv);
				}
			}
			// publish the debprint menu
			adddebvars("tester", dbTester.data(), dbTester.size());
		}

		~tester()
		{
			// unpublish the debprint menu
			removedebvars("tester");
			for (auto mv : dbTester) {
				delete mv.name;
			}
		}
	private:
		double runNetwork()
		{
			// TODO: optimize
			S32 i, j;
			U32 k;
			vector<double> Z;
			U32 lastK;
			for (k = 1; k < topo.size(); ++k) {
				Z.resize(topo[k]);
				lastK = k - 1;
				for (j = 0; j < topo[k]; ++j) {
					Z[j] = layers[k].B[j];
					for (i = 0; i < topo[lastK]; ++i) {
						Z[j] += layers[lastK].A[i] * layers[k].W[j][i];
					}
					layers[k].A[j] = sigmoid(Z[j]);
				}
			}
			double retCost = 0.0;
			lastK = k - 1;
			for (j = 0; j < topo[lastK]; ++j) {
				double del = layers[lastK].A[j] - Y[j];
				retCost += del * del;
			}
			return retCost;
		}
	public:
		void gradientDescent() // gradient descent
		{
			S32 i, j;
			S32 k;
			// brute force derivatives and run network for cost
			for (k = 1; k < static_cast<S32>(topo.size()); ++k) {
				S32 lastK = k - 1;
				S32 row = topo[k];
				S32 col = topo[lastK];
				double* addr;
				for (j = 0; j < row; ++j) {
					addr = &layers[k].dCdW_CR[j][0];
					fill(addr, addr + col, 0.0);
					addr = &layers[k].dCdW_BF[j][0];
					fill(addr, addr + col, 0.0);
				}
				addr = &layers[k].dCdB_CR[0];
				fill(addr, addr + row, 0.0);
				addr = &layers[k].dCdB_BF[0];
				fill(addr, addr + row, 0.0);
			}

			avgCost = 0.0;
			vector<double>& Ain = layers[0].A;
			S32 lastLayer = topo.size() - 1;
			vector<double>& Aout = layers[lastLayer].A;
			S32 firstLayerSize = topo[0];
			S32 lastLayerSize = topo[lastLayer];

			for (U32 t = 0; t < nTrain; ++t) {
				copy(&inputs[t][0], &inputs[t][0] + firstLayerSize, &Ain[0]);
				copy(&desireds[t][0], &desireds[t][0] + lastLayerSize, &Y[0]);

				double cost = runNetwork(); // baseline
				// TODO: optimize
				for (k = 1; k < static_cast<S32>(topo.size()); ++k) {
					S32 lastK = k - 1;
					S32 row = topo[k];
					S32 col = topo[lastK];
					// brute force derivatives
					for (j = 0; j < row; ++j) {
						for (i = 0; i < col; ++i) {
							double save = layers[k].W[j][i];
							layers[k].W[j][i] += epsilon;
							double costPweight = runNetwork();
							layers[k].W[j][i] = save;
							layers[k].dCdW_BF[j][i] += (costPweight - cost) / epsilon;
						}
						double save = layers[k].B[j];
						layers[k].B[j] += epsilon;
						double costPbias = runNetwork();
						layers[k].B[j] = save;
						layers[k].dCdB_BF[j] += (costPbias - cost) / epsilon;
					}
				}
				runNetwork(); // need correct A? for chain rule derivatives and output
				copy(&Aout[0], &Aout[0] + lastLayerSize, &outputs[t][0]);

				avgCost += cost;
#if 1
				S32 outSize = topo.size() - 1;
				S32 ls = topo[outSize];
				vector<double> DcostDZ;// (Ls); // same as DcostDBL, used for backtrace
				vector<double> DcostDA;// (Ls);
				for (k = topo.size() - 1; k > 0; --k) {
					// TODO: optimize
					// chain rule derivatives, the last layer
					// DcostDWL = DcostDAL * DALDZL * DZLDWL
					// DcostDBL = DcostDAL * DALDZL * DZLDBL
					// derived
					// DcostDAL = 2.0*(AL - Y)
					// DALDZL = (1.0 - AL)*AL
					// DZLDWL = al2
					// DZLDBL = 1
					S32 L = k;
					S32 P = L - 1;
					S32 N = L + 1;
					S32 Ls = topo[L]; // current layer
					S32 Ps = topo[P]; // previous layer
					S32 Ns = topo[N];
					S32 row = Ls;
					S32 col = Ps;
					vector<double>& AL = layers[L].A;
					vector<double>& AP = layers[P].A;
					// cost
					DcostDA = vector<double>(Ls);
					if (k == topo.size() - 1) {
						for (j = 0; j < Ls; ++j) {
							DcostDA[j] = 2.0*(AL[j] - Y[j]);
						}
					} else { // backtrace
						for (j = 0; j < Ls; ++j) {
							for (i = 0; i < Ns; ++i) {
								DcostDA[j] += layers[N].W[i][j] * DcostDZ[i];
							}
							//logger("Tester: DcosDA[%d] = %12.9f\n", j, DcostDA[j]);
						}
						//break;
#if 0
						// backtrace
						// DcostDal2 = [DcostDal3 * Dal3Dzl3] * Dzl3Dal2 = DcostDzl3 * Dzl3Dal2
						double DcostDal2[3]{};
						for (j = 0; j < 3; ++j) {
							for (i = 0; i < 2; ++i) {
								DcostDal2[j] += weight3[i][j] * DcostDzl3[i];
							}
						}
#endif

					}
					//DcostDA = vector<double>(Ls);
					DcostDZ = vector<double>(Ls);
					for (j = 0; j < Ls; ++j) {
						// z
						double DALDZL = (1.0 - AL[j])*AL[j];
						// bias
						DcostDZ[j] = DcostDA[j] * DALDZL; // same as DcostDBL
						layers[L].dCdB_CR[j] += DcostDZ[j]; // add to train/cost
						// weight
						for (i = 0; i < Ps; ++i) {
							double DcostDWL = DcostDZ[j] * AP[i];
							layers[L].dCdW_CR[j][i] += DcostDWL; // add to train/cost
						}
					}
					//break;
				}
#endif
#if 0
				// chain rule derivatives
				// chain rule derivatives, the last layer
				// DcostDw3 = DcostDal3 * Dal3Dzl3 * Dzl3Dw3
				// DcostDb3 = DcostDal3 * Dal3Dzl3 * Dzl3Db3
				// derived
				// DcostDal3 = 2.0*(al3 - Y)
				// Dal3Dzl3 = (1.0 - al3)*al3
				// Dzl3Dw3 = al2
				// Dzl3Db3 = 1
				double DcostDzl3[2]{}; // same as DcostDb3, used for backtrace
				for (j = 0; j < 2; ++j) {
					// cost
					double DcostDal3 = 2.0*(al3[j] - Y[j]);
					// z
					double Da3Dz3 = (1.0 - al3[j])*al3[j];
					// bias
					DcostDzl3[j] = DcostDal3 * Da3Dz3; // same as DcostDb3
					DcostDbias3CR[j] += DcostDzl3[j]; // add to train/cost
					// weight
					for (i = 0; i < 3; ++i) {
						double DcostDw3 = DcostDzl3[j] * al2[i];
						DcostDweight3CR[j][i] += DcostDw3; // add to train/cost
					}
				}

				// backtrace
				// DcostDal2 = [DcostDal3 * Dal3Dzl3] * Dzl3Dal2 = DcostDzl3 * Dzl3Dal2
				double DcostDal2[3]{};
				for (j = 0; j < 3; ++j) {
					for (i = 0; i < 2; ++i) {
						DcostDal2[j] += weight3[i][j] * DcostDzl3[i];
					}
				}

				// chain rule derivatives, the previous layer
				// DcostDw2 = DcostDal2 * Dal2Dzl2 * Dzl2Dw2
				// DcostDb2 = DcostDal2 * Dal2Dzl2 * Dzl2Db2
				// derived
				// Dal2Dzl2 = (1.0 - al2)*al2
				// Dzl2Dw2 = al1
				// Dzl2Db2 = 1
				double DcostDzl2[3]{}; // same as DcostDb2, used for backtrace
				for (j = 0; j < 3; ++j) {
					// z
					double Da2Dz2 = (1.0 - al2[j])*al2[j];
					// bias
					DcostDzl2[j] = DcostDal2[j] * Da2Dz2; // same as DcostDb2
					DcostDbias2CR[j] += DcostDzl2[j]; // add to train/cost
					// weight
					for (i = 0; i < 2; ++i) {
						double DcostDw2 = DcostDzl2[j] * al1[i];
						DcostDweight2CR[j][i] += DcostDw2; // add to train/cost
					}
				}

				// backtrace
				// DcostDal1 = [DcostDal2 * Dal3Dzl2] * Dzl2Dal1 = DcostDzl2 * Dzl2Dal1
				double DcostDal1[2]{};
				for (j = 0; j < 2; ++j) {
					for (i = 0; i < 3; ++i) {
						DcostDal1[j] += weight2[i][j] * DcostDzl2[i];
					}
				}

				// chain rule derivatives, the first layer
				// DcostDw1 = DcostDal1 * Dal1Dzl1 * Dzl1Dw1
				// DcostDb1 = DcostDal1 * Dal2Dzl1 * Dzl1Db1
				// derived
				// Dal1Dzl1 = (1.0 - al1)*al1
				// Dzl1Dw1 = al0
				// Dzl1Db1 = 1
				double DcostDzl1[2]{}; // same as DcostDb1, used for backtrace
				for (j = 0; j < 2; ++j) {
					// z
					double Da1Dz1 = (1.0 - al1[j])*al1[j];
					// bias
					DcostDzl1[j] = DcostDal1[j] * Da1Dz1; // same as DcostDb2
					DcostDbias1CR[j] += DcostDzl1[j]; // add to train/cost
					// weight
					for (i = 0; i < 3; ++i) {
						double DcostDw1 = DcostDzl1[j] * al0[i];
						DcostDweight1CR[j][i] += DcostDw1; // add to train/cost
					}
				}
#endif

			} // end train

			// average derivatives and cost
			// TODO: optimize
			for (k = 1; k < static_cast<S32>(topo.size()); ++k) {
				S32 lastK = k - 1;
				S32 row = topo[k];
				S32 col = topo[lastK];
				for (j = 0; j < row; ++j) {
					for (i = 0; i < col; ++i) {
						layers[k].dCdW_BF[j][i] /= nTrain;
						layers[k].dCdW_CR[j][i] /= nTrain;
					}
					layers[k].dCdB_BF[j] /= nTrain;
					layers[k].dCdB_CR[j] /= nTrain;
				}
			}
			avgCost /= nTrain;

			// run the gradient descent learn
			// TODO: optimize
			for (k = 1; k < static_cast<S32>(topo.size()); ++k) {
				S32 lastK = k - 1;
				S32 row = topo[k];
				S32 col = topo[lastK];
				// average
				for (j = 0; j < row; ++j) {
					for (i = 0; i < col; ++i) {
						layers[k].W[j][i] -= layers[k].dCdW_CR[j][i] * learn;
					}
					layers[k].B[j] -= layers[k].dCdB_CR[j] * learn;
				}
			}
		}
	};
	tester* aTester;
#endif


#ifdef DO_NEURAL4
	// neural (3 - 2 - 3 - 2) 25 vars, 4 cost
	void neuralInit()
	{
		weight1[0][0] = frand();
		weight1[0][1] = frand();
		weight1[0][2] = frand();
		weight1[1][0] = frand();
		weight1[1][1] = frand();
		weight1[1][2] = frand();
		bias1[0] = frand();
		bias1[1] = frand();
		weight2[0][0] = frand();
		weight2[0][1] = frand();
		weight2[1][0] = frand();
		weight2[1][1] = frand();
		weight2[2][0] = frand();
		weight2[2][1] = frand();
		bias2[0] = frand();
		bias2[1] = frand();
		bias2[2] = frand();
		weight3[0][0] = frand();
		weight3[0][1] = frand();
		weight3[0][2] = frand();
		weight3[1][0] = frand();
		weight3[1][1] = frand();
		weight3[1][2] = frand();
		bias3[0] = frand();
		bias3[1] = frand();
	}

	// side effects: al1, al2, al3
	//(3 - 2 - 3 - 2)
	double runNetwork()
	{
		double zl1[2];
		zl1[0] = al0[0] * weight1[0][0] + al0[1] * weight1[0][1] + al0[2] * weight1[0][2] + bias1[0];
		zl1[1] = al0[0] * weight1[1][0] + al0[1] * weight1[1][1] + al0[2] * weight1[1][2] + bias1[1];
		al1[0] = sigmoid(zl1[0]);
		al1[1] = sigmoid(zl1[1]);
		double zl2[3];
		zl2[0] = al1[0] * weight2[0][0] + al1[1] * weight2[0][1] + bias2[0];
		zl2[1] = al1[0] * weight2[1][0] + al1[1] * weight2[1][1] + bias2[1];
		zl2[2] = al1[0] * weight2[2][0] + al1[1] * weight2[2][1] + bias2[2];
		al2[0] = sigmoid(zl2[0]);
		al2[1] = sigmoid(zl2[1]);
		al2[2] = sigmoid(zl2[2]);
		double zl3[2];
		zl3[0] = al2[0] * weight3[0][0] + al2[1] * weight3[0][1] + al2[2] * weight3[0][2] + bias3[0];
		zl3[1] = al2[0] * weight3[1][0] + al2[1] * weight3[1][1] + al2[2] * weight3[1][2] + bias3[1];
		al3[0] = sigmoid(zl3[0]);
		al3[1] = sigmoid(zl3[1]);
		double del[2];
		del[0] = al3[0] - Y[0];
		del[1] = al3[1] - Y[1];
		double retCost = del[0] * del[0] + del[1] * del[1];
		return retCost;
	}

	// side effects: weight?, bias?, DcostDweight?BF, DcostDbias?BF, DcostDweight?CR, DcostDbias?CR, avgCost
	// ? = 1 or 2
	void gradientDescent()
	{
		S32 i, j;
		// brute force derivatives and run network for cost
		fill(&DcostDweight1CR[0][0], &DcostDweight1CR[0][0] + 2 * 3, 0.0);
		fill(&DcostDbias1CR[0], &DcostDbias1CR[0] + 2, 0.0);
		fill(&DcostDweight1BF[0][0], &DcostDweight1BF[0][0] + 2 * 3, 0.0);
		fill(&DcostDbias1BF[0], &DcostDbias1BF[0] + 2, 0.0);

		fill(&DcostDweight2CR[0][0], &DcostDweight2CR[0][0] + 3 * 2, 0.0);
		fill(&DcostDbias2CR[0], &DcostDbias2CR[0] + 3, 0.0);
		fill(&DcostDweight2BF[0][0], &DcostDweight2BF[0][0] + 3 * 2, 0.0);
		fill(&DcostDbias2BF[0], &DcostDbias2BF[0] + 3, 0.0);

		fill(&DcostDweight3CR[0][0], &DcostDweight3CR[0][0] + 2 * 3, 0.0);
		fill(&DcostDbias3CR[0], &DcostDbias3CR[0] + 2, 0.0);
		fill(&DcostDweight3BF[0][0], &DcostDweight3BF[0][0] + 2 * 3, 0.0);
		fill(&DcostDbias3BF[0], &DcostDbias3BF[0] + 2, 0.0);

		avgCost = 0.0;
		for (S32 t = 0; t < nTrain; ++t) {
			copy(&input[t][0], &input[t][0] + 3, &al0[0]);
			copy(&desired[t][0], &desired[t][0] + 2, &Y[0]);

			double cost = runNetwork(); // baseline
			// brute force derivatives
			// weight
			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 3; ++i) {
					double save = weight1[j][i];
					weight1[j][i] += epsilon;
					double costPweight = runNetwork();
					weight1[j][i] = save;
					DcostDweight1BF[j][i] += (costPweight - cost) / epsilon;
				}
			}

			for (j = 0; j < 3; ++j) {
				for (i = 0; i < 2; ++i) {
					double save = weight2[j][i];
					weight2[j][i] += epsilon;
					double costPweight = runNetwork();
					weight2[j][i] = save;
					DcostDweight2BF[j][i] += (costPweight - cost) / epsilon;
				}
			}

			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 3; ++i) {
					double save = weight3[j][i];
					weight3[j][i] += epsilon;
					double costPweight = runNetwork();
					weight3[j][i] = save;
					DcostDweight3BF[j][i] += (costPweight - cost) / epsilon;
				}
			}
			// bias
			for (j = 0; j < 2; ++j) {
				double save = bias1[j];
				bias1[j] += epsilon;
				double costPbias = runNetwork();
				bias1[j] = save;
				DcostDbias1BF[j] += (costPbias - cost) / epsilon;
			}

			for (j = 0; j < 3; ++j) {
				double save = bias2[j];
				bias2[j] += epsilon;
				double costPbias = runNetwork();
				bias2[j] = save;
				DcostDbias2BF[j] += (costPbias - cost) / epsilon;
			}
			for (j = 0; j < 2; ++j) {
				double save = bias3[j];
				bias3[j] += epsilon;
				double costPbias = runNetwork();
				bias3[j] = save;
				DcostDbias3BF[j] += (costPbias - cost) / epsilon;
			}
			runNetwork(); // need correct al? for chain rule derivatives and output
			copy(&al3[0], &al3[0] + 2, &output[t][0]);

			avgCost += cost;


			// chain rule derivatives, the last layer
			// DcostDw3 = DcostDal3 * Dal3Dzl3 * Dzl3Dw3
			// DcostDb3 = DcostDal3 * Dal3Dzl3 * Dzl3Db3
			// derived
			// DcostDal3 = 2.0*(al3 - Y)
			// Dal3Dzl3 = (1.0 - al3)*al3
			// Dzl3Dw3 = al2
			// Dzl3Db3 = 1
			double DcostDal3[2]{};
			for (j = 0; j < 2; ++j) {
				// cost
				DcostDal3[j] = 2.0*(al3[j] - Y[j]);
			}
			double DcostDzl3[2]{}; // same as DcostDb3, used for backtrace
			for (j = 0; j < 2; ++j) {
				// z
				double Da3Dz3 = (1.0 - al3[j])*al3[j];
				// bias
				DcostDzl3[j] = DcostDal3[j] * Da3Dz3; // same as DcostDb3
				DcostDbias3CR[j] += DcostDzl3[j]; // add to train/cost
				// weight
				for (i = 0; i < 3; ++i) {
					double DcostDw3 = DcostDzl3[j] * al2[i];
					DcostDweight3CR[j][i] += DcostDw3; // add to train/cost
				}
			}

			// backtrace
			// DcostDal2 = [DcostDal3 * Dal3Dzl3] * Dzl3Dal2 = DcostDzl3 * Dzl3Dal2
			double DcostDal2[3]{};
			for (j = 0; j < 3; ++j) {
				for (i = 0; i < 2; ++i) {
					DcostDal2[j] += weight3[i][j] * DcostDzl3[i];
				}
				//logger("Neural4: DcostDal2[%d] = %12.9f\n", j, DcostDal2[j]);
			}

			// chain rule derivatives, the previous layer
			// DcostDw2 = DcostDal2 * Dal2Dzl2 * Dzl2Dw2
			// DcostDb2 = DcostDal2 * Dal2Dzl2 * Dzl2Db2
			// derived
			// Dal2Dzl2 = (1.0 - al2)*al2
			// Dzl2Dw2 = al1
			// Dzl2Db2 = 1
			double DcostDzl2[3]{}; // same as DcostDb2, used for backtrace
			for (j = 0; j < 3; ++j) {
				// z
				double Da2Dz2 = (1.0 - al2[j])*al2[j];
				// bias
				DcostDzl2[j] = DcostDal2[j] * Da2Dz2; // same as DcostDb2
				DcostDbias2CR[j] += DcostDzl2[j]; // add to train/cost
				// weight
				for (i = 0; i < 2; ++i) {
					double DcostDw2 = DcostDzl2[j] * al1[i];
					DcostDweight2CR[j][i] += DcostDw2; // add to train/cost
				}
			}

			// backtrace
			// DcostDal1 = [DcostDal2 * Dal3Dzl2] * Dzl2Dal1 = DcostDzl2 * Dzl2Dal1
			double DcostDal1[2]{};
			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 3; ++i) {
					DcostDal1[j] += weight2[i][j] * DcostDzl2[i];
				}
			}

			// chain rule derivatives, the first layer
			// DcostDw1 = DcostDal1 * Dal1Dzl1 * Dzl1Dw1
			// DcostDb1 = DcostDal1 * Dal2Dzl1 * Dzl1Db1
			// derived
			// Dal1Dzl1 = (1.0 - al1)*al1
			// Dzl1Dw1 = al0
			// Dzl1Db1 = 1
			double DcostDzl1[2]{}; // same as DcostDb1, used for backtrace
			for (j = 0; j < 2; ++j) {
				// z
				double Da1Dz1 = (1.0 - al1[j])*al1[j];
				// bias
				DcostDzl1[j] = DcostDal1[j] * Da1Dz1; // same as DcostDb2
				DcostDbias1CR[j] += DcostDzl1[j]; // add to train/cost
				// weight
				for (i = 0; i < 3; ++i) {
					double DcostDw1 = DcostDzl1[j] * al0[i];
					DcostDweight1CR[j][i] += DcostDw1; // add to train/cost
				}
			}
		}
		// average
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 3; ++i) {
				DcostDweight1BF[j][i] /= nTrain;
				DcostDweight1CR[j][i] /= nTrain;
			}
		}
		for (j = 0; j < 3; ++j) {
			for (i = 0; i < 2; ++i) {
				DcostDweight2BF[j][i] /= nTrain;
				DcostDweight2CR[j][i] /= nTrain;
			}
		}
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 3; ++i) {
				DcostDweight3BF[j][i] /= nTrain;
				DcostDweight3CR[j][i] /= nTrain;
			}
		}
		for (j = 0; j < 2; ++j) {
			DcostDbias1BF[j] /= nTrain;
			DcostDbias1CR[j] /= nTrain;
		}
		for (j = 0; j < 3; ++j) {
			DcostDbias2BF[j] /= nTrain;
			DcostDbias2CR[j] /= nTrain;
		}
		for (j = 0; j < 2; ++j) {
			DcostDbias3BF[j] /= nTrain;
			DcostDbias3CR[j] /= nTrain;
		}
		avgCost /= nTrain;
		// run the gradient learn
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 3; ++i) {
				weight1[j][i] -= DcostDweight1CR[j][i] * learn;
			}
		}
		for (j = 0; j < 3; ++j) {
			for (i = 0; i < 2; ++i) {
				weight2[j][i] -= DcostDweight2CR[j][i] * learn;
			}
		}
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 3; ++i) {
				weight3[j][i] -= DcostDweight3CR[j][i] * learn;
			}
		}
		for (j = 0; j < 2; ++j) {
			bias1[j] -= DcostDbias1CR[j] * learn;
		}
		for (j = 0; j < 3; ++j) {
			bias2[j] -= DcostDbias2CR[j] * learn;
		}
		for (j = 0; j < 2; ++j) {
			bias3[j] -= DcostDbias3CR[j] * learn;
		}
	}
#endif

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
		if (KEY == 'r') { // reset weights and biases
			randomInit();
#if defined DO_NEURAL1 || defined DO_NEURAL2 || defined DO_NEURAL3 || defined DO_NEURAL4
			neuralInit();
			// TODO: reset tester if using timeb
#endif
#ifdef TESTER
			delete aTester;
			randomInit();
			aTester = new tester(aTesterTopology, inputTester, desiredTester);
#endif
			randomNextSeed();
		}
		learn = range(0.0, learn, 100.0);
		calcAmount = range(-1, calcAmount, 1000000);
		calcSpeed = range(1, calcSpeed, 10000);
		if (calcAmount != 0) {
			for (S32 step = 0; step < calcSpeed; ++step) {
#if defined DO_NEURAL1 || defined DO_NEURAL2 || defined DO_NEURAL3 || defined DO_NEURAL4
				gradientDescent();
#endif
#ifdef DO_GRAD_TEST
				gradientDescentPoly();
#endif
#ifdef TESTER
				aTester->gradientDescent();
#endif
			}
			if (calcAmount > 0) {
				--calcAmount;
			}
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


} // end namespace neural

using namespace neural;

void plot2neuralinit()
{
	// initialize graph paper
	plotter2init();
	adddebvars("neural_network", plot2neuralDeb, nplot2neuralDeb);
	randomInit();
#if defined DO_NEURAL1 || defined DO_NEURAL2 || defined DO_NEURAL3 || defined DO_NEURAL4
	neuralInit();
#endif
#ifdef DO_GRAD_TEST
	gradTestInit();
#endif
#ifdef TESTER
	randomInit();
	aTester = new tester(aTesterTopology, inputTester, desiredTester);
#endif
	randomNextSeed();
}

void plot2neuralproc()
{
	// interact with graph paper
	plotter2proc();
	// calc neural interactive
	commonProc();
}

void plot2neuraldraw2d()
{
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
}

void plot2neuralexit()
{
	// free graph paper, free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("neural_network");
#ifdef TESTER
	delete aTester;
	aTester = nullptr;
#endif
}
