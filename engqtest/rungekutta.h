#define MAXKS 100
#if 0
#if 1
template<typename F>
void dorungekutta(float *objstate,int *offsets,int noffsets,float t0,float ts,
						 F changefunc);
#else
void dorungekutta(float *objstate,int *offsets,int nobjstate,float t0,float ts,
						 void (*changefunc)(float *os,float *delos,float time));
#endif
#else
#if 1
template<typename F>
void dorungekutta(float *objstate,int *offsets,int noffsets,float t0,float ts,
						 F changefunc)
#else
void dorungekutta(float *objstate,int *offsets,int noffsets,float t0,float ts,
						 void (*changefunc)(float *os,float *delos,float time))
#endif
{
	int i,j;
	static float ks[4][MAXKS];
	static float delos[MAXKS];

	(*changefunc)(objstate,ks[0],t0);
	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		ks[0][j]*=ts;
	}

	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		delos[j]=objstate[j]+ks[0][j]*.5f;
	}
	(*changefunc)(delos,ks[1],t0+ts*.5f);
	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		ks[1][j]*=ts;
	}

	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		delos[j]=objstate[j]+ks[1][j]*.5f;
	}
	(*changefunc)(delos,ks[2],t0+ts*.5f);
	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		ks[2][j]*=ts;
	}

	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		delos[j]=objstate[j]+ks[2][j];
	}
	(*changefunc)(delos,ks[3],t0+ts);
	for (i=0;i<noffsets;i++) {
		j=offsets[i];
		ks[3][j]*=ts;
	}

	for (i=0;i<noffsets;i++) {
		j=offsets[i];
//		objstate[j]+=ks[0][j];
//		objstate[j]+=(1.0f/2.0f)*(ks[0][j]+ks[1][j]);
		objstate[j]+=(1.0f/6.0f)*(ks[0][j]+2.0f*ks[1][j]+2.0f*ks[2][j]+ks[3][j]);
	}
}

#endif
