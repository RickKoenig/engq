#include <m_eng.h>
#include "u_neuralNetwork.h"
#include "m_perf.h"

C8* neuralNet::copyStr(const C8* in) // free with delete, all string names in dbNeuralNet are allocated and are to be freed
{
	C8* ret = new C8[strlen(in) + 1];
	strcpy(ret, in);
	return ret;
}

neuralNet::neuralNet(const string& namea, const vector<U32>& topology
	, vector<vector<double>>& inTrain, vector<vector<double>>& desTrain
	,vector<vector<double>>& inTester, vector<vector<double>>& desTester)
	: name(namea), inputs(inTrain), desireds(desTrain), nTrain(desTrain.size())
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
	menuvar mv{ copyStr("@cyan@--- Neural Network ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	stringstream title;
	title << name << " [ ";
	for (auto v : topo) {
		title << v << " ";
	}
	title << "]";

	mv = { copyStr(title.str().c_str()), NULL, D_VOID, 0 }; // name of the neural network
	dbNeuralNet.push_back(mv);
	// build a random network
	U32 k;
	for (k = 0; k < topology.size(); ++k) {
		S32 cols = topology[k];
		layer aLayer;
		if (k > 0) {
			if (k != topology.size() - 1) {
				// no need for A[0] or A[nL - 1], it comes from input and output
				aLayer.A = vector<double>(cols);
			}
			// Layer 0 has none of this, it's the input layer
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
#endif
	// add network to debprint menu
	for (k = 1; k < layers.size(); ++k) {
		layer& lay = layers[k];
		U32 rowsW = lay.W.size();
		U32 colsW = lay.W[0].size();
		for (U32 j = 0; j < rowsW; ++j) {
			vector<double>& aRow = lay.W[j];
			for (U32 i = 0; i < colsW; ++i) {
				aRow[i] = frand();
#ifdef SHOW_WEIGHT_BIAS
				stringstream ssW;
				ssW << "weight" << k << "_" << j << i;
				C8* name = copyStr(ssW.str().c_str());
				mv = { name, &aRow[i], D_DOUBLE, FLOATUP / 8 };
				dbNeuralNet.push_back(mv);
#endif
			}
		}
		for (U32 j = 0; j < rowsW; ++j) {
			lay.B[j] = frand();
#ifdef SHOW_WEIGHT_BIAS
			stringstream ssB;
			ssB << "bias" << k << "_" << j;
			C8* name = copyStr(ssB.str().c_str());
			mv = { name, &lay.B[j], D_DOUBLE, FLOATUP / 8 };
			dbNeuralNet.push_back(mv);
#endif
		}
	}
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
	mv = { copyStr("Training Cost Total"), &totalCost, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
	dbNeuralNet.push_back(mv);
#ifdef SHOW_TESTING_DATA
	mv = { copyStr("@brown@--- Testing data ---"), NULL, D_VOID, 0 };
	dbNeuralNet.push_back(mv);
	// add testing inputs to debprint menu
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
	// add testing desireds and outputs to debprint menu
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
#endif
	mv = { copyStr("@brown@Testing Cost Average"), &avgCostTest, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
	dbNeuralNet.push_back(mv);
	mv = { copyStr("Testing Cost Total"), &totalCostTest, D_DOUBLEEXP | D_RDONLY, FLOATUP / 8 };
	dbNeuralNet.push_back(mv);
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

bool neuralNet::loadNetwork(U32 slot)
{
	//return false;
	pushandsetdir("neural");
	stringstream ss;
	ss << name << "_" << slot << ".mdl";
	string fname = ss.str();
	if (!fileexist(fname.c_str())) {
		popdir();
		return false; // can't open file, doesn't exist
	}
	FILE* fh = fopen2(fname.c_str(), "rb");
	if (!fh) {
		popdir();
		return false; // can't open file
	}
	// load and check topo
	U32 ts = filereadU32LE(fh);
	if (ts != topo.size()) {
		fclose(fh); // wrong topology size
		popdir();
		return false;
	}
	vector<U32> tpo (topo.size());
	fileread(fh, &tpo[0], topo.size() * sizeof tpo[0]);
	if (tpo != topo) {
		fclose(fh); // wrong topology configuration
		popdir();
		return false;
	}
	// load weights and biases for each layer
	for (U32 k = 1; k < topo.size(); ++k) {
		for (U32 j = 0; j < topo[k]; ++j) {
			fileread(fh, &layers[k].W[j][0], topo[k - 1] * sizeof layers[k].W[0][0]);
		}
		fileread(fh, &layers[k].B[0], topo[k] * sizeof layers[k].B[0]);
	}
	fclose(fh);
	popdir();
	return true;
}

void neuralNet::saveNetwork(U32 slot)
{
	pushandsetdir("neural");
	stringstream ss;
	ss << name << "_" << slot << ".mdl";
	FILE* fh = fopen2(ss.str().c_str(), "wb");
	// save topo
	filewriteU32LE(fh, topo.size());
	filewrite(fh, &topo[0], topo.size() * sizeof topo[0] );
	// save weights and biases for each layer
	for (U32 k = 1; k < topo.size(); ++k) {
		for (U32 j = 0; j < topo[k]; ++j) {
			filewrite(fh, &layers[k].W[j][0], topo[k - 1] * sizeof layers[k].W[0][0]);
		}
		filewrite(fh, &layers[k].B[0], topo[k] * sizeof layers[k].B[0]);
	}
	fclose(fh);
	popdir();
}

double neuralNet::runNetwork(const vector<double>& in, const vector<double>& des, vector<double>& out)
{
	perf_start(RUN_NETWORK);
	U32 i, j;
	U32 k;
	U32 lastK;
	for (k = 1; k < topo.size(); ++k) {
		Z.resize(topo[k]);
		lastK = k - 1;
		layer& lastLayer = layers[lastK];
		layer& curLayer = layers[k];
		vector<double>& curA = k == topo.size() - 1 ? out : curLayer.A;
		const vector<double>& lastA = lastK ? lastLayer.A : in;
		U32 ic = topo[lastK];
		U32 jc = topo[k];
		vector<double>& curB = curLayer.B;
		vector<vector<double>>& curW = curLayer.W;
		for (j = 0; j < jc; ++j) {
			vector<double>& curWrow = curW[j];
			Z[j] = curB[j];
			double& ZjRow = Z[j];
			for (i = 0; i < ic; ++i) {
				ZjRow += lastA[i] * curWrow[i];
			}
			curA[j] = sigmoid(ZjRow);
		}
	}
	double retCost = 0.0;
	lastK = k - 1;
	U32 jc = topo[lastK];
	for (j = 0; j < jc; ++j) {
		double del = out[j] - des[j];
		retCost += del * del;
	}
	perf_end(RUN_NETWORK);
	return retCost;
}

void neuralNet::testNetwork()
{
	totalCostTest = 0.0;
	//vector<double>& Ain = layers[0].A;
	//S32 lastLayer = topo.size() - 1;
	//vector<double>& Aout = layers[lastLayer].A;
	//S32 firstLayerSize = topo[0];
	//S32 lastLayerSize = topo[lastLayer];
	for (U32 t = 0; t < nTest; ++t) {
		//copy(&inputsTest[t][0], &inputsTest[t][0] + firstLayerSize, &Ain[0]);
		//copy(&desiredsTest[t][0], &desiredsTest[t][0] + lastLayerSize, &Y[0]);
		double cost = runNetwork(inputsTest[t], desiredsTest[t], outputsTest[t]);
		//copy(&Aout[0], &Aout[0] + lastLayerSize, &outputsTest[t][0]);
		totalCostTest += cost;
	}
	avgCostTest = totalCostTest / nTest;
}

void neuralNet::gradientDescent(double learn) // gradient descent
{
	perf_start(GRAD_DESCENT);
	S32 i, j;
	U32 k;
	// brute force derivatives and run network for cost
	for (k = 1; k < topo.size(); ++k) {
		S32 lastK = k - 1;
		S32 row = topo[k];
		S32 col = topo[lastK];
		layer& curLayer = layers[k];
		vector<double>& curdCdB_CR = curLayer.dCdB_CR;
		vector<vector<double>>& curdCdW_CR = curLayer.dCdW_CR;
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		vector<double>& curdCdB_BF = curLayer.dCdB_BF;
		vector<vector<double>>& curdCdW_BF = curLayer.dCdW_BF;
#endif
		double* addr;
		for (j = 0; j < row; ++j) {
			addr = &curdCdW_CR[j][0];
			fill(addr, addr + col, 0.0);
#ifdef DO_BRUTE_FORCE_DERIVATIVES
			addr = &curdCdW_BF[j][0];
			fill(addr, addr + col, 0.0);
#endif
		}
		addr = &curdCdB_CR[0];
		fill(addr, addr + row, 0.0);
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		addr = &curdCdB_BF[0];
		fill(addr, addr + row, 0.0);
#endif
	}

	totalCost = 0.0;
	//vector<double>& Ain = layers[0].A;
	//S32 lastLayer = topo.size() - 1;
	//vector<double>& Aout = layers[lastLayer].A;
	//S32 firstLayerSize = topo[0];
	//S32 lastLayerSize = topo[lastLayer];

	for (U32 t = 0; t < nTrain; ++t) {
		//copy(&inputs[t][0], &inputs[t][0] + firstLayerSize, &Ain[0]);
		//copy(&desireds[t][0], &desireds[t][0] + lastLayerSize, &Y[0]);

		double cost = runNetwork(inputs[t], desireds[t], outputs[t]); // baseline
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		for (k = 1; k < topo.size(); ++k) {
			S32 lastK = k - 1;
			S32 row = topo[k];
			S32 col = topo[lastK];
			layer& curLayer = layers[k];
			vector<double>& curB = curLayer.B;
			vector<double>& curdCdB_BF = curLayer.dCdB_BF;
			vector<vector<double>>& curW = curLayer.W;
			vector<vector<double>>& curdCdW_BF = curLayer.dCdW_BF;
			// brute force derivatives
			for (j = 0; j < row; ++j) {
				vector<double>& curWrow = curW[j];
				vector<double>& curdcDw_CRrow = curdCdW_BF[j];
				for (i = 0; i < col; ++i) {
					double save = curWrow[i];
					curWrow[i] += epsilon;
					double costPweight = runNetwork(inputs[t], desireds[t], outputs[t]);
					curWrow[i] = save;
					curdcDw_CRrow[i] += (costPweight - cost) / epsilon;
				}
				double& curBrow = curB[j];
				double save = curBrow;
				curBrow += epsilon;
				double costPbias = runNetwork(inputs[t], desireds[t], outputs[t]);
				curBrow = save;
				curdCdB_BF[j] += (costPbias - cost) / epsilon;
			}
		}
		runNetwork(inputs[t], desireds[t], outputs[t]); // need correct A? for chain rule derivatives and output
#endif
		//copy(&Aout[0], &Aout[0] + lastLayerSize, &outputs[t][0]);

		totalCost += cost;

		//S32 outSize = topo.size() - 1;
		//S32 ls = topo[outSize];
		//vector<double> DcostDZ;// (Ls); // same as DcostDBL, used for backtrace
		//vector<double> DcostDA;// (Ls);
		for (k = topo.size() - 1; k > 0; --k) {
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
			vector<double>& AL = N == topo.size() ? outputs[t] : layers[L].A;
			vector<double>& AP = P ? layers[P].A : inputs[t];
			// cost
			DcostDA.resize(Ls);
			//DcostDA = vector<double>(Ls);
			if (k == topo.size() - 1) {
				for (j = 0; j < Ls; ++j) {
					DcostDA[j] = 2.0*(AL[j] - desireds[t][j]);
				}
			} else { // backtrace
				S32 Ns = topo[N];
				layer& curLayerN = layers[N];
				vector<vector<double>>& curW = curLayerN.W;
#if 1
				//for (j = 0; j < Ls; ++j) {
				//	DcostDA[j] = 0.0;
				//}
				fill(&DcostDA[0], &DcostDA[0] + Ls, 0.0);
				for (j = 0; j < Ns; ++j) {
					double& DcostDZrow = DcostDZ[j];
					vector<double>& curWrow = curW[j];
					for (i = 0; i < Ls; ++i) {
						DcostDA[i] += curWrow[i] * DcostDZrow;
					}
				}
#else
				for (j = 0; j < Ls; ++j) {
					double& DcostDArow = DcostDA[j];
					DcostDArow = 0.0;
					for (i = 0; i < Ns; ++i) {
						DcostDArow += curW[i][j] * DcostDZ[i]; // TODO, why is i and j reversed?
					}
				}
#endif
			}
			DcostDZ.resize(Ls);
			//DcostDZ = vector<double>(Ls);
			layer& curLayerL = layers[L];
			vector<double>& curdCdB_CR = curLayerL.dCdB_CR;
			vector<vector<double>>& curdCdW_CR = curLayerL.dCdW_CR;
			for (j = 0; j < Ls; ++j) {
				// z
				double DALDZL = (1.0 - AL[j])*AL[j];
				// bias
				double& DcostDZrow = DcostDZ[j];
				DcostDZrow = DcostDA[j] * DALDZL; // same as DcostDBL
				curdCdB_CR[j] += DcostDZrow / nTrain; // add to train/cost
				// weight
				vector<double>& curdCdW_CRrow = curdCdW_CR[j];
				for (i = 0; i < Ps; ++i) {
					double DcostDWL = DcostDZrow * AP[i];
					curdCdW_CRrow[i] += DcostDWL / nTrain; // add to train/cost
				}
			}
		}
	} // end train

#ifdef DO_BRUTE_FORCE_DERIVATIVES
	// average brute force derivatives
	// TODO: optimize
	for (k = 1; k < topo.size(); ++k) {
		S32 lastK = k - 1;
		S32 row = topo[k];
		S32 col = topo[lastK];
		layer& curLayer = layers[k];
		vector<double>& curdCdB_BF = curLayer.dCdB_BF;
		vector<vector<double>>& curdCdW_BF = curLayer.dCdW_BF;
		for (j = 0; j < row; ++j) {
			vector<double>& curdCdW_CRrow = curdCdW_BF[j];
			for (i = 0; i < col; ++i) {
				curdCdW_CRrow[i] /= nTrain;
			}
			curdCdB_BF[j] /= nTrain;
		}
	}
#endif
	// average cost
	avgCost  = totalCost / nTrain;

	// run the gradient descent learn
	for (k = 1; k < topo.size(); ++k) {
		S32 lastK = k - 1;
		S32 row = topo[k];
		S32 col = topo[lastK];
		layer& curLayer = layers[k];
		vector<double>& curB = curLayer.B;
		vector<double>& curdCdB_CR = curLayer.dCdB_CR;
		vector<vector<double>>& curW = curLayer.W;
		vector<vector<double>>& curdCdW_CR = curLayer.dCdW_CR;
		// average
		for (j = 0; j < row; ++j) {
			vector<double>& curWrow = curW[j];
			vector<double>& curdCdW_CRrow = curdCdW_CR[j];
			for (i = 0; i < col; ++i) {
				curWrow[i] -= curdCdW_CRrow[i] * learn;
			}
			curB[j] -= curdCdB_CR[j] * learn;
		}
	}
	perf_end(GRAD_DESCENT);
}
