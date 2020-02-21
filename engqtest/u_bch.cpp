#include <m_eng.h>
//#include "u_ply.h"
#include "u_bch.h"

#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

//// BCH
U32 g15_5 = 0x537;  //   10100110111 generator
U32 g18_6 = 0x1f25; // 1111100100101 generator

U32 encoder(U32 m,U32 g,U32 cb,U32 mb)
{
	U32 gdeg = cb - mb;
	U32 c,r=m<<gdeg;
	S32 i;
	for (i=mb-1;i>=0;--i) // m<<(gdeg)%g
		if (r & (1 << (i+gdeg)))
			r ^= (g << i);
	c = (m<<gdeg) ^ r;
	return c; // code
}

U32 dist0(U32 x)
{
	U32 d = 0;
	while (x>0) {
		d += (x&1);
		x >>= 1;
	}
	return d;
}

U32 decoder(U32 c,U32 e,U32 g,U32 cb,U32 mb,S32* nbits,S32* neras)
{
	c &= ~e; // zero code where erasure occurs
	U32 bestc = c;
	U32 bestd = 10000;
	bool er = false;
	S32 m;
	S32 cnt = 1<<mb;
	S32 de = dist0(e);
	for (m=0;m<cnt;++m) {
		U32 tc = encoder(m,g,cb,mb);
		U32 d = dist0((c ^ tc) | e);
		if (d < bestd) {
			bestd = d;
			bestc = tc;
			er = false;
		} else if (d == bestd) {
			bestc = c;
			er = true;
		}
	}
	if (nbits) {
		if (er)
			*nbits = -1;
		else
			*nbits = bestd - de;

	}
	if (neras) {
		*neras = de;
	}
	return bestc;
}

#ifdef USENS
}
#endif

U32 encoder15_5(U32 m)
{
	return encoder(m,g15_5,15,5);
}

U32 encoder18_6(U32 m)
{
	return encoder(m,g18_6,18,6);
}

U32 decoder15_5(U32 c,U32 e,S32* nbits,S32* neras)
{
	return decoder(c,e,g15_5,15,5,nbits,neras);
}

U32 decoder18_6(U32 c,U32 e,S32* nbits,S32* neras)
{
	return decoder(c,e,g18_6,18,6,nbits,neras);
}


//// Reed Solomon
#ifdef USENS
namespace {
#endif
U32 gf_exp[512]; // for wrap
U32 gf_log[256];

bool allzeros(rs in)
{
	U32 i;
	for (i=0;i<in.size();++i)
		if (in[i])
			return false;
	return true;
}

void gf_init()
{
	static bool inited = false;
	if (inited)
		return;
	inited = true;
	gf_exp[0] = 1;
	U32 x = 1;
	S32 i;
	for (i=1;i<255;++i) {
	   x <<= 1;
	   if (x & 0x100) {
		  x ^= 0x11d; // prim poly is 1000011101
	   }
	   gf_exp[i] = x;
	   gf_log[x] = i;
	}
	for (i=0;i<255;++i)
	   gf_exp[i+255] = gf_exp[i];
}


U32 gf_mul(U32 a,U32 b)
{
	gf_init();
	if (!a || !b)
		return 0;
	return gf_exp[gf_log[a] + gf_log[b]];
}

S32 gf_div_err = 0;
U32 gf_div(U32 a,U32 b)
{
	gf_init();
	if (b == 0) {
		//errorexit("gf_div divide by 0");
		logger("gf_div divide by 0\n");
		++gf_div_err;
		return 0;
	}
	if (a == 0)
		return 0;
	return gf_exp[gf_log[a] + 255 - gf_log[b]];
}

rs gf_poly_scale(const rs& p,U32 s)
{
	U32 n = p.size();
	rs r(n);
	U32 i;
	for (i=0;i<n;++i)
		r[i] = gf_mul(p[i],s);
	return r;
}

rs gf_poly_add(const rs& a,const rs& b)
{
	rs r(max(a.size(),b.size()));
	U32 i;
	for (i=0;i<a.size();++i)
		r[i] = a[i];
	for (i=0;i<b.size();++i)
		r[i] ^= b[i];
	return r;
}

rs gf_poly_mul(const rs& a,const rs& b)
{
	rs r(a.size()+b.size()-1);
	U32 i,j;
	for (j=0;j<b.size();++j)
		for (i=0;i<a.size();++i)
			r[i+j] ^= gf_mul(a[i],b[j]);
	return r;
}

U32 gf_poly_eval(const rs& p,U32 x)
{
	S32 i = p.size() - 1;
	U32 r = p[i--];
	while(i >= 0) {
		r = gf_mul(r,x) ^ p[i];
		--i;
	}
	return r;
}

const rs& rs_generator_poly(U32 nsyma)
{
	static rs g; // cache
	static U32 nsym = ~0U;
	if (nsym == nsyma)
		return g;
	gf_init();
	nsym = nsyma;
	g.assign(1,1);
	rs t(2,1);
	U32 i;
	for (i=0;i<nsym;++i) {
		t[0] = gf_exp[i];
		g = gf_poly_mul(g,t);
	}
	return g;
}

rs rs_forney_syndromes(const rs& synd,const rs& pos,U32 nmess)
{
	rs fsynd = synd;      // make a copy
 	gf_init();
	U32 i,j;
	for (i=0;i<pos.size();++i) {
		U32 x = gf_exp[pos[i]];
		for (j=0;j<fsynd.size()-1;++j) {
			fsynd[j] = gf_mul(fsynd[j], x) ^ fsynd[j+1];
		}
		//	  fsynd.erase(fsynd.begin());
		fsynd.pop_back();
	}
	return fsynd;
}

#ifdef USENS
}
#endif

rs rs_encode_msg(const rs& in, U32 nsym)
{
	const rs& gen = rs_generator_poly(nsym);
	rs out(in.size()+nsym);
	S32 i,j;
	for (i=0;i<(S32)in.size();++i)
		out[i+nsym] = in[i];
	for (i=in.size()-1;i>=0;--i) {
		U32 coef = out[i+nsym];
		if (coef != 0)
			for (j=0;j<(S32)gen.size();++j)
				out[i+j] ^= gf_mul(gen[j],coef);
	}
	for (i=0;i<(S32)in.size();++i)
		out[i+nsym] = in[i];
	return out;
}

rs rs_calc_syndromes(const rs& code, U32 nsym)
{
	gf_init();
	rs synd(nsym);
	U32 i;
	for (i=0;i<nsym;++i)
		synd[i] = gf_poly_eval(code, gf_exp[i]);
	return synd;
}


void rs_correct_errata(rs& code,const rs& synd,const rs& pos)
{
// calculate error locator polynomial
	gf_init();
	rs q(1,1);
	U32 i;
	for (i=0;i<pos.size();++i) {
		U32 x = gf_exp[pos[i]];
		rs t(2);
		t[0] = 1;
		t[1] = x;
		q = gf_poly_mul(q,t);
	}
// calculate error evaluator polynomial
	rs p(synd.begin(),synd.begin()+pos.size());
	p = gf_poly_mul(p,q);
	p = rs(p.begin(),p.begin()+pos.size());
// formal derivative of error locator eliminates even terms
	q.erase(q.begin());
	for (i=1;i<q.size();i+=2)
		q[i] = 0;
// compute corrections
	for (i=0;i<pos.size();++i) {
		U32 x = gf_exp[255-pos[i]];
		U32 y = gf_poly_eval(p,x);
		U32 z = gf_poly_eval(q,x);
		code[pos[i]] ^= gf_div(y,gf_mul(x,z));
	}
}

rs rs_find_errors(const rs& synd,U32 nmess,S32* nerrs)
{
	*nerrs = -1;
	// find error locator polynomial with Berlekamp-Massey algorithm
	rs err_poly(1,1);
	rs old_poly(1,1);
	U32 i,j;
	gf_init();
	for (i=0;i<synd.size();++i) {
		old_poly.insert(old_poly.begin(),0);
		U32 delta = synd[i];
		for (j=1;j<err_poly.size();++j) {
			delta ^= gf_mul(err_poly[j], synd[i-j]);
		}
		if (delta != 0) {
			if (old_poly.size() > err_poly.size()) {
				rs new_poly = gf_poly_scale(old_poly, delta);
				old_poly = gf_poly_scale(err_poly, gf_div(1,delta));
				err_poly = new_poly;
			}
			err_poly = gf_poly_add(err_poly, gf_poly_scale(old_poly, delta));
		}
	}
	U32 errs = err_poly.size()-1;
	rs err_pos;
	if (errs*2 > synd.size()) {
		err_pos.clear();
		return err_pos;    // too many errors to correct
	}
	// find zeros of error polynomial
	for (i=0;i<nmess;++i) {
		if (gf_poly_eval(err_poly, gf_exp[255-i]) == 0) {
			err_pos.push_back(i);
		}
	}
	if (err_pos.size() != errs) {
		err_pos.clear();
		return err_pos;    // couldn't find error locations
	}
	*nerrs = err_pos.size();
	return err_pos;
}

rs rs_correct_msg(const rs msg_in, U32 nsym,S32 *nerrs,S32 *nerasures)
{
	gf_init();
	*nerrs = -1;
	*nerasures = 0;
	rs msg_out = msg_in;     // copy of message
	// find erasures
	rs erase_pos;
	U32 i;
	for (i=0;i<msg_out.size();++i) {
		if (msg_out[i] == ~0U) {
			msg_out[i] = 0;
			erase_pos.push_back(i);
//			erase_pos.push_back(msg_in.size()-1-i);
			++*nerasures;
		}
	}
	if (erase_pos.size() > nsym) {
		return msg_out;     // too many erasures to correct
	}
	rs synd = rs_calc_syndromes(msg_out, nsym);
	if (allzeros(synd)) {
		*nerrs = 0;
		return msg_out;  // no errors
	}
	rs fsynd = rs_forney_syndromes(synd, erase_pos, msg_out.size());
	rs err_pos = rs_find_errors(fsynd, msg_out.size(),nerrs);
	if (*nerrs == -1) {
		return msg_out;   // error location failed
	}
	erase_pos.insert(erase_pos.end(),err_pos.begin(),err_pos.end());
	rs_correct_errata(msg_out, synd, erase_pos);
//	rs_correct_errata(msg_out, synd, erase_pos + err_pos);
	synd = rs_calc_syndromes(msg_out, nsym);
	if (!allzeros(synd)) {
		*nerrs = -1; // message is still not right
	}
	return msg_out;
}
