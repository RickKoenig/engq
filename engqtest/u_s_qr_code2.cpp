#define INCLUDE_WINDOWS
//#define D2_3D
#include <m_eng.h>
#include <l_misclibm.h>
#include <vfw.h>
#include "u_states.h"
#include "u_qr_code2parse.h"
#include "u_qr_code2scan.h"

#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

static shape *rl,*focus=0;
static pbut* quit;
static pbut* vcstart,*see,*click,*vcstop,*loadp,*loadm,*scan,*del,*url,*vcquit;
static pbut* pvidcomp,*pvidformat,*pvidsource;
static text* tstatus;

static struct bitmap32* cbm32; // owned by vcam
static struct bitmap32* gallbm,*scanbm; // owned by u_s_qr_code2
string messageresult;

static S32 frame;

class gallery {
	vector<string> gall;
	S32 curidx; // -1 if none
	U32 next; // unused, number
public:
	string load();
	string loadp();
	string loadm();
	string save();
	string del();
	string getname();
	gallery();
};

gallery* galli;

gallery::gallery() : curidx(-1),next(0)
{
//	U32 so[5] = {5,3,7};
	scriptdir sc(0);
	S32 i,j; // qrcode00000.png
#if 1
	for (i=0;i<sc.num();++i) {
		bool goodext = false;
		C8 ext[256];
		mgetext(sc.idx(i).c_str(),ext,256);
		if (!my_stricmp(ext,"jpg"))
			goodext = true;
		else if (!my_stricmp(ext,"png"))
			goodext = true;
		if (goodext) {
			const string& f = sc.idx(i); // look for grcodennnnn.nnn, numbered .pngs
			if (f.size() == 15) {
				string f2 = f;
				for (j=0;j<(S32)f.size();++j) {
					if (f2[j] >= '0' && f2[j] <= '9')
						f2[j] = '0';
				}
				if (f2 == "qrcode00000.png") {
					string ss = f.substr(6,5);
					U32 v = atoi(ss.c_str());
					// logger("found a numbered qrcode '%s' %'\n",f.c_str(),v);
					if (v >= next) // unused file
						next = v + 1;
				}
			}
			gall.push_back(f);
		}
	}
	::sort(gall.begin(),gall.end());
//	for (i=0;i<(S32)gall.size();++i)
//		logger("gall[%d] = '%s'\n",i,gall[i].c_str());
#endif
	logger("next gallery save number = %d\n",next);
}

// name of file to save click to
string gallery::save()
{
	C8 rc[100];
	sprintf(rc,"qrcode%05d.png",next);
	gall.push_back(rc);
	++next;
	::sort(gall.begin(),gall.end());
	vector<string>::iterator res = ::find(gall.begin(),gall.end(),rc);
	curidx = res - gall.begin();
	return gall[curidx];
}
//	if (idx	gall.erase(gall.begin()+5);

string gallery::del()
{
	if (gall.empty())
		return "";
	if (curidx == -1)
		return "";
	gall.erase(gall.begin()+curidx);
	if (curidx >= (S32)gall.size())
		curidx = gall.size()-1;
	if (curidx == -1)
		return "";
	return gall[curidx];
}

string gallery::loadp()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	++curidx;
	if (curidx >= (S32)gall.size())
		curidx = 0;
	return gall[curidx];
}

string gallery::loadm()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	--curidx;
	if (curidx < 0)
		curidx = (S32)gall.size() - 1;
	return gall[curidx];
}

string gallery::getname()
{
	if (gall.empty())
		return "";
	if (curidx == -1)
		return "";
	return gall[curidx];
}

string gallery::load()
{
	if (gall.empty()) {
		curidx = -1;
		return "";
	}
	if (curidx == -1)
		curidx = 0;
	return gall[curidx];
}


void freebms()
{
	if (scanbm) {
		bitmap32free(scanbm);
		scanbm = 0;
	}
	if (gallbm) {
		bitmap32free(gallbm);
		gallbm = 0;
	}
}

#ifdef USENS
}
#endif

void qrcodeinit2()
{
// setup state
	video_setupwindow(800,600);//,565);
	pushandsetdir("ifield");
	rl=res_loadfile("qrcode2res.txt");
#if 1
	equiv ec;
	ec.addsi(squarei(52,152,5));
	ec.addsi(squarei(50,50,5));
	ec.addsi(squarei(150,50,5));
	ec.addsi(squarei(50,52,5));
	ec.addsi(squarei(152,50,5));
	ec.addsi(squarei(50,150,5));
	ec.addsi(squarei(0,0,5));
	ec.addsi(squarei(0,0,5));
	ec.addsi(squarei(0,0,5));
	ec.addsi(squarei(0,0,5));
	vector<pointi2> tgs = ec.gettargets();
#endif

#if 0
// test message
	U32 test[26] = {0x40,0xd2,0x75,0x47,0x76,0x17,0x32,0x06,0x27,0x26,0x96,0xc6,0xc6,0x96,0x70,0xec,
		0xbc,0x2a,0x90,0x13,0x6b,0xaf,0xef,0xfd,0x4b,0xe0};
	S32 i;
	U32 testr[26];
	for (i=0;i<26;++i)
		testr[i] = test[26-i-1];
	rs testrs(testr,testr+26);
	S32 nerrs = 0;
	S32 nerasu = 0;
	rs_correct_msg(testrs,10,&nerrs,&nerasu);
	logger("errs = %d, erasu = %d\n",nerrs,nerasu);
	// end test message
#endif
// enum camera devices
	frame=0;
	galli = new gallery();
#if 0
	logger("loadp = '%s'\n",galli->loadp().c_str());
	logger("loadp = '%s'\n",galli->loadp().c_str());
	logger("loadp = '%s'\n",galli->loadp().c_str());
	logger("del = '%s'\n",galli->del().c_str());
	logger("loadp = '%s'\n",galli->loadp().c_str());
	logger("del = '%s'\n",galli->del().c_str());
	logger("save = '%s'\n",galli->save().c_str());
	logger("save = '%s'\n",galli->save().c_str());
#endif
	// get dir of qrcode?????.png files
// find all buttons
	pvidcomp=rl->find<pbut>("PVIDCOMP");
//	pviddisplay=rl->find<pbut>("PVIDDISPLAY");
	pvidformat=rl->find<pbut>("PVIDFORMAT");
	pvidsource=rl->find<pbut>("PVIDSOURCE");

	vcstart=rl->find<pbut>("VCSTART");
	see=rl->find<pbut>("SEE");
	click=rl->find<pbut>("SAVE");
	loadp=rl->find<pbut>("LOADP");
	loadm=rl->find<pbut>("LOADM");
	scan=rl->find<pbut>("SCAN");
	del=rl->find<pbut>("DEL");
	url=rl->find<pbut>("URL");
	vcstop=rl->find<pbut>("VCSTOP");
	vcquit=rl->find<pbut>("VCQUIT");
//	p3d=rl->find<pbut>("P3D");
	quit=rl->find<pbut>("QUIT");
	tstatus=rl->find<text>("TSTATUS");
	focus=0;
	gallbm = 0;
	scanbm = 0;
}

void qrcodeproc2()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
	scan->setvis(gallbm!=0);
	click->setvis(cbm32!=0 && gallbm==0 && scanbm==0);
	del->setvis(gallbm!=0 && scanbm==0);
	url->setvis(scanbm!=0 && !messageresult.empty());
// set focus
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
// if something selected...
	bool ss = false;
	if (focus) {
		S32 ret=focus->proc();
		if (ret == 1) {
			if (focus==quit) { // quit button
				popstate();
			} else if (focus==pvidcomp) {
				vcamcompression();
			} else if (focus==pvidformat) {
				vcamformat();
			} else if (focus==pvidsource) {
				vcamsource();


			} else if (focus==vcstart) {
				if (vcaminit())
					tstatus->settname("vcam driver inited");
				else
					tstatus->settname("can't init vcam driver");

			} else if (focus==loadp) { // load a file into snapshot
				string lf = gallbm && !scanbm ? galli->loadp() : galli->load(); // if gallbm visible then loadp
				freebms();
				gallbm = gfxread32(lf.c_str());
			} else if (focus==loadm) { // load a file into snapshot
				string lf = gallbm && !scanbm ? galli->loadm() : galli->load(); // if gallbm visible then loadm
				freebms();
				gallbm = gfxread32(lf.c_str());
			} else if (focus==scan) { // scan a snapshot
				if (scanbm) {
					bitmap32free(scanbm);
					scanbm = 0;
				}
				if (gallbm) {
					vector<U8> mess = readqrcode(galli->getname(),gallbm,&scanbm);
					mess.push_back(0); // safety, null terminate to print out as a cstr
					logger("mess result = '%s'\n",&mess[0]);
					messageresult = (C8*)&mess[0];
				}
			} else if (focus==url) { // launch a browser from the message
				ShellExecute(0,"open",messageresult.c_str(),0,0,SW_SHOWNORMAL);
			} else if (focus==del) { // delete a snapshot
				string delf = galli->getname();
				if (!delf.empty())
					logger("remove '%s'\n",delf.c_str());
				remove(delf.c_str());
				string lf = galli->del(); // remove from list and find next index
				freebms();
				if (!lf.empty())
					gallbm = gfxread32(lf.c_str());
			} else if (focus==see) { // see live, null out snapshot
				freebms();
				if (vcamstartstream()) {
					tstatus->settname("vcam streaming");
				} else
					tstatus->settname("can't stream vcam");
			} else if (focus==click) { // load camera to snapshot and save snapshot later
				ss = true;

			} else if (focus==vcstop) {
				vcamstopstream();
				tstatus->settname("done streaming");
			} else if (focus==vcquit) {
				vcamstopstream();
				vcamexit();
				tstatus->settname("done with driver");
			}
		}
	}
	cbm32=vcamgrabframe();
	if (cbm32 && ss) {
		freebms();
		gallbm = bitmap32copy(cbm32);
		string ss = galli->save();
		gfxwrite32(ss.c_str(),gallbm);
	}
	frame++;
}

void qrcodedraw2d2()
{
	clipclear32(B32,C32(20,20,80));
	if (scanbm) {
		clipblit32(scanbm,B32,0,0,(WX-scanbm->size.x)/2,24,scanbm->size.x,scanbm->size.y);
		outtextxyc32(B32,WX/2,scanbm->size.y + 32,C32WHITE,"Scanned image");
		outtextxyfc32(B32,WX/2,scanbm->size.y + 40,C32WHITE,"Message = '%s'",messageresult.c_str());
	} else if (gallbm) {
		clipblit32(gallbm,B32,0,0,(WX-gallbm->size.x)/2,24,gallbm->size.x,gallbm->size.y);
		C8 str[256];
		sprintf(str,"gall filename = '%s'",galli->getname().c_str());
		outtextxyc32(B32,WX/2,gallbm->size.y + 32,C32WHITE,str);
	} else if (cbm32) {
		clipblit32(cbm32,B32,0,0,(WX-cbm32->size.x)/2,24,cbm32->size.x,cbm32->size.y);
		const C8* format = getformatname();
		outtextxyfc32(B32,WX/2,cbm32->size.y + 32,C32WHITE,"Live feed, hit save to put in gallery read to scan '%s'",format);
	} else {
		outtextxyc32(B32,WX/2,256,C32WHITE,"Load  or  init driver and see");
	}
	rl->draw();
	outtextxyf32(B32,10,10,C32WHITE,"vidcap frame %6d",frame);
}

void qrcodeexit2()
{
	vcamstopstream();
	vcamexit();
	delete rl;
	popdir();
	freebms();
	delete galli;
}
