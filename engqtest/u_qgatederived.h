// this was a very bad idea!!
// looked good at the beginning, but with so many gates, it's kind of tough
// should have made this more data driven, no derived classes
// using new system in engw (webgl)

// hadamard gate
class qgatehadamard : public qgatebase {
public:
	qgatehadamard(U32 ida) : qgatebase(ida) {}

	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatehadamard(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatehadamard(*this);
		//return new qgatehadamard(id);
	}

	virtual qtype getkind() const
	{
		return HADAMARD;
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
		return HADAMARD;
	}

	virtual qmat getqmat() const;

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// measure 0 gate
class qgatem0 : public qgatebase {
public:
	qgatem0(U32 ida) : qgatebase(ida) {}

	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatem0(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatem0(*this);
		//return new qgatehadamard(id);
	}

	virtual qtype getkind() const
	{
		return M0;
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
		return M0;
	}

	virtual qmat getqmat() const;

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// measure 1 gate
class qgatem1 : public qgatebase {
public:
	qgatem1(U32 ida) : qgatebase(ida) {}

	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatem1(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatem1(*this);
		//return new qgatehadamard(id);
	}

	virtual qtype getkind() const
	{
		return M1;
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
		return M1;
	}

	virtual qmat getqmat() const;

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// splitter gate
class qgatesplitter : public qgatebase {
public:
	qgatesplitter(U32 ida) : qgatebase(ida) {}

	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatesplitter(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatesplitter(*this);
		//return new qgatesplitter(id);
	}

	virtual qtype getkind() const
	{
		return SPLITTER;
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
		return SPLITTER;
	}

	virtual qmat getqmat() const;

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// mirror gate
class qgatemirror : public qgatebase {
public:
	qgatemirror(U32 ida) : qgatebase(ida) {}

	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatemirror(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatemirror(*this);
		//return new qgatemirror(id);
	}

	virtual qtype getkind() const
	{
		return MIRROR;
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
		return MIRROR;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// square root of not gate twisted oak
class qgatesnotto : public qgatebase {
	void drawthinsnot(bitmap32* bm,U32 xoff,U32 yoff) const; // one pass at drawing snot, thin
public:
	qgatesnotto(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatesnotto(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatesnotto(*this);
		//return new qgatesnot(id);
	}

	virtual qtype getkind() const
	{
		return SNOTTO;
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
		return SNOTTO;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// square root of not gate wikipedia
class qgatesnotwp : public qgatebase {
	void drawthinsnot(bitmap32* bm,U32 xoff,U32 yoff) const; // one pass at drawing snot, thin
public:
	qgatesnotwp(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatesnotwp(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatesnotwp(*this);
		//return new qgatesnot(id);
	}

	virtual qtype getkind() const
	{
		return SNOTWP;
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
		return SNOTWP;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// not gate (PX)
class qgatenot : public qgatebase {
public:
	qgatenot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatenot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatenot(*this);
		//return new qgatenot(id);
	}

	virtual qtype getkind() const
	{
		return NOT;
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
		return NOT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// PX gate (NOT)
class qgatepx : public qgatebase {
public:
	qgatepx(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatepx(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatepx(*this);
		//return new qgatepx(id);
	}

	virtual qtype getkind() const
	{
		return PX;
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
		return PX;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// PY gate
class qgatepy : public qgatebase {
public:
	qgatepy(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatepy(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatepy(*this);
		//return new qgatepy(id);
	}

	virtual qtype getkind() const
	{
		return PY;
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
		return PY;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// PZ gate
class qgatepz : public qgatebase {
public:
	qgatepz(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatepz(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatepz(*this);
		//return new qgatepz(id);
	}

	virtual qtype getkind() const
	{
		return PZ;
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
		return PZ;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// S gate
class qgates : public qgatebase {
public:
	qgates(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgates(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgates(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PZO2;
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
		return PZO2;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// St gate
class qgatest : public qgatebase {
public:
	qgatest(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatest(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatest(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PZMO2;
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
		return PZMO2;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// T gate
class qgatet : public qgatebase {
public:
	qgatet(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatet(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatet(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PZO4;
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
		return PZO4;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// Tt gate
class qgatett : public qgatebase {
public:
	qgatett(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatett(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatett(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PZMO4;
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
		return PZMO4;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// G over 3 gate, helps make W state
class qgateg1over3 : public qgatebase {
public:
	qgateg1over3(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgateg1over3(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgateg1over3(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PG;
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
		return PG;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// G over 3 gate inverse, helps make W state
class qgategm1over3 : public qgatebase {
public:
	qgategm1over3(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgategm1over3(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgategm1over3(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PMG;
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
		return PMG;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// P45 gate, test phase
class qgatep45 : public qgatebase {
public:
	qgatep45(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatep45(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatep45(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return P45;
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
		return P45;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// P-45 gate, test phase
class qgatepm45 : public qgatebase {
public:
	qgatepm45(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatepm45(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatepm45(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PM45;
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
		return PM45;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// P30 gate, test phase
class qgatep30 : public qgatebase {
public:
	qgatep30(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatep30(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatep30(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return P30;
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
		return P30;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// P-30 gate, test phase
class qgatepm30 : public qgatebase {
public:
	qgatepm30(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new qgatepm30(ida);
	}

	virtual qgatebase* copy() const
	{
		return new qgatepm30(*this);
		//return new qgates(id);
	}

	virtual qtype getkind() const
	{
		return PM30;
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
		return PM30;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};


// 2 qubit gates

// SWAP gate, top half
class swaptop : public qgatebase {
public:
	swaptop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new swaptop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new swaptop(*this);
		//return new swaptop(id);
	}

	virtual qtype getkind() const
	{
		return SWAPT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return SWAPT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// SWAP gate, bottom half
class swapbot : public qgatebase {
public:
	swapbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new swapbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new swapbot(*this);
		//return new swapbot(id);
	}

	virtual qtype getkind() const
	{
		return SWAPB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return SWAPT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CNOT gate, top half, control qubit
class cnottop : public qgatebase {
public:
	cnottop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cnottop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cnottop(*this);
		//return new cnottop(id);
	}

	virtual qtype getkind() const
	{
		return CNOTT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CNOTT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CNOT (CX) gate, bottom half, what gets controlled qubit
class cnotbot : public qgatebase {
public:
	cnotbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cnotbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cnotbot(*this);
		//return new cnotbot(id);
	}

	virtual qtype getkind() const
	{
		return CNOTB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CNOTT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CZ gate, top half, control qubit
class cztop : public qgatebase {
public:
	cztop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cztop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cztop(*this);
		//return new cztop(id);
	}

	virtual qtype getkind() const
	{
		return CZT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CZT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CZ gate, bottom half, what gets controlled qubit
class czbot : public qgatebase {
public:
	czbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new czbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new czbot(*this);
		//return new czbot(id);
	}

	virtual qtype getkind() const
	{
		return CZB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CZT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CY gate, top half, control qubit
class cytop : public qgatebase {
public:
	cytop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cytop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cytop(*this);
		//return new cytop(id);
	}

	virtual qtype getkind() const
	{
		return CYT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CYT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CY gate, bottom half, what gets controlled qubit
class cybot : public qgatebase {
public:
	cybot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cybot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cybot(*this);
		//return new cybot(id);
	}

	virtual qtype getkind() const
	{
		return CYB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CYT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CH gate, top half, control qubit
class chtop : public qgatebase {
public:
	chtop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new chtop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new chtop(*this);
		//return new chtop(id);
	}

	virtual qtype getkind() const
	{
		return CHT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CHT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CH gate, bottom half, what gets controlled qubit
class chbot : public qgatebase {
public:
	chbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new chbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new chbot(*this);
		//return new chbot(id);
	}

	virtual qtype getkind() const
	{
		return CHB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CHT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};




// CS gate, top half, control qubit
class cstop : public qgatebase {
public:
	cstop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cstop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cstop(*this);
		//return new cstop(id);
	}

	virtual qtype getkind() const
	{
		return CST;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CST;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CS gate, bottom half, what gets controlled qubit
class csbot : public qgatebase {
public:
	csbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new csbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new csbot(*this);
		//return new csbot(id);
	}

	virtual qtype getkind() const
	{
		return CSB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CST;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CSt gate, top half, control qubit
class csttop : public qgatebase {
public:
	csttop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new csttop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new csttop(*this);
		//return new csttop(id);
	}

	virtual qtype getkind() const
	{
		return CStT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CStT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CSt gate, bottom half, what gets controlled qubit
class cstbot : public qgatebase {
public:
	cstbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cstbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cstbot(*this);
		//return new cstbot(id);
	}

	virtual qtype getkind() const
	{
		return CStB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CStT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};



// CT gate, top half, control qubit
class cttop : public qgatebase {
public:
	cttop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cttop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cttop(*this);
		//return new cttop(id);
	}

	virtual qtype getkind() const
	{
		return CTT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CTT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CT gate, bottom half, what gets controlled qubit
class ctbot : public qgatebase {
public:
	ctbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new ctbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new ctbot(*this);
		//return new ctbot(id);
	}

	virtual qtype getkind() const
	{
		return CTB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CTT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CTt gate, top half, control qubit
class ctttop : public qgatebase {
public:
	ctttop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new ctttop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new ctttop(*this);
		//return new ctttop(id);
	}

	virtual qtype getkind() const
	{
		return CTtT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CTtT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CTt gate, bottom half, what gets controlled qubit
class cttbot : public qgatebase {
public:
	cttbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cttbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cttbot(*this);
		//return new cttbot(id);
	}

	virtual qtype getkind() const
	{
		return CTtB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CTtT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};




// CP45 gate, top half, control qubit
class cp45top : public qgatebase {
public:
	cp45top(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cp45top(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cp45top(*this);
		//return new cp45top(id);
	}

	virtual qtype getkind() const
	{
		return CP45T;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CP45T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CP45 gate, bottom half, what gets controlled qubit
class cp45bot : public qgatebase {
public:
	cp45bot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cp45bot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cp45bot(*this);
		//return new cp45bot(id);
	}

	virtual qtype getkind() const
	{
		return CP45B;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CP45T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CP -45 gate, top half, control qubit
class cpm45top : public qgatebase {
public:
	cpm45top(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cpm45top(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cpm45top(*this);
		//return new cpm45top(id);
	}

	virtual qtype getkind() const
	{
		return CPM45T;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CPM45T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CP -45 gate, bottom half, what gets controlled qubit
class cpm45bot : public qgatebase {
public:
	cpm45bot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cpm45bot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cpm45bot(*this);
		//return new cpm45bot(id);
	}

	virtual qtype getkind() const
	{
		return CPM45B;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CPM45T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CP30 gate, top half, control qubit
class cp30top : public qgatebase {
public:
	cp30top(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cp30top(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cp30top(*this);
		//return new cp30top(id);
	}

	virtual qtype getkind() const
	{
		return CP30T;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CP30T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CP30 gate, bottom half, what gets controlled qubit
class cp30bot : public qgatebase {
public:
	cp30bot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cp30bot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cp30bot(*this);
		//return new cp30bot(id);
	}

	virtual qtype getkind() const
	{
		return CP30B;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CP30T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// CP -30 gate, top half, control qubit
class cpm30top : public qgatebase {
public:
	cpm30top(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cpm30top(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cpm30top(*this);
		//return new cpm30top(id);
	}

	virtual qtype getkind() const
	{
		return CPM30T;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return CPM30T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	virtual qmat getqmat() const;
};

// CP -30 gate, bottom half, what gets controlled qubit
class cpm30bot : public qgatebase {
public:
	cpm30bot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new cpm30bot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new cpm30bot(*this);
		//return new cpm30bot(id);
	}

	virtual qtype getkind() const
	{
		return CPM30B;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return CPM30T;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// DEC gate, top half
class dectop : public qgatebase {
public:
	dectop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new dectop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new dectop(*this);
		//return new dectop(id);
	}

	virtual qtype getkind() const
	{
		return DECT;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return DECT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return true; }
	virtual qmat getqmat() const;
};

// DEC gate, bottom half
class decbot : public qgatebase {
public:
	decbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new decbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new decbot(*this);
		//return new decbot(id);
	}

	virtual qtype getkind() const
	{
		return DECB;
	}

	virtual U32 getnumparts() const
	{
		return 2;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return DECT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return true; }
};

// FREDKIN gate, top third
class fredkintop : public qgatebase {
public:
	fredkintop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new fredkintop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new fredkintop(*this);
		//return new fredkintop(id);
	}

	virtual qtype getkind() const
	{
		return FREDKINT;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return FREDKINT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};

// FREDKIN gate, middle third
class fredkinmid : public qgatebase {
public:
	fredkinmid(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new fredkinmid(ida);
	}

	virtual qgatebase* copy() const
	{
		return new fredkinmid(*this);
		//return new fredkinmid(id);
	}

	virtual qtype getkind() const
	{
		return FREDKINM;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return FREDKINT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// FREDKIN gate, bottom third
class fredkinbot : public qgatebase {
public:
	fredkinbot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new fredkinbot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new fredkinbot(*this);
		//return new fredkinbot(id);
	}

	virtual qtype getkind() const
	{
		return FREDKINB;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 2;
	}

	virtual qtype getbasekind() const
	{
		return FREDKINT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI gate, top third
class toffolitop : public qgatebase {
public:
	toffolitop(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffolitop(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffolitop(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLIT;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLIT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};

// TOFFOLI gate, middle third
class toffolimid : public qgatebase {
public:
	toffolimid(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffolimid(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffolimid(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLIM;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLIT;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI gate, bottom third
class toffolibot : public qgatebase {
public:
	toffolibot(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffolibot(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffolibot(*this);
		//return new toffolibot(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLIB;
	}

	virtual U32 getnumparts() const
	{
		return 3;
	}

	virtual U32 getcurpart() const
	{
		return 2;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLIT;
	}

	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 4 gate, C0
class toffoli4c0 : public qgatebase {
public:
	toffoli4c0(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli4c0(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli4c0(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI4C0;
	}

	virtual U32 getnumparts() const
	{
		return 4;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI4C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};

// TOFFOLI 4 gate, C1
class toffoli4c1 : public qgatebase {
public:
	toffoli4c1(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli4c1(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli4c1(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI4C1;
	}

	virtual U32 getnumparts() const
	{
		return 4;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI4C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	//virtual qmat getqmat() const;
};

// TOFFOLI 4 gate, C2
class toffoli4c2 : public qgatebase {
public:
	toffoli4c2(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli4c2(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli4c2(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI4C2;
	}

	virtual U32 getnumparts() const
	{
		return 4;
	}

	virtual U32 getcurpart() const
	{
		return 2;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI4C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 4 gate, Target (T)
class toffoli4t : public qgatebase {
public:
	toffoli4t(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli4t(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli4t(*this);
		//return new toffolibot(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI4T;
	}

	virtual U32 getnumparts() const
	{
		return 4;
	}

	virtual U32 getcurpart() const
	{
		return 3;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI4C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 5 gate, C0
class toffoli5c0 : public qgatebase {
public:
	toffoli5c0(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli5c0(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli5c0(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI5C0;
	}

	virtual U32 getnumparts() const
	{
		return 5;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI5C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};

// TOFFOLI 5 gate, C1
class toffoli5c1 : public qgatebase {
public:
	toffoli5c1(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli5c1(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli5c1(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI5C1;
	}

	virtual U32 getnumparts() const
	{
		return 5;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI5C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	//virtual qmat getqmat() const;
};

// TOFFOLI 5 gate, C2
class toffoli5c2 : public qgatebase {
public:
	toffoli5c2(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli5c2(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli5c2(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI5C2;
	}

	virtual U32 getnumparts() const
	{
		return 5;
	}

	virtual U32 getcurpart() const
	{
		return 2;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI5C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 5 gate, C3
class toffoli5c3 : public qgatebase {
public:
	toffoli5c3(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli5c3(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli5c3(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI5C3;
	}

	virtual U32 getnumparts() const
	{
		return 5;
	}

	virtual U32 getcurpart() const
	{
		return 3;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI5C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 5 gate, Target (T)
class toffoli5t : public qgatebase {
public:
	toffoli5t(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli5t(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli5t(*this);
		//return new toffolibot(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI5T;
	}

	virtual U32 getnumparts() const
	{
		return 5;
	}

	virtual U32 getcurpart() const
	{
		return 4;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI5C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 6 gate, C0
class toffoli6c0 : public qgatebase {
public:
	toffoli6c0(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6c0(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6c0(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6C0;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 0;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	virtual qmat getqmat() const;
};

// TOFFOLI 6 gate, C1
class toffoli6c1 : public qgatebase {
public:
	toffoli6c1(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6c1(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6c1(*this);
		//return new toffolitop(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6C1;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 1;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
	//virtual bool hasmultibox() const { return false; }
	//virtual qmat getqmat() const;
};

// TOFFOLI 6 gate, C2
class toffoli6c2 : public qgatebase {
public:
	toffoli6c2(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6c2(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6c2(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6C2;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 2;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 6 gate, C3
class toffoli6c3 : public qgatebase {
public:
	toffoli6c3(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6c3(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6c3(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6C3;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 3;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 6 gate, C4
class toffoli6c4 : public qgatebase {
public:
	toffoli6c4(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6c4(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6c4(*this);
		//return new toffolimid(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6C4;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 4;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};

// TOFFOLI 6 gate, Target (T)
class toffoli6t : public qgatebase {
public:
	toffoli6t(U32 ida) : qgatebase(ida) {}
	// factory
	static qgatebase* create(U32 ida)
	{
		return new toffoli6t(ida);
	}

	virtual qgatebase* copy() const
	{
		return new toffoli6t(*this);
		//return new toffolibot(id);
	}

	virtual qtype getkind() const
	{
		return TOFFOLI6T;
	}

	virtual U32 getnumparts() const
	{
		return 6;
	}

	virtual U32 getcurpart() const
	{
		return 5;
	}

	virtual qtype getbasekind() const
	{
		return TOFFOLI6C0;
	}

	// draw
	virtual void draw(bitmap32* bm,U32 xoff,U32 yoff) const;
};
