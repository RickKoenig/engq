// process the snapshot, returning a 2d square byte array 0,1 of the qrcode bits
vector<vector<U8> > doqrscan(bitmap32* pic);
struct squarei {
	S32 x,y,s;
	bool overlap(const squarei& rhs) const;
	squarei(S32 xa,S32 ya,S32 sa) : x(xa),y(ya),s(sa) {}
};

class equiv {
//	vector<squarei> clusters;
	vector<vector<squarei> > split;
public:
// assume well presented , clusters overlap every other member in same cluster, otherwise, might get more clusters
	void addsi(const squarei& si);
	vector<pointi2> gettargets();
};
