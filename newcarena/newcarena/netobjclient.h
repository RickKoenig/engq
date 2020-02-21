class netclientobj : public netobj {
	S32 starttime;
	S32 clienttimeout; // a global timeout
	U32 myip;
	clnt client;
	avaj myaj; // backup copy for hi packet
	welcomej* wc;
public:
	netclientobj(gamedescj* gd);
	virtual string proc();
// key packets, game will keep track of current time
	virtual void writekey(const keys& key); // ingame: send key to collective
	virtual void writenone(); // ingame: send none to collective, (keepalive for server, waiting for all players to arrive)
	virtual ~netclientobj();
	virtual S32 getnumsockets();
	virtual S32 getwritebuffsize(S32 slot);
	virtual S32 getwritebuffused(S32 slot);
	virtual void writegen(PACK_KIND id,const vector<U8>& mess);
};
