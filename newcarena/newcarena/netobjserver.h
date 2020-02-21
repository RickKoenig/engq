class netserverobj : public netobj {
	S32 starttime;
	S32 servertimeout; // a timeout
	U32 myip;
	bool live; // true when network ready, never resets, even after master is shutdown
	S32 curclients;
	S32 curclientsloggedon;
	bool curclientschanged;
	socker* master;
	vector<clnt> clients;
	bool wrotego; // sent all gos to WRITEBUFF
public:
	netserverobj(gamedescj* gd);
	virtual string proc();
// key packets, game will keep track of current time
	virtual void writekey(const keys& key); // ingame: send key to collective
	virtual ~netserverobj();
	virtual S32 getnumsockets();
	virtual S32 getwritebuffsize(S32 slot);
	virtual S32 getwritebuffused(S32 slot);
	virtual void writegen(PACK_KIND id,const vector<U8>& mess);

	S32 getcurclientsloggedon() const { return curclientsloggedon; }
	bool getcurclientschanged() { bool r = curclientschanged; curclientschanged=false; return r;}
	const socker* getmaster() const { return master; }
};
