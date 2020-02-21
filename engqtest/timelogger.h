// print various log files every minute,hour,day,new,delete
//#define MINUTELOG
#define HOURLOG
#define DAYLOG

#define MINUTENEW 1
#define HOURNEW 2
#define DAYNEW 4

class timelogger {
	FILE* minutelog;
	FILE* hourlog;
	FILE* daylog;
	const C8* fdir,*fprefix;
	tm curtime;
	U32 changed;
	void proc();
public:
	timelogger(const C8* folder="",const C8* prefix="timelog_");
	void printf(const C8* form,...); 
	~timelogger();
	U32 getchange() {U32 ret=changed; changed=0; return ret;} // returns 'or' of MINUTENEW,HOURNEW,DAILYNEW
};
