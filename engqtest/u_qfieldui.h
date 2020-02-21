// graphical representation of a quantum circuit, all the columns in a field
class qfield {
	vector<qcolumn> qcolumns; // from left to right, array of qcolumn
	S32 hilitpos; // which column to highlight
	bool hilitcenter;
	U32 qfxoffset; // for scrolling
	void basic(U32 numqubits);
	bitmap32* blochbm;
	bitmap32* bakerbm;
public:
	static C32 brt,brtbloch,bakecolor;
	static C32 hili;
	static const S32 blochsize = 300;
	void sethilitpos(S32 hi,bool center)  {hilitpos = hi;hilitcenter = center;}
	S32 gethilitpos() const {return hilitpos;}
	void setxoffset(U32 xoff) {qfxoffset = xoff;}
	// place to draw whole circuit
	static const S32 maxcolumns = 100; // how many steps to allocate
	// location of qfield on window/screen
	static const U32 leftlabels = 60;
	static const U32 fpixleft = 200 + leftlabels + 32;
	static const U32 fpixtop = 20;
	static const U32 fpixwid = 1060 - leftlabels - 32;
	static const U32 fpixhit = 500;
	qfield(U32 numqubits); // default
	qfield(const string& fname,S32 hlp); // from a file, load
	void save(const string& fname) const; // save to a file
	const qgatebase* getqgate(U32 col,U32 qub) const;
// replace a qgate inside the grid of qgates
	void changeqgate(U32 col,U32 qub,qgatebase::qtype qt);
// copy whole column of gates 'from' to column 'to'
	void copycolumn(U32 from,U32 to);
	void proc(); // update UI and circuits for qfield
// graphical representation of a quantum circuit
	U32 getnumqubits() const;
	void morequbits();
	void lessqubits();
	static void toBlochAngleAxisMat(const qmat& m,pointf3* axis,float* angleRet,float* phaseRet);
	static void toBlochQuatMat(const qmat& m,pointf3* q,float* phaseRet);
	static pointf3 qstateToBloch(const qstate& qs);
	void drawvector(bitmap32* bm,S32 bsh,const pointf3& p,C32 col,float scl = 1,bool drawline = true) const;
	void drawbloch() const;
	void draw(const qgatebase* qg) const;
	static bool isinside(U32 mx,U32 my);
	bool getcolumncursor(U32 mx,U32 my,S32* cl,float* frac=0) const;
	bool getaccumcursor(U32 mx,U32 my,S32* cl) const;
	bool getrowcolumncursor(U32 mx,U32 my,pointi2* p) const;
	const qmat getqmatcol(S32 col) const; // return matrix for this stage
	const qmat getqmatacc(S32 col) const; // return matrix for this stage
	~qfield();
	void fcalc(); // recalculate the whole thing (given col matrices)
	void setqmatacc(S32 col,const qmat& qmata); // what is this??
	void makelink(S32 linkfrom,S32 linkto);
	void freelink(S32 col);
	void calclinks();
};
