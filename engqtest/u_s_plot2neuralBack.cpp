// plotter2 runs in FPU control high precision, so you can use doubles correctly if necessary
// interactively test neural networks

#include <m_eng.h>
#include "u_plotter2.h"

using namespace u_plotter2;
// Keyboard shortcuts, 'r' to reset weights and biases

// lots of switches
// random
#define DO_RANDOM // random values for weights and biases instead of preset values
#ifdef DO_RANDOM
//#define USE_TIMEB // randomize based on time
U32 randomSeed = 123456;
//#define RANDOM_SEED 123456 // if not using USE_TIMEB, randomize based on this seed
#endif

// sub switches, what to run in this state
//#define DO_NEURAL1 // Layers 1 (1 - 1), 2 vars, 2 costs
//#define DO_NEURAL2 // Layers 2 (1 - 1 - 1), 4 vars, 2 costs
//#define DO_NEURAL3 // Layers 2 (2 - 2 - 2), 12 vars, 4 costs
#define DO_NEURAL4 // Layers 3 (3 - 2 - 3 - 2), 25 vars, 4 costs
//#define DO_NEURAL5 // Layers 3 (6 - 6 - 4), 70 vars, 60 costs // TODO: check 4 more costs for a total of 64 states
//#define DO_NEURAL6 // Layers 3 (784 - 16 - 16 - 10), 13,002 vars, 60,000 costs // TODO: check 10,000 more costs
//#define DO_GRAD_TEST // 1 var, minimize the adjustable quartic equation
#define SHOW_SIGMOID

#ifdef USE_TIMEB
#include <sys/timeb.h>
#endif

namespace neural {
	// common data
	// calc gradient descent
	double learn = 0.0; // .03125;
	S32 calcAmount = -1;
	S32 calcSpeed = 1;
	// for brute force derivatives
	const double epsilon = 1e-8;

#ifdef DO_NEURAL1
	// network
	double weight1;
	double bias1;
	// per cost
	double al0; // current input
	double al1; // current output
	double Y; // current desired output

	// inputs, desires, cost
	const S32 nTrain = 2;
	double input[nTrain] = { .3, .4 };
	double output[nTrain];
	double desired[nTrain] = { .8, .5 };
	double avgCost;

	// cost derivatives for nTrain
	// chain rule
	double DcostDweight1CR;
	double DcostDbias1CR;
	// brute force
	double DcostDweight1BF;
	double DcostDbias1BF;
#endif
#ifdef DO_NEURAL2
	// network
	double weight1;
	double bias1;
	double weight2;
	double bias2;
	// per cost
	double al0; // current input
	double al1; // current hidden layer
	double al2; // current output
	double Y; // current desired output

	// inputs, desires, cost
	const S32 nTrain = 2;
	double input[nTrain] = { .3, .4 };
	double output[nTrain];
	double desired[nTrain] = { .8, .5 };
	double avgCost;

	// cost derivatives for nTrain
	// chain rule
	double DcostDweight1CR;
	double DcostDbias1CR;
	double DcostDweight2CR;
	double DcostDbias2CR;
	// brute force
	double DcostDweight1BF;
	double DcostDbias1BF;
	double DcostDweight2BF;
	double DcostDbias2BF;
#endif
#ifdef DO_NEURAL3
	// (2 - 2 - 2) // for now hard coded, but using arrays
	// network
	double weight1[2][2];
	double bias1[2];
	double weight2[2][2];
	double bias2[2];
	// per cost
	double al0[2]; // current input
	double al1[2]; // current hidden layer
	double al2[2]; // current output
	double Y[2]; // current desired output

	// inputs, desires, train/cost
	const S32 nTrain = 4;
	double input[nTrain][2] = { {.12, .16},  {.13, .97},  {.94, .18},  {.95, .99} };
	double output[nTrain][2];
	double desired[nTrain][2] = { {.11, .12}, {.93, .14}, {.95, .16}, {.97, .98} }; // for now try or and and gates
	double avgCost;

	// cost derivatives for nTrain
	// chain rule
	double DcostDweight1CR[2][2];
	double DcostDbias1CR[2];
	double DcostDweight2CR[2][2];
	double DcostDbias2CR[2];
	// brute force
	double DcostDweight1BF[2][2];
	double DcostDbias1BF[2];
	double DcostDweight2BF[2][2];
	double DcostDbias2BF[2];
#endif
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
	// for debvars
	struct menuvar plot2neuralDeb[] = {
		{"@yellow@--- neural network vars ---", NULL, D_VOID, 0},
		{"calcAmount", &calcAmount, D_INT, 1},
		{"calcSpeed", &calcSpeed, D_INT, 32},
		{"learn", &learn, D_DOUBLE, FLOATUP / 32},
#ifdef DO_NEURAL1
		// network
		{"weight1", &weight1, D_DOUBLE, FLOATUP / 8},
		{"bias1", &bias1, D_DOUBLE, FLOATUP / 8},

		// inputs, desires, outputs, cost
		{"input0", &input[0], D_DOUBLE, FLOATUP / 8},
		{"input1", &input[1], D_DOUBLE, FLOATUP / 8},
		{"desired0", &desired[0], D_DOUBLE, FLOATUP / 8},
		{"desired1", &desired[1], D_DOUBLE, FLOATUP / 8},
		{"output0", &output[0], D_DOUBLE | D_RDONLY},
		{"output1", &output[1], D_DOUBLE | D_RDONLY},
		{"costAverage", &avgCost, D_DOUBLEEXP | D_RDONLY},

		// cost derivatives
		// chain rule
		{"DcostDweight1_cr", &DcostDweight1CR,D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias1  _cr", &DcostDbias1CR,D_DOUBLEEXP | D_RDONLY},
		// brute force
		{"DcostDweight1_bf", &DcostDweight1BF, D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias1  _bf", &DcostDbias1BF, D_DOUBLEEXP | D_RDONLY},
#endif
#ifdef DO_NEURAL2
		// network
		{"weight1", &weight1, D_DOUBLE, FLOATUP / 8},
		{"bias1", &bias1, D_DOUBLE, FLOATUP / 8},
		{"weight2", &weight2, D_DOUBLE, FLOATUP / 8},
		{"bias2", &bias2, D_DOUBLE, FLOATUP / 8},

		// inputs, desires, outputs, cost
		{"input0", &input[0], D_DOUBLE, FLOATUP / 8},
		{"input1", &input[1], D_DOUBLE, FLOATUP / 8},
		{"desired0", &desired[0], D_DOUBLE, FLOATUP / 8},
		{"desired1", &desired[1], D_DOUBLE, FLOATUP / 8},
		{"output0", &output[0], D_DOUBLE | D_RDONLY},
		{"output1", &output[1], D_DOUBLE | D_RDONLY},
		{"costAverage", &avgCost, D_DOUBLEEXP | D_RDONLY},

		// cost derivatives
		// chain rule and brute force
		{"DcostDweight1_CR", &DcostDweight1CR,D_DOUBLEEXP | D_RDONLY},
		{"DcostDweight1_BF", &DcostDweight1BF, D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias1  _CR", &DcostDbias1CR,D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias1  _BF", &DcostDbias1BF, D_DOUBLEEXP | D_RDONLY},
		{"DcostDweight2_CR", &DcostDweight2CR,D_DOUBLEEXP | D_RDONLY},
		{"DcostDweight2_BF", &DcostDweight2BF, D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias2  _CR", &DcostDbias2CR,D_DOUBLEEXP | D_RDONLY},
		{"DcostDbias2  _BF", &DcostDbias2BF, D_DOUBLEEXP | D_RDONLY},
#endif
#ifdef DO_NEURAL3
		// network
		{"weight1_00", &weight1[0][0], D_DOUBLE, FLOATUP / 8},
		{"weight1_01", &weight1[0][1], D_DOUBLE, FLOATUP / 8},
		{"weight1_10", &weight1[1][0], D_DOUBLE, FLOATUP / 8},
		{"weight1_11", &weight1[1][1], D_DOUBLE, FLOATUP / 8},
		{"bias1_0", &bias1[0], D_DOUBLE, FLOATUP / 8},
		{"bias1_1", &bias1[1], D_DOUBLE, FLOATUP / 8},
		{"weight2_00", &weight2[0][0], D_DOUBLE, FLOATUP / 8},
		{"weight2_01", &weight2[0][1], D_DOUBLE, FLOATUP / 8},
		{"weight2_10", &weight2[1][0], D_DOUBLE, FLOATUP / 8},
		{"weight2_11", &weight2[1][1], D_DOUBLE, FLOATUP / 8},
		{"bias2_0", &bias2[0], D_DOUBLE, FLOATUP / 8},
		{"bias2_1", &bias2[1], D_DOUBLE, FLOATUP / 8},

		// inputs, desires, outputs, cost
		{ "input0_0", &input[0][0], D_DOUBLE, FLOATUP / 8 },
		{ "input0_1", &input[0][1], D_DOUBLE, FLOATUP / 8 },
		{ "input1_0", &input[1][0], D_DOUBLE, FLOATUP / 8 },
		{ "input1_1", &input[1][1], D_DOUBLE, FLOATUP / 8 },
		{ "input2_0", &input[2][0], D_DOUBLE, FLOATUP / 8 },
		{ "input2_1", &input[2][1], D_DOUBLE, FLOATUP / 8 },
		{ "input3_0", &input[3][0], D_DOUBLE, FLOATUP / 8 },
		{ "input3_1", &input[3][1], D_DOUBLE, FLOATUP / 8 },
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

		// cost derivatives
		// chain rule and brute force
		{ "DcostDweight1_00_CR", &DcostDweight1CR[0][0], D_DOUBLEEXP | D_RDONLY},
		{ "DcostDweight1_00_BF", &DcostDweight1BF[0][0], D_DOUBLEEXP | D_RDONLY},
		{ "DcostDweight1_01_CR", &DcostDweight1CR[0][1], D_DOUBLEEXP | D_RDONLY},
		{ "DcostDweight1_01_BF", &DcostDweight1BF[0][1], D_DOUBLEEXP | D_RDONLY},
		{ "DcostDweight1_10_CR", &DcostDweight1CR[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_10_BF", &DcostDweight1BF[1][0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_11_CR", &DcostDweight1CR[1][1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDweight1_11_BF", &DcostDweight1BF[1][1], D_DOUBLEEXP | D_RDONLY },

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

		{ "DcostDbias2_0_CR"   , &DcostDbias2CR[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_0_BF"   , &DcostDbias2BF[0], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_1_CR"   , &DcostDbias2CR[1], D_DOUBLEEXP | D_RDONLY },
		{ "DcostDbias2_1_BF"   , &DcostDbias2BF[1], D_DOUBLEEXP | D_RDONLY },
#endif
#ifdef DO_NEURAL4
		// network
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

	// if needs random stuff
#ifdef DO_RANDOM
	// time based or seed based
	void randomInit()
	{
#ifdef USE_TIMEB
		struct timeb t;
		ftime(&t);
		unsigned int seed = static_cast<unsigned int>(t.millitm);
#else
		unsigned int seed = randomSeed++;
#endif
		srand(seed);
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
#endif // DO_RANDOM

#ifdef DO_NEURAL1
	// neural (1 - 1) 2 vars, 2 cost
	void neuralInit()
	{
#ifdef DO_RANDOM
		randomInit();
		weight1 = frand();
		bias1 = frand();
#else
		weight1 = .123;
		bias1 = .223;
#endif
	}

	// side effects: al1
	double runNetwork()
	{
		double zl1 = al0 * weight1 + bias1;
		al1 = sigmoid(zl1);
		double del = al1 - Y;
		double retCost = del * del;
		return retCost;
	}

	// side effects: weight1, bias1, DcostDweight1BF, DcostDbias1BF, DcostDweight1CR, DcostDbias1CR, avgCost
	void gradientDescent()
	{
		double saveWeight1 = weight1;
		double saveBias1 = bias1;
		// brute force derivatives and run network for cost
		DcostDweight1CR = 0.0;
		DcostDbias1CR = 0.0;
		DcostDweight1BF = 0.0;
		DcostDbias1BF = 0.0;
		avgCost = 0.0;
		for (S32 t = 0; t < nTrain; ++t) {
			al0 = input[t];
			Y = desired[t];

			// brute force derivatives
			// weight
			weight1 += epsilon;
			double costPweight1 = runNetwork();
			weight1 = saveWeight1;
			// bias
			bias1 += epsilon;
			double costPbias1 = runNetwork();
			bias1 = saveBias1;
			// none
			double cost = runNetwork();
			output[t] = al1; // need al1 for chain rule derivatives
			DcostDweight1BF += (costPweight1 - cost) / epsilon;
			DcostDbias1BF += (costPbias1 - cost) / epsilon;

			avgCost += cost;

			// chain rule derivatives
			// DcostDw1 = DcostDal1 * Dal1Dzl1 * Dzl1Dw1
			// DcostDb1 = DcostDal1 * Dal1Dzl1 * Dzl1Db1
			// DcostDal1 = 2.0 * (al1 - Y)
			// Dal1Dzl1 = (1.0 - al1) * al1
			// Dzl1Dw1 = al0
			// Dzl1Db1 = 1
			double DcostDb1 = 2.0 * (al1 - Y) * (1.0 - al1) * al1;
			double DcostDw1 = al0 * DcostDb1;
			DcostDbias1CR += DcostDb1;
			DcostDweight1CR += DcostDw1;

		}
		// average
		DcostDweight1BF /= nTrain;
		DcostDbias1BF /= nTrain;
		DcostDweight1CR /= nTrain;
		DcostDbias1CR /= nTrain;
		avgCost /= nTrain;
		// run the gradient learn
		weight1 -= DcostDweight1CR * learn;
		bias1 -= DcostDbias1CR * learn;
	}
#endif

#ifdef DO_NEURAL2
	// neural (1 - 1 - 1) 4 vars, 2 cost
	void neuralInit()
	{
#ifdef DO_RANDOM
		randomInit();
		weight1 = frand();
		bias1 = frand();
		weight2 = frand();
		bias2 = frand();
#else
		weight1 = .123;
		bias1 = .223;
		weight2 = .323;
		bias2 = .423;
#endif
	}

	// side effects: al1, al2
	double runNetwork()
	{
		double zl1 = al0 * weight1 + bias1;
		al1 = sigmoid(zl1);
		double zl2 = al1 * weight2 + bias2;
		al2 = sigmoid(zl2);
		double del = al2 - Y;
		double retCost = del * del;
		return retCost;
	}

	// side effects: weight?, bias?, DcostDweight?BF, DcostDbias?BF, DcostDweight?CR, DcostDbias?CR, avgCost
	// ? = 1 or 2
	void gradientDescent()
	{
		double saveWeight1 = weight1;
		double saveBias1 = bias1;
		double saveWeight2 = weight2;
		double saveBias2 = bias2;
		// brute force derivatives and run network for cost
		DcostDweight1CR = 0.0;
		DcostDbias1CR = 0.0;
		DcostDweight1BF = 0.0;
		DcostDbias1BF = 0.0;
		DcostDweight2CR = 0.0;
		DcostDbias2CR = 0.0;
		DcostDweight2BF = 0.0;
		DcostDbias2BF = 0.0;
		avgCost = 0.0;
		for (S32 t = 0; t < nTrain; ++t) {
			al0 = input[t];
			Y = desired[t];

			// brute force derivatives
			// weight
			weight1 += epsilon;
			double costPweight1 = runNetwork();
			weight1 = saveWeight1;
			weight2 += epsilon;
			double costPweight2 = runNetwork();
			weight2 = saveWeight2;
			// bias
			bias1 += epsilon;
			double costPbias1 = runNetwork();
			bias1 = saveBias1;
			bias2 += epsilon;
			double costPbias2 = runNetwork();
			bias2 = saveBias2;
			// none
			double cost = runNetwork();
			output[t] = al2; // need al? for chain rule derivatives
			DcostDweight1BF += (costPweight1 - cost) / epsilon;
			DcostDbias1BF += (costPbias1 - cost) / epsilon;
			DcostDweight2BF += (costPweight2 - cost) / epsilon;
			DcostDbias2BF += (costPbias2 - cost) / epsilon;

			avgCost += cost;

			// chain rule derivatives
			// DcostDw2 = DcostDal2 * Dal2Dzl2 * Dzl2Dw2
			// DcostDb2 = DcostDal2 * Dal2Dzl2 * Dzl2Db2
			// DcostDal2 = 2.0*(al2 - Y)
			// Dal1Dzl2 = (1.0 - al2)*al2
			// Dzl2Dw2 = al1
			// Dzl2Db2 = 1
			double DcostDb2 = 2.0*(al2 - Y) * (1.0 - al2)*al2;
			double DcostDw2 = DcostDb2 * al1;
			DcostDbias2CR += DcostDb2;
			DcostDweight2CR += DcostDw2;

			// back one layer
			// DcostDw1 = DcostDal2 * Dal2Dzl2 * Dzl2Dal1 * Dal1Dzl1 * Dzl1Dw1
			// DcostDb1 = DcostDal2 * Dal2Dzl2 * Dzl2Dal1 * Dal1Dzl1 * Dzl1Db1
			// DcostDal2 * Dal2Dzl2 = DcostDb2
			// Dzl2Dal1 = weight2
			// Dal1Dzl1 = (1.0 - al1)*al1
			// Dzl1Dw1 = al0
			// Dzl1Db1 = 1
			double DcostDb1 = DcostDb2 * weight2 * (1.0 - al1) * al1;
			double DcostDw1 = DcostDb1 * al0;
			DcostDbias1CR += DcostDb1;
			DcostDweight1CR += DcostDw1;

		}
		// average
		DcostDweight1BF /= nTrain;
		DcostDbias1BF /= nTrain;
		DcostDweight1CR /= nTrain;
		DcostDbias1CR /= nTrain;
		DcostDweight2BF /= nTrain;
		DcostDbias2BF /= nTrain;
		DcostDweight2CR /= nTrain;
		DcostDbias2CR /= nTrain;
		avgCost /= nTrain;
		// run the gradient learn
		weight1 -= DcostDweight1CR * learn;
		bias1 -= DcostDbias1CR * learn;
		weight2 -= DcostDweight2CR * learn;
		bias2 -= DcostDbias2CR * learn;
	}
#endif
#ifdef DO_NEURAL3
	// neural (2 - 2 - 2) 12 vars, 4 cost
	void neuralInit()
	{
#ifdef DO_RANDOM
		randomInit();
		weight1[0][0] = frand();
		weight1[0][1] = frand();
		weight1[1][0] = frand();
		weight1[1][1] = frand();
		bias1[0] = frand();
		bias1[1] = frand();
		weight2[0][0] = frand();
		weight2[0][1] = frand();
		weight2[1][0] = frand();
		weight2[1][1] = frand();
		bias2[0] = frand();
		bias2[1] = frand();
#else
		weight1[0][0] = .05;
		weight1[0][1] = .155;
		weight1[1][0] = .25;
		weight1[1][1] = .355;
		bias1[0] = .45;
		bias1[1] = .105;
		weight2[0][0] = .2;
		weight2[0][1] = .305;
		weight2[1][0] = .4;
		weight2[1][1] = .565;
		bias2[0] = .67;
		bias2[1] = .78;
#endif
	}

	// side effects: al1, al2
	double runNetwork()
	{
		double zl1[2];
		zl1[0] = al0[0] * weight1[0][0] + al0[1] * weight1[0][1] + bias1[0];
		zl1[1] = al0[0] * weight1[1][0] + al0[1] * weight1[1][1] + bias1[1];
		al1[0] = sigmoid(zl1[0]);
		al1[1] = sigmoid(zl1[1]);
		double zl2[2];
		zl2[0] = al1[0] * weight2[0][0] + al1[1] * weight2[0][1] + bias2[0];
		zl2[1] = al1[0] * weight2[1][0] + al1[1] * weight2[1][1] + bias2[1];
		al2[0] = sigmoid(zl2[0]);
		al2[1] = sigmoid(zl2[1]);
		double del[2];
		del[0] = al2[0] - Y[0];
		del[1] = al2[1] - Y[1];
		double retCost = del[0] * del[0] + del[1] * del[1];
		return retCost;
	}

	// side effects: weight?, bias?, DcostDweight?BF, DcostDbias?BF, DcostDweight?CR, DcostDbias?CR, avgCost
	// ? = 1 or 2
	void gradientDescent()
	{
		S32 i, j;// , k;
		// brute force derivatives and run network for cost
		fill(&DcostDweight1CR[0][0], &DcostDweight1CR[0][0] + 2 * 2, 0.0);
		fill(&DcostDbias1CR[0], &DcostDbias1CR[0] + 2, 0.0);
		fill(&DcostDweight1BF[0][0], &DcostDweight1BF[0][0] + 2 * 2, 0.0);
		fill(&DcostDbias1BF[0], &DcostDbias1BF[0] + 2, 0.0);
		fill(&DcostDweight2CR[0][0], &DcostDweight2CR[0][0] + 2 * 2, 0.0);
		fill(&DcostDbias2CR[0], &DcostDbias2CR[0] + 2, 0.0);
		fill(&DcostDweight2BF[0][0], &DcostDweight2BF[0][0] + 2 * 2, 0.0);
		fill(&DcostDbias2BF[0], &DcostDbias2BF[0] + 2, 0.0);
		avgCost = 0.0;
		for (S32 t = 0; t < nTrain; ++t) {
			copy(&input[t][0], &input[t][0] + 2, &al0[0]);
			copy(&desired[t][0], &desired[t][0] + 2, &Y[0]);

			double cost = runNetwork();
			// brute force derivatives
			// weight
			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 2; ++i) {
					double save = weight1[j][i];
					weight1[j][i] += epsilon;
					double costPweight = runNetwork();
					weight1[j][i] = save;
					DcostDweight1BF[j][i] += (costPweight - cost) / epsilon;
					save = weight2[j][i];
					weight2[j][i] += epsilon;
					costPweight = runNetwork();
					weight2[j][i] = save;
					DcostDweight2BF[j][i] += (costPweight - cost) / epsilon;
				}
			}
			// bias
			for (j = 0; j < 2; ++j) {
				double save = bias1[j];
				bias1[j] += epsilon;
				double costPbias = runNetwork();
				bias1[j] = save;
				DcostDbias1BF[j] += (costPbias - cost) / epsilon;
				runNetwork(); // need correct al? for chain rule derivatives

				save = bias2[j];
				bias2[j] += epsilon;
				costPbias = runNetwork();
				bias2[j] = save;
				DcostDbias2BF[j] += (costPbias - cost) / epsilon;
				runNetwork(); // need correct al? for chain rule derivatives
			}
			copy(&al2[0], &al2[0] + 2, &output[t][0]);

			avgCost += cost;
			// chain rule derivatives, the last layer
			// DcostDw2 = DcostDal2 * Dal2Dzl2 * Dzl2Dw2
			// DcostDb2 = DcostDal2 * Dal2Dzl2 * Dzl2Db2
			// derived
			// DcostDal2 = 2.0*(al2 - Y)
			// Dal2Dzl2 = (1.0 - al2)*al2
			// Dzl2Dw2 = al1
			// Dzl2Db2 = 1
			double DcostDzl2[2]{}; // same as DcostDb2, used for backtrace
			double DcostDal1[2]{};
			for (j = 0; j < 2; ++j) {
				// cost
				double DcostDal2 = 2.0*(al2[j] - Y[j]);
				// z
				double Da2Dz2 = (1.0 - al2[j])*al2[j];
				// bias
				DcostDzl2[j] = DcostDal2 * Da2Dz2; // same as DcostDb2
				DcostDbias2CR[j] += DcostDzl2[j]; // add to train/cost
				// weight
				for (i = 0; i < 2; ++i) {
					double DcostDw2 = DcostDzl2[j] * al1[i];
					DcostDweight2CR[j][i] += DcostDw2; // add to train/cost
				}
			}
			double DcostDzl1[2]{}; // same as DcostDb1, used for backtrace
			// backtrace
			// DcostDal1 = [DcostDal2 * Dal2Dzl2] * Dzl2Dal1 = DcostDzl2 * Dzl2Dal1
			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 2; ++i) {
					DcostDal1[j] += weight2[i][j] * DcostDzl2[i];
				}
			}

			// chain rule derivatives, the previous layer
			// DcostDw1 = DcostDal1 * Dal1Dzl1 * Dzl1Dw1
			// DcostDb1 = DcostDal1 * Dal1Dzl1 * Dzl1Db1
			// derived
			// Dal1Dzl1 = (1.0 - al1)*al1
			// Dzl1Dw1 = al0
			// Dzl1Db1 = 1
			for (j = 0; j < 2; ++j) {
				// z
				double Da1Dz1 = (1.0 - al1[j])*al1[j];
				// bias
				DcostDzl1[j] = DcostDal1[j] * Da1Dz1; // same as DcostDb1
				DcostDbias1CR[j] += DcostDzl1[j]; // add to train/cost
				// weight
				for (i = 0; i < 2; ++i) {
					double DcostDw1 = DcostDzl1[j] * al0[i];
					DcostDweight1CR[j][i] += DcostDw1; // add to train/cost
				}
			}
		}
		// average
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 2; ++i) {
				DcostDweight1BF[j][i] /= nTrain;
				DcostDweight1CR[j][i] /= nTrain;
				DcostDweight2BF[j][i] /= nTrain;
				DcostDweight2CR[j][i] /= nTrain;
			}
		}
		for (j = 0; j < 2; ++j) {
			DcostDbias1BF[j] /= nTrain;
			DcostDbias1CR[j] /= nTrain;
			DcostDbias2BF[j] /= nTrain;
			DcostDbias2CR[j] /= nTrain;
		}
		avgCost /= nTrain;
		// run the gradient learn
		for (j = 0; j < 2; ++j) {
			for (i = 0; i < 2; ++i) {
				weight1[j][i] -= DcostDweight1CR[j][i] * learn;
				weight2[j][i] -= DcostDweight2CR[j][i] * learn;
			}
		}
		for (j = 0; j < 2; ++j) {
			bias1[j] -= DcostDbias1CR[j] * learn;
			bias2[j] -= DcostDbias2CR[j] * learn;
		}
	}
#endif
#ifdef DO_NEURAL4
	// neural (3 - 2 - 3 - 2) 25 vars, 4 cost
	void neuralInit()
	{
#ifdef DO_RANDOM
		randomInit();
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
#else
		weight1[0][0] = .05;
		weight1[0][1] = .155;
		weight1[0][2] = .255;
		weight1[1][0] = .25;
		weight1[1][1] = .455;
		weight1[1][2] = .555;
		bias1[0] = .45;
		bias1[1] = .105;
		weight2[0][0] = .2;
		weight2[0][1] = .305;
		weight2[1][0] = .4;
		weight2[1][1] = .565;
		weight2[2][0] = .478;
		weight2[2][1] = .578;
		bias2[0] = .67;
		bias2[1] = .78;
		bias2[2] = .91;
		weight3[0][0] = .23;
		weight3[0][1] = .31053;
		weight3[0][2] = .3053;
		weight3[1][0] = .41303;
		weight3[1][1] = .5653;
		weight3[1][2] = .75653;
		bias3[0] = .673;
		bias3[1] = .783;
#endif
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

			double cost = runNetwork();
			// brute force derivatives
			// weight
			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 3; ++i) {
					double save = weight1[j][i];
					weight1[j][i] += epsilon;
					double costPweight = runNetwork();
					weight1[j][i] = save;
					DcostDweight1BF[j][i] += (costPweight - cost) / epsilon;
					//runNetwork(); // need correct al? for chain rule derivatives
				}
			}

			for (j = 0; j < 3; ++j) {
				for (i = 0; i < 2; ++i) {
					double save = weight2[j][i];
					weight2[j][i] += epsilon;
					double costPweight = runNetwork();
					weight2[j][i] = save;
					DcostDweight2BF[j][i] += (costPweight - cost) / epsilon;
					//runNetwork(); // need correct al? for chain rule derivatives
				}
			}

			for (j = 0; j < 2; ++j) {
				for (i = 0; i < 3; ++i) {
					double save = weight3[j][i];
					weight3[j][i] += epsilon;
					double costPweight = runNetwork();
					weight3[j][i] = save;
					DcostDweight3BF[j][i] += (costPweight - cost) / epsilon;
					//runNetwork(); // need correct al? for chain rule derivatives
				}
			}
			// bias
			for (j = 0; j < 2; ++j) {
				double save = bias1[j];
				bias1[j] += epsilon;
				double costPbias = runNetwork();
				bias1[j] = save;
				DcostDbias1BF[j] += (costPbias - cost) / epsilon;
				//runNetwork(); // need correct al? for chain rule derivatives
			}

			for (j = 0; j < 3; ++j) {
				double save = bias2[j];
				bias2[j] += epsilon;
				double costPbias = runNetwork();
				bias2[j] = save;
				DcostDbias2BF[j] += (costPbias - cost) / epsilon;
				//runNetwork(); // need correct al? for chain rule derivatives
			}
			for (j = 0; j < 2; ++j) {
				double save = bias3[j];
				bias3[j] += epsilon;
				double costPbias = runNetwork();
				bias3[j] = save;
				DcostDbias3BF[j] += (costPbias - cost) / epsilon;
				//runNetwork(); // need correct al? for chain rule derivatives

			}
			copy(&al3[0], &al3[0] + 2, &output[t][0]);

			avgCost += cost;

			runNetwork(); // need correct al? for chain rule derivatives

			// chain rule derivatives, the last layer
			// DcostDw3 = DcostDal3 * Dal3Dzl3 * Dzl3Dw3
			// DcostDb3 = DcostDal3 * Dal3Dzl3 * Dzl3Db3
			// derived
			// DcostDal3 = 2.0*(al3 - Y)
			// Dal3Dzl3 = (1.0 - al3)*al3
			// Dzl3Dw3 = al2
			// Dzl3Db3 = 1
			double DcostDzl3[2]{}; // same as DcostDb3, used for backtrace
			double DcostDal2[3]{};
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
			double DcostDal1[2]{};
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
		//y = x(4ax2 + 3bx + 2c) + d
		//y = x(x(4ax + 3b) + 2c) + d
		//return x * (4.0 * A + 3.0 * B)
		//return x * (x * (x * (x * Acoeff + Bcoeff) + Ccoeff) + Dcoeff) + Ecoeff;
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
#if defined DO_NEURAL1 || defined DO_NEURAL2 || defined DO_NEURAL3 || defined DO_NEURAL4
		if (KEY == 'r') {
			neuralInit();
		}
#endif
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
#if defined DO_NEURAL1 || defined DO_NEURAL2 || defined DO_NEURAL3 || defined DO_NEURAL4
	neuralInit();
#endif
#ifdef DO_GRAD_TEST
	gradTestInit();
#endif
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
}
