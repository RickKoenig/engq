// all the gates in a column
// does one step in calculation
class qcolumn {
	// this class has to manage these pointers to qgates
	vector<qgatebase*> qgates; // numqubits is qgates.size()
	qmat qmcol,qmacc;
	S32 linkback; // instead of these gates, copy from linkback column instead (over writing)
public:
	static const U32 maxiqubits = 6;
	static const U32 miniqubits = 1;
	static const U32 defaultqubits = 4;
	static const U32 cpixwid = 55; // width of the column and the gates inside it
	qcolumn(U32 numqubits);
	qcolumn(const qcolumn& rhs); // copy constructor
	qcolumn operator=(const qcolumn& rhs); // assigment
	const qgatebase* getqgatecol(U32 qub) const;
	//void changeqgatecol(U32 qub,qgatebase::qtype qt,U32 ida); // change a qgate, set id manually, (file load)
	void changeqgatecol(U32 qub,const string& qname,U32 ida); // change a qgate, set id manually, (file load)
	void changeqgatecolautoid(U32 qub,qgatebase::qtype qt); // change a qgate, gen an id based on what other
															// gates are in column (UI gate drop)
	U32 calcid(qgatebase::qtype qt) const;
	qgatebase::qtype getkind(U32 idx) const;
	U32 getid(U32 idx) const;
	U32 getnumqubits() const {return qgates.size();}
	void morequbits();
	void lessqubits();
	void draw(U32 xoff,U32 scrollxoff,bool center) const; // draw column
	void drawlinkback(U32 xoff,U32 scrollxoff) const; // draw column link back if has one
	void calc();
	~qcolumn();
	//void setqmat(const qmat& qma) {qm = qma;}
	qmat getqmatcol() const {return qmcol;}
	qmat getqmatacc() const {return qmacc;}
	void setqmatacc(const qmat& ac) {qmacc = ac;}
	S32 getlinkback() const { return linkback; };
	void setlinkback(S32 linkbacka) { linkback = linkbacka; };
};
