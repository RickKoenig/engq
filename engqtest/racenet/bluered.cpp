#include <engine1.h>

static unsigned short *bluemask;
static unsigned short *redmask;

void initblueredmask()
{
	unsigned int i;
	logger("init blueredmask with %d\n",BPP);
	bluemask=memalloc(0x10000*sizeof(unsigned short));
	redmask=memalloc(0x10000*sizeof(unsigned short));
	for (i=0;i<0x10000;i++) {
		unsigned int r,g,b;
		if (BPP==565) {
			unsigned int five;//,six;
			r=i>>(6+5);
			g=(i>>5)&0x3f;
			b=i&0x1f;
			five=(126*r+62*g+126*b+189)/378;
//			six=(126*r+62*g+126*b+93)/186;
			bluemask[i]=five;
			redmask[i]=(five<<11);
		} else if (BPP==555) {
			unsigned int five;
			r=(i>>(5+5))&0x1f;
			g=(i>>5)&0x1f;
			b=i&0x1f;
			five=(2*r+2*g+2*b+3)/6;
			bluemask[i]=five;
			redmask[i]=(five<<10);
		} else {
			bluemask[i]=random(65536);
			redmask[i]=random(65536);
		}
	}
}

void freeblueredmask()
{
	logger("free blueredmask\n");
	memfree(bluemask);
	memfree(redmask);
}

void makeblue(struct bitmap16 *src,struct bitmap16 *dest)
{
	unsigned int i,prd;
	unsigned short *ps,*pd;
	ps=src->plane;
	pd=dest->plane;
	prd=src->x*src->y;
	for (i=0;i<prd;i++)
		pd[i]=bluemask[ps[i]];
}

void combinebluered(struct bitmap16 *src,struct bitmap16 *dest) 
{
	unsigned int i,prd;
	unsigned short *ps,*pd;
	ps=src->plane;
	pd=dest->plane;
	prd=src->x*src->y;
	for (i=0;i<prd;i++)
		pd[i]=redmask[pd[i]]|ps[i];
}
