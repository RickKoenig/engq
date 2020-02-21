#include <m_eng.h>
#include <l_misclibm.h>

#include "ring2.h"
#include "augmatz.h"
#include "augmatz_ui.h"

augmatz_ui::augmatz_ui(S32 moda,S32 neqa,S32 nvara,shape* rla) : augmatz(moda,neqa,nvara)
{
	S32 i,j;
	if (nvara>64 || neqa > 64 || nvara<1 || neqa<1)
		errorexit("autmatn: bad args for constructor");
	static const C8* varnames= "XYZWABCDEFGHIJLM"; // 16 + 'Q'
	if (nvara<=5) {
		startx = startx1;
		sepx = sepx1;
		sizex = sizex1;
	} else if (nvara<=16) {
		startx = startx2;
		sepx = sepx2;
		sizex = sizex2;
	} else {
		startx = startx3;
		sepx = sepx3;
		sizex = sizex3;
	}
	if (neqa<=5) {
		starty = starty1;
		sepy = sepy1;
		sizey = sizey1;
	} else if (neqa<=16) {
		starty = starty2;
		sepy = sepy2;
		sizey = sizey2;
	} else {
		starty = starty3;
		sepy = sepy3;
		sizey = sizey3;
	}
	pbutshapes.resize(nr);
	if (nvara<=16) {
		// draw vars
		for (i=0;i<nc;++i) {
			stringstream ss;
			ss << varnames[i];
			string s = ss.str();
			text* atext=new text("var",startx+sepx*i,starty-sepy,sizex,sizey,s.c_str());
			rla->addchild(atext);
		}
		text* atext=new text("nonhom",startx+sepx*nc,starty-sepy,sizex,sizey,"Q");
		rla->addchild(atext);
	}
	// build aug
	for (j=0;j<nr;++j) { // row
		pbutshapes[j].resize(nc+1);
//		vals[j].resize(nc+1);
//		calcvals[j].resize(nc+1);
		for (i=0;i<=nc;++i) { // column, include augment column
			stringstream ss;
			ss << (j+1) << " " << (i+1);
			string name = ss.str();
			stringstream ss2;
			ss2 << (S32)vals[j][i].v;
			string vs = ss2.str();
			pbut* abut=new pbut(name.c_str(),startx+sepx*i,starty+sepy*j,sizex,sizey,vs.c_str());
			pbutshapes[j][i] = abut;
			rla->addchild(abut);
		}
	}
}

bool augmatz_ui::isfocus(shape* s,S32& ridx,S32& cidx)
{
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			if (pbutshapes[j][i] == s) {
				ridx = j;
				cidx = i;
				return true;
			}
		}
	}
	return false;
}

void augmatz_ui::dofocus(S32 cr,S32 cc,U32 butval) // after isfocus returns true
{
	pbut* pb = dynamic_cast<pbut*>(pbutshapes[cr][cc]);
	if (pb) {
		stringstream ss2;
		T& vr = vals[cr][cc];
		if (butval==1) {
			--vr;
		} else if (butval==2) {
			++vr;
		}
		ss2 << (S32)vals[cr][cc];
		string vs = ss2.str();
		pb->settname(vs.c_str());
	}
}

void augmatz_ui::update()
{
	S32 i,j;
	for (j=0;j<nr;++j) { // row
		for (i=0;i<=nc;++i) { // column, include augment column
			pbut* pb = dynamic_cast<pbut*>(pbutshapes[j][i]);
			if (pb) {
				stringstream ss2;
				ss2 << (S32)vals[j][i].v;
				string vs = ss2.str();
				pb->settname(vs.c_str());
			}
		}
	}
}
