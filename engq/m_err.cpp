#define INCLUDE_WINDOWS
#include <m_eng.h>

void errorexit(const C8* fmt,...)
{
	C8 errorstr[1024];
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(errorstr,fmt,arglist);
	va_end(arglist);
	logger("ERROR: '%s'\nadded from ERROR: MEMCLEAN\n",errorstr);
	MessageBox((HWND)wininfo.MainWindow,errorstr,"There has been an error...",MB_ICONERROR|MB_OK);
	exit(1);
}

void error(const C8* fmt,...)
{
	C8 errorstr[1024];
	va_list arglist;
	va_start(arglist,fmt);
	vsprintf(errorstr,fmt,arglist);
	va_end(arglist);
	logger("WARNING: '%s'\n",errorstr);
	MessageBox((HWND)wininfo.MainWindow,errorstr,"This is just a warning...",MB_ICONERROR|MB_OK);
}

