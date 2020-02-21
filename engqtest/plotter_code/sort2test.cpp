#define SORT2TEST
#ifdef SORT2TEST

// find kth value from 2 sorted arrays
// O lg(na+nb)
S32 deref2(const S32* a,S32 na,const S32* b,S32 nb,S32 k)
{
	if (na == 0)
		return b[k];
	else if (nb == 0)
		return a[k];
	S32 ai = na/2;
	S32 bi = nb/2;
	//S32 kle = ai + bi;
	S32 kge = ai + bi + 1;
	if (a[ai] < b[bi]) {
		if (k < kge) {
			return deref2(a,na,b,bi,k); // remove upper b
		} else {
			S32 ra = ai+1;
			return deref2(a+ra,na-ra,b,nb,k-ra); // remove lower a
		}
	} else {
		if ( k < kge) {
			return deref2(a,ai,b,nb,k); // remove upper a
		} else {
			S32 rb = bi+1;
			return deref2(a,na,b+rb,nb-rb,k-rb); // remove lower b
		}
	}
	return 0;
}

// check
S32 deref2c(const S32* a,S32 na,const S32* b,S32 nb,S32 k)
{
	S32 nc = na + nb;
	S32* c = new S32[nc];
	std::copy(a,a+na,c);
	std::copy(b,b+nb,c+na);
	std::sort(c,c+nc);
	S32 ret = c[k];
	delete[] c;
	return ret;
}

// random shuffle
void shuffle(S32* a,S32 na)
{
	for (S32 i=na-1;i>0;--i) {
		S32 j = mt_random(i+1);
		exch(a[i],a[j]);
	}
}

void showarray(C8* name,S32*a ,S32 na)
{
	logger("%s ",name);
	S32 i;
	for (i=0;i<na;++i)
		logger("%5d ",a[i]);
	logger("\n");
}
void sort2test()
{
	logger("###### start doing sort2test\n");
//	S32 a[] = {1,2,10,11,70};
//	S32 b[] = {9,19,29,30,40};
	//S32 k = 4;
	S32 arrsize = 20;
	S32* c = new S32[arrsize];
	S32 na = arrsize/2;
	S32* a = c;
	S32 nb = arrsize - na;
	S32* b = c + na;
	S32 i,k,n;
	c[0] = 1;
	c[1] = 2;
	for (i=2;i<arrsize;++i)
		c[i] = c[i-1] + c[i-2];

	S32 runs = 100000;
	S32 nbad = 0;
	for (n=0;n<runs;++n) {
		bool bad = false;
		for(k=0;k<arrsize;++k) {
			S32 d = deref2(a,na,b,nb,k); // k assumed to be >= 0 and < sum of lengths of a and b arrays
			S32 d2 = deref2c(a,na,b,nb,k); // k assumed to be >= 0 and < sum of lengths of a and b arrays
			if (d!=d2) {
				bad = true;
				++nbad;
			}
		}
		if (bad) {
			showarray("a",a,na);
			showarray("b",b,nb);
			for(k=0;k<arrsize;++k) {
				S32 d = deref2(a,na,b,nb,k); // k assumed to be >= 0 and < sum of lengths of a and b arrays
				S32 d2 = deref2c(a,na,b,nb,k); // k assumed to be >= 0 and < sum of lengths of a and b arrays
				if (d!=d2)
					logger("deref2 of %5d is %5d or %5d\n",k,d,d2);
			}
		}
		shuffle(c,arrsize);
		std::sort(a,a+na);
		std::sort(b,b+nb);
	}
	delete[] c;
	logger("###### end doing sort2test, nbad = %d\n",nbad);
}
#endif
