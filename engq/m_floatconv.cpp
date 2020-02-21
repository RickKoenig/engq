//#define INCLUDE_WINDOWS
#include <iomanip>
#include <m_eng.h>
//#define VERBOSE
#if 0
static U64 nums[]={3,5,7,9,11,13,15,17,19,21,23,25,125,625,3125}; // find binary reciprocals of these
static void float_test1()
{
	static const int nnums=sizeof(nums)/sizeof(nums[0]);
	U64 i,j;
	for (i=0;i<nnums;++i) {
		for (j=1;j<10000000LL;++j) {
			U64 t=nums[i]*j;
			if (!(t&(t+1))) {
				logger("%lld * %lld = %lld\n",nums[i],j,t);
				break;
			}
		}
	}
}
#endif
#ifdef VERBOSE
static string int2str(U32 base,U32 v,S32 prec) {
	string ret;
	if (prec==0) {
		if (v==0)
			return "0";
		while(v!=0) {
			U32 m=v%base;
			if (m<10)
				ret = string(1,'0'+m) + ret;
			else
				ret = string(1,'A'+(m-10)) + ret;
			v/=base;
		}
	}
	for (int i=0;i<prec;++i) {
		U32 m=v%base;
		if (m<10)
			ret = string(1,'0'+m) + ret;
		else
			ret = string(1,'A'+(m-10)) + ret;
		v/=base;
	}
	return ret;
}
#endif
static U64 powi(U64 b,S32 e)
{
	const U32 nbases=16;
	const U32 npows=64;
	static bool inited[nbases];
	static U64 pows[nbases][npows];
	static U64 pow5[npows];
	if (b<1 || b>=nbases) {
//		errorexit("powi: bad base %ull, try 2 or 5",b);
		return 1;
	}
	if (!inited[b]) {
		inited[b]=true;
		S32 i;
		U64 p=1;
		for (i=0;i<64;++i) {
			pows[b][i]=p;
			p*=b;
		}
	}
	if (e<=0)
		return 1;
	if (e>=npows) {
//		errorexit("powi: bad exp %d, try less than %d",e,npows);
		return 1;
	}
	return pows[b][e];
}


// convert fraction to (repeating) decimal/base with bankers rounding in prec
#if 0
static void frac2decrnd2(U32 base,U64 num,U64 den,S32 prec,bool fancyuseadjden,U64 adjden) // assum num>=0 , den > 0
{
	U64 numsav=num;
	num%=den;
	C8* bp="";
	U64 precpow = 1;
	precpow=powi(base,prec);
	string ans;
// take care of integer part
	ans = '.';
	U64 mulbig;
	U64 frac;
	U32 rem;
	U64 whole;
	if (fancyuseadjden) {
		mulbig = U64(num);
		frac = U32(mulbig/adjden);
//		rem = U32(mulbig%adjden);
		whole = numsav/den;
	} else {
		mulbig = U64(num)*precpow;
		frac = U32(mulbig/den);
		whole = numsav/den;
	}
	rem = U32(mulbig%den);
// find exact
	if (rem==0) {
		bp = "E"; // exact
	} else {
		U32 remd = rem<<1; // double rem
		if (remd > den) { // round up
			++frac;
			bp="R+";
		} else if (remd < den ) { // round down, do nothing, same as truncate
			bp="R-";
		} else { // banker, for now round up
			if (frac&1) { // it's now odd, make even
				++frac;
				bp="B+";
			} else {
				bp="B-";
			}
		}
	}
// check for overflow, because of round up, like 16/17 if prec = 3, becomes 1.000 in binary
	if (frac>=precpow) {
		frac-=precpow;
		++whole;
		logger("%3llu/%3llu = 2.0 overflow\n",numsav,den);
	} else {
		logger("%3llu/%3llu = %s %s.%s\n",numsav,den,bp,int2str(base,U32(whole),0).c_str(),int2str(base,U32(frac),prec).c_str());
	}
}
#endif
// convert fraction to (repeating) decimal/base with bankers rounding in prec
// already has precpow pre multiplied in...
//assumes num*precpow>=den && num*precpow<2*den, not anymore
#if 0
static float frac2decrndp(U32 base,S32 exp,bool neg,U64 num,U64 den,S32 prec) // assum num>=0 , den > 0
{
	C8* bp="";
// take care of integer part
	U64 whole;
	U64 frac;
	U64 rem;
	U64 precpow = powi(base,prec);
	U64 realden = den*precpow; // now without pre multiplied prec so num/realden is around 1 to 2
	whole = num/realden; // 1 to 2
	frac = num%realden; // 0 to realden-1 or 0 to den*precpow-1
//	frac = U32(num/den); // 0 - 2^prec-1
	rem = frac%den; // 0 to realden-1
	frac = frac/den; // 0 to precpow
// find exact
	if (rem==0) {
		bp = "E"; // exact
	} else {
		U64 remd = rem<<1; // double rem
		if (remd > den) { // round up
			++frac;
			bp="R+";
		} else if (remd < den ) { // round down, do nothing, same as truncate
			bp="R-";
		} else { // banker, for now round up
			if (frac&1) { // it's now odd, make even
				++frac;
				bp="B+";
			} else {
				bp="B-";
			}
		}
	}
// check for overflow, because of round up, like 16/17 if prec = 3, becomes 1.000 in binary
	if (frac>=precpow) {
		frac-=precpow;
		++whole;
		++exp;
		string overstr(prec,'0');
		logger("%3llu / %3llu = %s %s.%s overflow\n",num,realden,bp,int2str(base,U32(whole),0).c_str(),overstr.c_str());
	} else {
		logger("%3llu / %3llu = %s %s.%s\n",num,realden,bp,int2str(base,U32(whole),0).c_str(),int2str(base,U32(frac),prec).c_str());
	}
	volatile int retip=0;

//	if (prec==23) {
		U32* retip = (U32*)&ret;
		if (neg)
			*retip |= 0x80000000;
		*retip |= ((exp+127)&0xff) << 23;
		*retip |= (frac<<(23-prec)) & 0x007fffff;
//	}
	return ret;
}
#endif
// convert fraction to (repeating) decimal/base with bankers rounding in prec
// already has precpow pre multiplied in to num/den
//assumes num*precpow>=den && num*precpow<2*den, not anymore
static U32 frac2decrnd(U32 base,S32* exp,U64* whole,U64 num,U64 den,S32 prec) // assum num>=0 , den > 0
{
	C8* bp="";
// take care of integer part
	U64 frac;
	U64 rem;
	U64 precpow = powi(base,prec);
	U64 realden = den*precpow; // now without pre multiplied prec so num/realden is around 1 to 2
	*whole = num/realden; // 1 to 2
	frac = num%realden; // 0 to realden-1 or 0 to den*precpow-1
//	frac = U32(num/den); // 0 - 2^prec-1
	rem = frac%den; // 0 to realden-1
	frac = frac/den; // 0 to precpow
// find exact
	if (rem==0) {
		bp = "E"; // exact
	} else {
		U64 remd = rem<<1; // double rem
		if (remd > den) { // round up
			++frac;
			bp="R+";
		} else if (remd < den ) { // round down, do nothing, same as truncate
			bp="R-";
		} else { // banker, for now round up
			if (frac&1) { // it's now odd, make even
				++frac;
				bp="B+";
			} else {
				bp="B-";
			}
		}
	}
// check for overflow, because of round up, like 16/17 if prec = 3, becomes 1.000 in binary
	U64 ws=*whole;
	if (frac>=precpow) {
		frac-=precpow;
		++(*whole);
		ws=*whole;
#ifdef VERBOSE
		logger("OV ");
#endif
		if (*whole>=base) {
#ifdef VERBOSE
			logger("overflow ");
#endif
			++(*exp);
			*whole/=base;
		}
	}
#ifdef VERBOSE
	logger("%3llu / %3llu = %s %s.%s\n",num,realden,bp,int2str(base,U32(ws),0).c_str(),int2str(base,U32(frac),prec).c_str());
#endif
	return (U32)frac;
}

#ifdef VERBOSE
// convert fraction to (repeating) decimal/base, displays number with single quotes around the repeating part
static void frac2dec(U64 base,U64 num,U64 den) // assum num=>0 , den > 0
{
	U64 numsav=num;
	U64 whole=num/den;
	num%=den;
	vector<U64> rems;
	string ans;
// take care of integer part
	ans = '.';
	num *= base;
// do long division
	U32 i,j;
	const U32 wd=100;
	for (i=0;i<wd;++i) { // watchdog
		rems.push_back(num);
		U64 q = num/den;
		num = num%den;
		if (q<10)
			ans += ('0' + C8(q));
		else
			ans += ('A' + C8(q-10));
		if (num==0) {
			j=rems.size();
			break;
		}
		num *= base;
		U32 n=rems.size();
		for (j=0;j<n;++j)
			if (rems[j]==num)
				break;
		if (j!=n) {
			break;
		}
	}
	U32 rep = rems.size()-j;
	if (i==wd) {
		ans += "...";
	} else if (rep) {
		ans += '\'';
		ans.insert(ans.begin()+j+1,'\'');
	}
	logger("%llu / %llu = %s%s rep=%u, remsize %u, j %u\n",
	  numsav, den, int2str(U32(base),U32(whole),0).c_str(), ans.c_str(), rep,rems.size(), j);
}
#endif
// convert fraction to (repeating) decimal/base with bankers rounding in prec
// num/den * baspow^exp , example 1.3E20
// does some extra work in fraction reduction
// not yet ready, needs some work, can prevent some overflows tho
#if 0
static void frac2decrnd3fancy(U32 base,U32 num,U32 den,U32 basepow,U32 exp,U32 prec) // assum num>=0 , den > 0
{
	if (basepow!=10)
		errorexit("frac2decrnd3 is using a base 10 exponent only");
	logger("------- test frac2decrnd3 with base %u, num %u, den %u, basepow %u, exp %u, prec %u ------\n",
	  base,num,den,basepow,exp,prec);
	if (num==0) {
		logger("ZERO\n");
		return;
	}
	float a,b;
	a=float(num)/den*powf(5.0f,float(exp));
	FLOG2(a,b); // asm macro
	S32 redmexp=S32(b); // can be negative
	redmexp-=2; // test... , remove later
	U64 rnum = num*powi(5LL,exp);
	U64 rden = den*powi(2LL,redmexp);
	logger("redmexp = %d, %f\n",redmexp,b);
	logger("rnum = %llu, rden = %llu\n",rnum,rden);
	if (rden==0)
		rden=1;
	U32 q = U32(rnum/rden);
	logger("q = %u\n",q);
// adjust redmexp if necc.
	switch(q) {
		case 0:
			logger("0 red by -1\n");
			rden>>=1;
			redmexp-=1;
			break;
		case 1:
			logger("1 red by 0\n");
			break;
		case 2:
			logger("2 red by 1\n");
			rden<<=1;
			redmexp+=1;
			break;
		case 3:
			logger("3 red by 1\n");
			rden<<=1;
			redmexp+=1;
			break;
		case 4:
			logger("4 red by 2\n");
			rden<<=2;
			redmexp+=2;
			break;
		case 5:
			logger("5 red by 2\n");
			rden<<=2;
			redmexp+=2;
			break;
		case 6:
			logger("6 red by 2\n");
			rden<<=2;
			redmexp+=2;
			break;
		case 7:
			logger("7 red by 2\n");
			rden<<=2;
			redmexp+=2;
			break;
	}
	logger("redmexp = %d, %f\n",redmexp,b);
//	rnum = num*powi(5LL,exp);
//	rden = den*powi(2LL,redmexp);
	logger("rnum = %llu, rden = %llu\n",rnum,rden);
	q = U32(rnum/rden);
	logger("q = %u\n",q);
	U64 rdennp = den*powi(2LL,redmexp-prec);
	frac2decrnd2(base,rnum,rden,prec,true,rdennp); // assum num>=0 , den > 0
}
#endif
#if 0
// convert fraction to (repeating) decimal/base with bankers rounding in prec
// num/den * baspow^exp , example 1.3E20
static void frac2decrnd3(U32 base,U32 num,U32 den,U32 basepow,U32 exp,U32 prec) // assum num>=0 , den > 0
{
	logger("------- test frac2decrnd3 with base %u, num %u, den %u, basepow %u, exp %u, prec %u ------\n",
	  base,num,den,basepow,exp,prec);
	if (num==0) {
		logger("ZERO\n");
		return;
	}
	U64 rnum,rden;
	rnum = num*powi(10LL,exp);
	rden = den;
	frac2dec(base,U32(rnum),U32(rden));
	float a,b;
	a=float(num)/den*powf(10.0f,float(exp));
	FLOG2(a,b); // asm macro
	S32 red=S32(b); // can be negative
//	red-=1; // test... , remove later
// adjust redmexp if necc.
	for (S32 i=0;i<10;++i) {
		if (red>0) {
			rnum = num*powi(10LL,exp);
			rden = den*powi(2LL,red);
		} else if (red<0) {
			rnum = num*powi(10LL,exp)*powi(2LL,-red);
			rden = den;
		} else {
			rnum = num*powi(10LL,exp);
			rden = den;
		}
		logger("red = %d, %f\n",red,b);
		logger("rnum = %llu, rden = %llu\n",rnum,rden);
		if (rden==0)
			rden=1;
		if (rden==0)
			logger("rden == 0, (1)\n");
		U32 q = U32(rnum/rden);
		logger("q = %u\n",q);
		if (q<1) {
			logger("<1 --red\n");
			red-=1;
		} else if (q>1) {
			logger(">1 ++red\n");
			red+=1;
		} else {
			break;
		}
	}
	logger("red = %d, %f\n",red,b);
//	rnum = num*powi(5LL,exp);
//	rden = den*powi(2LL,redmexp);
//	logger("rnum = %llu, rden = %llu\n",rnum,rden);
//	if (rden==0)
//		logger("rden == 0 ,(2)\n");
//	U64 rdennp = den*powi(2LL,redmexp-prec);
	logger("red = %d, ",red);
	frac2decrnd2(base,rnum,rden,prec,false,0); // assum num>=0 , den > 0
}
#endif
// for now, just print stuff...
//	dec2float(13,9,4); // 13 * 10^9 1.xxxx, convert base 10 to base 2
static float dec2float(U64 mant,bool neg,S32 expdec,U32 precbin=23) // assum num>=0 , den > 0
{
#ifdef VERBOSE
	logger("------- test dec2float with %llu * 10^%d prec %u ------\n",
	  mant,expdec,precbin);
#endif
	if (mant==0) {
#ifdef VERBOSE
		logger("ZERO\n");
#endif
		return 0.0f;
	}
// find expbin in float for now...
	float numf = powf(10.0f,float(expdec))*mant; // this is the number
	float numf_lg;
	FLOG2(numf,numf_lg);
#ifdef VERBOSE
	logger("expbin = %f\n",numf_lg);
#endif
	S32 expbin = S32(numf_lg);
	S32 messaround = 0;
	expbin +=messaround; // try to mess up the expbin
	S32 wd;
	U64 num,den;
// normalize and fine tune to >=1 to <2
	for (wd=0;wd<10;++wd) { // try to fix up the expbin
		num = mant * powi(5,expdec) * powi(2,expdec-expbin);
		den = powi(2,expbin-expdec) * powi(5,-expdec);
#ifdef VERBOSE
		logger("2^%d * %llu / %llu\n",expbin,num,den);
#endif
		U64 q = num/den;
		if (q==0) {
#ifdef VERBOSE
			logger("q = %llu, expbin = %d is too high, decreasing\n",q,expbin);
#endif
			--expbin;
		} else if (q>=2) {
#ifdef VERBOSE
			logger("q = %llu, expbin = %d is too low, increasing\n",q,expbin);
#endif
			++expbin;
		} else { // 1 <= q < 2
#ifdef VERBOSE
			logger("q = %llu, expbin = %d is just right\n",q,expbin);
#endif
			break;
		}
	}
	if (wd==10) {
//		errorexit("bad normalize in dec2float");
		return 0.0f;
	}
// print exact
#ifdef VERBOSE
	logger_disableindent();
	logger("2^%d * ",expbin);
	frac2dec(2,num,den);
	logger_enableindent();
#endif
// multiply in prec
//	U64 numprec = num*powi(2,-expbin+expdec+prec);
//	U64 denprec = powi(2,expbin-expdec-prec);
	U64 numprec = mant * powi(5,expdec) * powi(2,expdec-expbin+precbin);
	U64 denprec = powi(2,expbin-expdec-precbin) * powi(5,-expdec);
#ifdef VERBOSE
	logger("2^%d * %llu / %llu * 2^-%d\n",expbin,numprec,denprec,precbin);
	logger_disableindent();
	logger("2^%d * ",expbin);
#endif
#if 0
	float ret=frac2decrndp(2,expbin,neg,numprec,denprec,prec);
#else
	S32 reti = 0;
	U64 whole;
	U32 frc=frac2decrnd(2,&expbin,&whole,numprec,denprec,precbin); // assum num>=0 , den > 0
//	if (precbin==23) {
		float* ret = (float*)&reti;
		if (neg)
			reti |= 0x80000000;
		reti |= ((expbin+127)&0xff) << 23;
		reti |= (frc<<(23-precbin)) & 0x007fffff;
//	}
#ifdef VERBOSE
	logger_enableindent();
#endif
#endif
	return *ret;
#if 0
// normalize somehow
	U64 rnum,rden;
	rnum = num*powi(10LL,exp);
	rden = den;
	frac2dec(base,U32(rnum),U32(rden));
	float a,b;
	a=float(num)/den*powf(10.0f,float(exp));
	FLOG2(a,b); // asm macro
	S32 red=S32(b); // can be negative
//	red-=1; // test... , remove later
// adjust redmexp if necc.
	for (S32 i=0;i<10;++i) {
		if (red>0) {
			rnum = num*powi(10LL,exp);
			rden = den*powi(2LL,red);
		} else if (red<0) {
			rnum = num*powi(10LL,exp)*powi(2LL,-red);
			rden = den;
		} else {
			rnum = num*powi(10LL,exp);
			rden = den;
		}
		logger("red = %d, %f\n",red,b);
		logger("rnum = %llu, rden = %llu\n",rnum,rden);
		if (rden==0)
			rden=1;
		if (rden==0)
			logger("rden == 0, (1)\n");
		U32 q = U32(rnum/rden);
		logger("q = %u\n",q);
		if (q<1) {
			logger("<1 --red\n");
			red-=1;
		} else if (q>1) {
			logger(">1 ++red\n");
			red+=1;
		} else {
			break;
		}
	}
	logger("red = %d, %f\n",red,b);
//	rnum = num*powi(5LL,exp);
//	rden = den*powi(2LL,redmexp);
//	logger("rnum = %llu, rden = %llu\n",rnum,rden);
//	if (rden==0)
//		logger("rden == 0 ,(2)\n");
//	U64 rdennp = den*powi(2LL,redmexp-precbin);
	logger("red = %d, ",red);
	frac2decrnd2(base,rnum,rden,precbin,false,0); // assum num>=0 , den > 0
#endif
}

// pass in a float, get back sign, expdec, mant
static U32 float2dec(float f,S32* expdec,U64* whole,bool* neg,bool* zero,U32 precdec=6)
{
	U32 precbin=23;
	if (f==0) {
		*zero=true;
		return 0;
	}
	*zero=false;
	float fs=f;
	if (f<0) {
		f=-f;
		*neg=true;
	} else {
		*neg=false;
	}
	if (f>1.0e20f) // too big
		f=1.0e20f;
	if (f<1.0e-20f) // too small
		return 0;
	U32* ip=(U32*)&f;
	U32 mant = 0x00800000 + (*ip&0x007fffff);
	S32 expbin = S32((*ip&0x7f800000)>>23)-127;
	const C8* ns = *neg ? "-" : " ";
#ifdef VERBOSE
	logger("%g = %s %u * 2^%d\n",fs,ns,mant,expbin-23);
#endif
// find expbin in float for now...
	float numf = f; // this is the number
	float numf_log10;
	float flog10;
	float ten = 10.0f;
	FLOG2(ten,flog10);
	FLOG2(numf,numf_log10);
	numf_log10/=flog10;
#ifdef VERBOSE
	logger("expdec = %f\n",numf_log10);
#endif
#if 1
	*expdec = S32(numf_log10);
	S32 messaround = 0;
	expdec +=messaround; // try to mess up the expbin
	S32 wd;
	U64 num,den;
// normalize and fine tune to >=1 to <2
	for (wd=0;wd<10;++wd) { // try to fix up the expbin
		num = mant * powi(2,expbin-*expdec-precbin) * powi(5,-*expdec);
		den = powi(5,*expdec) * powi(2,-expbin+*expdec+precbin);
#ifdef VERBOSE
		logger("10^%d * %llu / %llu\n",*expdec,num,den);
#endif
		U64 q = num/den;
		if (q==0) {
#ifdef VERBOSE
			logger("q = %llu, expdec = %d is too high, decreasing\n",q,*expdec);
#endif
			--*expdec;
		} else if (q>=10) {
#ifdef VERBOSE
			logger("q = %llu, expdec = %d is too low, increasing\n",q,*expdec);
#endif
			++*expdec;
		} else { // 1 <= q < 10
#ifdef VERBOSE
			logger("q = %llu, expdec = %d is just right\n",q,*expdec);
#endif
			break;
		}
	}
	if (wd==10) {
		return 0;
//		errorexit("bad normalize in float2dec");
	}
#endif
	U64 numprec = mant * powi(2,expbin-*expdec-precbin+precdec) * powi(5,-*expdec+precdec);
	U64 denprec = powi(5,*expdec-precdec) * powi(2,-expbin+*expdec+precbin-precdec);
#ifdef VERBOSE
	logger("10^%d * %llu / %llu * 10^-%d\n",*expdec,numprec,denprec,precdec);
	logger_disableindent();
	logger("%s 10^%d * ",ns,*expdec);
#endif
	U32 frc=frac2decrnd(10,expdec,whole,numprec,denprec,precdec); // assum num>=0 , den > 0
#ifdef VERBOSE
	logger_enableindent();
#endif
	return frc;
}

float str2float(const C8* s)
{
	bool neg=false;
	U32 whole=0;
//	U32 wholenum=0;

	U32 frac=0;
	U32 fracnum=0;

	bool negexp=false;
	S32 exp=0;
//	U32 expnum=0;

	U32 p=0;
// sign
	if (s[p]=='-') {
		neg=true;
		++p;
	}
// whole
	if (!isdigit(s[p]) && s[p]!='.')
		errorexit("str2float: not a number '?xxx'");
	while(isdigit(s[p])) {
		whole=10*whole+(s[p]-'0');
//		++wholenum;
		++p;
	}
// fraction
	if (s[p]=='.') {
		++p;
		if (!isdigit(s[p]))
			errorexit("str2float: not a number 'nn.?xx'");
		while(isdigit(s[p])) {
			frac=10*frac+(s[p]-'0');
			++fracnum;
			++p;
		}
	}
// exp
	if (s[p]=='e' || s[p]=='E') {
		++p;
		if (s[p]=='+') {
			++p;
		} else if (s[p]=='-') {
			negexp=true;
			++p;
		}
		if (!isdigit(s[p]))
			errorexit("str2float: not a number 'nn.nnE?xx'");
		while(isdigit(s[p])) {
			exp=10*exp+(s[p]-'0');
//			++expnum;
			++p;
		}
	}
	if (s[p]!='\0')
		errorexit("str2float: not a number 'nn?'");

	if (negexp)
		exp=-exp;
	return dec2float(whole*powi(10,fracnum)+frac,neg,exp-fracnum);
}

static S32 log10i(U32 v)
{
	if (v==0)
		return 0;//-1;
	if (v<10)
		return 0;
	if (v<100)
		return 1;
	if (v<1000)
		return 2;
	if (v<10000)
		return 3;
	if (v<100000)
		return 4;
	if (v<1000000)
		return 5;
	if (v<10000000)
		return 6;
	if (v<100000000)
		return 7;
	if (v<1000000000)
		return 8;
	return 9;
}

string float2str(float f)
{
	const U32 precdec=5;
	const S32 expmax=6;
	const S32 expmin=-4;
	S32 expdec;
	U64 whole;
	bool neg;
	bool zero;
	U32 fracnum=precdec;
	U32 frac=float2dec(f,&expdec,&whole,&neg,&zero,fracnum);
	if (zero)
		return "0";
#define REMOVE_TRAILING_ZEROS
#ifdef REMOVE_TRAILING_ZEROS
	while(true) {
		if (!frac) {
			fracnum=0;
			break;
		}
		if (frac%10)
			break;
		frac/=10;
		--fracnum;
	}
#endif
#define REMOVE_EXP
#ifdef REMOVE_EXP
	if (expdec<=expmax && expdec>0) { // remove exp if -4 <= exp <= 6
		while(expdec>0) { // 0 < exp <= 6
			if (fracnum==0) {
				whole*=10;
			} else {
				U32 pn=(U32)powi(10,fracnum-1);
				U32 q=frac/pn;
				U32 qp=q*pn;
				frac-=qp;
				whole*=10;
				whole+=q;
				--fracnum;
			}
			--expdec;
		}
	} else if (expdec>=expmin && expdec<0) {
		while(expdec<0) { // -4 <= exp < 0
			if (whole==0) {
				++fracnum;	// .000xxx adds a 0
			} else if (whole>=10) {
				errorexit("float2str 10 overflow (%ull)",whole);
			} else {
				U32 pn=(U32)powi(10,fracnum);
				U32 qp=(U32)whole*pn;
				frac+=qp;
				whole=0;
				++fracnum;
			}
			++expdec;
		}
	}
#endif
	stringstream ret;
	if (neg)
		ret << '-';
	ret << whole;
	if (fracnum) {
		ret << '.';
		ret << string(fracnum-log10i(frac)-1,'0'); // .000xxx
		ret << frac;
	}
#ifdef REMOVE_EXP
	if (expdec)
#endif
		ret << 'E' << expdec;
	return ret.str();
}

void float_test()
{
#if 0
	U32 bas = 2;
	U32 nfracdig = 23; // precision
//	frac2decrnd(2,7,16,3);
	for (U32 i=0;i<16;++i)
		logger("%3d = '%s'\n",i,int2str(bas,i,4).c_str());
//	frac2dec(bas,15,16);
//	frac2decrnd3(bas,15,16,10,0,3);
	U32 baspow = 10;
	U32 exp = 10;
	U32 num = 13;
	U32 den = 10;
#endif
#if 0
	for (S32 i=-5;i<=7;++i) {
//	for (S32 i=0;i<=0;++i) {
		char ss[30];
		sprintf(ss,"990099e%d",i);
//		sprintf(ss,"0.0001000625");
		float g=str2float(ss);
		logger("str2float = %g\n",g);
		string s=float2str(g);
		logger("float2str = '%s'\n",s.c_str());
		logger("\n");
	}
#endif
#if 1
//   diff s2f2s '9.860000E-4' 0.00098600006 '9.860001E-4'
#endif
#if 0 // run through all possible floats
	const U32 precdec=5;
	const U32 maxprecdec=5;
	U32 i;
	S32 j;
//	const S32 emin=-6,emax=6;
	const S32 emin=-11,emax=19;
//	const S32 emin=16,emax=30;
	const U32 nmant=U32(powi(10,precdec+1));
	const U32 div=U32(powi(10,precdec));
	const U32 mantmult=U32(powi(10,maxprecdec-precdec));
	for (j=emin;j<=emax;++j) {
		for (i=div;i<nmant;++i) {
	// build a string
			C8 ss1[100];
			U32 frc=i%div;
			U32 p=i/div;
			frc*=mantmult;
//			p*=10;
			sprintf(ss1,"%d.%05dE%d",p,frc,j);
	// 2 float
			float f=str2float(ss1);
			string ss2=float2str(f);
			if (ss1!=ss2) {
				logger("diff s2f2s '%s' %13.10g '%s'\n",ss1,f,ss2.c_str());
				break;
			} else {
//				logger("same s2f2s '%s' %13.1g '%s'\n",ss1,f,ss2.c_str());
			}
		}
		if (i!=nmant)
			break;
	}
	if (i==nmant && j==emax+1)
		logger("all strings the same\n");
#endif
#if 0
	const U32 precbin=3;
	S32 i,j;
	const S32 emin=0,emax=0;
	for (j=emin;j<=emax;++j) {
//	for (j=-20;j<=20;++j) {
		for (i=0;i<(1<<precbin);++i) {
			U32 reti=0;
			float* ret = (float*)&reti;
	// build a float
			bool neg=false;
			if (neg)
				reti |= 0x80000000;
			S32 expbin=j;
			U32 frc=i;
			reti |= ((expbin+127)&0xff) << 23;
			reti |= (frc<<(23-precbin)) & 0x007fffff;
	// done build a float
			string ss=float2str(*ret);
			float f=str2float(ss.c_str());
			string ss2=float2str(f);
			if (ss!=ss2) {
				logger("diff f2s2f2s %11.8g '%s' %11.8g '%s'\n",*ret,ss.c_str(),f,ss2.c_str());
				break;
			}
			float f2=str2float(ss2.c_str());
			if (f!=f2) {
				logger("diff f2s2f2s2f %11.8g '%s' %11.8g '%s' %11.8g\n",*ret,ss.c_str(),f,ss2.c_str(),f2);
				break;
			} else {
				logger("f2s2f2s2f %11.8g '%s' %11.8g '%s' %11.8g\n",*ret,ss.c_str(),f,ss2.c_str(),f2);
			}
		}
		if (i!=(1<<precbin))
			break;
	}
	if (i==(1<<precbin) && j==emax+1)
		logger("all floats the same, all strings the same\n");
#endif
#if 0
//	for (S32 i=3;i>=-9;--i) {
	for (S32 i=19;i>=-11;--i) {
// first go from str to float
//		float res = dec2float(125,true,i-2,4); // 13 * 10^9 1.xxxx
		float res = dec2float(9988776,true,i-6); // 13 * 10^9 1.xxxx
		logger("%g\n",res);
// now from float to str
		float f=res;
		S32 expdec;
		bool neg,zero;
		U64 whole;
		U32 ret=float2dec(f,&expdec,&whole,&neg,&zero,6);
		if (zero)
			logger("%f = ZERO\n",f);
		else {
			const C8* ns = neg ? "-" : " ";
			logger("%g = %s %llu.%u * 10^%d\n",f,ns,whole,ret,expdec);
		}
		logger("\n");
	}
//	frac2decrndp(2,16*8,17,3); // 29/16
#endif
#if 0
	num=1;
	den=16;
	frac2decrndp(bas,num*8*1000000000000000ULL,den*1000000000000000ULL,3);
	for (den=1;den<=20;++den) {
//		U32 num=1;
		for (num=0;num<=20;++num) {
			frac2dec(bas,num,den);
//			frac2decrnd2(bas,num,den,nfracdig,false,0);
//			frac2decrnd3(bas,num,den,10,0,3);
			frac2decrndp(bas,num*8*1000000000000000ULL,den*1000000000000000ULL,3);
//			frac2decrndp(bas,num*8*1000ULL,den*1000ULL,3);
			logger("\n");
		}
	}
//	frac2decrnd3(bas,num,den,baspow,exp,23); // num/den * baspow^exp , example 1.3E20
//	frac2decrnd2(2,124,70,23,false,0);
#endif
}
#if 0
float str2floathex(const C8* s)
{
	U32 reti = 0; // NYI
	float* ret = (float*)&reti;
	return *ret;
#if 0
	float* ret = (float*)&reti;
	if (neg)
		reti |= 0x80000000;
	reti |= ((expbin+127)&0xff) << 23;
	reti |= (frc<<(23-precbin)) & 0x007fffff;
	U32* ip=(U32*)&f;
#endif
}
#endif
// return a string in the form s1.nnnnnnhsee, s is sign n is mantissa e is exponent, all in hex
string float2strhex(float f)
{
	stringstream ss;
	U32* ip=(U32*)&f;
	if (*ip==0)
		return "+0.000000h+00";
	if (*ip==0x80000000)
		return "-0.000000h+00";
	U32 mant = (*ip&0x007fffff);
	S32 expbin = S32((*ip&0x7f800000)>>23)-127;
	bool expsign;
	if (expbin >= 0) {
		expsign = false;
	} else {
		expbin = -expbin;
		expsign = true;
	}
	S32 sign = *ip&0x80000000;
	if (sign)
		ss << "-1.";
	else
		ss << "+1.";
	ss << setw(6);
	ss << setfill('0');
	ss << hex;
	ss << (mant<<1);
	ss << "h";
	if (expsign)
		ss << "-";
	else
		ss << "+";
	ss << setw(2);
	ss << expbin;
	return ss.str();
}

static S32 atohr(const C8* s,const C8* e)
{
	S32 ret = 0;
	while(s<e) {
		ret <<= 4;
		if (*s>='0' && *s<='9')
			ret += *s - '0';
		else if (*s>='a' && *s<='f')
			ret += *s - 'a' + 0xa;
		else if (*s>='A' && *s<='F')
			ret += *s - 'A' + 0xA;
		++s;
	}
	return ret;
}

float str2floathex(const C8* s)
{
	if (strlen(s)==13) {
		if (s[9]=='h' || s[9]=='H') {
			if (s[0]=='+' || s[0]=='-') {
				if (s[10]=='+' || s[10]=='-') {
					if (s[2]=='.') {
						if (s[1]=='0') {
							return 0;
						} else if (s[1]=='1') {
							U32 m=atohr(s+3,s+9);
							S32 e=atohr(s+11,s+13);
							U32 sm = 0;
							if (s[0]=='-')
								sm = 1;
							if (s[10]=='-')
								e = -e;
							S32 reti = 0;
							float* ret = (float*)&reti;
							if (sm)
								reti |= 0x80000000;
							reti |= ((e+127)&0xff) << 23;
							reti |= (m>>1) & 0x007fffff;
							return *ret;
						}
					}
				}
			}
		}
	}
	errorexit("bad str2floathex '%s'",s);
	return 0;
}
