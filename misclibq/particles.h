// particle / fountain class
enum pk { // particle graphic types
	PK_NULL=0,
	PK_FIRE=1,
	PK_SMOKE=2,
	PK_SPARK=3,
	PK_NUM=4,
};

class particle {
	script* sc;
	vector<tree2*> parttrees;
public:
	static particle* curparticle;
	particle();
	tree2* genpart(pk idx,bool (*tp)(tree2*),S32 iarg=0,float farg=0);
	~particle();
};

// user functions
bool spawnproc(tree2* t);
bool explosionproc(tree2* t);

// end particle / fountain class
