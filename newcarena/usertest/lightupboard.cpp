#include <m_eng.h>

#include "ring2.h"
#include "lightupboard.h"
#include "lightuppatterns.h"

// lightupboard base class
bool lightupboard::getcoord(S32 mxa,S32 mya,S32& cx,S32& cy)
{
	if (mxa<startx)
		return false;
	if (mya<starty)
		return false;
	mxa -= startx;
	mya -= starty;
	if (mxa>=dimx*stepx)
		return false;
	if (mya>=dimy*stepy)
		return false;
	S32 sx = mxa % stepx;
	S32 sy = mya % stepy;
	if (sx>=sizex)
		return false;
	if (sy>=sizey)
		return false;
	cx = mxa / stepx;
	cy = mya / stepy;
	return true;
}

lightupboard::lightupboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa) :
	name(namea),
	startx(stxa),
	starty(stya),
	stepx(24),
	stepy(24),
	sizex(18),
	sizey(18),
	dimx(dimxa),
	dimy(dimya),
	pat(pata),
	wrap(wrapa),
	prod(dimxa*dimya)
{
	data = new U8[prod];
	reset();
}

void lightupboard::reset()
{
	::fill(data,&data[prod],0);
}

U8 lightupboard::getcell(S32 xa,S32 ya)
{
	return data[xa + ya*dimx];
}

void lightupboard::draw()
{
	S32 i,j;
	outtextxy32(B32,startx,starty-12,C32WHITE,name.c_str());
	for (j=0;j<dimy;++j) {
		for (i=0;i<dimx;++i) {
			cliprect32(B32,startx+i*stepx,starty+j*stepy,sizex,sizey,C32LIGHTGRAY);
			outtextxyfc32(B32,startx+i*stepx+sizex/2,starty+j*stepy+sizey/2,C32WHITE,"%d",data[i+dimx*j]);
		}
	}
}

lightupboard::~lightupboard()
{
	delete[] data;
}


// patternboard
void patternboard::update()
{
//	::fill(data,&data[prod],0);
//	data[overx+dimx*overy] = 1;
	getpattern(overx,overy,dimx,dimy,pat,wrap,data);

}

patternboard::patternboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa) :
	lightupboard(namea,stxa,stya,dimxa,dimya,pata,wrapa),
	overx((dimxa-1)/2),
	overy((dimya-1)/2)
{
	update();
}

void patternboard::setovermouse(S32 mxa,S32 mya,S32 mbuta)
{
	if (!mbuta)
		return;
	if (getcoord(mxa,mya,overx,overy))
		update();
}

void patternboard::setoverraw(S32 mxa,S32 mya)
{
	overx = mxa;
	overy = mya;
	update();
}

void patternboard::getoverraw(S32& mxa,S32& mya)
{
	mxa = overx;
	mya = overy;
}


// gameboard
void gameboard::update(S32 cxa,S32 cya,S32 deltaa)
{
	getpattern(cxa,cya,dimx,dimy,pat,wrap,patdata);
	S32 i;
	for (i=0;i<prod;++i) {
		zee2 val = data[i];
		val += deltaa*patdata[i];
		data[i] = val;
	}
//	::copy(patdata,patdata+prod,data);
}

gameboard::gameboard(string namea,S32 stxa,S32 stya,S32 dimxa,S32 dimya,S32 pata,S32 wrapa,S32 moda) :
	lightupboard(namea,stxa,stya,dimxa,dimya,pata,wrapa),
	mod(moda)
{
	patdata = new U8[prod];
	::fill(patdata,&patdata[prod],0);
}

void gameboard::click(S32 mxa,S32 mya,S32 deltaa)
{
	S32 cx,cy;
	if (getcoord(mxa,mya,cx,cy))
		update(cx,cy,deltaa);
}	

gameboard::~gameboard()
{
	delete[] patdata;
}

