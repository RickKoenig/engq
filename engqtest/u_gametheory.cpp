#include <m_eng.h>

#define DIM 3
#define RES 50

#if DIM==2
static S32 payoff[DIM][DIM]={
	{ 0,-1},
	{ 1, 0},
	};
#endif
#if DIM==3
static S32 payoff[DIM][DIM]={
	{ 0,-1, 1},
	{ 1, 0,-1},
	{-1, 1, 0},
	};
#endif
#if DIM==4
static S32 payoff[DIM][DIM]={
	{ 0,-1, 1, 1},
	{ 1, 0,-1, 1},
	{-1, 1, 0,-1},
	{-1,-1, 1, 0},
	};
#endif
#if DIM==5
static S32 payoff[DIM][DIM]={
	{ 0,-1, 1, 1, 1},
	{ 1, 0,-1, 1, 1},
	{-1, 1, 0,-1, 1},
	{-1,-1, 1, 0,-1},
	{-1,-1,-1, 1, 0},
	};
#endif
#if DIM==6
static S32 payoff[DIM][DIM]={
	{ 0,-1, 1, 1, 1, 1},
	{ 1, 0,-1, 1, 1, 1},
	{-1, 1, 0,-1, 1, 1},
	{-1,-1, 1, 0,-1, 1},
	{-1,-1,-1, 1, 0,-1},
	{-1,-1,-1,-1, 1, 0},
	};
#endif
#if DIM==7
static S32 payoff[DIM][DIM]={
	{ 0,-1, 1, 1, 1, 1, 1},
	{ 1, 0,-1, 1, 1, 1, 1},
	{-1, 1, 0,-1, 1, 1, 1},
	{-1,-1, 1, 0,-1, 1, 1},
	{-1,-1,-1, 1, 0,-1, 1},
	{-1,-1,-1,-1, 1, 0,-1},
	{-1,-1,-1,-1,-1, 1, 0},
	};
#endif

static float payme(U32* pr,U32* qr)
{
	U32 i,j;
	float r=0;
	for (i=0;i<DIM;++i) {
		float pi=pr[i]/(float)RES;
		for (j=0;j<DIM;++j) {
			float pj=qr[j]/(float)RES;
			r+=pi*pj*payoff[i][j];
		}
	}
	return r;
}

U32 iter(U32 *pr)
{
	U32 curdim=0;
	U32 i;
	while(curdim<DIM) {
		U32 sum=0;
		++pr[curdim];
		for (i=0;i<DIM;++i)
			sum+=pr[i];
		if (sum>RES) {
			pr[curdim]=0;
			++curdim;
		} else
			break;
	}
	return curdim==DIM;
}

// good if sum==RES
U32 psum(U32 *pr)
{
	U32 i;
	U32 sum=0;
	for (i=0;i<DIM;++i)
		sum+=pr[i];
	return sum;
}

void rungametheory()
{
	U32 i;
	float worstpay;
//	return;
	logger("in gametheory\n");
	U32 p[DIM];
	U32 q[DIM];
	memset(p,0,sizeof(p));
	memset(q,0,sizeof(q));
	while(1) {
		if (psum(p)==RES) {
			worstpay=2;
			while(1) {
				if (psum(q)==RES) {
// output valid probability
					float poy=payme(p,q);
/*					logger("p ");
					for (i=0;i<DIM;++i)
						logger("%d ",p[i]);
					logger("q ");
					for (i=0;i<DIM;++i)
						logger("%d ",q[i]);
					logger("pay %7.4f\n",poy); */
					if (poy<worstpay)
						worstpay=poy;
				}
// next
				if (iter(q))
					break;
			}
			if (worstpay>-.2f) {
				logger("p ");
				for (i=0;i<DIM;++i)
					logger("%2d ",p[i]);
				logger("worstpay %7.4f\n",worstpay); 
			}
		}
// next
		if (iter(p))
			break;
	}
	logger("done gametheory\n");
}
