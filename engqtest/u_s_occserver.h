void occserverinit(),occserverproc(),occserverdraw2d(),occserverexit();

// machine endian
U32 readU16(const C8* buff);
U32 readU32(const C8* buff);
float readfloat(const C8* buff);
void writeU16(C8* buff,U32 val);
void writeU32(C8* buff,U32 val);
void writefloat(C8* buff,float val);
#define MAXGAMES 15000
#define MAXPLAYERS 5 // per game
#define MAXCLIENTS (MAXGAMES*MAXPLAYERS)

// everything here little endian
extern S32 serverport;

// packet format is like this
// 4bytes total length of packet, 1 byte messtype,length-1 data

// prefix is who sends message

// messtypes

// servermessages from server
enum {S2C_MESS,S2C_SLOT,S2C_WEEK=9,S2C_CHAT=10,S2C_POS}; // text reply from server
// mess: 
//	1 byte messtype, len-1 bytes of char data
// slot: 
//	1 byte messtype, 1 byte playernum, 2 bytes game number
// chat: 
//	1 byte messtype, 1 byte playernum(0-3), len-2 bytes of char data
// pos:
//	1 byte messtype, 1 byte playernum, 3 floats for pos, 2 bytes for level number
//	for a total byte lenth of 16
// clientmessages to server

enum {C2S_LOGON,C2S_CHAT=10,C2S_POS};
// logon: 
//  params.. playernum 0-3, gamenum 0-16383 for a max of 65536 players and password
//	1 byte messtype, 2bytes value = playernum + maxplayers*gamenum (slotnum requested), len-3 bytes for passord
// chat: 
//	1 byte messtype, len-1 bytes of char data (kinda like SRV_MESS
// pos:
//	1 byte messtype, 3 floats for pos, 2 bytes for level number
//	for a total byte lenth of 15
