struct edge {
	int ei[2];
};

struct facevert {
//	int nface;
//	face *faces;
//	int nvert;
//	pointf3 *verts;
//	uv *uvs;
//	struct edge *edges;
//	int nedge;
	vector<face> faces;
	vector<pointf3> verts;
	vector<uv> uvs;
	vector<edge> edges;
};

struct backgndsectioninfo {
	pointf3 start,step;
	int nstepsx,nstepsz;
};

struct tracksectioninfo {
	pointf3 start;
	int nsteps;
	float yaw,pitch,len,trackwidth,outerwidth,invrad,startroll,midroll,endroll;
};

facevert *buildbackgndsectionfv(backgndsectioninfo *bs);
facevert *buildtracksectionfv(tracksectioninfo *ts);

void getbacky(pointf3 *v);

//void freefacevert(facevert *bfv);
