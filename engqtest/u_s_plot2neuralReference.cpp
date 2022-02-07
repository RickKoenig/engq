//////// for reference ////////
#include <vector>
#include <iostream>
#include <iomanip>

#include <conio.h> // for _kbhit and maybe _getch


// lots of switches
//#define USE_TIMEB // randomize based on time
#define RANDOM_SEED 1234 // if not using TIMEB, randomize based on seed
#define INPUT 6
#define HIDDEN	6
#define OUTPUT 4
#define BIAS 1.0 // keep this at 1.0

#define DELTA .9
#define COST_ERROR .01 // get cost below this value
//#define DO_RELU // instead of sigmoid

#define NUMTESTS 60
#define EXTRACREDIT 4

//#define USE_VECTOR
// end of lots of switches


#ifdef USE_TIMEB
#include <sys/timeb.h>
#endif

using namespace std;

#ifdef USE_VECTOR
// the network nodes
vector<double> inputs(INPUT + 1);
vector<double> hidden(HIDDEN + 1);
vector<double> outputs(OUTPUT);
vector<double> desired(OUTPUT);

// network weights including bias
vector<vector<double>> wij(HIDDEN + 1, vector<double>(OUTPUT));
vector<vector<double>> wki(INPUT + 1, vector<double>(HIDDEN));
#else
// the network nodes, plus the bias
double inputs[INPUT + 1];
double hidden[HIDDEN + 1];
double outputs[OUTPUT];
double desired[OUTPUT];

// network weights including bias
double wij[HIDDEN + 1][OUTPUT];
double wki[INPUT + 1][HIDDEN];
#endif

#ifdef DO_RELU
// relu
double activate(double x)
{
	if (x > 10000000.0) {
		cout << "pretty big double" << endl;
	}
	if (x < 0) {
		return .001 * x;
	} else {
		return x;
	}
}
#else
// sigmoid
double activate(double x)
{
	return 1.0 / (1.0 + exp(-x));
}
#endif

// random number for [0 to range)
int random(int range)
{
	return rand() * range / (RAND_MAX + 1);
}

// time based or seed based
void randomInit()
{
#ifdef USE_TIMEB
	struct timeb t;
	ftime(&t);
	unsigned int seed = static_cast<unsigned int>(t.time);
#else
	unsigned int seed = RANDOM_SEED;
#endif
	srand(seed);
}

double frand()
{
	return rand() / (RAND_MAX + 1.0);
}

void runnet()
{
	int i, j, k;
	for (i = 0; i < HIDDEN; ++i) {
		hidden[i] = 0.0;
		for (k = 0; k <= INPUT; ++k) {
			hidden[i] += wki[k][i] * inputs[k];
		}
		hidden[i] = activate(hidden[i]);
	}
	for (j = 0; j < OUTPUT; ++j) {
		outputs[j] = 0.0;
		for (i = 0; i <= HIDDEN; ++i) {
			outputs[j] += wij[i][j] * hidden[i];
		}
		outputs[j] = activate(outputs[j]);
	}
}

void initbias()
{
	inputs[INPUT] = BIAS;
	hidden[HIDDEN] = BIAS;
}

void initnetwork()
{
	int i, j, k;
	for (j = 0; j < OUTPUT; ++j) {
		for (i = 0; i <= HIDDEN; ++i) {
			wij[i][j] = 1.0 - 2.0 * frand();
		}
	}
	for (i = 0; i < HIDDEN; ++i) {
		for (k = 0; k <= INPUT; ++k) {
			wki[k][i] = 1.0 - 2.0 * frand();
		}
	}
}

// this has what we want for results
//#define SIMPLE_TEST
#define ADDITION_TEST

#ifdef SIMPLE_TEST
void settest(int t)
{
	int k;
	for (k = 0; k < INPUT; ++k) {
		if (t == k) {
			if (k < OUTPUT) {
				desired[k] = 1.0;
			}
			inputs[k] = 1.0;
		} else {
			if (k < OUTPUT) {
				desired[k] = 0.0;
			}
			inputs[k] = 0.0;
		}
	}
}
#endif

#ifdef ADDITION_TEST
// test binary addition, add 2 3 bit binary numbers to get a 4 bit number
// only test first 60 out of 64 possibilites, then see how the last 4 cases came out
void settest(int t)
{
	int a;
	int b;
	a = t & 7;
	b = t >> 3;
	t = a + b; // do the addition

	inputs[0] = a & 1;
	a >>= 1;
	inputs[1] = a & 1;
	a >>= 1;
	inputs[2] = a & 1;

	inputs[3] = b & 1;
	b >>= 1;
	inputs[4] = b & 1;
	b >>= 1;
	inputs[5] = b & 1;

	desired[0] = t & 1;
	t >>= 1;
	desired[1] = t & 1;
	t >>= 1;
	desired[2] = t & 1;
	t >>= 1;
	desired[3] = t & 1;
}
#endif

double checkerr()
{
	double error = 0.0;
	int j;
	for (j = 0; j < OUTPUT; ++j) {
		double d = outputs[j] - desired[j];
		error += d * d;
	}
	return error;
}

double checkerrors()
{
	int t;
	double error = 0.0;
	for (t = 0; t < NUMTESTS; ++t) {
		settest(t);
		runnet();
		error += checkerr();
	}
	return error;
}

void train()
{
	int i, j, k;
	int t;
#ifdef USE_VECTOR
	vector<vector<double>> ewij(HIDDEN + 1, vector<double>(OUTPUT));
	vector<vector<double>> ewki(INPUT + 1, vector<double>(HIDDEN));
	vector<double> eaj(OUTPUT);
	vector<double> eij(OUTPUT);
	vector<double> eai(HIDDEN + 1);
	vector<double> eii(HIDDEN + 1);
#else
	double ewij[HIDDEN + 1][OUTPUT];
	double ewki[INPUT + 1][HIDDEN];
	double eaj[OUTPUT];
	double eij[OUTPUT];
	double eai[HIDDEN + 1];
	double eii[HIDDEN + 1];
#endif
	t = random(NUMTESTS);
	settest(t);

	runnet();

	for (j = 0; j < OUTPUT; ++j) {
		eaj[j] = outputs[j] - desired[j];
		eij[j] = eaj[j] * outputs[j] * (1 - outputs[j]);
		for (i = 0; i <= HIDDEN; ++i) {
			ewij[i][j] = eij[j] * hidden[i];
		}
	}
	for (i = 0; i <= HIDDEN; ++i) {
		eai[i] = 0;
		for (j = 0; j < OUTPUT; ++j) {
			eai[i] += eij[j] * wij[i][j];
		}
		eii[i] = eai[i] * hidden[i] * (1 - hidden[i]);
	}
	for (k = 0; k < INPUT + 1; ++k) {
		for (i = 0; i < HIDDEN; ++i) {
			ewki[k][i] = eii[i] * inputs[k];
		}
	}
	for (j = 0; j < OUTPUT; ++j) {
		for (i = 0; i < HIDDEN + 1; ++i) {
			wij[i][j] -= DELTA * ewij[i][j];
		}
	}
	for (i = 0; i < HIDDEN; ++i) {
		for (k = 0; k < INPUT + 1; ++k) {
			wki[k][i] -= DELTA * ewki[k][i];
		}
	}
}

void showweights()
{
	cout << "Show weights\n";
	cout << setprecision(4) << setfill(' ');
	int i, j, k;
	for (j = 0; j < OUTPUT; ++j) {
		cout << "wij[][" << j << "] = ";
		for (i = 0; i <= HIDDEN; ++i) {
			cout << setw(13) << wij[i][j] << " ";
		}
		cout << endl;
	}
	for (i = 0; i < HIDDEN; ++i) {
		cout << "wki[][" << i << "] = ";
		for (k = 0; k <= INPUT; ++k) {
			cout << setw(13) << wki[k][i] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void showtests()
{
	int t;
	int j;
	cout << fixed;
	cout << "results\n";
	double totalCost = 0.0;
	double totalCostExtra = 0.0;
	for (t = 0; t < NUMTESTS + EXTRACREDIT; ++t) {
		if (t == NUMTESTS) {
			cout << "extra credit\n";
		}
		settest(t);
		runnet();
		cout << setprecision(1);
		cout << " in ";
		for (j = 0; j < INPUT; ++j) {
			cout << setw(4) << inputs[j];
		}
		cout << "  hid ";
		cout << setprecision(3);
		for (j = 0; j < HIDDEN; ++j) {
			cout << setw(6) << hidden[j];
		}
		cout << "  out ";
		for (j = 0; j < OUTPUT; ++j) {
			cout << setw(6) << outputs[j];
		}
		cout << "  desired ";
		cout << setprecision(1);
		for (j = 0; j < OUTPUT; ++j) {
			cout << setw(4) << desired[j];
		}
		cout << "  cost ";
		double cost = checkerr();
		cout << setprecision(3);
		cout << setw(6) << cost;
		cout << endl;
		if (t < NUMTESTS) {
			totalCost += cost;
		}
		totalCostExtra += cost;
	}
	cout << "totalCost = " << totalCost << ", totalCostExtra = " << totalCostExtra << endl;
}

bool runTraining()
{
	bool runIt = false;
	double error;
	double oldError = DBL_MAX;
	int watchDog = 0;
	int maxwatchDog = 500'000'000; // final break if no decision
	initbias();
	initnetwork();
	while (true) {
		error = checkerrors();
		if (error < oldError) {
			cout << "(" << setw(9) << watchDog << ") " << setw(7) << error << endl;
			oldError = error * .995;
		}
		if (error < COST_ERROR) {
			break; // we're done training
		}
		bool doBreak = false;
		if (_kbhit()) {
			char c = _getch(); // leave early, abort
			switch (c) {
			case 'p': // peek
				oldError = DBL_MAX;
				break;
			case 'r': // run again
				doBreak = true;
				runIt = true;
				break;
			default:
				doBreak = true; // abort
				break;
			}
		}
		++watchDog;
		if (watchDog == maxwatchDog) {
			cout << "WATCHDOG HIT !!!";
			break;
		}
		if (doBreak) { // restart
			doBreak = false;
			break;
		}
		train();
	}
	return runIt;
}

void neural_net()
{
	bool runIt{ true }; // run it again maybe
	while (runIt) {
		runIt = false;
		ios cout_state(nullptr); // cout format state
		cout_state.copyfmt(cout); // save current cout format state
		cout << "start Neural Net\n\n";
		randomInit();
		cout << "random number = " << frand() << endl;
		cout << "Gradient descent, hit 'p' to peek at progress, 'r' to restart\n";
		cout << setprecision(4) << fixed;

		/////// do do a full training run
		runIt = runTraining(); // returns true if re run the training without asking
		/////// done do a full training run


		// show everything
		cout << "\nDone Gradient descent\n\n";
		showweights();
		showtests();
		cout << "end Neural Net\n\n";
		cout.copyfmt(cout_state); // restore cout format state
		if (!runIt) { // ask to run again
			cout << "run again? (y / n)";
			char c = _getch();
			switch (c) {
			case 'y':
				runIt = true;
				break;
			default:
				break;
			}
		}
	}
}
