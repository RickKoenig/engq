#define DO_NORMALIZE // all data is normalized to mean = 0, standard deviation = 1

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
	const double LO{ .1 };
	const double HI{ .9 };
public:
	idxFile(const C8* fNameInput, const C8* fNameDesired, U32 limit = 0);
	U32 getNumData();
	// for network
	vector<vector<double>>* getInput();
	vector<vector<double>>* getDesired();
	// for display
	U32 getOneDesired(U32 idx);
	vector<vector<U8>>* getOneImage(U32 idx);
	static void normalize(vector<double>& anInput, double& mean, double& stdDev);

};
