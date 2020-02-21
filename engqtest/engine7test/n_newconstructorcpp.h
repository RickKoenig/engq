void newconstructorinit(),newconstructorproc(),newconstructordraw3d(),newconstructordraw2d(),newconstructorexit();

struct section {
	float len;
	float heading;
	float pitch;
	float bank;
	float inwid;
	float outwid;
};

struct newtrack {
//	int nsections;
	pointf3 start,pyr;
//	int connect; // for lap based races
//	section *sections;
	vector<section> sections;
};

/*struct newtrack {
	struct newtrk *trkdata;
//	TREE *treedata;
};
*/
newtrack *loadnewtrackscript(const char *trackname);
void freenewtrackscript(newtrack *);
tree2 *buildnewconsscene(const newtrack *t);

const char *savenewconstrack(const char *name,const newtrack *td,int uniq);

newtrack *readnewtrack(const U8 *data,U32 len);
U8* writenewtrack(const newtrack *t,U32* len);
