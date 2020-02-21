// match soundmotor.txt
enum motor {
	ENG1,
	ENG2,
	NMOTOR
};

class soundemitman;
class soundemit {
	class soundemitman* sm;
	wavehandle* wh;
	soundhandle* sh;
	pointf3x pos,vel;
	float vol,rpm;
public:
	soundemit(class soundemitman& sm,enum motor m);
	void update(const pointf3& pos,const pointf3& vel,float rpm=1,float vol=1);
	~soundemit();
	friend class soundemitman;
};

class soundemitman {
	script* sc;
	vector<wavehandle*> wavese;
	vector<soundemit*> emits;
	wavehandle* getwaveidx(enum motor m);
public:
	soundemitman();
	void update(const pointf3& earpos,const pointf3& earrot,const pointf3& earvel,float sos); // location of ears
	~soundemitman(); // afterwords, soundemit ptrs are invalid
	friend class soundemit;
	S32 getnumwaves();
};

