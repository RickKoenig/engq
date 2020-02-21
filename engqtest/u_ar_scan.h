struct circleScan
{
	S32 x,y,r;
};

// convert captured bitmap into an array of circles
vector<circleScan> doscanar(bitmap32* b,string& mess);

extern S32 blackwhitethresh;

void ar_test_wider_neighbor1D();

namespace ar_scan {
extern float darkratio;// = .00525f; // good with what's in the gallery
}
