#define MAXLIGHTS 100

struct li {
	bool dodefaultlights;
	S32 uselights;
	S32 numlights;
	tree2* lightlist[MAXLIGHTS];
// local
	S32 ndirlights;
//	struct pointf3 worldlightdirs[MAXLIGHTS];
//	struct pointf3 worldlightpos[MAXLIGHTS];
	pointf3 deflightpos[MAXLIGHTS];
	pointf3 deflightrot[MAXLIGHTS];
	mat4 light2world[MAXLIGHTS];
//	struct pointf3 locallightdirs[MAXLIGHTS];
	struct pointf3 lightcolors[MAXLIGHTS];
	struct pointf3 ambcolor;
};
extern li lightinfo;


void addlighttolist(tree2* t);
void removelightfromlist(tree2* t);	// called automatically when tree2 is deleted

// called by engine
void dolights(); // calc world pos,dir of lights using either default or trees
//void dolightsdestrec( tree2* t);
void donelights();
