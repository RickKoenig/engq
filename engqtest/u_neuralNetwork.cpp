#include <m_eng.h>
#include "u_neuralNetwork.h"

C8* neuralNet::copyStr(const C8* in) // free with delete, all string names in dbNeuralNet are allocated and are to be freed
{
	C8* ret = new C8[strlen(in) + 1];
	strcpy(ret, in);
	return ret;
}

neuralNet::neuralNet(const vector<U32>& topology
	, vector<vector<double>>& inTrain, vector<vector<double>>& desTrain
	,vector<vector<double>>& inTester, vector<vector<double>>& desTester)
	: inputs(inTrain), desireds(desTrain), nTrain(desTrain.size())
	,inputsTest(inTester), desiredsTest(desTester), nTest(desTester.size())
{
	if (inTrain.empty()) {
		errorexit("intrain needs data!");
	}
	// check dimensions of everything
	if (topology.size() < 2) {
		errorexit("topology must be 2 layers or more!");
	}
	// training
	if (inTrain.size() != desTrain.size()) {
		errorexit("training data must have same in and desired size!");
	}
	if (topology[0] != inTrain[0].size()) {
		errorexit("each training input data size must match input layer size!");
	}
	if (topology[topology.size() - 1] != desTrain[0].size()) {
		errorexit("each training desired data size must match output layer size!");
	}
	// testing
	if (inTester.size() != desTester.size()) {
		errorexit("Testing data must have same in and desired size!");
	}
	if (topology[0] != inTester[0].size()) {
		errorexit("each testing input data size must match input layer size!");
	}
	if (topology[topology.size() - 1] != desTester[0].size()) {
		errorexit("each testing desired data size must match output layer size!");
	}
	topo = topology;
	// create outputs for both training and testing
	S32 outputSize = desireds[0].size();
	outputs = vector<vector<double>>(nTrain, vector<double>(outputSize));
	outputsTest = vector<vector<double>>(nTest, vector<double>(outputSize));
	Y = vector<double>(outputSize);
	menuvar mv{ copyStr("@green@--- Neural Network ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// build a random network
	U32 k;
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
#ifdef DO_BRUTE_FORCE_DERIVATIVES
			aLayer.dCdB_BF = vector<double>(cols);
			aLayer.dCdW_BF = vector<vector<double>>(cols, vector<double>(rows));
#endif
		}
		layers.push_back(aLayer);
	}
#ifdef SHOW_WEIGHT_BIAS
	mv = { copyStr("@lightgreen@--- Weights and Biases ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// add network to debprint menu
	for (k = 1; k < layers.size(); ++k) {
		layer& lay = layers[k];
		U32 rowsW = lay.W.size();
		U32 colsW = lay.W[0].size();
		for (U32 j = 0; j < rowsW; ++j) {
			vector<double>& aRow = lay.W[j];
			for (U32 i = 0; i < colsW; ++i) {
				aRow[i] = frand();
				stringstream ssW;
				ssW << "weight" << k << "_" << j << i;
				C8* name = copyStr(ssW.str().c_str());
				mv = { name, &aRow[i], D_DOUBLE, FLOATUP / 8 };
				dbNeuralNet.push_back(mv);
			}
		}
		for (U32 j = 0; j < rowsW; ++j) {
			lay.B[j] = frand();
			stringstream ssB;
			ssB << "bias" << k << "_" << j;
			C8* name = copyStr(ssB.str().c_str());
			mv = { name, &lay.B[j], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
		}
	}
#endif
#ifdef SHOW_TRAINING_DATA
	mv = { copyStr("@yellow@--- Training data ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// add inputs to debprint menu
	for (U32 j = 0; j < nTrain; ++j) {
		vector<double>& anInput = inputs[j];
		for (U32 i = 0; i < anInput.size(); ++i) {
			stringstream ssInput;
			ssInput << "input" << j << "_" << i;
			C8* name = copyStr(ssInput.str().c_str());
			mv = { name, &anInput[i], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
		}
	}
	// add desireds and outputs to debprint menu
	for (U32 j = 0; j < nTrain; ++j) {
		vector<double>& aDesired = desireds[j];
		vector<double>& anOutput = outputs[j];
		for (U32 i = 0; i < aDesired.size(); ++i) {
			stringstream ssDesired;
			ssDesired << "desired" << j << "_" << i;
			C8* name = copyStr(ssDesired.str().c_str());
			mv = { name, &aDesired[i], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
			stringstream ssOutput;
			ssOutput << "output " << j << "_" << i;
			name = copyStr(ssOutput.str().c_str());
			mv = { name, &anOutput[i], D_DOUBLE | D_RDONLY };
			dbNeuralNet.push_back(mv);
		}
	}
#endif
	mv = { copyStr("@yellow@Training Cost Average"), &avgCost, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
	dbNeuralNet.push_back(mv);
#ifdef SHOW_TESTING_DATA
	mv = { copyStr("@brown@--- Testing data ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// add inputs to debprint menu
	for (U32 j = 0; j < nTest; ++j) {
		vector<double>& anInput = inputsTest[j];
		for (U32 i = 0; i < anInput.size(); ++i) {
			stringstream ssInput;
			ssInput << "input" << j << "_" << i;
			C8* name = copyStr(ssInput.str().c_str());
			mv = { name, &anInput[i], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
		}
	}
	// add desireds and outputs to debprint menu
	for (U32 j = 0; j < nTest; ++j) {
		vector<double>& aDesired = desiredsTest[j];
		vector<double>& anOutput = outputsTest[j];
		for (U32 i = 0; i < aDesired.size(); ++i) {
			stringstream ssDesired;
			ssDesired << "desired" << j << "_" << i;
			C8* name = copyStr(ssDesired.str().c_str());
			mv = { name, &aDesired[i], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
			stringstream ssOutput;
			ssOutput << "output " << j << "_" << i;
			name = copyStr(ssOutput.str().c_str());
			mv = { name, &anOutput[i], D_DOUBLE | D_RDONLY };
			dbNeuralNet.push_back(mv);
		}
	}
	mv = { copyStr("Testing Cost Average"), &avgCostTest, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
	dbNeuralNet.push_back(mv);
#endif
#ifdef SHOW_DERIVATIVES
	mv = { copyStr("@lightgreen@--- Derivatives of Weights and Biases ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// add derivatives to debprint menu
	for (k = 1; k < layers.size(); ++k) {
		layer& lay = layers[k];
		U32 rowsW = lay.W.size();
		U32 colsW = lay.W[0].size();
		for (U32 j = 0; j < rowsW; ++j) {
			vector<double>& aRowCR = lay.dCdW_CR[j];
#ifdef DO_BRUTE_FORCE_DERIVATIVES
			vector<double>& aRowBF = lay.dCdW_BF[j];
#endif
			for (U32 i = 0; i < colsW; ++i) {
				stringstream ssW;
				ssW << "DcostDweight" << k << "_" << j << i << "_CR";
				C8* name = copyStr(ssW.str().c_str());
				mv = { name, &aRowCR[i], D_DOUBLEEXP | D_RDONLY };
				dbNeuralNet.push_back(mv);
#ifdef DO_BRUTE_FORCE_DERIVATIVES
				ssW.str(string()); // clear
				ssW << "DcostDweight" << k << "_" << j << i << "_BF";
				name = copyStr(ssW.str().c_str());
				mv = { name, &aRowBF[i], D_DOUBLEEXP | D_RDONLY };
				dbNeuralNet.push_back(mv);
#endif
			}
		}
		for (U32 j = 0; j < rowsW; ++j) {
			stringstream ssB;
			ssB << "DcostDbias" << k << "_" << j << "_CR";
			C8* name = copyStr(ssB.str().c_str());
			mv = { name, &lay.dCdB_CR[j], D_DOUBLEEXP | D_RDONLY };
			dbNeuralNet.push_back(mv);
#ifdef DO_BRUTE_FORCE_DERIVATIVES
			ssB.str(string()); // clear
			ssB << "DcostDbias" << k << "_" << j << "_BF";
			name = copyStr(ssB.str().c_str());
			mv = { name, &lay.dCdB_BF[j], D_DOUBLEEXP | D_RDONLY };
			dbNeuralNet.push_back(mv);
#endif
		}
	}
#endif
	// publish the debprint menu
	adddebvars("neuralNet", dbNeuralNet.data(), dbNeuralNet.size());
}

neuralNet::~neuralNet()
{
	// unpublish the debprint menu
	removedebvars("neuralNet");
	for (auto mv : dbNeuralNet) {
		delete mv.name;
	}
}

double neuralNet::runNetwork()
{
	// TODO: optimize
	U32 i, j;
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

void neuralNet::testNetwork()
{
	avgCostTest = 0.0;
	vector<double>& Ain = layers[0].A;
	S32 lastLayer = topo.size() - 1;
	vector<double>& Aout = layers[lastLayer].A;
	S32 firstLayerSize = topo[0];
	S32 lastLayerSize = topo[lastLayer];
	for (U32 t = 0; t < nTest; ++t) {
		copy(&inputsTest[t][0], &inputsTest[t][0] + firstLayerSize, &Ain[0]);
		copy(&desiredsTest[t][0], &desiredsTest[t][0] + lastLayerSize, &Y[0]);
		double cost = runNetwork();
		copy(&Aout[0], &Aout[0] + lastLayerSize, &outputsTest[t][0]);
		avgCostTest += cost;
	}
	avgCostTest /= nTest;
}

void neuralNet::gradientDescent(double learn) // gradient descent
{
	S32 i, j;
	U32 k;
#ifdef DO_BRUTE_FORCE_DERIVATIVES
	// brute force derivatives and run network for cost
	for (k = 1; k < topo.size(); ++k) {
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
#endif

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
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		// TODO: optimize
		for (k = 1; k < topo.size(); ++k) {
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
#endif
		copy(&Aout[0], &Aout[0] + lastLayerSize, &outputs[t][0]);

		avgCost += cost;

		//S32 outSize = topo.size() - 1;
		//S32 ls = topo[outSize];
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
			//S32 row = Ls;
			//S32 col = Ps;
			vector<double>& AL = layers[L].A;
			vector<double>& AP = layers[P].A;
			// cost
			DcostDA = vector<double>(Ls);
			if (k == topo.size() - 1) {
				for (j = 0; j < Ls; ++j) {
					DcostDA[j] = 2.0*(AL[j] - Y[j]);
				}
			} else { // backtrace
				S32 Ns = topo[N];
				for (j = 0; j < Ls; ++j) {
					for (i = 0; i < Ns; ++i) {
						DcostDA[j] += layers[N].W[i][j] * DcostDZ[i];
					}
				}
			}
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
		}
	} // end train

	// average derivatives and cost
	// TODO: optimize
	for (k = 1; k < topo.size(); ++k) {
		S32 lastK = k - 1;
		S32 row = topo[k];
		S32 col = topo[lastK];
		for (j = 0; j < row; ++j) {
			for (i = 0; i < col; ++i) {
#ifdef DO_BRUTE_FORCE_DERIVATIVES
				layers[k].dCdW_BF[j][i] /= nTrain;
#endif
				layers[k].dCdW_CR[j][i] /= nTrain;
			}
#ifdef DO_BRUTE_FORCE_DERIVATIVES
			layers[k].dCdB_BF[j] /= nTrain;
#endif
			layers[k].dCdB_CR[j] /= nTrain;
		}
	}
	avgCost /= nTrain;

	// run the gradient descent learn
	// TODO: optimize
	for (k = 1; k < topo.size(); ++k) {
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
