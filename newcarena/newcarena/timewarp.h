#define NRECTPOINTS 8
#define NWORKINGSHOCKS 4
#define NSHOCKPOINTS 8
//#define NRECTPOINTS 8 // timewarp.h
#define NMIDPOINTS 2
// #define NWORKINGSHOCKS 4 // timewarp.h


#define SHOCKPOINTOFF 0
#define RECTPOINTOFF (SHOCKPOINTOFF+NSHOCKPOINTS)
#define MIDPOINTOFF (RECTPOINTOFF+NRECTPOINTS)

#define NCARPOINTS (MIDPOINTOFF+NMIDPOINTS)

// pointf3x with hash, move somewhere else
struct pointf3xh : public pointf3x
{
	pointf3xh(float xa=0,float ya=0,float za=0,float wa=1) : pointf3x(xa,ya,za,wa) {}
	pointf3xh(const pointf3x& p) : pointf3x(p) {}
	pointf3xh(const pointf3& p) : pointf3x(p) {}
	void hashadd(const hashi& h) const;
	string log() const;
	string logh() const;
};

// global TW / gamestate
struct twgg {
	S32 clock,clockoffset;
	S32 gamenum;
	S32 nextplace; // counts up, next car finishing gets this place
	twgg() : clock(0),clockoffset(0),gamenum(0),nextplace(1) {}
	void hashadd(const hashi& h) const;
};

// player TW
struct twcaro
{
	enum ratchet {READY,LESS,GREATER,DONE};
// car ai/ checkpoint/rabbits
	pointi2x lastloc;
	static const S32 MAXLOC = 2;
	pointi2x nextloc[MAXLOC];
	S32 nnextloc;
	S32 curlap;
	ratchet lapratchet; // less to greater or greater to less triggers a lap, then is set to done
//	place
	S32 finplace; // 0 not yet, nonzero your place
	S32 fintime; // 0 not yet, nonzero your raw finish race time
// rematch
	bool rematchyes; // 'yes' was selected
//	car physics
	pointf3xh pos,rot,vel,rotvel;//,normj; // last one is for the track slopes
//	pointf3xh lastpos; // for doppler
	float shocklen[NWORKINGSHOCKS];
	float accelspin;
	float steertime;
	S32 noshocktime,nocheckpointtime,noresettime;
// functions
	twcaro() : nnextloc(0),curlap(0),lapratchet(READY),finplace(0),fintime(0),rematchyes(false),accelspin(0),
		steertime(0),noshocktime(0),nocheckpointtime(0),noresettime(0)
	{
		fill(shocklen,shocklen+NWORKINGSHOCKS,0.0f);
	}
	void hashadd(const hashi& h) const;
	void log() const;
};

// functions
//void twsave0(); // save state0
void twrestore0(); // rematch
//void twsaven(); // save staten
//void twrestoren(); // restore staten
void twsave();

void timewarp(S32 ul);
