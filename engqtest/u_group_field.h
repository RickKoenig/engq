#define LOCKGROUPS
// also any base upto 32
C8 hexchar(S32 i);

class group {
	const S32 bsize;
	const int OFFX;
	const int OFFY;
	const int SQSIZE;
public:
	static const int MAXBSIZE=32;
	static const int SMALLSIZE=16;
	static const int MAXHOMOSIZE=11;
private:
	struct grouperror { // pod, false if err
	// must all be true to be a group
		bool identee; // ident is at 0,0
		bool identskel; // valid ident skeleton
		bool assoc; // passes associativity test
		C8 ac,bc,cc; // if fail assoc
		bool complete;
		bool rows[MAXBSIZE];
		bool cols[MAXBSIZE];
	// optional
		bool isabelian;
	};
	const bool ismul;
	S32 board[MAXBSIZE][MAXBSIZE];
	grouperror ge;
	bool colbuttons[MAXBSIZE];
public:
	group(int startsize,int offx,int offy,int sqsize,bool ismula) : bsize(ismula?startsize-1:startsize),OFFX(offx),OFFY(offy),SQSIZE(startsize<=SMALLSIZE?sqsize:sqsize/2),ismul(ismula) {
		if (bsize>MAXBSIZE)
			errorexit("field too big %d",bsize);
		resetge();
		resetgroup();
	}
	void resetge();
	void resetgroup();
	void backupgroup();
	bool checkgroup(); // return true if group okay
	void swaprows(S32 a,S32 b);
	void swapcols(S32 a,S32 b);
	void swapsymbols(S32 a,S32 b);
	void procbuttons(group* alt);
	void drawboard();
	void drawbuttons();
	int getorder() const { return ismul?bsize+1:bsize; }
//	grouperror geterror();
	void drawerror();
	int op(int a,int b) const;
	int opm(int a,int b) const; // used for mult group, dist law
	void setboard(S32 row,S32 col,S32 val);
};

struct fielderror {
	bool leftdistrib;	// passes a*(b+c)=a*b+a*c;
	bool rightdistrib;	// passes (a+b)*c=a*c+b*c;
	C8 lac,lbc,lcc; // if fail assoc
	C8 rac,rbc,rcc; // if fail assoc
};

fielderror checkfield(const group* ga,const group* gm);
