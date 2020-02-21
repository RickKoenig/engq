// json stuff
// avatar
class avaj {
public:
	static const C8 *botnames[];
	static const S32 nbotnames;
	string playername;
	string carbodyname;
	S32 paint;
	S32 decal;
	vector<string> weaps;
	vector<S32> energies; // no, should be in timewarp
	S32 ownerid; // used from connect onwards into game...
	avaj() : paint(0),decal(0),ownerid(-1) {}
	avaj(const json& j);
	json save() const;
	void randombot();
};

class calco {
public:
//	pointf3 p[NCARPOINTS],pr[NCARPOINTS];
	pointf3 pr[NCARPOINTS];
	pointf3 c2cpnts[NRECTPOINTS];
};

struct caroj : public avaj {
	caroj() : carnull(0),carbodyprism(0),cc(0) { fill(&carshocks[0],&carshocks[NWORKINGSHOCKS],(tree2*)0); }
	caroj(const avaj& a) : avaj(a),carnull(0),carbodyprism(0),cc(0),emit(0) { fill(carshocks,carshocks+NWORKINGSHOCKS,(tree2*)0); }
// car graphic
	tree2* carnull,*carbodyprism;//,*carbody;
	tree2* carshocks[NWORKINGSHOCKS];
	tree2* carwheels[NWORKINGSHOCKS];
	n_carclass* cc;
// no TW constants
	pointi2x stpceloc; // it's a constant, tw not necessary, maybe move out to a 'constants' struct
// non constants, nonpredicted section
	calco scr; // per object scratchpad
// motorsound
	class soundemit* emit;
// TW // time warp / gamestates
	twcaro cs; // time warp predicted current car state
	twcaro cs0; // time warp rematch car state
	twcaro csn; // time warp last non predicted car state
};
