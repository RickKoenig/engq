// class that handles the ui of the circuit area of screen, drag delete etc..

// a simple gate for 1 qubit, for the qfield grid
// probably should have just used one class instead of 60 different qgatebsae derivations
// will do it that way in 'engw' webgl engine
class qgatebase { // passthru and base class
protected:
	C32 drawcolor; // for errors
	S32 nyoff; // how far to draw a vertical line to connect multi qubit gates
public:
	// static
	static const float boxsmaller;// = 4;
	static const U32 gpixhit = 80; // height of the gate
	static C32 bordercolor; // very light gray
	// factory
	enum qtype {
		// 1 qubit
		PASSTHRU,
		HADAMARD,
		NOT,
		PX,
		PY,
		PZ,

		// from Q experience
		PZO2,  // S  gate,  sqrt(PZ) ,  PI/2
		PZMO2, // St gate, -sqrt(PZ) , -PI/2
		PZO4,  // T  gate,  sqrt(S ) ,  PI/4
		PZMO4, // Tt gate, -sqrt(St) , -PI/4
		// help make a W 3 qubit state
		SNOTTO,
		SNOTWP,
		PG,
		PMG,
		SPLITTER,
		MIRROR,
		P45,
		PM45,
		P30,
		PM30,

		M0, // measurement gates
		M1,

		// 2 qubits
		SWAPT, // top half
		SWAPB, // bottom half
		CNOTT,
		CNOTB,
		CZT,
		CZB,
		CYT,
		CYB,
		CHT,
		CHB,
		CST,
		CSB,
		CStT,
		CStB,
		CTT,
		CTB,
		CTtT,
		CTtB,
		CP45T,
		CP45B,
		CPM45T,
		CPM45B,
		CP30T,
		CP30B,
		CPM30T,
		CPM30B,
		DECT,
		DECB,
		// 3 qubits
		FREDKINT,
		FREDKINM,
		FREDKINB,
		TOFFOLIT,
		TOFFOLIM,
		TOFFOLIB,
		// 4 qubits
		TOFFOLI4C0,
		TOFFOLI4C1,
		TOFFOLI4C2,
		TOFFOLI4T,
		// 5 qubits
		TOFFOLI5C0,
		TOFFOLI5C1,
		TOFFOLI5C2,
		TOFFOLI5C3,
		TOFFOLI5T,
		// 6 qubits
		TOFFOLI6C0,
		TOFFOLI6C1,
		TOFFOLI6C2,
		TOFFOLI6C3,
		TOFFOLI6C4,
		TOFFOLI6T,
		// done
		QTYPEENUM,
	};
	static const C8* qgnames[qgatebase::QTYPEENUM];
	typedef qgatebase*(*createQGateFun)(U32 ida);
	static const createQGateFun qgateFactory[qgatebase::QTYPEENUM];/// = {

	U32 id;

	qgatebase(U32 ida) : drawcolor(C32BLACK),nyoff(0),id(ida)
	{
	}

	static qtype findqtypebyname(const string& qname)
	{
		C8 fc = qname[0];
		if (isdigit(fc) || fc == ' ') { // number, qgatebase::qtype
			return qtype(atoi(qname.c_str()));
		} else {
			U32 i,n = QTYPEENUM;
			for (i=0;i<n;++i) {
				if (!strcmp(qname.c_str(),qgnames[i])) {
					return qtype(i);
				}
			}
		}
		return PASSTHRU;
	}

/*
	qgatebase(const qgatebase& rhs)
	{
		id = rhs.id;
	}
*/
	static qgatebase* build(qtype kind,U32 ida = 0) // build any type of gate
	{
		if (kind >= QTYPEENUM)
			kind = PASSTHRU;
		return qgateFactory[kind](ida);
	}

	static qgatebase* create(U32 ida)
	{
		return new qgatebase(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatebase(*this);
		//return new qgatebase(id);
	}

	// end factory

	virtual qtype getkind() const
	{
		return PASSTHRU;
	}

	virtual U32 getnumparts() const
	{
		return 1;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return PASSTHRU;
	}


	U32 getid() const
	{
		return id;
	}

	// primitive draw routines for the qgates
	void drawoutline(bitmap32* bm,U32 xoff,U32 yoff) const;
	void drawpassthru(bitmap32* bm,U32 xoff,U32 yoff) const;
	void drawbox(bitmap32* bm,U32 xoff,U32 yoff) const;
	void drawtext(bitmap32* bm,U32 xoff,U32 yoff,const string& text) const;
	void drawcircleoutline(bitmap32* bm,U32 xoff,U32 yoff) const; // one pass at drawing snot, thin
	//void drawconnectdown(U32 xoff,U32 yoff) const; // draw down to bottom half of gate
	//void drawconnectup(U32 xoff,U32 yoff) const; // draw up to top half of gate
	//void drawboxdown(U32 xoff,U32 yoff) const; // draw upper half of box downwards
	void drawconnect(bitmap32* bm,U32 xoff,U32 yoff) const; // draw up to top half of gate
	//void drawboxup(U32 xoff,U32 yoff) const; // draw lower half of box upwards
	void drawboxm(bitmap32* bm,U32 xoff,U32 yoff) const;
	// qgatebase draw
	void setdrawcolor(C32 ca)
	{
		drawcolor = ca;
	}
	void setnextyoffset(S32 nyoffa)
	{
		nyoff = nyoffa;
	}
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};
