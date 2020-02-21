#include <m_eng.h>
#include <l_misclibm.h>
#include "u_states.h"

#include "u_ply.h"
#include "u_bch.h"

#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

// ui
shape* rl;
shape* focus,*oldfocus;

pbut* bquit;
//pbut* bnext,*bprev;

pbut* bm[5],*bc[15];
text* tc[15];
text* tnbits;
pbut* bm2[6],*bc2[18];
text* tc2[18];
text* tnbits2;
U32 mess,code,erasu,fixe;
U32 mess2,code2,erasu2,fixe2;

const S32 ncodmax = 20;
S32 nmes = 6;
S32 nsym = 6;
vscroll* vrsm[ncodmax];
text* trsm[ncodmax];
U32 mes[ncodmax];

text* trsc[ncodmax];
vscroll* vrscd[ncodmax];
pbut* brsce[ncodmax];
text* trscd[ncodmax];
U32 codd[ncodmax];
U32 codp[ncodmax];
bool eras[ncodmax];
text* trscf[ncodmax];
text* tinput,*terrors,*tstatus;
pbut* bmesp,*bmesm,*bcheckp,*bcheckm;
// end ui;

/// BCH ////
void calcbch_15_5(S32 knd,S32 bit)
{
	static C8 s[2];
	S32 nbits = 0;
	S32 nerasu = 0;
	if (knd == 0) { // message
		mess ^= (1<<bit);
		code = encoder15_5(mess);
		erasu = 0;
		fixe = decoder15_5(code,erasu,&nbits,&nerasu);
	} else { // code
		if ((erasu>>bit)&1) {
			erasu ^= (1<<bit);
		} else {
			if ((code>>bit)&1) {
				erasu ^= (1<<bit);
			}
			code ^= (1<<bit);
		}
		fixe = decoder15_5(code,erasu,&nbits,&nerasu);
	}
	S32 i;
	for (i=0;i<5;++i) {
		U32 b = (mess>>i)&1;
		s[0] = '0' + b;
		bm[i]->settname(s);
	}
	for (i=0;i<15;++i) {
		U32 e = (erasu>>i)&1;
		if (e) {
			s[0] = 'E';
		} else {
			U32 b = (code>>i)&1;
			s[0] = '0' + b;
		}
		bc[i]->settname(s);
	}
	for (i=0;i<15;++i) {
		U32 b = (fixe>>i)&1;
//		if (err)
//			s[0] = 'E';
//		else
			s[0] = '0' + b;
		tc[i]->settname(s);
	}
	C8 str[100];
	if (nbits < 0)
		sprintf(str,"decode err");
	else
		sprintf(str,"errors = %d, erasures = %d",nbits,nerasu);
	tnbits->settname(str);
}

void calcbch_18_6(S32 knd,S32 bit)
{
	static C8 s[2];
	S32 nbits = 0;
	S32 nerasu = 0;
	if (knd == 0) { // message
		mess2 ^= (1<<bit);
		code2 = encoder18_6(mess2);
		erasu2 = 0;
		fixe2 = decoder18_6(code2,erasu2,&nbits,&nerasu);
	} else { // code
		if ((erasu2>>bit)&1) {
			erasu2 ^= (1<<bit);
		} else {
			if ((code2>>bit)&1) {
				erasu2 ^= (1<<bit);
			}
			code2 ^= (1<<bit);
		}
		fixe2 = decoder18_6(code2,erasu2,&nbits,&nerasu);
	}
	S32 i;
	for (i=0;i<6;++i) {
		U32 b = (mess2>>i)&1;
		s[0] = '0' + b;
		bm2[i]->settname(s);
	}
	for (i=0;i<18;++i) {
		U32 e = (erasu2>>i)&1;
		if (e) {
			s[0] = 'E';
		} else {
			U32 b = (code2>>i)&1;
			s[0] = '0' + b;
		}
		bc2[i]->settname(s);
	}
	for (i=0;i<18;++i) {
		U32 b = (fixe2>>i)&1;
//		if (err)
//			s[0] = 'E';
//		else
			s[0] = '0' + b;
		tc2[i]->settname(s);
	}
	C8 str[100];
	if (nbits < 0)
		sprintf(str,"decode err");
	else
		sprintf(str,"errors = %d, erasures = %d",nbits,nerasu);
	tnbits2->settname(str);
}

#if 1
void testrs()
{
//	rs g = rs_generator_poly(4);

	const U32 mlen = 3;
	const U32 nsym = 4;
	U32 mpod[mlen] = {0x56,0x34,0x12};
	rs m(mpod,mpod+mlen);
	rs c = rs_encode_msg(m,nsym);
	rs s = rs_calc_syndromes(c,nsym);

	const U32 mlen2 = 16;
	const U32 nsym2 = 10;
	U32 mpod2[mlen2] = {
		0xec,0x70,0x96,0xc6,0xc6,0x96,0x26,0x27,
		0x06,0x32,0x17,0x76,0x47,0x75,0xd2,0x40};
	rs m2(mpod2,mpod2+mlen2);
	rs c2 = rs_encode_msg(m2,nsym2);
	rs s2 = rs_calc_syndromes(c2,nsym2);

	rs c2d = c2;
	c2d[c2d.size()-1] = 0;
	rs s2d = rs_calc_syndromes(c2d,nsym2);

	rs pos(1,c2d.size()-1);
	rs_correct_errata(c2d,s2d,pos);

	c2d[c2d.size()-1] = 6;
	c2d[c2d.size()-1-10] = 7;
	c2d[c2d.size()-1-20] = 8;
//	c2d[c2d.size()-1-21] = 4;
//	c2d[c2d.size()-1-22] = 5;
//	c2d[c2d.size()-1-23] = 6;
	s2d = rs_calc_syndromes(c2d,nsym2);
	S32 nerrs = -1;
	S32 nerasures;
	pos = rs_find_errors(s2d,c2d.size(),&nerrs);
	rs_correct_errata(c2d,s2d,pos);

	c2d = c2;
	c2d[0] = -1;
	c2d[1] = 13;
	rs c2dfix = rs_correct_msg(c2d,10,&nerrs,&nerasures);

	rs smallmess(1);
	smallmess[0] = 69;
	const S32 nsmallcheck = 2;
	rs smallcode = rs_encode_msg(smallmess,nsmallcheck);
	rs smallcoded = smallcode;
	smallcoded[0] = 34;
	rs smallcodefix = rs_correct_msg(smallcoded,nsmallcheck,&nerrs,&nerasures);
}
#endif

#if 1
void testrs2()
{
	logger("testrs2\n");
//	rs g = rs_generator_poly(4);
#if 1
	const U32 nmess = 223;
	const U32 nsym = 32;
#else
	const U32 nmess = 5;
	const U32 nsym = 20;
#endif
	mt_setseed(23);
	S32 i,j,k;
#if 1
//	S32 nerase = nsym;
//	S32 nerase = 0;
	S32 nerase = nsym/2;
	S32 nerr = (nsym - nerase)/2; //nsym/2;
	logger("nsym = %d, nerase = %d, nerr = %d\n",nsym,nerase,nerr);
#else
#endif
	for (i=0;i<10;++i) {
		// make a message
		rs m(nmess);
#if 1
		for (j=0;j<nmess;++j) {
			m[j] = mt_random(256);
		}
#else
		m[0] = 33;
#endif
		// encode it
		rs c = rs_encode_msg(m,nsym);
		// damage it
		rs cd = c;
		for (k=0;k<nerr;++k) {
			cd[mt_random(nmess+nsym)] = mt_random(256);
		}
		for (k=0;k<nerase;++k) {
			cd[mt_random(nmess+nsym)] = ~0; // erasure symbol
		}
		// correct it
		S32 nerrs=0,nerasures=0;
		rs cfix = rs_correct_msg(cd,nsym,&nerrs,&nerasures);
		// get message back
		rs mfix(cfix.begin()+nsym,cfix.end());
		// check
		if (nerrs < 0)
			logger("too many errors in decode\n");
		else if (c != cfix)
			logger("badlogic, code doesn't match!\n");
		else if (m != mfix)
			logger("badlogic, message doesn't match!\n");
	}
	logger("end testrs2\n");
}
#endif

// message was altered
void calcdamage()
{
	S32 i;
	S32 ncod = nmes + nsym;
	S32 neras = 0;
	S32 nerrs = 0;
	C8 ns[100];
	for (i=0;i<ncod;++i) {
		if (eras[i]) {
			sprintf(ns,"E");
			brsce[i]->settname("E");
			++neras;
		} else {
			brsce[i]->settname("");
			if (codd[i] != codp[i]) {
				sprintf(ns,"*%d",codd[i]);
				++nerrs;
			} else {
				sprintf(ns,"%d",codd[i]);
			}
		}
		trscd[i]->settname(ns);
		trscf[i]->settname(ns);
	}
	C8 st[100];
	if (nerrs*2+neras > nsym)
		strcpy(st,"too big!");
	else
		strcpy(st,"   ok   ");
	sprintf(ns,"Damage: erase %d, errors %d, nsym %d   %s",neras,nerrs,nsym,st);
	terrors->settname(ns);
	rs mi(codd,codd+ncod);
	for (i=0;i<ncod;++i)
		if (eras[i])
			mi[i] = ~0;
/*	U32 test[12]  = {217,~0U,~0U,~0U,169,~0U,128,156,194,80,191,128};
	U32 testb[12] = {128,191,80,194,156,128,-1,169,-1,-1,-1,217};
	mi.assign(test,test+12); */
	rs f = rs_correct_msg(mi,nsym,&nerrs,&neras);
	for (i=0;i<ncod;++i) {
		sprintf(ns,"%d",f[i]);
		trscf[i]->settname(ns);
	}
	C8 ns2[100];
	if (nerrs<0)
		strcpy(ns2,"Decode Err");
	else
		strcpy(ns2,"Decode OK");
	sprintf(ns,"status: errors = %d, erase = %d, %s",nerrs,neras,ns2);
	tstatus->settname(ns);
}

void calcmes()
{
	S32 i;
	S32 ncod = nmes + nsym;
	C8 ns[100];
	for (i=0;i<nmes;++i) {
		sprintf(ns,"%d",vrsm[i]->getidx());
		trsm[i]->settname(ns);
	}
	sprintf(ns,"Reed Solomon: (%d,%d) check %d",nmes+nsym,nmes,nsym);
	tinput->settname(ns);
	rs cod = rs_encode_msg(rs(mes,mes+nmes),nsym);
	::copy(cod.begin(),cod.end(),codd);
	::fill(eras,eras+ncod,false);
	for (i=0;i<ncod;++i) {
		sprintf(ns,"%d",cod[i]);
		vrscd[i]->setidx(cod[i]);
		trsc[i]->settname(ns);
		codp[i] = cod[i];
	}
	calcdamage();
}

#ifdef USENS
}
#endif

void qrcodeinit()
{
//	gf_init();
	testrs();
//	testrs2();

	pushandsetdir("ifield");
	video_setupwindow(1024,768);
	ply::setmod(2);
// ui
	rl=res_loadfile("qrcoderes.txt");
	bquit=rl->find<pbut>("BQUIT");
//	bnext=rl->find<pbut>("BNEXT");
//	bprev=rl->find<pbut>("BPREV");
	tinput=rl->find<text>("TINPUT");
	terrors=rl->find<text>("TERRORS");
	tstatus=rl->find<text>("TSTATUS");
	bmesp=rl->find<pbut>("BMESP");
	bmesm=rl->find<pbut>("BMESM");
	bcheckp=rl->find<pbut>("BCHECKP");
	bcheckm=rl->find<pbut>("BCHECKM");
	S32 i,j;
	for (i=0;i<5;++i) {
		C8 s[100];
		sprintf(s,"BM%d",i);
		bm[i] = rl->find<pbut>(s);
	}
	for (i=0;i<15;++i) {
		C8 s[100];
		sprintf(s,"BC%d",i);
		bc[i] = rl->find<pbut>(s);
	}
	for (i=0;i<15;++i) {
		C8 s[100];
		sprintf(s,"T%d",i);
		tc[i] = rl->find<text>(s);
	}
	tnbits = rl->find<text>("TNBITS");
	for (i=0;i<6;++i) {
		C8 s[100];
		sprintf(s,"B2M%d",i);
		bm2[i] = rl->find<pbut>(s);
	}
	for (i=0;i<18;++i) {
		C8 s[100];
		sprintf(s,"B2C%d",i);
		bc2[i] = rl->find<pbut>(s);
	}
	for (i=0;i<18;++i) {
		C8 s[100];
		sprintf(s,"T2%d",i);
		tc2[i] = rl->find<text>(s);
	}
	tnbits2 = rl->find<text>("T2NBITS");
	mess = 0;
	code = 0;
	erasu = 0;
	fixe = 0;
	mess2 = 0;
	code2 = 0;
	erasu2 = 0;
	fixe2 = 0;
#if 0
	if (fileexist("decode_data.bin")) {
		FILE* fh = fopen2("decode_data.bin","rb");
		decode_data = new deco[32768];
		S32 i;
		for (i=0;i<32768;++i) {
			decode_data[i].cod = filereadU32LE(fh);
			decode_data[i].nbits = filereadU32LE(fh);
		}
		fclose(fh);
	}
#endif
	shape* r = new text("",0,0,1024,12,"M E S S A G E");
	rl->addchild(r);
	r = new text("",0,210,1024,12,"C O D E");
	rl->addchild(r);
	r = new text("",0,270,1024,12,"D A M A G E");
	rl->addchild(r);
	r = new text("",0,500,1024,12,"F I X");
	rl->addchild(r);
	for (j=0;j<nmes;++j) {
		i = nmes - j - 1;
		vrsm[i] = new vscroll("",20+50*j, 20,20,140);
		vrsm[i]->setminmaxval(0,255);
		rl->addchild(vrsm[i]);
		trsm[i] = new text("",   10+50*j,165,40, 12,"0");
		rl->addchild(trsm[i]);
	}
	S32 ncod = nmes + nsym;
	for (j=0;j<ncod;++j) {
		i = ncod - j - 1;
		trsc[i] = new text("",   10+50*j,230,40, 12,"0");
		rl->addchild(trsc[i]);
		vrscd[i] = new vscroll("",20+50*j,290,20,140);
		vrscd[i]->setminmaxval(0,255);
		rl->addchild(vrscd[i]);
		brsce[i] = new pbut("",   22+50*j,440,16, 12,"");
		rl->addchild(brsce[i]);
		trscd[i] = new text("",   10+50*j,460,40, 12,"0");
		rl->addchild(trscd[i]);
		trscf[i] = new text("",   10+50*j,520,40, 12,"0");
		rl->addchild(trscf[i]);
	}

	focus=oldfocus=0;
	::fill(mes,mes+nmes,0);
	calcmes();
// end ui
}

void qrcodeproc()
{
	switch(KEY) {
	case K_ESCAPE:
		poporchangestate(STATE_MAINMENU);
		break;
	}
// ui
	if (wininfo.mleftclicks)
		focus=rl->getfocus();
	S32 ret=-1;
	if (focus) {
		ret=focus->proc();
	}
	if (oldfocus && oldfocus!=focus)
		oldfocus->deactivate();
	oldfocus=focus;
// quit
	S32 i;
	for (i=0;i<nmes;++i) {
		if (focus == vrsm[i]) {
			S32 gi = vrsm[i]->getidx();
			if (mes[i] != gi) {
				mes[i] = gi;
				calcmes();
			}
		}
	}
	if (i != nmes)
		return;
	S32 ncod = nmes + nsym;
	for (i=0;i<ncod;++i) {
		if (focus == vrscd[i]) {
			S32 ci = vrscd[i]->getidx();
			if (codd[i] != ci) {
				codd[i] = ci;
				eras[i] = false;
				calcdamage();
			}
		}
	}
	if (i != ncod)
		return;
	for (i=0;i<ncod;++i) {
		if (focus == brsce[i]) {
			if (ret == 1) {
				eras[i] = !eras[i];
				calcdamage();
			}
		}
	}
	if (i != ncod)
		return;
	if (ret==1) {
		if (focus == bquit) {
			poporchangestate(STATE_MAINMENU);
//		} else if (focus == bnext) {
//		} else if (focus == bprev) {
		} else if (focus == bmesp) {
			if (nmes + nsym < ncodmax) {
				++nmes;
				changestate(STATE_QRCODE);
			}
		} else if (focus == bmesm) {
//			if (nmes > 0) { // degenerate check
			if (nmes > 1) {
				--nmes;
				changestate(STATE_QRCODE);
			}
		} else if (focus == bcheckp) {
			if (nmes + nsym < ncodmax) {
				++nsym;
				changestate(STATE_QRCODE);
			}
		} else if (focus == bcheckm) {
			if (nsym > 0) {
				--nsym;
				changestate(STATE_QRCODE);
			}
		} else {
			for (i=0;i<5;++i) {
				if (focus == bm[i]) {
					calcbch_15_5(0,i);
					break;
				}
			} 
			if (i == 5) {
				for (i=0;i<15;++i) {
					if (focus == bc[i]) {
						calcbch_15_5(1,i);
						break;
					}
				} 
			}
			for (i=0;i<6;++i) {
				if (focus == bm2[i]) {
					calcbch_18_6(0,i);
					break;
				}
			} 
			if (i == 6) {
				for (i=0;i<18;++i) {
					if (focus == bc2[i]) {
						calcbch_18_6(1,i);
						break;
					}
				} 
			}
		}
	}
// end ui
}

void qrcodedraw2d()
{
	clipclear32(B32,C32BLUE);
	rl->draw();
}

void qrcodeexit()
{
	popdir();
	delete rl;
#if 0
	delete[] decode_data;
	decode_data = 0;
#endif
}
