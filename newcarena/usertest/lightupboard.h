// handles both game board and pattern board
class lightupboard {
	string name;
protected:
	S32 startx,starty;
	S32 stepx,stepy;
private:
	S32 sizex,sizey;
protected:
	S32 dimx,dimy;
	S32 pat;
	S32 wrap;
	S32 prod;
	U8* data;
	bool getcoord(S32 mxa,S32 mya,S32& cx,S32& cy);
public:
	lightupboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa);
	void reset();
	U8 getcell(S32 xa,S32 ya);
	S32 getdimx() { return dimx; }
	S32 getdimy() { return dimy; }
	S32 getpat() { return pat; }
	S32 getwrap() { return wrap; }
	void draw();
	virtual ~lightupboard();
};

class patternboard : public lightupboard {
	S32 overx,overy;
	void update();
public:
	patternboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa);
	void setovermouse(S32 mxa,S32 mya,S32 mbuta);
	void setoverraw(S32 mxa,S32 mya);
	void getoverraw(S32& mxa,S32& mya);
//	~patternboard();
};

class gameboard : public lightupboard {
	S32 mod;
	U8* patdata;
	void update(S32 cxa,S32 cya,S32 deltaa);
public:
	gameboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa,S32 moda);
	void click(S32 mxa,S32 mya,S32 deltaa);
	~gameboard();
};
