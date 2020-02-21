// augmented matrices
typedef zee2 T;
class augmatz_ui : public augmatz  {
public:
	// 1 to 5 vars
	static const S32 startx1 = 10;
	static const S32 sizex1 = 100;
	static const S32 sepx1 = 120;
	// 6 to 16 vars
	static const S32 startx2 = 3;
	static const S32 sizex2 = 28;
	static const S32 sepx2 = 48;
	// 17 to 64 vars
	static const S32 startx3 = 5;
	static const S32 sizex3 = 8;
	static const S32 sepx3 = 12;

	// 1 to 5 vars
	static const S32 starty1 = 45;
	static const S32 sizey1 = 20;
	static const S32 sepy1 = 40;
	// 6 to 16 vars
	static const S32 starty2 = 45;
	static const S32 sizey2 = 10;
	static const S32 sepy2 = 15;
	// 17 to 64 vars
	static const S32 starty3 = 5;
	static const S32 sizey3 = 8;
	static const S32 sepy3 = 11;

	S32 sepx,startx,sizex;
	S32 sepy,starty,sizey;
private:
	vector<vector<shape*> > pbutshapes;
public:
	augmatz_ui(S32 moda,S32 neqa,S32 nvara,shape* rla);
	bool isfocus(shape* s,S32& ri,S32& ci);
	void dofocus(S32 cr,S32 cc,U32 butval);
	void update();
};
