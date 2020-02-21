// match soundfx.txt
enum fxe {
	CRASH1,
	CRASH2,
	CRASH3,
	ENGSTART,
	FINISH1,
	FINISH2,
	FINISH3,
	FINISH4,
	HORN,
	LAND,
	START1,
	START2,
	START3,

	NSOUNDFX
};
class soundfx {
	bool doears;
	pointf3x earpos,earrot;
	script* sc;
	vector<wavehandle*> waves;
public:
	soundfx(); // load soundfx
	void setears(const pointf3& pos,const pointf3& rot);
	void playsnd(const pointf3* emitpos,fxe se);
	~soundfx();
};

bool getvolpan(const pointf3* emitpos=0,const pointf3* earpos=0,const pointf3* earrot=0,float* vol=0,float* pan=0);
float dodoppler(const pointf3& srcpos,const pointf3& srcvel,const pointf3& destpos,const pointf3& destvel,float sos);
