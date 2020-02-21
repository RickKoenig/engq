#include <m_eng.h>

#include "u_bch.h"
#include "u_qr_code_layout_v2.h"
#include "u_qr_code2scan.h"

#define USENS // use namespace, turn off for debugging
#ifdef USENS
namespace {
#endif

	U8 domask(S32 i,S32 j,S32 mask)
{
	U8 r = 0;
	switch(mask) {
		case 0:
			r = (i+j) % 2;
			break;
		case 1:
			r = j % 2;
			break;
		case 2:
			r = i % 3;
			break;
		case 3:
			r = (i+j) % 3;
			break;
		case 4:
			r = (i/3+j/2) % 2;
			break;
		case 5:
			r = (i*j)%2 + (i*j)%3;
			break;
		case 6:
			r = ((i*j)%3 + i*j)%2;
			break;
		case 7:
			r = ((i*j) % 3 + i + j)%2;
			break;
		default:
			r = 0;
			break;
	}
	if (r)
		r = 0;
	else
		r = 1;
	return r;
}

void drawmasks()
{
	S32 i,j,k;
	for (k=0;k<8;++k) {
		logger("mask %d\n",k);
		for (j=0;j<25;++j) {
			for (i=0;i<25;++i) {
				C8 c = domask(i,j,k)!=0 ? '#' : '.';
				logger("%c",c);
			}
			logger("\n");
		}
	}
}

#if 0
void drawlayout(S32 modu)
{
	S32 ncodes = qr_layout.size();
	S32 i,j,k;
	vector<vector<U8> > datai(modu);
	logger("layout bytes\n");
	for (j=0;j<modu;++j) {
		datai[j].assign(modu,99);
	}
	for (k=0;k<ncodes;++k) {
		for (j=0;j<8;++j) {
			const pointi2& pr = qr_layout[k][j];
			datai[pr.y][pr.x] = k;
		}
	}
	for (j=0;j<modu;++j) {
		for (i=0;i<modu;++i) {
			C8 c;
			U8 d = datai[j][i];
			if (d == 99)
				c = '.';
			else
				if (d < 26)
					c = d + 'a';
				else
					c = d - 26 + 'A';
				logger("%c",c);
		}
		logger("\n");
	}
	logger("\n");
	logger("layout bits\n");
	for (j=0;j<modu;++j) {
		for (i=0;i<modu;++i) {
			datai[j][i] = 99;
		}
	}
	for (k=0;k<ncodes;++k) {
		for (j=0;j<8;++j) {
			const pointi2& pr = qr_layout[k][j];
			datai[pr.y][pr.x] = j;
		}
	}
	for (j=0;j<modu;++j) {
		for (i=0;i<modu;++i) {
			C8 c;
			U8 d = datai[j][i];
			if (d == 99)
				c = '.';
			else
				c = d + '0';
				logger("%c",c);
		}
		logger("\n");
	}
	logger("\n");
}
#endif

rs buildrsv2(const vector<vector<U8> >& dat)
{
	S32 ncodes = qr_layout.size();
	rs ret(ncodes);
	S32 i,b;
	for (i=0;i<ncodes;++i) {
		U8 va = 0;
		for (b=0;b<8;++b) {
			const pointi2& p = qr_layout[i][b];
			U8 vb = dat[p.y][p.x];
			va += (vb<<b);
//			va += (vb<<(7-b));
		}
//		ret[i] = va;
		ret[ncodes-1-i] = va;
	}
	return ret;
}

class bitstream {
	rs data;
	U32 pos;
	U32 bit;
public:
	bitstream(const rs& dataa);
	U32 getbits(U32 nbits);
	void rewind();
};

bitstream::bitstream(const rs& dataa) : pos(0),bit(7)
{
	data = dataa;
}

U32 bitstream::getbits(U32 nbits)
{
	U32 i;
	U32 r = 0;
	for (i=0;i<nbits;++i) {
		r = (r<<1);
		if (pos < data.size()) {
			r += ((data[pos]>>bit)&1);
			if (bit == 0) {
				bit = 7;
				++pos;
			} else {
				--bit;
			}
		}
	}
	return r;
}

void bitstream::rewind()
{
	pos = 0;
	bit = 7;
}

vector<U8> parseqrcode(vector<vector<U8> > data)
{
	vector<U8> ret;
	// print scanned data
	S32 i,j;
	S32 modu = data.size();
	logger("data\n");
#if 0
	for (j=0;j<modu;++j) {
		for (i=0;i<modu;++i) {
			C8 c = data[j][i] ? '#' : '.'; // # 0 light,  . 1 dark
			logger("%c",c);
		}
		logger("\n");
	}
#else
	for (j=0;j<modu;++j) {
		logger("{");
		for (i=0;i<modu;++i) {
			C8 c = data[j][i] ? '1' : '0'; // # 0 light,  . 1 dark
			logger("%c,",c);
		}
		logger("},\n");
	}
#endif
	logger("\n");
	// extra: check timing patterns NYI
	// decode scanned 2d bit array
	// do format info first
	U32 format0=0,format1=0;
	for (i=0;i<15;++i) {
		S32 x = fc0[i].x;
		S32 y = fc0[i].y;
		if (x<0)
			x = modu + x;
		if (y<0)
			y = modu + y;
		format0 += (data[y][x]<<i);
	}
	for (i=0;i<15;++i) {
		S32 x = fc1[i].x;
		S32 y = fc1[i].y;
		if (x<0)
			x = modu + x;
		if (y<0)
			y = modu + y;
		format1 += (data[y][x]<<i);
	}
	U32 fmask = 0x5412; // 101010000010010
	format0 ^= fmask;
	format1 ^= fmask;
	logger("before damage, format0 = %x, format1 = %0x\n",format0,format1);
//	format0 ^= 0x7000; // test damage, try 3 errors and 1 erasure
//	format1 ^= 0x7001; // test damage
//	logger("after damage, format0 = %x, format1 = %0x\n",format0,format1);
	U32 format = format0 & format1;
	U32 erasure = format0 ^ format1;
	S32 nerr = 0,nera = 0;
	U32 corcod = decoder15_5(format,erasure,&nerr,&nera); // nerr == -1 decode error
	logger("correctcode = %x, nerror = %d, nerasure = %d\n",corcod,nerr,nera);
	U32 mess = corcod >> 10;
	logger("mess = %x\n",mess);
	if (nerr < 0) {
		logger("bad format, too many errors!\n");
		return ret;
	}
//	drawmasks();
	S32 msk = mess & 7;
	S32 qual = (mess >> 3) & 3;
	S32 ver = (data.size()-17)/4;
	const C8* const qualstr[4] = {
		"l(M)qh",
		"(L)mqh",
		"lmq(H)",
		"lm(Q)h",
	};
	logger("version = %d, qual = %d '%s', mask = %d\n",ver,qual,qualstr[qual],msk);
#if 0
	C8 str[200];
	sprintf(str,"version = %d, qual = %d, mask = %d\n",ver,qual,msk);
	ret.assign((U8*)str,(U8*)str+strlen(str)+1);
	return ret;
#endif
#if 1
//	for (k=0;k<8;++k) {
//		msk = k;
		// un apply potential mask to data
		for (j=0;j<modu;++j) {
			for (i=0;i<modu;++i) {
				data[j][i] ^= domask(i,j,msk);
			}
		}
		// show unmasked data
		logger("\nunmasked data\n");
		for (j=0;j<modu;++j) {
			for (i=0;i<modu;++i) {
				C8 c = data[j][i] ? '#' : '.'; // # 0 light,  . 1 dark
				logger("%c",c);
			}
			logger("\n");
		}
		struct codeinfo {
			S32 ndatas;
			S32 nsyms;
		};
		struct verinfo {
			S32 nblocks;
			codeinfo ci[4];
		};
	#if 0
		const S32 nsyms[7][4] = {
	// M L H Q, 1 0 3 2 // quality, versions 1 to 6
			{0},           // version 0, placeholder
			{10, 7,17,13}, // version 1 // 26 codewords
			{16,10,28,22}, // version 2 // 44 codewords
		};
	#endif
		static const verinfo verinfos[7][4] = {
	// M L H Q, 1 0 3 2 // quality, versions 1 to 6
			{
				{0},
			},
			{ // ver 1 26
				{1,{{16,10}}}, // m
				{1,{{19, 7}}}, // l
				{1,{{ 9,17}}}, // h
				{1,{{13,13}}}, // q
			},
			{ // ver 2 44
				{1,{{28,16}}}, // m
				{1,{{34,10}}}, // l
				{1,{{16,28}}}, // h
				{1,{{22,22}}}, // q
			},
			{ // ver 3 70
				{1,{{44,26}}}, // m
				{1,{{55,15}}}, // l
				{2,{{13,22},{13,22}}}, // h
				{2,{{17,18},{17,18}}}, // q
			},
			{ // ver 4 100
				{2,{{32,18},{32,18}}}, // m
				{1,{{80,20}}}, // l
				{4,{{ 9,16},{ 9,16},{ 9,16},{ 9,16}}}, // h
				{2,{{24,26},{24,26}}}, // q
			},
			{ // ver 5 134
				{2,{{43,24},{43,24}}}, // m
				{1,{{108,26}}}, // l
				{4,{{11,22},{11,22},{12,22},{12,22}}}, // h
				{4,{{15,18},{15,18},{16,18},{16,18}}}, // q
			},
			{ // ver 6 172
				{4,{{27,16},{27,16},{27,16},{27,16}}}, // m
				{2,{{68,18},{68,18}}}, // l
				{4,{{15,28},{15,28},{15,28},{15,28}}}, // h
				{4,{{19,24},{19,24},{19,24},{19,24}}}, // q
			},
		};
//		ver = 5;
//		qual = 3;
		if (ver > 6 || ver < 1) {
			logger("not version 1 to 6\n");
			return ret;
		}
		// interleave
		const verinfo& vi = verinfos[ver][qual];
//		if (vi.nblocks != 1) {
//			logger("numblocks != 1\n");
//			return ret;
//		}
		vector<U32> off; // offset
		U32 offa = 0;
		off.push_back(offa);
		for (i=0;i<vi.nblocks-1;++i) {
			offa += vi.ci[i].ndatas;
			off.push_back(offa);
		}
		offa += vi.ci[i].ndatas;
		off.push_back(offa);
		for (i=0;i<vi.nblocks-1;++i) {
			offa += vi.ci[i].nsyms;
			off.push_back(offa);
		}
		offa += vi.ci[i].nsyms;
		off.push_back(offa);
		U32 mds = vi.ci[vi.nblocks-1].ndatas;
		vector<U32> il; // interleave
		// data
		for (j=0;j<(S32)mds;++j) {
			for (i=0;i<vi.nblocks;++i) {
				if (j<vi.ci[i].ndatas) {
					il.push_back(j+off[i]);
				}
			}
		}
		// sym
		mds = vi.ci[vi.nblocks-1].nsyms;
		for (j=0;j<(S32)mds;++j) {
			for (i=0;i<vi.nblocks;++i) {
				if (j<vi.ci[i].nsyms) {
					il.push_back(j+off[i+vi.nblocks]);
				}
			}
		}
		logger("off size = %d: ",off.size());
		for (j=0;j<(S32)off.size();++j)
			logger("%d ",off[j]);
		logger("\n");
		logger("il size = %d: ",il.size());
		for (j=0;j<(S32)il.size();++j)
			logger("%d ",il[j]);
		logger("\n");
//		ver
		//return ret;
		// get code
		buildlayout(ver);
		rs cod = buildrsv2(data);
		if (cod.size() != offa) {
			logger("code size(%d) != version size(%d)\n",cod.size(),offa);
			return ret;
		}
/*		if (ver > 2) {
			logger("not version 1 to 2\n");
			return ret;
		} */
//		S32 q;
//		for (q=0;q<4;++q) {
		// fix errors in code
		logger("full interleaved code bytes   ");
		for (j=cod.size()-1;j>=0;--j)
			logger("%02x ",cod[j]);
		logger("\n\n");

		rs mss;
#if 1
		::reverse(cod.begin(),cod.end());
		rs codb = cod;
		for (j=0;j<(S32)offa;++j)
			cod[il[j]] = codb[j];
//			cod[j] = codb[il[j]];
//		}
		logger("fulluninterleaved code bytes  ");
		for (j=cod.size()-1;j>=0;--j)
			logger("%02x ",cod[j]);
		logger("\n\n");
		for (i=0;i<vi.nblocks;++i) {
			rs scod(cod.begin()+off[i],cod.begin()+off[i+1]);
			scod.insert(scod.end(),cod.begin()+off[i+vi.nblocks],cod.begin()+off[i+vi.nblocks+1]);
			S32 nerro = 0,nerasu = 0;
			::reverse(scod.begin(),scod.end());
			logger("uncorrected code bytes are    ");
			for (j=scod.size()-1;j>=0;--j)
				logger("%02x ",scod[j]);
			logger("\n");
			scod = rs_correct_msg(scod,vi.ci[i].nsyms,&nerro,&nerasu);
			//::reverse(scod.begin(),scod.end());
			logger("(%d,%d) , nerro = %d, nerasu = %d\n",scod.size(),scod.size()-vi.ci[i].nsyms,nerro,nerasu);
			if (nerro < 0) {
				logger("bad rscode too many errors!\n");
				return ret;
			}
			logger("  corrected code bytes are    ");
			for (j=scod.size()-1;j>=0;--j)
				logger("%02x ",scod[j]);
			logger("\n");
	//		}
			// get message bytes
			logger("            message bytes are ");
			for (j=0;j<(S32)scod.size()-vi.ci[i].nsyms;++j) {
				U8 c = scod[scod.size()-1-j];
				mss.push_back(c);
				logger("%02x ",c);
			}
			logger("\n\n");
		}
		logger("    full message bytes are    ");
		for (j=0;j<(S32)mss.size();++j)
			logger("%02x ",mss[j]);
		logger("\n\n");
#else
		for (i=0;i<vi.nblocks;++i) {
			rs scod(cod.begin()+off[i],cod.begin()+off[i+1]);
			scod.insert(scod.end(),cod.begin()+off[i+vi.nblocks],cod.begin()+off[i+vi.nblocks+1]);
			S32 nerro = 0,nerasu = 0;
			scod = rs_correct_msg(scod,vi.ci[i].nsyms,&nerro,&nerasu);
			logger("(%d,%d) msk = %d, q = %d, nerro = %d, nerasu = %d\n",scod.size(),scod.size()-vi.ci[i].nsyms,msk,qual,nerro,nerasu);
			if (nerro < 0) {
				logger("bad rscode too many errors!\n");
				return ret;
			}
			logger("  corrected code bytes are    ");
			for (j=scod.size()-1;j>=0;--j)
				logger("%02x ",scod[j]);
			logger("\n");
	//		}
			// get message bytes
			for (j=0;j<(S32)scod.size()-vi.ci[i].nsyms;++j)
				mss.push_back(scod[scod.size()-1-j]);
			logger("            message bytes are ");
			for (j=0;j<(S32)mss.size();++j)
				logger("%02x ",mss[j]);
			logger("\n");
		}
#endif
		// interpret bytes
		bitstream bs(mss);
		U32 mode = bs.getbits(4);
		logger("mode = %d\n",mode);
		if (mode == 4) {
			// byte mode
			U32 leng = bs.getbits(8);
			logger("good qrcode read byte! len = %d\n",leng);
			logger("8 bit ascii = '");
			for (i=0;i<(S32)leng;++i) {
				U8 byt = bs.getbits(8);
				ret.push_back(byt);
				logger("%c",byt);
			}
			logger("'\n");
			logger("8 bit hex = '");
			bs.rewind();
			bs.getbits(12); // skip mode and length for mode 4
			for (i=0;i<(S32)leng;++i)
				logger("%02x ",bs.getbits(8));
			logger("'\n\n");
			// re apply potential mask back to data
#if 0
			for (j=0;j<modu;++j) {
				for (i=0;i<modu;++i) {
					data[j][i] ^= domask(i,j,msk);
				}
			}
#endif
		} else if (mode == 2) {
			// alpha numeric mode
			static const U8 an[45] = {
				'0','1','2','3','4',
				'5','6','7','8','9',
				'A','B','C','D','E',
				'F','G','H','I','J',
				'K','L','M','N','O',
				'P','Q','R','S','T',
				'U','V','W','X','Y',
				'Z',' ','$','%','*',
				'+','-','.','/',':',
			};
			U32 leng = bs.getbits(9);
			logger("good qrcode read alphanumeric! len = %d\n",leng);
			logger("11 bit per 2 char ascii = '");
			for (i=0;i<(S32)leng-1;i+=2) {
				U32 twochar = bs.getbits(11);
				U32 q = twochar/45;
				U8 ch0 = q < 45 ? an[q] : '?';
				U8 ch1 = an[twochar%45];
				ret.push_back(ch0);
				logger("%c",ch0);
				ret.push_back(ch1);
				logger("%c",ch1);
			}
			if (leng&1) { // odd char
				//logger("odd char\n");
				U32 onechar = bs.getbits(6);
				U8 ch = onechar < 45 ? an[onechar] : '?';
				logger("%c",ch);
				ret.push_back(ch);
			}
			logger("\n\n");
//			logger("8 bit hex = '");
//			bs.rewind();
//			bs.getbits(12); // skip mode and length for mode 4
//			for (i=0;i<(S32)leng;++i)
//				logger("%02x ",bs.getbits(8));
//			logger("'\n");
			// re apply potential mask back to data
#if 0
			for (j=0;j<modu;++j) {
				for (i=0;i<modu;++i) {
					data[j][i] ^= domask(i,j,msk);
				}
			}
#endif
		} else {
			// other mode
			logger("not byte(4) or alphanumeric(2) mode!\n");
		}
//	}
//	drawlayout(modu);
#endif
	return ret;
}

#ifdef USENS
}
#endif

// returns byte data and work done on scan
vector<U8> readqrcode(const string& name,const bitmap32* pic,bitmap32** scanpic)
{
	if (*scanpic) {
		bitmap32free(*scanpic);
		//*scanpic = 0;
	}
	*scanpic = bitmap32copy(pic);
	logger("\n\n ------------------------ in readqrcode '%s' ----------------------------------\n",name.c_str());
	vector<U8> ret;
	vector<vector<U8> > data = doqrscan(*scanpic); // analyze raw bitmap return modules (big pixels)
	// done scan
	if (data.empty()) {
		logger("bad scan!\n");
	} else {
		ret = parseqrcode(data);
	}
	return ret;
}
