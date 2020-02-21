#include <time.h>
#include <m_eng.h>

#include "timelogger.h"

timelogger::timelogger(const C8* fdira,const C8* fprefixa) : 
		minutelog(0),hourlog(0),daylog(0),fdir(fdira),fprefix(fprefixa),changed(0)
{
	const time_t ti=time(0);
	tm* newtime=gmtime(&ti);
	curtime=*newtime;
	proc();
}

void timelogger::printf(const C8* fmt,...)
{
	va_list arglist;
	va_start(arglist,fmt);
	proc();
	if (daylog)
		vfprintf(daylog,fmt,arglist);
	if (hourlog)
		vfprintf(hourlog,fmt,arglist);
	if (minutelog)
		vfprintf(minutelog,fmt,arglist);
	va_end(arglist);
}

void timelogger::proc()
{
	const time_t ti=time(0);
	tm* newtime=gmtime(&ti);
#ifdef MINUTELOG
	if (!minutelog || newtime->tm_min!=curtime.tm_min) {
		if (minutelog) {
			fprintf(minutelog,"close minutelog at %s",asctime(newtime));
			fclose(minutelog);
			minutelog=0;
		}
		C8 fname[300];
		sprintf(fname,"%s%04d_%02d_%02d_%02d_%02d.log",
			fprefix,newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min);
		pushandsetdir(fdir);
		S32 fe=fileexist(fname);
		minutelog=fopen2(fname,"a");
		popdir();
		if (fe)
			fprintf(minutelog,"append minutelog at %s",asctime(newtime));
		else
			fprintf(minutelog,"open minutelog at %s",asctime(newtime)); 
		changed|=MINUTENEW;
	}
#endif
#ifdef HOURLOG
	if (!hourlog || newtime->tm_hour!=curtime.tm_hour) {
		if (hourlog) {
			fprintf(hourlog,"close hourlylog at %s",asctime(newtime));
			fclose(hourlog);
			hourlog=0;
		}
		C8 fname[300];
		sprintf(fname,"%s%04d_%02d_%02d_%02d.log",
			fprefix,newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour);
		pushandsetdir(fdir);
		S32 fe=fileexist(fname);
		hourlog=fopen2(fname,"a");
		popdir();
		if (fe)
			fprintf(hourlog,"append hourlylog at %s",asctime(newtime));
		else
			fprintf(hourlog,"open hourlylog at %s",asctime(newtime));
		changed|=HOURNEW;
	}
#endif
#ifdef DAYLOG
	if (!daylog || newtime->tm_mday!=curtime.tm_mday) {
		if (daylog) {
			fprintf(daylog,"close daylog at %s",asctime(newtime));
			fclose(daylog);
			daylog=0;
		}
		C8 fname[300];
		sprintf(fname,"%s%04d_%02d_%02d.log",
			fprefix,newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday);
		pushandsetdir(fdir);
		S32 fe=fileexist(fname);
		daylog=fopen2(fname,"a");
		popdir();
		if (fe)
			fprintf(daylog,"append daylog at %s",asctime(newtime));
		else
			fprintf(daylog,"open daylog at %s",asctime(newtime));
		changed|=DAYNEW;
	}
#endif
	curtime=*newtime;
}

timelogger::~timelogger()
{
	const time_t ti=time(0);
	tm* newtime=gmtime(&ti);
	if (minutelog) {
		fprintf(minutelog,"close minuteslog at %s",asctime(newtime));
		fclose(minutelog);
	}
	if (hourlog) {
		fprintf(hourlog,"close hourlylog at %s",asctime(newtime));
		fclose(hourlog);
	}
	if (daylog) {
		fprintf(daylog,"close daylog at %s",asctime(newtime));
		fclose(daylog);
	}
}
