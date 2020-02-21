#include <m_eng.h>

// general purpose matrix of any size
struct matn* matncreate(S32 ord)
{
	struct matn* ret;
//	ret=(struct matn*)memalloc(sizeof(struct matn));
	ret=new matn;
	ret->order=ord;
//	ret->data=(float*)memalloc(sizeof(float)*ord*ord);
	ret->data=new float[sizeof(float)*ord*ord];
	matnident(ret);
	return ret;
}

void matnfree(struct matn* a)
{
//	memfree(a->data);
	delete[] a->data;
//	memfree(a);
	delete a;
}

void matnsetcolrow(struct matn* a,S32 col,S32 row,float val)
{
	a->data[col+a->order*row]=val;
}

float matngetcolrow(const struct matn* a,S32 col,S32 row)
{
	return 	a->data[col+a->order*row];
}

void matnmult(const struct matn* a,const struct matn* b,struct matn* prod)
{
	S32 c,r,k;
	for (r=0;r<a->order;++r) {
		for (c=0;c<a->order;++c) {
			float sum=0;
			for (k=0;k<a->order;++k) {
				float av=matngetcolrow(a,k,r);
				float bv=matngetcolrow(b,c,k);
				sum+=av*bv;
			}
			matnsetcolrow(prod,c,r,sum);
		}
	}
}

// swap 2 rows in 2 matns
static void swaprowpair(struct matn* a,struct matn* i,S32 r1,S32 r2)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,r1);
		matnsetcolrow(a,c,r1,matngetcolrow(a,c,r2));
		matnsetcolrow(a,c,r2,t);
		t=matngetcolrow(i,c,r1);
		matnsetcolrow(i,c,r1,matngetcolrow(i,c,r2));
		matnsetcolrow(i,c,r2,t);
	}
}

// multiply a row by a number in 2 matns
static void multrowpair(struct matn* a,struct matn* i,S32 r1,float mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,r1);
		t*=mul;
		matnsetcolrow(a,c,r1,t);
		t=matngetcolrow(i,c,r1);
		t*=mul;
		matnsetcolrow(i,c,r1,t);
	}
}

// add a multiple of 1 row to another row in 2 matns
static void addmultpair(struct matn* a,struct matn* i,S32 rs,S32 rd,float mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,rs);
		t*=mul;
		t+=matngetcolrow(a,c,rd);
		matnsetcolrow(a,c,rd,t);
		t=matngetcolrow(i,c,rs);
		t*=mul;
		t+=matngetcolrow(i,c,rd);
		matnsetcolrow(i,c,rd,t);
	}
}

// swap 2 rows in 1 matn
static void swaprowsingle(struct matn* a,S32 r1,S32 r2)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,r1);
		matnsetcolrow(a,c,r1,matngetcolrow(a,c,r2));
		matnsetcolrow(a,c,r2,t);
	}
}

// multiply a row by a number in 1 matn
static void multrowsingle(struct matn* a,S32 r1,float mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,r1);
		t*=mul;
		matnsetcolrow(a,c,r1,t);
	}
}

// add a multiple of 1 row to another row in 1 matn
static void addmultsingle(struct matn* a,S32 rs,S32 rd,float mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		float t;
		t=matngetcolrow(a,c,rs);
		t*=mul;
		t+=matngetcolrow(a,c,rd);
		matnsetcolrow(a,c,rd,t);
	}
}

float matndet(const struct matn* ao)
{
	float ret=1.0f;
	S32 c,r,k;
	float v,v2;
	struct matn* a=matncreate(ao->order);
	matncopy(ao,a);
// for every column
	for (c=0;c<a->order;++c) {
// do the diagonal element first
		v=matngetcolrow(a,c,c);
		if (v==0.0f) { // find a replacment row by swapping with a nonzero row below
			for (k=c+1;k<a->order;++k) {
				v2=matngetcolrow(a,c,k);
				if (v2!=0.0f) { // find one?
					gcon_printf("det: swapping rows %d and %d\n",c,k);
					swaprowsingle(a,c,k);	
					ret=-ret;
					v=v2;
					break;
				}
			}
			if (k>=a->order) { // couldn't find one?
				matnfree(a);
				gcon_printf("det: 0\n",c);
				return 0.0f;
			}
		} 
// scale it to 1
		gcon_printf("det: multiplying row %d by %f\n",c,1.0f/v);
		multrowsingle(a,c,1.0f/v);
		ret*=v;
// finished with diagonal, now handle the rest
		for (r=c+1;r<a->order;++r) {
			float mul;
			mul=matngetcolrow(a,c,r);
			if (mul!=0.0f) { // gotta zero it, use addmult to do it
				mul=-mul;
				gcon_printf("det: multiplying row %d by %f\nand adding it to row %d\n",c,mul,r);
				addmultsingle(a,c,r,mul);
			}
		}
	}
	matnfree(a);
	return ret;
}

S32 matninverse(const struct matn* ao,struct matn* i) // returns true if invertable
{
	S32 c,r,k;
	float v,v2;
	struct matn* a=matncreate(ao->order);
	matncopy(ao,a);
	matnident(i);
// for every column
	for (c=0;c<a->order;++c) {
// do the diagonal element first
		v=matngetcolrow(a,c,c);
		if (v==0.0f) { // find a replacment row by swapping with a nonzero row below
			for (k=c+1;k<a->order;++k) {
				v2=matngetcolrow(a,c,k);
				if (v2!=0.0f) { // find one?
					gcon_printf("swapping rows %d and %d\n",c,k);
					swaprowpair(a,i,c,k);				
					v=v2;
					break;
				}
			}
			if (k>=a->order) { // couldn't find one?
				matnzero(i);
				matnfree(a);
				gcon_printf("no inverse\n",c);
				return 0;
			}
		} 
// scale it to 1
		gcon_printf("multiplying row %d by %f\n",c,1.0f/v);
		multrowpair(a,i,c,1.0f/v);		
// finished with diagonal, now handle the rest
		for (r=0;r<a->order;++r) {
			float mul;
			if (r==c)
				continue; // already done the diagonal
			mul=matngetcolrow(a,c,r);
			if (mul!=0.0f) { // gotta zero it, use addmult to do it
				mul=-mul;
				gcon_printf("multiplying row %d by %f\nand adding it to row %d\n",c,mul,r);
				addmultpair(a,i,c,r,mul);
			}
		}
	}
	matnfree(a);
	return 1;
}

void matnlogger(const struct matn* a,const C8* desc)
{
	S32 c,r;
	gcon_printf("---- matn '%s' ------\n",desc);
	for (r=0;r<a->order;++r) {
		for (c=0;c<a->order;++c) {
			gcon_printf("%9.3f",matngetcolrow(a,c,r));
		}
		gcon_printf("\n");
	}
}

void matncopy(const struct matn* a,struct matn* b) // copy a to b
{
	memcpy(b->data,a->data,sizeof(float)*a->order*a->order);
}

void matnident(struct matn* a)
{
	S32 i;
	matnzero(a);
	for (i=0;i<a->order;++i)
		matnsetcolrow(a,i,i,1.0f);
}

void matnzero(struct matn* a)
{
	memset(a->data,0,sizeof(float)*a->order*a->order);
}

struct vecn* vecncreate(S32 ord) // returns a zero vector
{
	struct vecn* ret;
//	ret=(struct vecn*)memalloc(sizeof(struct vecn));
	ret=new vecn;
	ret->order=ord;
//	ret->data=(float*)memalloc(sizeof(float)*ord);
	ret->data=new float[sizeof(float)*ord];
	vecnzero(ret);
	return ret;
}

void vecnfree(struct vecn* a)
{
//	memfree(a->data);
	delete[] a->data;
//	memfree(a);
	delete a;
}

void vecnsetele(struct vecn* a,S32 ele,float val)
{
	a->data[ele]=val;
}

float vecngetele(const struct vecn* a,S32 ele)
{
	return a->data[ele];
}

void vecnmult(const struct matn* a,const struct vecn* b,struct vecn* prod) // a*b=c assume same size
{
	S32 r,k;
	for (r=0;r<a->order;++r) {
		float sum=0;
		for (k=0;k<a->order;++k) {
			float av=matngetcolrow(a,k,r);
			float bv=vecngetele(b,k);
			sum+=av*bv;
		}
		vecnsetele(prod,r,sum);
	}
}

void vecnlogger(const struct vecn* a,const C8* desc)
{
	S32 e;
	gcon_printf("---- vecn '%s' ------\n",desc);
	for (e=0;e<a->order;++e)
		gcon_printf("%9.3f",vecngetele(a,e));
	gcon_printf("\n");
}

void vecncopy(const struct vecn* a,struct vecn* b) // copy a to b, assume same size
{
	memcpy(b->data,a->data,sizeof(float)*a->order);
}

void vecnzero(struct vecn* a)
{
	memset(a->data,0,sizeof(float)*a->order);
}

// swap 2 rows in a matn and a vecn
static void swaprowmv(struct matn* a,struct vecn* i,S32 r1,S32 r2)
{
	S32 c;
	float t;
	for (c=0;c<a->order;++c) {
		t=matngetcolrow(a,c,r1);
		matnsetcolrow(a,c,r1,matngetcolrow(a,c,r2));
		matnsetcolrow(a,c,r2,t);
	}
	t=vecngetele(i,r1);
	vecnsetele(i,r1,vecngetele(i,r2));
	vecnsetele(i,r2,t);
}

// multiply a row by a number in a matn and a vecn
static void multrowmv(struct matn* a,struct vecn* i,S32 r1,float mul)
{
	S32 c;
	float t;
	for (c=0;c<a->order;++c) {
		t=matngetcolrow(a,c,r1);
		t*=mul;
		matnsetcolrow(a,c,r1,t);
	}
	t=vecngetele(i,r1);
	t*=mul;
	vecnsetele(i,r1,t);
}

// add a multiple of 1 row to another row in a matn and a vecn
static void addmultmv(struct matn* a,struct vecn* i,S32 rs,S32 rd,float mul)
{
	S32 c;
	float t;
	for (c=0;c<a->order;++c) {
		t=matngetcolrow(a,c,rs);
		t*=mul;
		t+=matngetcolrow(a,c,rd);
		matnsetcolrow(a,c,rd,t);
	}
	t=vecngetele(i,rs);
	t*=mul;
	t+=vecngetele(i,rd);
	vecnsetele(i,rd,t);
}

S32 matnsolve(const struct matn* bu2lio,const struct vecn* li,struct vecn* bu) // returns 0 no, 1 one, 2 many
{
//	S32 hasinverse;
	S32 watchdog=0;
	S32 ret=0;
	S32 c,r,k;
	float v,v2;
	struct matn* bu2li=matncreate(bu2lio->order);
	matncopy(bu2lio,bu2li);
	vecncopy(li,bu);
	while(watchdog<=400) {
//		hasinvers=1; // assume for now
// for every column
		for (c=0;c<bu2li->order;++c) {
// do the diagonal element first
			v=matngetcolrow(bu2li,c,c);
			if (v==0.0f) { // find a replacment row by swapping with a nonzero row below
				for (k=c+1;k<bu2li->order;++k) {
					v2=matngetcolrow(bu2li,c,k);
					if (v2!=0.0f) { // find one?
						gcon_printf("matnsolve: swapping rows %d and %d\n",c,k);
						swaprowmv(bu2li,bu,c,k);				
						matnvecnlogger(bu2li,bu,"swap");
						v=v2;
						break;
					}
				}
//				if (k>=bu2li->order) { // couldn't find one?
//					hasinverse=0;
//				}
			} 
// scale it to 1
			if (v!=0.0f && v!=1.0f) {
				gcon_printf("matnsolve: multiplying row %d by %f\n",c,1.0f/v);
				multrowmv(bu2li,bu,c,1.0f/v);		
				matnvecnlogger(bu2li,bu,"scale 1");
			}
// finished with diagonal, now handle the rest
			if (v!=0.0f) {
				for (r=0;r<bu2li->order;++r) {
					float mul;
					if (r==c)
						continue; // already done the diagonal
					mul=matngetcolrow(bu2li,c,r);
					if (mul!=0.0f) { // gotta zero it, use addmult to do it
						mul=-mul;
						gcon_printf("matnsolve: multiplying row %d by %f\nand adding it to row %d\n",c,mul,r);
						addmultmv(bu2li,bu,c,r,mul);
						matnvecnlogger(bu2li,bu,"mult/add");
					}
				}
			}
		}
// now look for degenerate rows
		for (r=0;r<bu2li->order;++r) {
			for (c=0;c<bu2li->order;++c) {
				if (matngetcolrow(bu2li,c,r)!=0)
					break;
			}
			if (c==bu2li->order) 
				break;
		}
// degnerater row found
		if (r!=bu2li->order) { // found a row of zeros
			S32 i,j,k;
			gcon_printf("degen row %d\n",r);
			if (vecngetele(bu,r)) {
				ret=0; // contradiction, no solutions
				break;
			}
// inject a 1 into degenerate row at the right spot, to keep independence
// first look for zerod columns
			for (i=0;i<bu2li->order;++i) {
				for (j=0;j<bu2li->order;++j) {
					if (matngetcolrow(bu2li,i,j)!=0)
						break;
				}
				if (j==bu2li->order)
					break;
			}
			if (i!=bu2li->order) { // found a column of zeros
				gcon_printf("zero column at %d, injecting 1.0f at %d,%d\n",i,i,r);
				matnsetcolrow(bu2li,i,r,1.0f);
			} else { // did not find a column of rows, look for 1 that doesn't match what we are putting in..
				for (k=0;k<bu2li->order;++k) {
					for (j=0;j<bu2li->order;++j) {
						for (i=0;i<bu2li->order;++i) {
							if (k==i) {
								if (matngetcolrow(bu2li,i,j)==0)
									break;
							} else {
								if (matngetcolrow(bu2li,i,j)!=0)
									break;
							}
						}
						if (i==bu2li->order) { // row j matches
							break;
						}
					}
					if (j==bu2li->order) { // no matches
						gcon_printf("found nomatch at column %d, injecting 1.0f at %d,%d\n",k,k,r);
						matnsetcolrow(bu2li,k,r,1.0f);
						break;
					}
				}
				if (k==bu2li->order)
					errorexit("can't find independent 1");
			}
			ret=2;
// no degenerate rows
		} else {
			if (!ret) // 1 solution
				ret=1;
			break;
		}
		++watchdog;
	}
	if (watchdog>400)
		errorexit("matnsolve: hit watchdog");
	if (ret==0) {
		gcon_printf("matnsolve: no solution\n");
		vecnzero(bu);
	} else if (ret==1) {
		gcon_printf("matnsolve: one solution\n",c);
	} else {
		gcon_printf("matnsolve: many solutions\n",c);
	}
	matnvecnlogger(bu2li,bu,"result");
	matnfree(bu2li);
	return ret;
}

void matnvecnlogger(const struct matn* m,const struct vecn* v,const C8* desc)
{
	S32 c,r;
	gcon_printf("---- matn | vecn '%s' ------\n",desc);
	for (r=0;r<m->order;++r) {
		for (c=0;c<m->order;++c) {
			gcon_printf("%9.3f",matngetcolrow(m,c,r));
		}
		gcon_printf(" |%9.3f\n",vecngetele(v,r));
	}
}

/////////////////////////////// special binary matrix
struct matb* matbcreate(S32 ord)
{
	struct matb* ret;
//	ret=(struct matb*)memalloc(sizeof(struct matb));
	ret=new matb;
	ret->order=ord;
//	ret->data=(C8*)memalloc(sizeof(U8)*ord*ord);
	ret->data = new C8[sizeof(U8)*ord*ord];
	matbident(ret);
	return ret;
}

void matbfree(struct matb* a)
{
//	memfree(a->data);
	delete[] a->data;
//	memfree(a);
	delete a;
}

void matbsetcolrow(struct matb* a,S32 col,S32 row,S32 val)
{
	a->data[col+a->order*row]=val;
}

S32 matbgetcolrow(const struct matb* a,S32 col,S32 row)
{
	return 	a->data[col+a->order*row];
}

void matbmult(const struct matb* a,const struct matb* b,struct matb* prod)
{
	S32 c,r,k;
	for (r=0;r<a->order;++r) {
		for (c=0;c<a->order;++c) {
			S32 sum=0;
			for (k=0;k<a->order;++k) {
				S32 av=matbgetcolrow(a,k,r);
				S32 bv=matbgetcolrow(b,c,k);
				sum+=av*bv;
			}
			matbsetcolrow(prod,c,r,sum&1);
		}
	}
}

// swap 2 rows in 2 matns
static void swaprowpairb(struct matb* a,struct matb* i,S32 r1,S32 r2)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,r1);
		matbsetcolrow(a,c,r1,matbgetcolrow(a,c,r2));
		matbsetcolrow(a,c,r2,t);
		t=matbgetcolrow(i,c,r1);
		matbsetcolrow(i,c,r1,matbgetcolrow(i,c,r2));
		matbsetcolrow(i,c,r2,t);
	}
}

// multiply a row by a number in 2 matns
static void multrowpairb(struct matb* a,struct matb* i,S32 r1,S32 mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,r1);
		t*=mul;
		matbsetcolrow(a,c,r1,t&1);
		t=matbgetcolrow(i,c,r1);
		t*=mul;
		matbsetcolrow(i,c,r1,t&1);
	}
}

// add a multiple of 1 row to another row in 2 matns
static void addmultpairb(struct matb* a,struct matb* i,S32 rs,S32 rd,S32 mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,rs);
		t*=mul;
		t+=matbgetcolrow(a,c,rd);
		matbsetcolrow(a,c,rd,t&1);
		t=matbgetcolrow(i,c,rs);
		t*=mul;
		t+=matbgetcolrow(i,c,rd);
		matbsetcolrow(i,c,rd,t&1);
	}
}

// swap 2 rows in 1 matn
static void swaprowsingleb(struct matb* a,S32 r1,S32 r2)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,r1);
		matbsetcolrow(a,c,r1,matbgetcolrow(a,c,r2));
		matbsetcolrow(a,c,r2,t);
	}
}

// multiply a row by a number in 1 matn
static void multrowsingleb(struct matb* a,S32 r1,S32 mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,r1);
		t*=mul;
		matbsetcolrow(a,c,r1,t&1);
	}
}

// add a multiple of 1 row to another row in 1 matn
static void addmultsingleb(struct matb* a,S32 rs,S32 rd,S32 mul)
{
	S32 c;
	for (c=0;c<a->order;++c) {
		S32 t;
		t=matbgetcolrow(a,c,rs);
		t*=mul;
		t+=matbgetcolrow(a,c,rd);
		matbsetcolrow(a,c,rd,t&1);
	}
}

S32 matbdet(const struct matb* ao)
{
	S32 ret=1;
	S32 c,r,k;
	S32 v,v2;
	struct matb* a=matbcreate(ao->order);
	matbcopy(ao,a);
// for every column
	for (c=0;c<a->order;++c) {
// do the diagonal element first
		v=matbgetcolrow(a,c,c);
		if (v==0) { // find a replacment row by swapping with a nonzero row below
			for (k=c+1;k<a->order;++k) {
				v2=matbgetcolrow(a,c,k);
				if (v2!=0) { // find one?
					gcon_printf("detb: swapping rows %d and %d\n",c,k);
					swaprowsingleb(a,c,k);	
					ret=-ret;
					v=v2;
					break;
				}
			}
			if (k>=a->order) { // couldn't find one?
				matbfree(a);
				gcon_printf("detb: 0\n",c);
				return 0;
			}
		} 
// scale it to 1
		gcon_printf("det: multiplying row %d by %d\n",c,1);
		multrowsingleb(a,c,1);
		ret*=v;
		ret&=1;
// finished with diagonal, now handle the rest
		for (r=c+1;r<a->order;++r) {
			S32 mul;
			mul=matbgetcolrow(a,c,r);
			if (mul!=0) { // gotta zero it, use addmult to do it
//				mul=-mul;
				gcon_printf("detb: multiplying row %d by %d\nand adding it to row %d\n",c,mul,r);
				addmultsingleb(a,c,r,mul);
			}
		}
	}
	matbfree(a);
	return ret;
}

S32 matbinverse(const struct matb* ao,struct matb* i) // returns true if invertable
{
	S32 c,r,k;
	S32 v,v2;
	struct matb* a=matbcreate(ao->order);
	matbcopy(ao,a);
	matbident(i);
// for every column
	for (c=0;c<a->order;++c) {
// do the diagonal element first
		v=matbgetcolrow(a,c,c);
		if (v==0) { // find a replacment row by swapping with a nonzero row below
			for (k=c+1;k<a->order;++k) {
				v2=matbgetcolrow(a,c,k);
				if (v2!=0) { // find one?
//					gcon_printf("swapping rows %d and %d\n",c,k);
					swaprowpairb(a,i,c,k);				
					v=v2;
					break;
				}
			}
			if (k>=a->order) { // couldn't find one?
				matbzero(i);
				matbfree(a);
//				gcon_printf("no inverse\n",c);
				return 0;
			}
		} 
// scale it to 1
//		gcon_printf("multiplying row %d by %d\n",c,1);
		multrowpairb(a,i,c,1);		
// finished with diagonal, now handle the rest
		for (r=0;r<a->order;++r) {
			S32 mul;
			if (r==c)
				continue; // already done the diagonal
			mul=matbgetcolrow(a,c,r);
			if (mul!=0) { // gotta zero it, use addmult to do it
//				mul=-mul;
//				gcon_printf("multiplying row %d by %d\nand adding it to row %d\n",c,mul,r);
				addmultpairb(a,i,c,r,mul);
			}
		}
	}
	matbfree(a);
	return 1;
}

void matblogger(const struct matb* a,const C8* desc)
{
	S32 c,r;
	gcon_printf("---- matb '%s' ------\n",desc);
	for (r=0;r<a->order;++r) {
		for (c=0;c<a->order;++c) {
			gcon_printf("%d ",matbgetcolrow(a,c,r));
		}
		gcon_printf("\n");
	}
}

void matbcopy(const struct matb* a,struct matb* b) // copy a to b
{
	memcpy(b->data,a->data,sizeof(U8)*a->order*a->order);
}

void matbident(struct matb* a)
{
	S32 i;
	matbzero(a);
	for (i=0;i<a->order;++i)
		matbsetcolrow(a,i,i,1);
}

void matbzero(struct matb* a)
{
	memset(a->data,0,sizeof(U8)*a->order*a->order);
}

S32 vecbcmp(const struct vecb* a,const struct vecb* b) // returns 0 if equal
{
	return memcmp(a->data,b->data,sizeof(U8)*a->order);
}

struct vecb* vecbcreate(S32 ord) // returns a zero vector
{
	struct vecb* ret;
//	ret=(struct vecb*)memalloc(sizeof(struct vecb));
	ret=new vecb;
	ret->order=ord;
//	ret->data=(C8*)memalloc(sizeof(U8)*ord);
	ret->data=new C8[sizeof(U8)*ord];
	vecbzero(ret);
	return ret;
}

void vecbfree(struct vecb* a)
{
//	memfree(a->data);
	delete[] a->data;
//	memfree(a);
	delete a;
}

void vecbsetele(struct vecb* a,S32 ele,S32 val)
{
	a->data[ele]=val;
}

S32 vecbgetele(const struct vecb* a,S32 ele)
{
	return a->data[ele];
}

void vecbmult(const struct matb* a,const struct vecb* b,struct vecb* prod) // a*b=c assume same size
{
	S32 r,k;
	for (r=0;r<a->order;++r) {
		S32 sum=0;
		for (k=0;k<a->order;++k) {
			S32 av=matbgetcolrow(a,k,r);
			S32 bv=vecbgetele(b,k);
			sum+=av*bv;
		}
		vecbsetele(prod,r,sum&1);
	}
}

void vecblogger(const struct vecb* a,const U8* desc)
{
	S32 e;
	gcon_printf("---- vecb '%s' ------\n",desc);
	for (e=0;e<a->order;++e)
		gcon_printf("%d ",vecbgetele(a,e));
	gcon_printf("\n");
}

void vecbcopy(const struct vecb* a,struct vecb* b) // copy a to b, assume same size
{
	memcpy(b->data,a->data,sizeof(U8)*a->order);
}

void vecbzero(struct vecb* a)
{
	memset(a->data,0,sizeof(U8)*a->order);
}

// swap 2 rows in a matn and a vecn
static void swaprowmvb(struct matb* a,struct vecb* i,S32 r1,S32 r2)
{
	S32 c;
	S32 t;
	for (c=0;c<a->order;++c) {
		t=matbgetcolrow(a,c,r1);
		matbsetcolrow(a,c,r1,matbgetcolrow(a,c,r2));
		matbsetcolrow(a,c,r2,t);
	}
	t=vecbgetele(i,r1);
	vecbsetele(i,r1,vecbgetele(i,r2));
	vecbsetele(i,r2,t);
}

// add a multiple of 1 row to another row in a matn and a vecn
static void addmultmvb(struct matb* a,struct vecb* i,S32 rs,S32 rd,S32 mul)
{
	S32 c;
	S32 t;
	for (c=0;c<a->order;++c) {
		t=matbgetcolrow(a,c,rs);
		t*=mul;
		t+=matbgetcolrow(a,c,rd);
		matbsetcolrow(a,c,rd,t&1);
	}
	t=vecbgetele(i,rs);
	t*=mul;
	t+=vecbgetele(i,rd);
	vecbsetele(i,rd,t&1);
}

S32 matbsolve(const struct matb* bu2lio,const struct vecb* li,struct vecb* bu) // returns 0 no, 1 one, 2 many
{
	static S32 maxwatchdog;
//	S32 hasinverse;
	S32 watchdog=0;
	S32 ret=0;
	S32 c,r,k;
	S32 v,v2;
	struct matb* bu2li=matbcreate(bu2lio->order);
	matbcopy(bu2lio,bu2li);
	vecbcopy(li,bu);
	while(watchdog<=400) {
//		hasinvers=1; // assume for now
// for every column
		for (c=0;c<bu2li->order;++c) {
// do the diagonal element first
			v=matbgetcolrow(bu2li,c,c);
			if (v==0) { // find a replacment row by swapping with a nonzero row below
				for (k=c+1;k<bu2li->order;++k) {
					v2=matbgetcolrow(bu2li,c,k);
					if (v2!=0) { // find one?
//						gcon_printf("matnsolve: swapping rows %d and %d\n",c,k);
						swaprowmvb(bu2li,bu,c,k);				
//						matnvecnlogger(bu2li,bu,"swap");
						v=v2;
						break;
					}
				}
//				if (k>=bu2li->order) { // couldn't find one?
//					hasinverse=0;
//				}
			} 
// scale it to 1
/*			if (v!=0 && v!=1) {
//				gcon_printf("matnsolve: multiplying row %d by %f\n",c,1/v);
				multrowmvb(bu2li,bu,c,1/v);		
//				matnvecnlogger(bu2li,bu,"scale 1");
			} */
// finished with diagonal, now handle the rest
			if (v!=0) {
				for (r=0;r<bu2li->order;++r) {
					S32 mul;
					if (r==c)
						continue; // already done the diagonal
					mul=matbgetcolrow(bu2li,c,r);
					if (mul!=0) { // gotta zero it, use addmult to do it
//						mul=-mul;
//						gcon_printf("matnsolve: multiplying row %d by %f\nand adding it to row %d\n",c,mul,r);
						addmultmvb(bu2li,bu,c,r,mul);
//						matnvecnlogger(bu2li,bu,"mult/add");
					}
				}
			}
		}
// now look for degenerate rows
		for (r=0;r<bu2li->order;++r) {
			for (c=0;c<bu2li->order;++c) {
				if (matbgetcolrow(bu2li,c,r)!=0)
					break;
			}
			if (c==bu2li->order) 
				break;
		}
// degnerater row found
		if (r!=bu2li->order) { // found a row of zeros
			S32 i,j,k;
//			gcon_printf("degen row %d\n",r);
			if (vecbgetele(bu,r)) {
				ret=0; // contradiction, no solutions
				break;
			}
// inject a 1 into degenerate row at the right spot, to keep independence
// first look for zerod columns
			for (i=0;i<bu2li->order;++i) {
				for (j=0;j<bu2li->order;++j) {
					if (matbgetcolrow(bu2li,i,j)!=0)
						break;
				}
				if (j==bu2li->order)
					break;
			}
			if (i!=bu2li->order) { // found a column of zeros
//				gcon_printf("zero column at %d, injecting 1.0f at %d,%d\n",i,i,r);
				matbsetcolrow(bu2li,i,r,1);
			} else { // did not find a column of rows, look for 1 that doesn't match what we are putting in..
				for (k=0;k<bu2li->order;++k) {
					for (j=0;j<bu2li->order;++j) {
						for (i=0;i<bu2li->order;++i) {
							if (k==i) {
								if (matbgetcolrow(bu2li,i,j)==0)
									break;
							} else {
								if (matbgetcolrow(bu2li,i,j)!=0)
									break;
							}
						}
						if (i==bu2li->order) { // row j matches
							break;
						}
					}
					if (j==bu2li->order) { // no matches
//						gcon_printf("found nomatch at column %d, injecting 1.0f at %d,%d\n",k,k,r);
						matbsetcolrow(bu2li,k,r,1);
						break;
					}
				}
				if (k==bu2li->order)
					errorexit("can't find independent 1");
			}
			ret=2;
// no degenerate rows
		} else {
			if (!ret) // 1 solution
				ret=1;
			break;
		}
		++watchdog;
	}
	if (watchdog>400)
		errorexit("matbsolve: hit watchdog");
	logger("watchdog of matbsolve order %d = %d\n",bu2lio->order,watchdog);
	if (watchdog > maxwatchdog) {
		maxwatchdog = watchdog;
		logger("INCREASE: maxwatchdog of matbsolve order %d = %d\n",bu2lio->order,maxwatchdog);
	}
	if (ret==0) {
//		gcon_printf("matbsolve: no solution\n");
		vecbzero(bu);
//	} else if (ret==1) {
//		gcon_printf("matbsolve: one solution\n",c);
//	} else {
//		gcon_printf("matbsolve: many solutions\n",c);
	}
//	matnvecnlogger(bu2li,bu,"result");
	matbfree(bu2li);
	return ret;
}
