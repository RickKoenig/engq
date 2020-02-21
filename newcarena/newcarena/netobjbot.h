// implement this one first
class netbotgameobj : public netobj {
public:
	netbotgameobj(gamedescj* gd);
	virtual string proc();
// key packets, game will keep track of current time
	virtual S32 getnumsockets();
	virtual S32 getwritebuffsize(S32 slot);
	virtual S32 getwritebuffused(S32 slot);
};
