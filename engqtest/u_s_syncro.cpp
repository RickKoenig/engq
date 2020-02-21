#define INCLUDE_WINDOWS
#include <m_eng.h>

static int ribgs;		// save run in background state
static con32* con;		// console
static int cnt;			// counter for polling every 2 seconds
static int apprunning; 	// is hardrace running, check this variable to know

int isapprunning()
{
	HANDLE mut_drv;
	mut_drv=OpenMutex(0,0,"stuntmp"); // open an existing mutex
	if (mut_drv) {
		CloseHandle(mut_drv); // we opened mutex, we should now close it, app must be running, 
		return 1;
	} else {
		unsigned int err=GetLastError();
		if (err==ERROR_FILE_NOT_FOUND) // mutex doesn't exist, app is not running
			return 0;
		return 1; // mutex exists, but probably access denied, app is running
	}
}

void syncro_init()
{
	video_setupwindow(640,480);
	ribgs = wininfo.runinbackground;
	wininfo.runinbackground = 1; // lets keep this puppy running in background
	con=con32_alloc(320,200,C32WHITE,C32BLACK);
	cnt=0;
	con32_printf(con,"D R I V E R\n");
}

void syncro_proc()
{
	if (KEY==K_ESCAPE)
		popstate();
	++cnt;
	if (cnt==60) { // check to see if app is running once in a while
		apprunning=isapprunning();
		if (apprunning)
			con32_printf(con,"drv: app running\n");
		else
			con32_printf(con,"drv: app not running\n");
		cnt=0;
	}
}

void syncro_draw2d()
{
	clipclear32(B32,C32(0,0,255));	
	outtextxyf32(B32,20,20,C32WHITE,"Driver");
	bitmap32* cbm=con32_getbitmap32(con);
	clipblit32(cbm,B32,0,0,160,140,320,200);
}

void syncro_exit()
{
	wininfo.runinbackground=ribgs;
	con32_free(con);
}
