#define INCLUDE_WINDOWS
//#define D2_3D
//#define RES3D // alternate shape class
#include <m_eng.h>
#include <l_misclibm.h>
//#include "u_testasmmacros.h"
#include "utils/modelutil.h"

// RTTI used alot here... (dynamic_cast) in the menushapes->find<T> function

namespace newres {

#if 1
shape* menushapes;
//S32 mainidx=-1; // what is currently selected in state select listbox
shape* focus;
wavehandle* twh;
// mainmenu parts
listbox* mlb1,*mlb2,*mlb3,*mlb4,*mlb5,*mlb6,*mlb7,/**mlb8,*/*mlbxy,*mlbmic;
pbut* pb1,*pb2,*pb3,*pb4,*pb5,*pb6,*pb7;

hscroll* sliderr,*sliderg,*sliderb;
text* textrgb;
text* texthsv,*textrgb2;
S32 idxr=200,idxg=20,idxb=150; // 0 - 255
C32 rgbcol;
S32 hue=0,sat=255,val=255; // 0-359,0-255,0-255
S32 backr=255,backg=255,backb=255; // 0-255
hscroll* sliderhue;
S32 idxhue;
text* texthue;
hscroll* slidersat;
S32 idxsat;
text* textsat;
hscroll* sliderval;
S32 idxval;
text* textval;
#endif
edit *testhex,*testfloat;

//vscroll* scl1,*sclxy;
vector<S32> statenumlist;

}
using namespace newres;

void mainmenuinit()
{
//	U8* test=new U8[100];
//	delete[] test;
// setup video
	video_setupwindow(800,600);
// setup res factory and load a res, set all res variables
	factory2<shape> fact;
	pushandsetdir("mainmenu");
	script* sc=new script("mainres.txt");
	menushapes=fact.newclass_from_handle(*sc);
	delete sc;
	pb1=menushapes->find<pbut>("PBUT1");
	pb2=menushapes->find<pbut>("PBUT2");
	pb3=menushapes->find<pbut>("PBUT3");
	pb4=menushapes->find<pbut>("PBUT4");
	pb5=menushapes->find<pbut>("PBUT5");
	pb6=menushapes->find<pbut>("PBUT6");
	pb7=menushapes->find<pbut>("PBUT7");
	mlb1=menushapes->find<listbox>("LISTBOX1");
	mlb2=menushapes->find<listbox>("LISTBOX2");
	mlb3=menushapes->find<listbox>("LISTBOX3");
	mlb4=menushapes->find<listbox>("LISTBOX4");
	mlb5=menushapes->find<listbox>("LISTBOX5");
	mlb6=menushapes->find<listbox>("LISTBOX6");
	mlb7=menushapes->find<listbox>("LISTBOX7");
//	mlb8=menushapes->find<listbox>("LISTBOX8");
	mlbxy=menushapes->find<listbox>("LISTXY");
	mlbmic=menushapes->find<listbox>("LISTMIC");
//	scl1=menushapes->find<vscroll>("SCL1");
//	sclxy=menushapes->find<vscroll>("SCLXY");
// test rgb to hsv and back
	sliderr=menushapes->find<hscroll>("sliderr");
	sliderr->setnumidx(256);
	sliderr->setidx(idxr);
	sliderg=menushapes->find<hscroll>("sliderg");
	sliderg->setnumidx(256);
	sliderg->setidx(idxg);
	sliderb=menushapes->find<hscroll>("sliderb");
	sliderb->setnumidx(256);
	sliderb->setidx(idxb);
	textrgb=menushapes->find<text>("textrgb");
	texthsv=menushapes->find<text>("texthsv");
	textrgb2=menushapes->find<text>("textrgb2");
	sliderhue=menushapes->find<hscroll>("sliderhue");
	sliderhue->setnumidx(360);
	texthue=menushapes->find<text>("texthue");
	slidersat=menushapes->find<hscroll>("slidersat");
	slidersat->setnumidx(320);
	slidersat->setidx(255);
	textsat=menushapes->find<text>("textsat");
	sliderval=menushapes->find<hscroll>("sliderval");
	sliderval->setnumidx(256);
	sliderval->setidx(255);
	textval=menushapes->find<text>("textval");
// test hex <-> float
	testhex=menushapes->find<edit>("testhex");
	testfloat=menushapes->find<edit>("testfloat");
// load mainmenu state names and numbers
	sc=new script("mainmenu.txt");
	S32 i,n=sc->num();
	if (n%3) {
		for (i=0;i<n;++i)
			logger("%3d: \"%s\"\n",i,(*sc).printidx(i).c_str());
		errorexit("bad mainmenu.txt");
	}
// into LISTBOX1 and LISTBOX8
	for (i=0;i<n;) {
		if (!strcmp((*sc).idx(i).c_str(),"statedesc")) {
			C8 str[500];
			memset(str,' ',500);
			strcpy(str,(*sc).idx(i+2).c_str());
			str[strlen(str)]=' '; // what a hack!
//			mlb1->addstring((*sc).idx(i+2).c_str());
//			(rl,LISTBOX1,-1,(*sc)[i+2],SCL1);
			S32 statenum=atoi((*sc).idx(i+1).c_str());
			C8 str2[10];
			sprintf(str2,"%4d",statenum);
			strcpy(str+45,str2);
			mlb1->addstring(str);
			statenumlist.push_back(statenum);
//			addlistboxname(rl,LISTBOX8,-1,str,-1);
			i+=3;
		} else
			errorexit("unknown mainmenu.txt command \"%s\"",(*sc).printidx(i).c_str());
	}
//	scl1->setnumidx(mlb1->getnumoffsets());
	delete sc;
// load all gamexy resolutions into listbox LISTXY
	for (i=0;i<videoinfo.ngamexy;++i) {
		C8 str[50];
		sprintf(str,"%d %d",videoinfo.gamexy[i].x,videoinfo.gamexy[i].y);
//		addlistboxname(rl,LISTXY,-1,str,SCLXY);
		mlbxy->addstring(str);
	}
//	sclxy->setnumidx(mlbxy->getnumoffsets());
//	sortlistbox(rl,LISTBOX1,LISTBOX8,-1,-1,0);
//	mainidx=-1;
// load current state to run when 'start state' selected
	sc=new script("maincfg.txt");
	S32 mainidx=-1;
	if (sc->num()>0) {
		if (sc->num()!=2 || strcmp((*sc).idx(0).c_str(),"mainidx"))
			errorexit("bad maincfg.txt");
		mainidx=atoi((*sc).idx(1).c_str());
		if (mainidx>=0) {
			mlb1->setidxc(mainidx);
//			mlb8->setidxc(mainidx);
		}
	}
//	scl1->setidx(mlb1->getcuroffset());
	delete sc;
// set start state
	if (mainidx<0) {
		pb1->setvis(0);
	}
	focus=mlb1; //	settextfocus(rl,LISTBOX1);
// set video drivers listbox text
	sc=video_getnmaindrivers();//new script;
	for (i=0;i<sc->num();++i)
		mlb2->addstring((*sc).idx(i).c_str());
	delete sc;
	sc=video_getnsubdrivers(videoinfo.video_maindriver);//new script;
	for (i=0;i<sc->num();i++)
		mlb3->addstring((*sc).idx(i).c_str());
	delete sc;
	mlb2->setidxv(videoinfo.video_maindriver);
	mlb3->setidxv(videoinfo.video_subdriver);
// set audio drivers listbox text
	sc=audio_getnmaindrivers();//new script;
	for (i=0;i<sc->num();i++)
		mlb4->addstring((*sc).idx(i).c_str());
	delete sc;
	sc=audio_getnsubdrivers(audioinfo.audio_maindriver);//new script;
	for (i=0;i<sc->num();i++)
		mlb5->addstring((*sc).idx(i).c_str());
	delete sc;
	sc=mic_getnsubdrivers(audioinfo.audio_maindriver);//new script;
	for (i=0;i<sc->num();i++)
		mlbmic->addstring((*sc).idx(i).c_str());
	delete sc;
	mlb4->setidxv(audioinfo.audio_maindriver);
	mlb5->setidxv(audioinfo.audio_subdriver);
	mlbmic->setidxv(audioinfo.audio_micsubdriver);
// load audo test files
	twh=wave_load("synflute.wav",0);
//	midi_load("avemaria.mid");
// set input
	mlb6->addstring("message based");
	mlb6->addstring("dinput");
	mlb6->setidxv(wininfo.usedirectinput);
// set mousemode
	mlb7->addstring("normal");
	mlb7->addstring("infinite");
	mlb7->setidxv(wininfo.mousemode);
// set resolution
	mlbxy->setidxc(videoinfo.gamexyidx);
//	sclxy->setidx(mlbxy->getcuroffset());
	popdir();
// show what's loaded
	logger("shapes show\n");
	menushapes->show();
	showcursor(1);
}

static U32 atoh(const char *s)
{
	int ret=0;
	while(*s) {
		ret<<=4;
		if (*s>='0' && *s<='9')
			ret+=*s-'0';
		else if (*s>='A' && *s<='F')
			ret+=*s-'A'+0xA;
		else if (*s>='a' && *s<='f')
			ret+=*s-'a'+0xa;
		s++;
	}
	return ret;
}

void mainmenuproc()
{
// test rgb to hsv and back
	C8 str[80];
	idxr=sliderr->getidx();
	idxg=sliderg->getidx();
	idxb=sliderb->getidx();
	sprintf(str,"R%3d G%3d B%3d",idxr,idxg,idxb);
	textrgb->settname(str);
	idxhue=sliderhue->getidx();
	sprintf(str,"Hue %3d",idxhue);
	texthue->settname(str);
	idxsat=slidersat->getidx();
	sprintf(str,"Sat %3d",idxsat);
	textsat->settname(str);
	idxval=sliderval->getidx();
	sprintf(str,"Val %3d",idxval);
	textval->settname(str);
	S32 idxr2,idxg2,idxb2;
	idxsat=supersat(idxsat,idxr,idxg,idxb,&idxr2,&idxg2,&idxb2);
	rgb2hsv(idxr2,idxg2,idxb2,&hue,&sat,&val);
	hue=(hue+idxhue)%360;
	sat=(sat*idxsat)/255;
	val=(val*idxval)/255;
	hsv2rgb(hue,sat,val,&backr,&backg,&backb);
	sprintf(str,"H%3d S%3d V%3d",hue,sat,val);
	texthsv->settname(str);
	sprintf(str,"R%3d G%3d B%3d",backr,backg,backb);
	textrgb2->settname(str);


// hitting enter will start selected state
	if (KEY==K_RETURN && focus!=testfloat && focus!=testhex) {
		S32 mainidx=mlb1->getidx();
		if (mainidx>=0) {
			pushchangestate(statenumlist[mlb1->getidx()]);
		}
	}

// escape aborts
	if (KEY==K_ESCAPE)
		popstate();
// set focus
	if (wininfo.mleftclicks)
		focus=menushapes->getfocus();
// if something selected...
	if (focus) {
		S32 ret=focus->proc();
		if (focus==pb1) { // start state button, can't do a switch on pointers
			if (ret==1) {
				pushchangestate(statenumlist[mlb1->getidx()]);
			}
		} else if (focus==pb2) { // quit button
			if (ret==1) {
				popstate(); // quit button pressed
			}
		} else if (focus==pb3) { // video set driver
			if (ret==1) {
				S32 vmd=mlb2->getidx();
				S32 vsd=mlb3->getidx();
				if (vmd>=0 && vsd>=0) {
					video_init(vmd,vsd);
					video_setupwindow(GX,GY);
					video_setupwindow(800,600);
					mlb2->setidxc(videoinfo.video_maindriver);
					mlb3->setidxc(videoinfo.video_subdriver);
				}
			}
		} else if (focus==pb4) { // audio set driver
			if (ret==1) {
				S32 amd=mlb4->getidx();
				S32 asd=mlb5->getidx();
				S32 amsd=mlbmic->getidx();
				if (amd>=0 && asd>=0 && amsd>=0) {
					wave_unload(twh);
					midi_unload();
					audio_init(amd,asd,amsd);
					pushandsetdir("mainmenu");
					twh=wave_load("synflute.wav",0);
//					midi_load("avemaria.mid");
					popdir();
				}
			}
		} else if (focus==pb5) { // play a wave sound
			if (ret==1) {
				playasound(twh);
			}
		} else if (focus==pb6) { // set input driver
			if (ret==1) {
				S32 imd=mlb6->getidx();
				input_init(imd);
			}
		} else if (focus==pb7) { // play a midi sound
			if (ret==1) {
				pushandsetdir("mainmenu");
				midi_load("avemaria.mid");
				popdir();
				midi_play(1.0f,2); // float vol 1 max, S32 repeat, 0 loop forever
			}
		} else if (focus==mlb1) { // statedesc listbox changed
			if (ret>=0) {
				pb1->setvis(1);
//				mlb8->setidxv(ret); // change statenum listbox
//				scl1->setidx(mlb1->getcuroffset());
			}
/*		} else if (focus==mlb8) { // statenum listbox changed
			if (ret>=0) {
				pb1->setvis(1);
				mlb1->setidxv(ret); // change statedesc listbox
//				scl1->setidx(mlb1->getcuroffset());
			} */
		} else if (focus==mlb2) { // video main driver changed
			if (ret>=0) {
				mlb3->clear(); // update video sub driver listbox
				script* sc=video_getnsubdrivers(ret);
				S32 i;
				for (i=0;i<sc->num();++i)
					mlb3->addstring((*sc).idx(i).c_str());
				delete sc;
				mlb3->setidxv(0);
			}
		} else if (focus==mlb4) { // audio main driver changed
			if (ret>=0) {
				mlb5->clear(); // update audio sub driver listbox
				mlbmic->clear(); // and update mic sub driver listbox
				script* sc=audio_getnsubdrivers(ret);
				S32 i;
				for (i=0;i<sc->num();++i)
					mlb5->addstring((*sc).idx(i).c_str());
				delete sc;
				mlb5->setidxv(0);
				sc=mic_getnsubdrivers(ret);
				for (i=0;i<sc->num();++i)
					mlbmic->addstring((*sc).idx(i).c_str());
				delete sc;
				mlbmic->setidxv(0);
			}
		} else if (focus==mlb7) { // change mouse mode
			if (ret>=0) {
				wininfo.mousemode=ret;
			}
		} else if (focus==mlbxy) { // change default resolution
			if (ret>=0) {
				S32 mxsave=MX;
				S32 mysave=MY;
				videoinfo.gamexyidx=ret;
				video_setupwindow(GX,GY);
				video_setupwindow(800,600);
				setmousexy(mxsave,mysave);
				mlbxy->setidxv(ret);
//				sclxy->setidx(mlbxy->getcuroffset());
			}
		} else if (focus==testhex) {
			if (ret>=0) {
				U32 val;
				val=atoh(testhex->gettname());
//				val=3.14159f;
//				*ival=0x3f800000;
				float fval=*(float *)&val;
				sprintf(str,"%g",fval);
				testfloat->settname(str);
				focus=testfloat;
			}
		} else if (focus==testfloat) {
			if (ret>=0) {
				float val;
				val=(float)atof(testfloat->gettname());
				U32 ival=*(U32*)&val;
				sprintf(str,"%X",ival);
				testhex->settname(str);
				focus=testhex;
			}
		}
/*		} else if (focus==scl1) {
			if (ret>=0) {
				mlb1->setcuroffset(ret);
				mlb8->setcuroffset(ret);
			}
		} else if (focus==sclxy) {
			if (ret>=0) {
				mlbxy->setcuroffset(ret);
			} */
	}
}

void mainmenudraw2d()
{
//	video_lock();
//	cliprect32(B32,0,0,WX,WY,C32GREEN); // test
	menushapes->draw();
	outtextxyf32(B32,WX/2-5*4,10,C32WHITE,"main menu");
	rgbcol.r=idxr;
	rgbcol.g=idxg;
	rgbcol.b=idxb;
	cliprect32(B32,25,120,50,50,rgbcol);
	rgbcol.r=backr;
	rgbcol.g=backg;
	rgbcol.b=backb;
	cliprect32(B32,100,120,50,50,rgbcol);
//	video_unlock();
}

void mainmenuexit()
{
	FILE *fp;
	pushandsetdir("mainmenu");
	fp=fopen2("maincfg.txt","w");
	if (fp) {
		fprintf(fp,"mainidx %d\n",mlb1->getidx());
		fclose(fp);
	}
	popdir();
	wave_unload(twh);
	midi_unload();
	delete menushapes;
	statenumlist.clear();
}
