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
	,vector<vector<double>>& inTester, vector<vector<double>>& desTester
	,costCorr doCorr)
	: name(namea), inputsTrain(inTrain), desiredsTrain(desTrain), nTrain(desTrain.size())
	,inputsTest(inTester), desiredsTest(desTester), nTest(desTester.size())
	,doCorrect(doCorr)
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
	topo = topology;
	// create outputs for both training and testing
	S32 outputSize = desiredsTrain[0].size();
	outputsTrain = vector<vector<double>>(nTrain, vector<double>(outputSize));
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
			aLayer.Z = vector<double>(cols);
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
				aRow[i] = frand() * 2.0 - 1.0;
#ifdef SHOW_WEIGHT_BIAS
				stringstream ssW;
				ssW << "weight" << k << "_" << j << "_" << i;
				C8* name = copyStr(ssW.str().c_str());
				mv = { name, &aRow[i], D_DOUBLE, FLOATUP / 8 };
				dbNeuralNet.push_back(mv);
#endif
			}
		}
		for (U32 j = 0; j < rowsW; ++j) {
			lay.B[j] = frand() * 2.0 - 1.0;
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
		vector<double>& anInput = inputsTrain[j];
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
		vector<double>& aDesired = desiredsTrain[j];
		vector<double>& anOutput = outputsTrain[j];
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
	// TODO: maybe add min cost and max cost and number correct
	stringstream ssOutput;
	ssOutput << "@yellow@Training Cost " << inputsTrain.size() << " Average";
	const C8* name = copyStr(ssOutput.str().c_str());
	mv = { name, &avgCostTrain, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Training Cost " << inputsTrain.size() << " Total";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &totalCostTrain, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Training Cost " << inputsTrain.size() << " Minimum";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &minCostTrain, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Training Cost " << inputsTrain.size() << " Maximum";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &maxCostTrain, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	if (doCorrect != costCorr::NONE) {
		ssOutput.str(string());
		ssOutput << "Training Cost " << inputsTrain.size() << " Correct";
		name = copyStr(ssOutput.str().c_str());
		mv = { name, &correctTrain, D_INT | D_RDONLY };
		dbNeuralNet.push_back(mv);
	}

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
	ssOutput.str(string());
	ssOutput << "@brown@Testing Cost " << inputsTest.size() << " Average";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &avgCostTest, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Testing Cost " << inputsTest.size() << " Total";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &totalCostTest, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Testing Cost " << inputsTest.size() << " Minimum";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &minCostTest, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	ssOutput.str(string());
	ssOutput << "Testing Cost " << inputsTest.size() << " Maximum";
	name = copyStr(ssOutput.str().c_str());
	mv = { name, &maxCostTest, D_DOUBLEEXP | D_RDONLY};
	dbNeuralNet.push_back(mv);

	if (doCorrect != costCorr::NONE) {
		ssOutput.str(string());
		ssOutput << "Testing Cost " << inputsTest.size() << " Correct";
		name = copyStr(ssOutput.str().c_str());
		mv = { name, &correctTest, D_INT | D_RDONLY };
		dbNeuralNet.push_back(mv);
	}

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
				ssW << "DcostDweight" << k << "_" << j << "_" << i << "_CR";
				C8* name = copyStr(ssW.str().c_str());
				mv = { name, &aRowCR[i], D_DOUBLEEXP | D_RDONLY };
				dbNeuralNet.push_back(mv);
#ifdef DO_BRUTE_FORCE_DERIVATIVES
				ssW.str(string()); // clear
				ssW << "DcostDweight" << k << "_" << j << "_" << i << "_BF";
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

void neuralNet::runNetwork(const vector<double>& in, vector<double>& out)
{
	perf_start(RUN_NETWORK);
	U32 i, j;
	U32 k;
	U32 lastK;
	for (k = 1; k < topo.size(); ++k) {
		lastK = k - 1;
		layer& lastLayer = layers[lastK];
		layer& curLayer = layers[k];
		bool outputLayer = k == topo.size() - 1;
		vector<double>& curA = outputLayer ? out : curLayer.A;
		const vector<double>& lastA = lastK ? lastLayer.A : in;
		U32 ic = topo[lastK];
		U32 jc = topo[k];
		vector<double>& curB = curLayer.B;
		vector<vector<double>>& curW = curLayer.W;
		vector<double>& curZ = curLayer.Z;
		for (j = 0; j < jc; ++j) {
			vector<double>& curWrow = curW[j];
			curZ[j] = curB[j];
			double& ZjRow = curZ[j];
			for (i = 0; i < ic; ++i) {
				ZjRow += lastA[i] * curWrow[i];
			}
#ifdef USE_TANH_HIDDEN
			curA[j] = outputLayer ? neuralNet::sigmoid(ZjRow) : neuralNet::tangentH(ZjRow);
#else
			curA[j] = neuralNet::sigmoid(ZjRow);
#endif
		}
	}
	perf_end(RUN_NETWORK);
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
	for (U32 t = 0; t < nTrain; ++t) {
		runNetwork(inputsTrain[t], outputsTrain[t]); // baseline
#ifdef DO_BRUTE_FORCE_DERIVATIVES
		double cost = calcOneCost(desiredsTrain[t], outputsTrain[t]);
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
					runNetwork(inputsTrain[t], outputsTrain[t]);
					double costPweight = calcOneCost(desiredsTrain[t], outputsTrain[t]);
					curWrow[i] = save;
					curdcDw_CRrow[i] += (costPweight - cost) / epsilon;
				}
				double& curBrow = curB[j];
				double save = curBrow;
				curBrow += epsilon;
				runNetwork(inputsTrain[t], outputsTrain[t]);
				double costPbias = calcOneCost(desiredsTrain[t], outputsTrain[t]);
				curBrow = save;
				curdCdB_BF[j] += (costPbias - cost) / epsilon;
			}
		}
		runNetwork(inputsTrain[t], outputsTrain[t]); // need correct A? for chain rule derivatives and output
#endif
		// back trace
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
			vector<double>& AL = N == topo.size() ? outputsTrain[t] : layers[L].A;
			vector<double>& AP = P ? layers[P].A : inputsTrain[t];
			// cost
			DcostDA.resize(Ls);
			if (k == topo.size() - 1) {
				for (j = 0; j < Ls; ++j) {
					DcostDA[j] = 2.0*(AL[j] - desiredsTrain[t][j]);
				}
			} else { // backtrace
				S32 Ns = topo[N];
				layer& curLayerN = layers[N];
				vector<vector<double>>& curW = curLayerN.W;
				fill(&DcostDA[0], &DcostDA[0] + Ls, 0.0);
				for (j = 0; j < Ns; ++j) {
					double& DcostDZrow = DcostDZ[j];
					vector<double>& curWrow = curW[j];
					for (i = 0; i < Ls; ++i) {
						DcostDA[i] += curWrow[i] * DcostDZrow;
					}
				}
			}
			DcostDZ.resize(Ls);
			layer& curLayerL = layers[L];
			vector<double>& curdCdB_CR = curLayerL.dCdB_CR;
			vector<vector<double>>& curdCdW_CR = curLayerL.dCdW_CR;
			vector<double>& ZL = curLayerL.Z;
			for (j = 0; j < Ls; ++j) {
				// z
#ifdef USE_TANH_HIDDEN
				bool outputLayer = k == topo.size() - 1;
				double DALDZL = outputLayer ? delSigmoid(ZL[j]) : delTangentH(ZL[j]); // this activation function should match the one in runNetwork
#else
				double DALDZL = delSigmoid(ZL[j]); // this activation function should match the one in runNetwork
#endif
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

bool neuralNet::loadNetwork(U32 slot)
{
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
	vector<U32> tpo(topo.size());
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
	filewrite(fh, &topo[0], topo.size() * sizeof topo[0]);
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

vector<double>& neuralNet::getOneTrainOutput(U32 idx)
{
	return outputsTrain[idx];
}

vector<double>& neuralNet::getOneTrainDesired(U32 idx)
{
	return desiredsTrain[idx];
}

vector<double>& neuralNet::getOneTestOutput(U32 idx)
{
	return outputsTest[idx];
}

vector<double>& neuralNet::getOneTestDesired(U32 idx)
{
	return desiredsTest[idx];
}

// one, fast
double neuralNet::calcOneCost(const vector<double>& des, vector<double>& out)
{
	double retCost = 0.0;
	U32 jc = des.size();
	for (U32 j = 0; j < jc; ++j) {
		double del = out[j] - des[j];
		retCost += del * del;
	}
	return retCost;
}

// array, includes avg, total, min, max, and optional correct costs
void neuralNet::calcCostArr(const vector<vector<double>>& inArr
	, const vector<vector<double>>& desArr
	, vector<vector<double>>& outArr
	, costCorr costCorrKind
	, double& totalCost, double& avgCost, double& minCost, double& maxCost, U32& correct)
{
	// costs
	U32 n = desArr.size();
	totalCost = 0.0;
	minCost = numeric_limits<double>::max();
	maxCost = 0.0;
	correct = 0;
	for (U32 t = 0; t < n; ++t) {
		const vector<double>& des = desArr[t];
		vector<double>& out = outArr[t];
		runNetwork(inArr[t], out);
		double cost = calcOneCost(des, out);
		totalCost += cost;
		if (cost < minCost) {
			minCost = cost;
		}
		if (cost > maxCost) {
			maxCost = cost;
		}
		bool curCorrect = true;
		double maxVal = 0;
		U32 maxIdx = 0;
		switch (costCorrKind) {
		case costCorr::THRESHHOLD:
			for (U32 j = 0; j < out.size(); ++j) {
				double dv = des[j];
				double ov = out[j];
				if (dv < LO_THRESH && ov < LO_THRESH || dv > HI_THRESH && ov > HI_THRESH) {
					continue;
				} else {
					curCorrect = false;
					break;
				}
			}
			break;
		case costCorr::DIGITS:
			for (U32 j = 0; j < out.size(); ++j) {
				double ov = out[j];
				if (ov > maxVal) {
					maxVal = ov;
					maxIdx = j;
				}
			}
			curCorrect = des[maxIdx] > .5;
			break;
		}
		if (curCorrect) {
			++correct;
		}
	}
	avgCost = totalCost / n;
}

void neuralNet::calcCostTrainAndTest()
{
#if 1
	calcCostArr(inputsTrain, desiredsTrain, outputsTrain
		, doCorrect
		, totalCostTrain, avgCostTrain, minCostTrain, maxCostTrain, correctTrain);
	calcCostArr(inputsTest, desiredsTest, outputsTest
		, doCorrect
		, totalCostTest, avgCostTest, minCostTest, maxCostTest, correctTest);
#else
	// train
	totalCostTrain = 0.0;
	minCostTrain = numeric_limits<double>::max();
	maxCostTrain = 0.0;
	for (U32 t = 0; t < nTrain; ++t) {
		runNetwork(inputsTrain[t], outputsTrain[t]);
		double cost = calcOneCost(desiredsTrain[t], outputsTrain[t]);
		totalCostTrain += cost;
		if (cost < minCostTrain) {
			minCostTrain = cost;
		}
		if (cost > maxCostTrain) {
			maxCostTrain = cost;
		}
	}
	avgCostTrain = totalCostTrain / nTrain;

	// test
	totalCostTest = 0.0;
	minCostTest = numeric_limits<double>::max();
	maxCostTest = 0.0;
	for (U32 t = 0; t < nTest; ++t) {
		runNetwork(inputsTest[t], outputsTest[t]);
		double cost = calcOneCost(desiredsTest[t], outputsTest[t]);
		totalCostTest += cost;
		if (cost < minCostTest) {
			minCostTest = cost;
		}
		if (cost > maxCostTest) {
			maxCostTest = cost;
		}
	}
	avgCostTest = totalCostTest / nTest;
#endif
}

neuralNet::~neuralNet()
{
	// unpublish the debprint menu
	removedebvars("neuralNet");
	for (auto mv : dbNeuralNet) {
		delete mv.name;
	}
}

// activation functions and their derivatives
double neuralNet::sigmoid(double x)
{
#ifdef EXTRA_SLOPE
	return 1.0 / (1.0 + exp(-x)) + x * EXTRA_SLOPE_AMOUNT;
#else
	return 1.0 / (1.0 + exp(-x));
#endif
}

// derivative of sigmoid more precision
double neuralNet::delSigmoid(double x)
{
	double e = exp(x);
	double ep1 = e + 1.0;
	double ret = e / (ep1 * ep1);
#ifdef EXTRA_SLOPE
	return ret + EXTRA_SLOPE_AMOUNT;
#else
	return ret;
#endif
}

double neuralNet::tangentH(double x)
{
#ifdef EXTRA_SLOPE
	return tanh(x) + x * EXTRA_SLOPE_AMOUNT;
#else
	return tanh(x);
#endif
}

double neuralNet::delTangentH(double x)
{
	double chsq = cosh(x);
#ifdef EXTRA_SLOPE
	return 1.0 / (chsq * chsq) + EXTRA_SLOPE_AMOUNT;
#else
	return 1.0 / (chsq * chsq);
#endif
}


