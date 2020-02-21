// sv to cl: from mess
// cl to sv: to mess
void sendapacket(int from,int to,int kind,int len,char *str); // if 'to' is -1 then to all
int getapacket(int *from,int *kind,unsigned char *str); // returns len
void packetread();	// read and process all packets
int packetwrite(); // write out all packets, returns 1 if all data sent

enum {
// ingame
	PKT_HERE=1, // also used to start a new game
	PKT_KEY=2,
	PKT_REQREMATCH=3, 
	PKT_DISCONNECT=4,
	PKT_NOP=5,
	PKT_GSTATE=6,
// connecting
	PKT_HI=7, // client sends this to server, has player info
	PKT_PL=8, // server sends this to clients, has track info
	PKT_GO=9, // server sends this to clients when it's time to load game and go, has other players info
// results
	PKT_RESULT=10,
};

void getinput(struct ol_playerdata *p);
void sendinput(struct ol_playerdata *p);
int someinput(struct ol_playerdata *p,int k);
void removeinput(int from,int ts); // this timestamp and earlier not needed anymore..
void resetinputp(int from); // for rematch
void packetaddinput(int from,int k,int wk);//,int ts);
// test
void getbackupinput(struct ol_playerdata *p);

void saveplayerstate2(struct ol_playerdata *p,struct ol_playerdata *pk);
void restoreplayerstate2(struct  ol_playerdata *pk,struct ol_playerdata *p);
void saveplayerstate(struct ol_playerdata *p,struct playerstatepacket *pk);
void restoreplayerstate(struct playerstatepacket *pk,struct ol_playerdata *p);
void restoreglobalstate(struct globalstate *globalstatepacket);
void saveglobalstate(struct globalstate *globalstatepacket);

void requestrematch();
