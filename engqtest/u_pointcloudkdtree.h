// kd trees

// TODO: make a template



// 2D

class pc_kdtree2d {
	vector<pointf2> pnts; // for reference and brute force and index based searches
	vector<pointf2> left0,middle0,right0; // level 0, for reference and brute force
	pointf2x boxmin,boxmax; // for comparision with straight average
	pc_kdtree2d* subleft,*subright; // subtrees, less than, greater than ...
	pointf2 center;
	U32 level;
	static const U32 maxdim = 2;
	void build();

	pointf2 closestrec(const pointf2& tp,float& bestr2) const; // return closest point
	void insidecirclerec(const pointf2& tp,float inrad,vector<pointf2>& rt) const; // return closest point

	void descend(); // not too sure how to do this ?
	void ascend();
public:
// constructor
	//pc_kdtree2d() {}
	pc_kdtree2d(const vector<pointf2>& points,U32 levela = 0);
	pc_kdtree2d(const pointf2* points,U32 npoints,U32 levela = 0);

// values
	vector<pointf2> insidecirclebrute(const pointf2& tp,float inrad) const; // return list of points inside circle brute force
	vector<pointf2> insidecircle(const pointf2& tp,float inrad) const; // return list of points inside circle
	pointf2 closestbrute(const pointf2& tp,float& clrad) const; // return closest point brute force
	pointf2 closest(const pointf2& tp,float& clrad) const; // return closest point

// index
	vector<U32> insidecirclebrutei(const pointf2& tp,float rad) const; // return list of point indices inside circle brute force
	vector<U32> insidecirclei(const pointf2& tp,float rad) const; // return list of point indices inside circle
	U32 closestbrutei(const pointf2& tp,float& crad) const; // return closest point index brute force
	U32 closesti(const pointf2& tp,float& crad) const; // return closest point index

// reference data
	//const pointf2* getdata() const;
	// test get first level split data
	void getsplitdata(vector<pointf2>& left,vector<pointf2>& middle,vector<pointf2>& right);

	~pc_kdtree2d();
};



// 3D

class pc_kdtree3d {
	vector<pointf3> pnts;
	pointf3x boxmin,boxmax;
	vector<pc_kdtree3d> subs; // subtrees
	void build();
public:
	pc_kdtree3d() {}
	pc_kdtree3d(const vector<pointf3>& points);
	pc_kdtree3d(const pointf3* points,U32 npoints);
	
	~pc_kdtree3d();
};
