// random static
// and test small groups out with u_smallgroups
#include <m_eng.h>
#include "m_perf.h"
#include "u_states.h"

#include "u_smallgroups.h"

using namespace smallgroups;

void smallgroupstest()
{
	perf_start(GANAL);
	logger("--------------- in smallgroupstest --------------\n");
#if 1

// test perm
	perm p(4);
	perm q = p;
	do {
		p.printp();
		//p.printc();
		++p;

	} while (q!=p);
// build a group, add some perms, print it out
	group gt(4,false);
	gt.printc("group gt");
// build a group S3
	{
		group s3(3,true); // order 6
		s3.printc("group S3");
	}
	{
// build a group c2 x c2, order 4 Klein group
		perm k0(4);
		const S32 k1a[]={0,1,3,2};
		perm k1(k1a,4);
		const S32 k2a[]={1,0,2,3};
		perm k2(k2a,4);
		perm k3=k2*k1;
		vector<perm> k4v;
//		k4v.push_back(k0);
		k4v.push_back(k1);
		k4v.push_back(k2);
//		k4v.push_back(k3);
		group k4(k4v); // order 4 Klein group
		k4.printc("group KLEIN4");
	}
	{
// build a group c4
		perm c0(4);
		const S32 c4g[]={1,2,3,0};
		perm c1(c4g,4);
		perm c2=c1*c1;
		perm c3=c1*c2;
		vector<perm> c4v;
//		c4v.push_back(c0);
		c4v.push_back(c1);
//		c4v.push_back(c2);
//		c4v.push_back(c3);
		group c4(c4v); // order 4 cycle group
		c4.printc("group C4");
	}
// build a d4
	{
		const S32 c3g[]={1,2,3,0};
		const S32 c2g[]={2,1,0,3};
		perm c3(c3g,4);
		perm c2(c2g,4);
		vector<perm> c4v;
		c4v.push_back(c3);
		c4v.push_back(c2);
		group c4(c4v);
		c4.printc("group D4");
	}
// build s4
	{
		group s4(4,true);
		s4.printc("group S4");
	}
#endif
#if 1
#if 1
	{
		cgroup cay1(1);
//		cay1.printc("Cayley for cay 1 is");
		vector<cgroup> cgl=cay1.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 1 identskel");
	}
#endif
	{
		cgroup cay2(2);
//		cay2.printc("Cayley for cay 2 is");
		vector<cgroup> cgl=cay2.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 2 identskel");
	}
	{
		cgroup cay3(3);
//		cay3.printc("Cayley for cay 3 is");
		vector<cgroup> cgl=cay3.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 3 identskel");
	}
	{
		cgroup cay4(4);
//		cay4.printc("Cayley for cay 4 is");
		vector<cgroup> cgl=cay4.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 4 identskel");
	}
	{
		cgroup cay5(5);
		cgroup cay5b(5);
		if (cay5==cay5b)
			logger("cgroup 5 they are equal\n");
		else
			logger("cgroup 5 they are NOT equal\n");
//		cay5.printc("Cayley for cay 5 is");
		vector<cgroup> cgl=cay5.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 5 identskel");
	}
	{
		cgroup cay6(6);
//		cay6.printc("Cayley for cay 6 is");
		vector<cgroup> cgl=cay6.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 6 identskel");
	}
	{
		cgroup cay7(7);
//		cay7.printc("Cayley for cay 7 is");
		vector<cgroup> cgl=cay7.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 7 identskel");
	}
#endif
	{
		cgroup cay8(8);
//		cay8.printc("Cayley for cay 8 is");
		vector<cgroup> cgl=cay8.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("cgroup Cayley for cay order 8 identskel");
	}
#if 0
	{
		cgroup cay9(9);
//		cay8.printc("Cayley for cay 9 is");
		vector<cgroup> cgl=cay9.genidentskel();
		S32 i,n=cgl.size();
		for (i=0;i<n;++i)
			cgl[i].printc("Cayley for cay");
	}
#endif
	logger("--------------- end in smallgroupstest ----------\n");
	perf_end(GANAL);
}

void user1init()
{
	video_setupwindow(GX,GY);
	smallgroupstest();
}

void user1proc()
{
	if (KEY=='=') {
		changeglobalxyres(1);
		video_setupwindow(GX,GY);
		return;
	}
	if (KEY=='-') {
		changeglobalxyres(-1);
		video_setupwindow(GX,GY);
		return;
	}
	if (wininfo.mmiddleclicks) {
		S32 togvidmode=videoinfo.video_maindriver;
		togvidmode^=1;
		video_init(togvidmode,0);
		video_setupwindow(GX,GY);
	}
	if (KEY==K_ESCAPE)
		poporchangestate(STATE_MAINMENU);
}

void user1draw2d()
{
	S32 i,j;
	C32* uip;
	uip=B32->data;
	for (j=0;j<B32->cliprect.size.y;j++) {
		for (i=0;i<B32->cliprect.size.x;i++)
			uip[i].c32=mt_randU32();
		uip+=B32->size.x;
	}
}

void user1exit()
{
}
