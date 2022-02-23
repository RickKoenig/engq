#include <m_eng.h>

#include "m_perf.h"
#include "u_idxfile.h"

void idxFile::normalize(vector<double>& anInput, double& mean, double& stdDev)
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

	string doubleName = string() + fNameInput + ".doubleNorm.bin";
	bool readRawDoubles = false;
	if (fileexist(doubleName.c_str())) {
		readRawDoubles = true;
	}
	//readRawDoubles = false;

	FILE* fhd;
	if (readRawDoubles) { // cache normalized double data
		fhd = fopen2(doubleName.c_str(), "rb"); // read cached double data
	} else {
		fhd = fopen2(doubleName.c_str(), "wb"); // else calc and save them doubles
	}

	rawInput.resize(dataSize3);
	input.resize(dataSize3);
	for (U32 k = 0; k < dataSize3; ++k) {
		vector<vector<U8>> abm(height, vector<U8>(width));
		for (U32 j = 0; j < height; ++j) {
			vector<U8>& arow = abm[j];
			fileread(fh, &arow[0], width);
		}
		rawInput[k] = abm; // populate rawInput


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
			filewrite(fhd, &anInput[0], anInput.size() * sizeof anInput[0]);
		}
		input[k] = anInput; // populate input
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
	desired.resize(dataSize1);
	fileread(fh, &rawDesired[0], dataSize1); // populate rawDesired
	fclose(fh);
	popdir();
	for (U32 k = 0; k < dataSize1; ++k) {
		// desired output
		vector<double> aDesired = vector<double>(10); // number of digits
		for (U32 j = 0; j < 10; ++j) {
			aDesired[j] = j == rawDesired[k] ? HI : LO;
		}
		desired[k] = aDesired; // populate desired
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

