#include <m_eng.h>
#include "m_perf.h"

#define OPTIMIZED

static const C8* fnamec;
static FILE* fh;
//////////// DATA SECTION /////////////////////////////////////
// faster range
static U8 rangetab[128+256+128]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
	0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,

	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

#define rangetab2 (rangetab+256)
//////////// pixel stuff /////////////////////////////////////////////////////////////
struct floatbitmap {
	S32 x,y;
	float *data;
};

struct floattga {
	S32 x;
	S32 y;
	struct floatbitmap cols[3];
};

//////////////// huffman stuff //////////////////////////////////////////////////////
struct huffc {
	U8 nbits;
	U8 val;
};

struct huff {
	U8 numcats[16];
	S32 ncodes;
	U8 codes[256]; 
	struct huffc codes8[256];
	struct huffc codes16[65536];  // if not an 8 bit code
};

// needs special case of ff00 -> ff
static S32 basepos;
static S32 bitpos;

// jpeg reader /// jpeg types ////////////////////////////////////////////////////////
//static U8* colstr[6]={""," Y","Cb","Cr"," I"," Q"};
enum CID {NONE,Y,CB,CR,I,Q};

//static U8* dcac[2]={"DC","AC"};
enum DCAC {DC,AC};

struct component {
	S32 cid; // colorid
	S32 sfx,sfy; // sample factor
	S32 qtn; // quant table #
};

struct soscomponent {
	S32 cid; // colorid
	S32 dc; // dc
	S32 ac; // ac
//	S32 acusedc; // no ac table found, use dc for ac, .guess.
};

struct jpeginfo {
	// sizeof file
	S32 filesize;
	// data after sos
	U8* data;
	S32 datasize;
	// dimensions
	S32 width,height;
	// quant tables
	S32 hasqt[4];
	float qt[4][64];
	// huff tables
	S32 hashuff[2][4];
	struct huff huffs[2][4];
	// components always 1or3, merge
	S32 ncomps,nsoscomps;
	struct component comps[3];
	struct soscomponent soscomps[3];
	// rst interval
	S32 interval,curinterval,done;
	U32 intval;
	struct floattga curmcu;
	float curdu[8][8];
};
static struct jpeginfo ajpeginfo;


static void sof0(S32),dht(S32),sos(S32),dqt(S32),dri(S32),com(S32);
struct jpegtype {
	U8 haslen;
	U8 iserror;
	C8* name;
	void (*func)(S32 len);
};

static struct jpegtype jpegtypes[256]={
// 00-0f
	{1,0,"<00>",0},
	{0,0,"*TEM  <01>",0}, // usually causes a decoding error, may be ignored
	{1,0,"<02>",0},
	{1,0,"<03>",0},
	{1,0,"<04>",0},
	{1,0,"<05>",0},
	{1,0,"<06>",0},
	{1,0,"<07>",0},
	{1,0,"<08>",0},
	{1,0,"<09>",0},
	{1,0,"<0a>",0},
	{1,0,"<0b>",0},
	{1,0,"<0c>",0},
	{1,0,"<0d>",0},
	{1,0,"<0e>",0},
	{1,0,"<0f>",0},
// 10-1f
	{1,0,"<10>",0},
	{1,0,"<11>",0},
	{1,0,"<12>",0},
	{1,0,"<13>",0},
	{1,0,"<14>",0},
	{1,0,"<15>",0},
	{1,0,"<16>",0},
	{1,0,"<17>",0},
	{1,0,"<18>",0},
	{1,0,"<19>",0},
	{1,0,"<1a>",0},
	{1,0,"<1b>",0},
	{1,0,"<1c>",0},
	{1,0,"<1d>",0},
	{1,0,"<1e>",0},
	{1,0,"<1f>",0},
// 20-2f
	{1,0,"<20>",0},
	{1,0,"<21>",0},
	{1,0,"<22>",0},
	{1,0,"<23>",0},
	{1,0,"<24>",0},
	{1,0,"<25>",0},
	{1,0,"<26>",0},
	{1,0,"<27>",0},
	{1,0,"<28>",0},
	{1,0,"<29>",0},
	{1,0,"<2a>",0},
	{1,0,"<2b>",0},
	{1,0,"<2c>",0},
	{1,0,"<2d>",0},
	{1,0,"<2e>",0},
	{1,0,"<2f>",0},
// 30-3f
	{1,0,"<30>",0},
	{1,0,"<31>",0},
	{1,0,"<32>",0},
	{1,0,"<33>",0},
	{1,0,"<34>",0},
	{1,0,"<35>",0},
	{1,0,"<36>",0},
	{1,0,"<37>",0},
	{1,0,"<38>",0},
	{1,0,"<39>",0},
	{1,0,"<3a>",0},
	{1,0,"<3b>",0},
	{1,0,"<3c>",0},
	{1,0,"<3d>",0},
	{1,0,"<3e>",0},
	{1,0,"<3f>",0},
// 40-4f
	{1,0,"<40>",0},
	{1,0,"<41>",0},
	{1,0,"<42>",0},
	{1,0,"<43>",0},
	{1,0,"<44>",0},
	{1,0,"<45>",0},
	{1,0,"<46>",0},
	{1,0,"<47>",0},
	{1,0,"<48>",0},
	{1,0,"<49>",0},
	{1,0,"<4a>",0},
	{1,0,"<4b>",0},
	{1,0,"<4c>",0},
	{1,0,"<4d>",0},
	{1,0,"<4e>",0},
	{1,0,"<4f>",0},
// 50-5f
	{1,0,"<50>",0},
	{1,0,"<51>",0},
	{1,0,"<52>",0},
	{1,0,"<53>",0},
	{1,0,"<54>",0},
	{1,0,"<55>",0},
	{1,0,"<56>",0},
	{1,0,"<57>",0},
	{1,0,"<58>",0},
	{1,0,"<59>",0},
	{1,0,"<5a>",0},
	{1,0,"<5b>",0},
	{1,0,"<5c>",0},
	{1,0,"<5d>",0},
	{1,0,"<5e>",0},
	{1,0,"<5f>",0},
// 60-6f
	{1,0,"<60>",0},
	{1,0,"<61>",0},
	{1,0,"<62>",0},
	{1,0,"<63>",0},
	{1,0,"<64>",0},
	{1,0,"<65>",0},
	{1,0,"<66>",0},
	{1,0,"<67>",0},
	{1,0,"<68>",0},
	{1,0,"<69>",0},
	{1,0,"<6a>",0},
	{1,0,"<6b>",0},
	{1,0,"<6c>",0},
	{1,0,"<6d>",0},
	{1,0,"<6e>",0},
	{1,0,"<6f>",0},
// 70-7f
	{1,0,"<70>",0},
	{1,0,"<71>",0},
	{1,0,"<72>",0},
	{1,0,"<73>",0},
	{1,0,"<74>",0},
	{1,0,"<75>",0},
	{1,0,"<76>",0},
	{1,0,"<77>",0},
	{1,0,"<78>",0},
	{1,0,"<79>",0},
	{1,0,"<7a>",0},
	{1,0,"<7b>",0},
	{1,0,"<7c>",0},
	{1,0,"<7d>",0},
	{1,0,"<7e>",0},
	{1,0,"<7f>",0},
// 80-8f
	{1,0,"<80>",0},
	{1,0,"<81>",0},
	{1,0,"<82>",0},
	{1,0,"<83>",0},
	{1,0,"<84>",0},
	{1,0,"<85>",0},
	{1,0,"<86>",0},
	{1,0,"<87>",0},
	{1,0,"<88>",0},
	{1,0,"<89>",0},
	{1,0,"<8a>",0},
	{1,0,"<8b>",0},
	{1,0,"<8c>",0},
	{1,0,"<8d>",0},
	{1,0,"<8e>",0},
	{1,0,"<8f>",0},
// 90-9f
	{1,0,"<90>",0},
	{1,0,"<91>",0},
	{1,0,"<92>",0},
	{1,0,"<93>",0},
	{1,0,"<94>",0},
	{1,0,"<95>",0},
	{1,0,"<96>",0},
	{1,0,"<97>",0},
	{1,0,"<98>",0},
	{1,0,"<99>",0},
	{1,0,"<9a>",0},
	{1,0,"<9b>",0},
	{1,0,"<9c>",0},
	{1,0,"<9d>",0},
	{1,0,"<9e>",0},
	{1,0,"<9f>",0},
// a0-af
	{1,0,"<a0>",0},
	{1,0,"<a1>",0},
	{1,0,"<a2>",0},
	{1,0,"<a3>",0},
	{1,0,"<a4>",0},
	{1,0,"<a5>",0},
	{1,0,"<a6>",0},
	{1,0,"<a7>",0},
	{1,0,"<a8>",0},
	{1,0,"<a9>",0},
	{1,0,"<aa>",0},
	{1,0,"<ab>",0},
	{1,0,"<ac>",0},
	{1,0,"<ad>",0},
	{1,0,"<ae>",0},
	{1,0,"<af>",0},
// b0-bf
	{1,0,"<b0>",0},
	{1,0,"<b1>",0},
	{1,0,"<b2>",0},
	{1,0,"<b3>",0},
	{1,0,"<b4>",0},
	{1,0,"<b5>",0},
	{1,0,"<b6>",0},
	{1,0,"<b7>",0},
	{1,0,"<b8>",0},
	{1,0,"<b9>",0},
	{1,0,"<ba>",0},
	{1,0,"<bb>",0},
	{1,0,"<bc>",0},
	{1,0,"<bd>",0},
	{1,0,"<be>",0},
	{1,0,"<bf>",0},
// c0-cf
	{1,0,"SOF0  <c0>",sof0}, // Start Of Frame (baseline JPEG), for details see below
	{1,0,"SOF1  <c1>",sof0}, // dito
	{1,0,"SOF2  <c2>",sof0}, // usually unsupported
	{1,0,"SOF3  <c3>",0}, // usually unsupported
	{1,0,"DHT   <c4>",dht}, // Define Huffman Table, for details see below
	{1,0,"SOF5  <c5>",0}, // usually unsupported
	{1,0,"SOF6  <c6>",0}, // usually unsupported
	{1,0,"SOF7  <c7>",0}, // usually unsupported
	{1,1,"JPG   <c8>",0}, // undefined/reserved (causes decoding error)
	{1,1,"SOF9  <c9>",0}, // for arithmetic coding, usually unsupported
	{1,0,"SOF10 <ca>",0}, // usually unsupported
	{1,0,"SOF11 <cb>",0}, // usually unsupported
	{1,1,"DAC   <cc>",0}, // Define Arithmetic Table, usually unsupported
	{1,0,"SOF13 <cd>",0}, // usually unsupported
	{1,0,"SOF14 <ce>",0}, // usually unsupported
	{1,0,"SOF15 <cf>",0}, // usually unsupported
// d0-df
	{0,0,"*RST0 <d0>",0}, // RSTn are used for resync, may be ignored
	{0,0,"*RST1 <d1>",0},
	{0,0,"*RST2 <d2>",0},
	{0,0,"*RST3 <d3>",0},
	{0,0,"*RST4 <d4>",0},
	{0,0,"*RST5 <d5>",0},
	{0,0,"*RST6 <d6>",0},
	{0,0,"*RST7 <d7>",0},
	{1,1,"SOI   <d8>",0}, // Start Of Image
	{1,0,"EOI   <d9>",0}, // End Of Image
	{1,0,"SOS   <da>",sos}, // Start Of Scan, for details see below
	{1,0,"DQT   <db>",dqt}, // Define Quantization Table, for details see below
	{1,0,"DNL   <dc>",0}, // usually unsupported, ignore
	{1,0,"DRI   <dd>",dri}, // Define Restart Interval, for details see below
	{1,0,"DHP   <de>",0}, // ignore (skip)
	{1,0,"EXP   <df>",0}, // ignore (skip)
// e0-ef
//	{1,0,"APP0  <e0>",app0}, // JFIF APP0 segment marker, for details see below
	{1,0,"APP0  <e0>",0}, // JFIF APP0 segment marker, for details see below
	{1,0,"<e1>",0},
	{1,0,"<e2>",0},
	{1,0,"<e3>",0},
	{1,0,"<e4>",0},
	{1,0,"<e5>",0},
	{1,0,"<e6>",0},
	{1,0,"<e7>",0},
	{1,0,"<e8>",0},
	{1,0,"<e9>",0},
	{1,0,"<ea>",0},
	{1,0,"<eb>",0},
	{1,0,"<ec>",0},
	{1,0,"<ed>",0},
	{1,0,"<ee>",0},
	{1,0,"APP15 <ef>",0}, // ignore
// f0-ff
	{1,0,"JPG0  <f0>",0}, // ignore (skip)
	{1,0,"<f1>",0},
	{1,0,"<f2>",0},
	{1,0,"<f3>",0},
	{1,0,"<f4>",0},
	{1,0,"<f5>",0},
	{1,0,"<f6>",0},
	{1,0,"<f7>",0},
	{1,0,"<f8>",0},
	{1,0,"<f9>",0},
	{1,0,"<fa>",0},
	{1,0,"<fb>",0},
	{1,0,"<fc>",0},
	{1,0,"JPG13 <fd>",0}, // ignore (skip)
	{1,0,"COM   <fe>",com}, // Comment, for details see below
	{1,0,"<ff>",0}
};

/*
*TEM   = $01   usually causes a decoding error, may be ignored
SOF0  = $c0   Start Of Frame (baseline JPEG), for details see below
SOF1  = $c1   dito
SOF2  = $c2   usually unsupported
SOF3  = $c3   usually unsupported
DHT   = $c4   Define Huffman Table, for details see below
SOF5  = $c5   usually unsupported
SOF6  = $c6   usually unsupported
SOF7  = $c7   usually unsupported
JPG   = $c8   undefined/reserved (causes decoding error)
SOF9  = $c9   for arithmetic coding, usually unsupported
SOF10 = $ca   usually unsupported
SOF11 = $cb   usually unsupported
DAC   = $cc   Define Arithmetic Table, usually unsupported
SOF13 = $cd   usually unsupported
SOF14 = $ce   usually unsupported
SOF14 = $ce   usually unsupported
SOF15 = $cf   usually unsupported
*RST0  = $d0   RSTn are used for resync, may be ignored
*RST1  = $d1
*RST2  = $d2
*RST3  = $d3
*RST4  = $d4
*RST5  = $d5
*RST6  = $d6
*RST7  = $d7
SOI   = $d8   Start Of Image
EOI   = $d9   End Of Image
SOS   = $da   Start Of Scan, for details see below
DQT   = $db   Define Quantization Table, for details see below
DNL   = $dc   usually unsupported, ignore
DRI   = $dd   Define Restart Interval, for details see below
DHP   = $de   ignore (skip)
EXP   = $df   ignore (skip)
APP0  = $e0   JFIF APP0 segment marker, for details see below
APP15 = $ef   ignore
JPG0  = $f0   ignore (skip)
JPG13 = $fd   ignore (skip)
COM   = $fe   Comment, for details see below
*/

/*
// do something
SOF0  = $c0   Start Of Frame (baseline JPEG), for details see below
SOF1  = $c1   dito
DHT   = $c4   Define Huffman Table, for details see below
EOI   = $d9   End Of Image
SOS   = $da   Start Of Scan, for details see below
DQT   = $db   Define Quantization Table, for details see below
DRI   = $dd   Define Restart Interval, for details see below
APP0  = $e0   JFIF APP0 segment marker, for details see below
COM   = $fe   Comment, for details see below
// error
SOI   = $d8   Start Of Image, should already be processed 
SOF9  = $c9   for arithmetic coding, usually unsupported
JPG   = $c8   undefined/reserved (causes decoding error)
DAC   = $cc   Define Arithmetic Table, usually unsupported
// ignore
SOF2  = $c2   usually unsupported
SOF3  = $c3   usually unsupported
SOF5  = $c5   usually unsupported
SOF6  = $c6   usually unsupported
SOF7  = $c7   usually unsupported
SOF10 = $ca   usually unsupported
SOF11 = $cb   usually unsupported
SOF13 = $cd   usually unsupported
SOF14 = $ce   usually unsupported
SOF15 = $cf   usually unsupported
DNL   = $dc   usually unsupported, ignore
DHP   = $de   ignore (skip)
EXP   = $df   ignore (skip)
APP15 = $ef   ignore
JPG0  = $f0   ignore (skip)
JPG13 = $fd   ignore (skip)
// parameterless
*TEM   = $01   usually causes a decoding error, may be ignored
*RST0  = $d0   RSTn are used for resync, may be ignored
*RST1  = $d1
*RST2  = $d2
*RST3  = $d3
*RST4  = $d4
*RST5  = $d5
*RST6  = $d6
*RST7  = $d7
*/

static S32 zigzag[64]={
	 0,  1,  8, 16,  9,  2,  3, 10,		 
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63, 
};
static S32 dcval[3],dcdiff,acval;
/////////////////////////////////// dct stuff /////////////////////////////
//#define LNMAX 3
#define ORDER 8
#define TRIGMASK (ORDER*4-1)

//static struct complex tempfft[NMAX];
#ifndef OPTIMIZED
static float tempdct[ORDER];
static float trigtable[ORDER*4]; // *4 for dct's
//static struct complex trigtable2[NMAX*4]; // for dct's
//static struct complex dctfft[NMAX*2];
static float dctdata[ORDER];

//static S32 trigmask;
//static S32 numelements;
//static S32 lognumelements;
static float c0,cn;
#endif
////////////////// CODE SECTION //////////////////////////////////////////////////
//////////// pixel stuff /////////////////////////////////////////////////////////////
/*static float float_clipgetpixel(struct floatbitmap *fb,S32 x,S32 y)
{
	return fb->data[fb->x*y+x];
}

static void float_clipputpixel(struct floatbitmap *fb,S32 x,S32 y,float v)
{
	fb->data[fb->x*y+x]=v;
}
*/
static void float_clear(struct floatbitmap *fb)
{
	memset(fb->data,0,sizeof(float)*fb->x*fb->y);
}

/*static void tga_float_clear(struct floattga *fb)
{
	float_clear(&fb->cols[0]);
	float_clear(&fb->cols[1]);
	float_clear(&fb->cols[2]);
}
*/
static void float_alloc_bitmap(struct floatbitmap *fb,S32 x,S32 y)
{
	fb->x=x;
	fb->y=y;
//	fb->data=(float*)memalloc(sizeof(float)*fb->x*fb->y);
	fb->data=new float[fb->x*fb->y];
	float_clear(fb);
}

static void tga_float_alloc_bitmap(struct floattga *ft,S32 x,S32 y)
{
	ft->x=x;
	ft->y=y;
	float_alloc_bitmap(&ft->cols[0],x,y);
	float_alloc_bitmap(&ft->cols[1],x,y);
	float_alloc_bitmap(&ft->cols[2],x,y);
}

static void float_free_bitmap(struct floatbitmap *fb)
{
//	memfree(fb->data);
	delete[] fb->data;
	fb->data=0;
}

static void tga_float_free_bitmap(struct floattga *ft)
{
	float_free_bitmap(&ft->cols[0]);
	float_free_bitmap(&ft->cols[1]);
	float_free_bitmap(&ft->cols[2]);
}

//////////////// huffman stuff //////////////////////////////////////////////////////
static void putnbits(S32 nbits)
{
	bitpos-=nbits;
}

// get 8 or 16 bits for fast huff lookup
static U32 get8or16bits(S32 nbits)
{
	U32 val;
	U32 b1,b2,b1p,b2p;
	b1=bitpos/8;
	b1p=bitpos&7;
	bitpos+=nbits;
	b2=(bitpos-1)/8;
	b2p=(bitpos-1)&7;
	if (b1p==0) { // byte aligned
		if (nbits==8) {
			val=ajpeginfo.data[b1];
		} else { // world aligned
			val=ajpeginfo.data[b1];
			val<<=8;
			val+=ajpeginfo.data[b1+1];
		}
	} else { // not byte aligned
		if (nbits==8) {
			val=ajpeginfo.data[b1];
			val<<=8;
			val+=ajpeginfo.data[b1+1];
			val>>=(8-b1p);
			val&=0xff;
		} else { // not word aligned
			val=ajpeginfo.data[b1];
			val<<=8;
			val+=ajpeginfo.data[b1+1];
			val<<=8;
			val+=ajpeginfo.data[b1+2];
			val>>=(8-b1p);
			val&=0xffff;
		}
	}
	return val;
}

static U32 getnbits(S32 nbits)
{
	U32 val;
	if (nbits==0) {
		val=0;
	} else if (nbits==8) {
		val=get8or16bits(8);
	} else if (nbits==16) {
		val=get8or16bits(16);
	} else if (nbits<8) {
		val=get8or16bits(8);
		putnbits(8-nbits);
		val>>=(8-nbits);
	} else {
		val=get8or16bits(16);
		putnbits(16-nbits);
		val>>=(16-nbits);
	}
//	ricklogger("getnbits %s",printbits(val,nbits));
	return val;
}

static S32 makeval(U32 bits,S32 cat)
{
	U32 bm,p;
	if (cat==0)
		return 0;
	bm=1<<(cat-1);
	p=bm<<1;
	if (bm&bits)
		return bits;
	return bits-p+1;
}

/*static U8* printbits(U32 val,S32 nbits)
{
	static U8 outstr[256];
	S32 i;
	U8* s;
	S32 b=1<<(nbits-1);
	s=outstr;
	*s++='\'';
	for (i=0;i<nbits;i++,b>>=1)
		if (b&val)
			*s++='1';
		else
			*s++='0';
	*s++='\'';
	*s++='\0';
	return outstr;
}
*/
static U32 gethuffbits(struct huff *hp)
{
	U32 hbits,nbits;
	U32 ucode;
//	U8 str[256];
	perf_start(JPEG6);
	hbits=get8or16bits(8);
	nbits=hp->codes8[hbits].nbits;
	if (nbits) {
		ucode=hp->codes8[hbits].val;
		putnbits(8-nbits);
//		ricklogger("huffbits %s",printbits(hbits>>(8-nbits),nbits));
	} else {
		putnbits(8);
		hbits=get8or16bits(16);
		nbits=hp->codes16[hbits].nbits;
		if (!nbits) {
//			sprintf(str,"no bits %s offset %d\n",printbits(hbits,16),bitpos/8);
//			logger(str);
			errorexit("jpeg error : gethuffbits");
		}
		ucode=hp->codes16[hbits].val;
		putnbits(16-nbits);
//		ricklogger("huffbits %s",printbits(hbits>>(16-nbits),nbits));
	}
	perf_end(JPEG6);
	return ucode;
}

static void openbits()
{
	bitpos=basepos=0;
}

// put file pointer right after last bit read
static void closebits()
{
}

static void alignbits()
{
	U32 val;
	U32 b1,b1p;
	b1=bitpos/8;
	b1p=bitpos&7;
	if (!b1p)
		return; // already aligned
	val=ajpeginfo.data[b1];
//	ricklogger("alignbits %s",printbits(val,8-b1p));
	bitpos+=(8-b1p);
}

// jpeg reader /// jpeg types 6 functions ////////////////////////////////////////////////////////
static void sof0(S32 len)
{
	S32 i;
//	U8 str[256];
	U8 bitssample;
//	U8 ncomponents;
//	U8 cid,sf,qtn;
	U8 sf;
	bitssample=filereadU8(fh); // 8,12,16
	if (bitssample!=8)
		errorexit("jpeg error : sample not 8 : sof0");
	ajpeginfo.height=filereadU16BE(fh); // >0 if dnl not supported
	ajpeginfo.width=filereadU16BE(fh); // >0 if dnl not supported
	ajpeginfo.ncomps=filereadU8(fh); // 1 gray, 3 color YCbCr or YIQ, 4 CMYK
	if (len<6+ajpeginfo.ncomps*3)
		errorexit("jpeg error : bad len : sof0");
//	logger("in sof0 len %d, bitssample %d, width %d, height %d, ncomponents %d\n",
//		len,bitssample,ajpeginfo.width,ajpeginfo.height,ajpeginfo.ncomps);
	for (i=0;i<ajpeginfo.ncomps;i++) {
		ajpeginfo.comps[i].cid=filereadU8(fh);
		sf=filereadU8(fh);
		ajpeginfo.comps[i].sfx=sf>>4;
		ajpeginfo.comps[i].sfy=sf&0xf;
		ajpeginfo.comps[i].qtn=filereadU8(fh);
/*		sprintf(str,"comp id %s, samp horz %d, vert %d, qt# %d\n",
			colstr[ajpeginfo.comps[i].cid],
			ajpeginfo.comps[i].sfx,
			ajpeginfo.comps[i].sfy,
			ajpeginfo.comps[i].qtn);
		logger(str); */
	}
}
static void dht(S32 len)
{
	struct huff *hp;
//	U8* hstr[2]={"DC","AC"};
	U8 hinfo,nht,htype;
	U32 hcode,sk;
//	U8 sym16[16],codes[256];
	S32 i,j,k;
//	U8 outstr[256],buildstr[64];
	perf_start(JPEG9);
	if (len<17)
		errorexit("jpeg error : bad len : dht");
	while (len>17) {
		hinfo=filereadU8(fh);
		nht=hinfo&0xf;
		htype=hinfo>>4;
		if (htype>1 || nht>3)
			errorexit("jpeg error : bad hinfo : dht");
		if (ajpeginfo.hashuff[htype][nht]==1)
			errorexit("jpeg error : already have a huff table : dht");
		ajpeginfo.hashuff[htype][nht]=1;
		perf_start(JPEG2);
//		memset(&ajpeginfo.huffs[htype][nht],0,sizeof(ajpeginfo.huffs[0][0]));
//		memset(&ajpeginfo.huffs[htype][nht].codes,0,sizeof(ajpeginfo.huffs[0][0].codes));
		memset(&ajpeginfo.huffs[htype][nht].codes8,0,sizeof(ajpeginfo.huffs[0][0].codes8));
//		memset(&ajpeginfo.huffs[htype][nht].codes16,0xaa,sizeof(ajpeginfo.huffs[0][0].codes16));
		perf_end(JPEG2);
		hp=&ajpeginfo.huffs[htype][nht];
		fileread(fh,hp->numcats,16);
/*		logger("in dht len %d, acdc %s, ht# %d\n",len,hstr[htype],nht);
		strcpy(outstr,"num bits   ");
		for (i=0;i<16;i++) {
			sprintf(buildstr," %3d",i+1);
			strcat(outstr,buildstr);
		}
		logger(outstr);
		strcpy(outstr,"\nnumsymbols ");
*/
		hp->ncodes=0;
		for (i=0;i<16;i++) {
			hp->ncodes+=hp->numcats[i];
//			sprintf(buildstr," %3d",hp->numcats[i]);
//			strcat(outstr,buildstr);
		}
//		logger("%s\n",outstr);
		fileread(fh,hp->codes,hp->ncodes);
//		outstr[0]='\0';
//		logger("ncodes = %d\n",hp->ncodes);
/*		for (i=0;i<hp->ncodes;i++) {
			sprintf(buildstr,"%02x ",hp->codes[i]);
			strcat(outstr,buildstr);
			if ((i&15)==15) {
//				ricklogger(outstr);
				outstr[0]='\0';
			}
		}
//		if ((i&15)!=15)
//			ricklogger(outstr);
*/
		k=hcode=0;
		for (i=0;i<16;i++) {
			for (j=0;j<hp->numcats[i];j++) {
				S32 nbits=i+1;
				S32 start=hcode;
				S32 end=hcode+1;
				if (nbits<=8) {
					start<<=8-nbits;
					end<<=8-nbits;
					for (sk=start;(S32)sk<end;sk++) {
						hp->codes8[sk].nbits=i+1;
						hp->codes8[sk].val=hp->codes[k];
					}
				} else {
					start<<=16-nbits;
					end<<=16-nbits;
					for (sk=start;(S32)sk<end;sk++) {
						hp->codes16[sk].nbits=i+1;
						hp->codes16[sk].val=hp->codes[k];
					}
				}
//				ricklogger("bits %2d, code %02x, huff %s",
//					i+1,hp->codes[k],printbits(hcode,i+1));
				k++;
				hcode++;
			}
			hcode<<=1;
		}
		len-=hp->ncodes;
		len-=16;
	}
	perf_end(JPEG9);
}

/*  - $ff, $da (SOS)
  - length (high byte, low byte), must be 6+2*(number of components in scan)
  - number of components in scan (1 byte), must be >= 1 and <=4 (otherwise
    error), usually 1 or 3
  - for each component: 2 bytes
     - component id (1 = Y, 2 = Cb, 3 = Cr, 4 = I, 5 = Q), see SOF0
     - Huffman table to use:
	- bit 0..3: AC table (0..3)
	- bit 4..7: DC table (0..3)
  - 3 bytes to be ignored (???)
  */

// start of scan, get info on which huffman table to use for each component ac and dc
static void sos(S32 len)
{
	S32 i;
//	U8 str[80];
	U8 sss,sse,suc,suca,sucb;
	U8 acdc; // ,ac,dc,cid;
	ajpeginfo.nsoscomps=filereadU8(fh);
//	logger("in sos len %d noc\n",len,ajpeginfo.nsoscomps);
	for (i=0;i<ajpeginfo.nsoscomps;i++) {
		ajpeginfo.soscomps[i].cid=filereadU8(fh);
		acdc=filereadU8(fh);
		ajpeginfo.soscomps[i].ac=acdc&0xf;
		ajpeginfo.soscomps[i].dc=acdc>>4;
//		logger("comp # %d, '%s',ac # %d,dc # %d\n",
//			i,colstr[ajpeginfo.soscomps[i].cid],ajpeginfo.soscomps[i].ac,ajpeginfo.soscomps[i].dc);
	}
//	for (i=0;i<100;i++)
//		ricklogger("some bytes %02x",filereadU8(fh));
	sss=filereadU8(fh);
	sse=filereadU8(fh);
	suc=filereadU8(fh);
	suca=suc>>4;
	sucb=suc&0xf;
//	logger("sss %d, sse %d, suca %d, sucb %d\n",sss,sse,suca,sucb);
	if (sss!=0 || sse!=63)
		errorexit("jpeg error : can't handle progressive : sos '%s'",fnamec);
}

#ifdef OPTIMIZED
static void makeqt2(float *qt,float *qt2);
#endif
static void dqt(S32 len)
{
//	U8 outstr[256],buildstr[64];
	S32 i,j,v;
	U8 infobyte,nqt,prec,prec2;
	while(len>32) {
		infobyte=filereadU8(fh);
		nqt=infobyte&0xf;
		prec=infobyte>>4;
		if (prec)
			prec2=2;
		else
			prec2=1;
		if (nqt>3)
			errorexit("jpeg error : nqt > 3 : dqt");
		if (prec2*64+1>len)
			errorexit("jpeg error : bad len : dqt");
//		logger("in dqt size %d, infobyte $%02x, prec nibble %d, precision %d, qt# %d\n",
//			len,infobyte,prec,prec2,nqt);
		if (ajpeginfo.hasqt[nqt]==1)
			errorexit("jpeg : already have a qt : dqt");
		ajpeginfo.hasqt[nqt]=1;
		if (prec2==1)
			for (j=0;j<8;j++) {
//				outstr[0]='\0';
				for (i=0;i<8;i++) {
//					sprintf(buildstr," %6d",
					v=filereadU8(fh);
					ajpeginfo.qt[nqt][zigzag[i+j*8]]=(float)v;
//					strcat(outstr,buildstr);
				}
//				ricklogger(outstr);
			}
		else
			for (j=0;j<8;j++) {
//				outstr[0]='\0';
				for (i=0;i<8;i++) {
//					sprintf(buildstr," %6d",
					v=filereadU16BE(fh);
					ajpeginfo.qt[nqt][zigzag[i+j*8]]=(float)v;
//					strcat(outstr,buildstr);
				}
//				ricklogger(outstr);
			}
		len-=prec2*64;
#ifdef OPTIMIZED
		makeqt2(ajpeginfo.qt[nqt],ajpeginfo.qt[nqt]);
#endif
	}
}

// get restart interval
static void dri(S32 len)
{
	ajpeginfo.interval=filereadU16BE(fh);
//	logger("in dri len %d, interval %d\n",len,ajpeginfo.interval);
}

static void com(S32 len)
{
	static U8 comstr[512];
	U8* s=comstr;
	fileread(fh,comstr,len);
	comstr[len]='\0';
	while(*s) {
		if (*s<0x20 || *s>0x7f)
			*s='.';
		s++;
	}
//	ricklogger("in com len %d '%s'",len,comstr);
}

/////////////////////////////////// dct stuff /////////////////////////////
#ifdef OPTIMIZED
static float sqrt2,sqrt2o2;
static float scaleqt[8];
static float param1,param2;

static void initdct()
{
	S32 i;
	static S32 inited;
	if (inited)
		return;
	sqrt2=(float)sqrt(2.0f);
	sqrt2o2=sqrt2*.5f;
	param1=(float)(1.0/(2*cos(PI/8)));
	param2=(float)(1.0/(2*cos(3*PI/8)));
	scaleqt[0]=1.0f;
	for (i=1;i<ORDER;i++)
		scaleqt[i]=sqrt2*.5f/(float)(cos((float)i*PI*(1.0/16.0)));
	inited=1;
}



#ifdef OPTIMIZED
static void makeqt2(float *qt,float *qt2)
{
	S32 i,j;
	for (j=0;j<8;j++)
		for (i=0;i<8;i++)
			qt2[8*j+i]=(1.0f/8.0f)*qt[8*j+i]*scaleqt[i]*scaleqt[j];
}

static void idct2a(float *a)
{
//	mulvec8(matnoscale,a,a);
	float m2_2,m2_3,m2_4,m2_5,m2_6,m2_7;
	float m3_4,m3_5,m3_6,m3_7;
	float m4_4,m4_6;
	float m5_0,m5_1,m5_2,m5_4,m5_5;
	float m6_6;
	float m7_3,m7_5,m7_6,m7_7;
	float m8_0,m8_1,m8_2,m8_3;

	m2_2=a[2]-a[6];
	m2_3=a[2]+a[6];
	m2_4=a[1]-a[7];
	m2_5=a[5]+a[3];
	m2_6=a[5]-a[3];
	m2_7=a[1]+a[7];

	m3_4=m2_4*param1;
	m3_5=m2_7-m2_5;
	m3_6=m2_6*param2;
	m3_7=m2_7+m2_5;

	m4_4=m3_4+m3_6;
	m4_6=m3_4-m3_6;

	m5_0=a[0]+a[4];
	m5_1=a[0]-a[4];
	m5_2=m2_2*sqrt2o2;
	m5_4=m4_4*sqrt2o2;
	m5_5=m3_5*sqrt2o2;

	m6_6=m5_4+m4_6;

	m7_3=m5_2+m2_3;
	m7_5=m5_4+m5_5;
	m7_6=m5_5+m6_6;
	m7_7=m6_6+m3_7;

	m8_0=m5_0+m7_3;
	m8_1=m5_1+m5_2;
	m8_2=m5_1-m5_2;
	m8_3=m5_0-m7_3;

	a[0]=m8_0+m7_7;
	a[1]=m8_1+m7_6;
	a[2]=m8_2+m7_5;
	a[3]=m8_3+m5_4;
	a[4]=m8_3-m5_4;
	a[5]=m8_2-m7_5;
	a[6]=m8_1-m7_6;
	a[7]=m8_0-m7_7;
}

static void idct2b(float *a)
{
//	mulvec8(matnoscale,a,a);
	float m2_2,m2_3,m2_4,m2_5,m2_6,m2_7;
	float m3_4,m3_5,m3_6,m3_7;
	float m4_4,m4_6;
	float m5_0,m5_1,m5_2,m5_4,m5_5;
	float m6_6;
	float m7_3,m7_5,m7_6,m7_7;
	float m8_0,m8_1,m8_2,m8_3;

	m2_2=a[2*8]-a[6*8];
	m2_3=a[2*8]+a[6*8];
	m2_4=a[1*8]-a[7*8];
	m2_5=a[5*8]+a[3*8];
	m2_6=a[5*8]-a[3*8];
	m2_7=a[1*8]+a[7*8];

	m3_4=m2_4*param1;
	m3_5=m2_7-m2_5;
	m3_6=m2_6*param2;
	m3_7=m2_7+m2_5;

	m4_4=m3_4+m3_6;
	m4_6=m3_4-m3_6;

	m5_0=a[0*8]+a[4*8];
	m5_1=a[0*8]-a[4*8];
	m5_2=m2_2*sqrt2o2;
	m5_4=m4_4*sqrt2o2;
	m5_5=m3_5*sqrt2o2;

	m6_6=m5_4+m4_6;

	m7_3=m5_2+m2_3;
	m7_5=m5_4+m5_5;
	m7_6=m5_5+m6_6;
	m7_7=m6_6+m3_7;

	m8_0=m5_0+m7_3;
	m8_1=m5_1+m5_2;
	m8_2=m5_1-m5_2;
	m8_3=m5_0-m7_3;

	a[0*8]=m8_0+m7_7;
	a[1*8]=m8_1+m7_6;
	a[2*8]=m8_2+m7_5;
	a[3*8]=m8_3+m5_4;
	a[4*8]=m8_3-m5_4;
	a[5*8]=m8_2-m7_5;
	a[6*8]=m8_1-m7_6;
	a[7*8]=m8_0-m7_7;
}

static void idct2d(float *f,float *qt) 
{
	S32 i,j;
	perf_start(JPEG7);
	for (j=0;j<64;j++)
		f[j]*=(float)qt[j];
// first the x
	for (j=0;j<64;j+=8)
		idct2a(&f[j]);	
// now the y
	for (i=0;i<8;i++)
		idct2b(&f[i]);	
	perf_end(JPEG7);
}
#endif
//////////////////////////////////
#else
static void initdct()
{
	float *trigptr;
	S32 i;
//	if (!(lognumelements=checkpower(order)))
//		errorexit("bad order %d, must be a power of 2",order);
//	numelements=order;
//	trigmask=4*numelements-1;
	for (i=0,trigptr=trigtable ; i<4*ORDER ; i++) {
		trigptr[i]=(float)cos((PI/2)*i/ORDER);
	}
	c0=(float)sqrt(1.0/ORDER);
	cn=(float)sqrt(2.0/ORDER);
}

static void idct(float data[])
{
	S32 t,f;
	float a;
	for (t=0;t<8;t++) {
		tempdct[t]=0.0;
		a=c0;
		for (f=0;f<8;f++) {
			tempdct[t]+=a*data[f]*trigtable[TRIGMASK&(f*(2*t+1))];
			a=cn;
		}
	}
	for (t=0;t<8;t++)
		data[t]=tempdct[t];
}

static void idct2d(float *f,float *qt)
{
	S32 i,j;
//	float v;

// first the x
	perf_start(JPEG7);
	for (j=0;j<64;j++)
		f[j]*=(float)qt[j];
	for (j=0;j<64;j+=8)
		idct(&f[j]);	
// now the y
	for (i=0;i<8;i++) {
		for (j=0;j<8;j++)
			dctdata[j]=f[(j<<3)+i];
		idct(dctdata);	
		for (j=0;j<8;j++)
			f[(j<<3)+i]=dctdata[j];
	} 
	perf_end(JPEG7);
}
#endif

////////////////////////////////////////////////////////////////////////////

static void procdu(S32 cc)
{
	S32 i;//,j;
//	S32 nb;
	struct huff *achp,*dchp;
	float *qt;
//	U32 hbits,nbits,val;
	U8 ucode,skip0;
	U32 val;

	perf_start(JPEG3);
	qt=ajpeginfo.qt[ajpeginfo.comps[cc].qtn];
	dchp=&ajpeginfo.huffs[DC][ajpeginfo.soscomps[cc].dc];
//	if (ajpeginfo.hashuff[AC][ajpeginfo.soscomps[cc].ac])
		achp=&ajpeginfo.huffs[AC][ajpeginfo.soscomps[cc].ac];
//	else
//		achp=dchp;

	i=0;
//	ricklogger("start dcu --------- offset %d, bit %d",bitpos/8,bitpos&7);
	ucode=gethuffbits(dchp);
//	ricklogger("DC code %d",ucode);
	val=getnbits(ucode);
	dcdiff=makeval(val,ucode);
//	if (ucode==9)
//		ricklogger("9bits code %04x, val %d",val,dcdiff);
	dcval[cc]+=dcdiff;
//	ajpeginfo.curdu[0][0]=0;
	ajpeginfo.curdu[0][0]=(float)(dcval[cc]);
//	ricklogger("DC diff %d, val %d",dcdiff,dcval[cc]);
	for (i=1;i<64;i++) {
		ucode=gethuffbits(achp);
		skip0=ucode>>4;
		ucode&=0xf;
//		ricklogger("AC rle element at %2d, is skipzero: %01x, nbits %01x",i,skip0,ucode);
		if (ucode==0 && skip0==0)
			break;
		if (ucode==0) { 
			i+=skip0;
			continue;
		}
//		if (ucode==0 && skip0==15) { // fix this
//			i+=15;
//			continue;
//		}
//		if (ucode==0)
//			ErrorExit("jpeg error","ucode 0 and nbits not 0 or 15","procdu");
		i+=skip0;
		val=getnbits(ucode);
		acval=makeval(val,ucode);
//		if (i>0)
			ajpeginfo.curdu[0][zigzag[i]]=(float)(acval);
//		else
//			ajpeginfo.curdu[0][zigzag[i]]=0;
//		ricklogger("AC val %d",acval);
	}
//	idct(testdata2);
//	idct2d(&testdu,&testdu);
	idct2d(ajpeginfo.curdu[0],qt);
	perf_end(JPEG3);

}

/*static void jshowbits()
{
	S32 j,k,el;
	U8 outstr[256];
//	ricklogger(" --- --- --- --- --- --- some bits --- -- ---- --- --- - ---- ");
	el=(ajpeginfo.datasize+7)/8;
	for (k=0;k<el;k++) {
		sprintf(outstr,"offset %6d: ",k*8);
		for (j=0;j<8;j++) {
			if (j+k*8<ajpeginfo.datasize)
				strcat(outstr,printbits(ajpeginfo.data[j+k*8],8));
			strcat(outstr," ");
		}
//		ricklogger(outstr);
	}
}
*/

static void scanffs()
{
	S32 offset=ftell(fh);
//	S32 offset=fileskip(0,FILE_CURRENT);
//	S32 val;
	S32 i;
	S32 oldsize;
	S32 newsize=0;
	U8* sp,*dp;
	oldsize=ajpeginfo.filesize-offset;
//	sp=dp=ajpeginfo.data=(U8*)memalloc(oldsize);
	sp=dp=ajpeginfo.data=new U8[oldsize];
	fileread(fh,ajpeginfo.data,oldsize);
	for (i=0;i<oldsize;i++) {
		if (sp[i]==0xff) {
//			if (sp[i+1]==0xd9)
//				ricklogger("offset %6d: [-1]%02x, %02x %02x",i,sp[i-1],sp[i],sp[i+1]);
//			else
//				ricklogger("offset %6d: [-1]%02x, %02x %02x %02x %02x",i,sp[i-1],sp[i],sp[i+1],sp[i+2],sp[i+3]);
			i++;
			if (sp[i]==0) { // ff 00 -> ff
				dp[newsize++]=0xff;
			} else {
				dp[newsize++]=0xff;
				dp[newsize++]=sp[i];
			} 
		} else {
			dp[newsize++]=sp[i];
		}
	}
	ajpeginfo.datasize=newsize;
}

static void procjpeg(struct bitmap32* rt)	// process jpeg
	{
	static S32 bitshift[9]={0,0,1,0,2,0,0,0,3};
	S32 cc;
	S32 i,j,x,y;
	S32 mcux,mcuy;
	S32 is,js;
	S32 ips,jps;
	S32 isize,jsize;
	U32 hibyte,lobyte;
//	float t[3];
//	struct rgb24 c;
	S32 ipsa[3],jpsa[3],isizea[3],jsizea[3];
	mcux=0;
	mcuy=0;
	dcval[0]=dcval[1]=dcval[2]=0;
	for (i=0;i<ajpeginfo.ncomps;i++) {
		if (ajpeginfo.comps[i].sfx>mcux)
			mcux=ajpeginfo.comps[i].sfx;
		if (ajpeginfo.comps[i].sfy>mcuy)
			mcuy=ajpeginfo.comps[i].sfy;
	}
	// size of mcu in pixels
	mcux*=8;
	mcuy*=8;
	tga_float_alloc_bitmap(&ajpeginfo.curmcu,mcux,mcuy);
//	float_alloc_bitmap(&ajpeginfo.curdu,8,8);
//	ricklogger("mcux %d, mcuy %d",mcux,mcuy);
//	initdct();
	openbits();
	// # of mcu's in image
	for (cc=0;cc<ajpeginfo.ncomps;cc++) {
		ipsa[cc]=mcux/ajpeginfo.comps[cc].sfx;
		jpsa[cc]=mcuy/ajpeginfo.comps[cc].sfy;
		isizea[cc]=bitshift[ipsa[cc]>>3];
		jsizea[cc]=bitshift[jpsa[cc]>>3];
	}
	for (j=0;j<ajpeginfo.height;j+=mcuy) {
//		tga_printconsole(acon,"j %d",j);
//		tga_clipblit(tga_getbitmap(acon),&winglue.tv,0,0,0,0,WX,WY);
//		endframe();
		for (i=0;i<ajpeginfo.width;i+=mcux) {
//	for (j=0;j<1;j+=mcuy) {
//		for (i=0;i<1;i+=mcux) {
			perf_start(JPEG4);
//			tga_float_clear(&ajpeginfo.curmcu);
			for (cc=0;cc<ajpeginfo.ncomps;cc++) {
				ips=ipsa[cc];
				jps=jpsa[cc];
				isize=isizea[cc]; //ips>>3; // pixels/sample
				jsize=jsizea[cc]; //jps>>3;
				for (js=0;js<mcuy;js+=jps)
					for (is=0;is<mcux;is+=ips) {
//						ricklogger("cc %d, i %d, j %d, isize %d, jsize %d, is %d, js %d, ips %d, jps %d",
//							cc,i,j,isize,jsize,is,js,ips,jps);
						if (!ajpeginfo.done) {
							memset(ajpeginfo.curdu[0],0,sizeof(float)*64);
//							float_clear(&ajpeginfo.curdu);
							procdu(cc);
							for (y=0;y<jps;y++) {
								float *dta=&ajpeginfo.curmcu.cols[cc].data[mcux*(y+js)+is];
								float *dtas=&ajpeginfo.curdu[0][((y>>jsize)<<3)];
								if (isize)
									for (x=0;x<ips;x++)
										dta[x]=dtas[(x>>isize)];
								else
									memcpy(dta,dtas,sizeof(float)*ips);
							}
						}
					}
			}
			perf_end(JPEG4);
			perf_start(JPEG8);
			if (cc==3) {
				S32 leny=min(mcuy,ajpeginfo.height-j);
				for (y=0;y<leny;y++) {
					S32 lenx;
					float *cr,*cg,*cb;
//					U8* pr,*pg,*pb;
					C32* p;
					S32 offs=rt->size.x*(y+j)+i;
//					pr=rt->r->plane+offs;
//					pg=rt->g->plane+offs;
//					pb=rt->b->plane+offs;
					p=rt->data+offs;
					lenx=min(mcux,ajpeginfo.width-i);
					cr=&ajpeginfo.curmcu.cols[0].data[mcux*y];
					cg=&ajpeginfo.curmcu.cols[1].data[mcux*y];
					cb=&ajpeginfo.curmcu.cols[2].data[mcux*y];
					for (x=0;x<lenx;x++) {
//						struct rgb24 c;
						float f;
						S32 it[3];
//						t[0]=;
//						t[1]=ajpeginfo.curmcu.cols[1].data[mcux*y+x];
//						t[2]=ajpeginfo.curmcu.cols[2].data[mcux*y+x];
						f=cr[x]+1.402f*cb[x];
						F2INT(f,it[0]);
						f=cr[x]-0.34414f*cg[x]-0.71414f*cb[x];
						F2INT(f,it[1]);
						f=cr[x]+1.772f*cg[x];
						F2INT(f,it[2]);
//						pr[x]=rangetab2[it[0]];
//						pg[x]=rangetab2[it[1]];
//						pb[x]=rangetab2[it[2]]; 
						p[x].r=rangetab2[it[0]];
						p[x].g=rangetab2[it[1]];
						p[x].b=rangetab2[it[2]];
						p[x].a=0xff;
//						pr[x]=(U8)range(0,it[0],255);
//						pg[x]=(U8)range(0,it[1],255);
//						pb[x]=(U8)range(0,it[2],255); 
//						c.r=(U8)range(0,it[0],255);
//						c.g=(U8)range(0,it[1],255);
//						c.b=(U8)range(0,it[2],255);
//						clipputpixel24(rt,x+i,y+j,c);
					}
				}
			} else if (cc==1) { // 8 bit
				S32 leny=min(mcuy,ajpeginfo.height-j);
				for (y=0;y<leny;y++) {
					S32 lenx;
					float *cr;//,*cg,*cb;
//					U8* pr,*pg,*pb;
					C32* p;
					S32 offs=rt->size.x*(y+j)+i;
//					pr=rt->r->plane+offs;
//					pg=rt->g->plane+offs;
//					pb=rt->b->plane+offs;
					p=rt->data+offs;
					lenx=min(mcux,ajpeginfo.width-i);
					cr=&ajpeginfo.curmcu.cols[0].data[mcux*y];
//					cg=&ajpeginfo.curmcu.cols[1].data[mcux*y];
//					cb=&ajpeginfo.curmcu.cols[2].data[mcux*y];
					for (x=0;x<lenx;x++) {
//						struct rgb24 c;
						S32 it;
//						t[0]=;
//						t[1]=ajpeginfo.curmcu.cols[1].data[mcux*y+x];
//						t[2]=ajpeginfo.curmcu.cols[2].data[mcux*y+x];
//						it=f2S32(cr[x]);
						F2INT(cr[x],it);
//						it[1]=f2S32(cr[x]-0.34414f*cg[x]-0.71414f*cb[x]);
//						it[2]=f2S32(cr[x]+1.772f*cg[x]);
//						pr[x]=pg[x]=pb[x]=rangetab2[it];
						p[x].r=p[x].g=p[x].b=rangetab2[it];
						p[x].a=0xff;
//						pg[x]=rangetab2[it[1]];
//						pb[x]=rangetab2[it[2]]; 
//						pr[x]=(U8)range(0,it[0],255);
//						pg[x]=(U8)range(0,it[1],255);
//						pb[x]=(U8)range(0,it[2],255); 
//						c.r=(U8)range(0,it[0],255);
//						c.g=(U8)range(0,it[1],255);
//						c.b=(U8)range(0,it[2],255);
//						clipputpixel24(rt,x+i,y+j,c);
					}
				}
			}
			perf_end(JPEG8);
			ajpeginfo.curinterval--;
			if (ajpeginfo.curinterval==0) {
				ajpeginfo.curinterval=ajpeginfo.interval;
				alignbits();
				hibyte=get8or16bits(8);
				lobyte=get8or16bits(8);
//				ricklogger("interval 2 bytes %04x",(hibyte<<8)+lobyte);
				if (hibyte!=0xff)
					errorexit("jpeg error : interval hibyte not ff : procjpeg");
				if (lobyte==0xd9)
					ajpeginfo.done=1;
				else {
					if (lobyte!=ajpeginfo.intval)
						errorexit("jpeg error : interval lobyte not right : procjpeg");
					dcval[0]=dcval[1]=dcval[2]=0;
					ajpeginfo.intval++;
					if (ajpeginfo.intval==0xd8)
						ajpeginfo.intval=0xd0;
				}
			}
		}
	}
	alignbits();
	if (!ajpeginfo.done) {
		hibyte=get8or16bits(8);
		lobyte=get8or16bits(8);
//		ricklogger("last 2 bytes %04x",(hibyte<<8)+lobyte);
		if (hibyte!=0xff)
			errorexit("jpeg error : done hibyte not ff : procjpeg");
		if (lobyte!=0xd9)
			errorexit("jpeg error : done lobyte not right : procjpeg");
	}
	closebits();
	tga_float_free_bitmap(&ajpeginfo.curmcu);
//	float_free_bitmap(&ajpeginfo.curdu);
}

/////// main function
struct bitmap32* jpegread(FILE* fp)
//struct bitmap32* gfxread32_jpg(const C8* fname)
{
	fh=fp;
	struct bitmap32* rettga;
	S32 i;
	S32 curpos;
	C8 errstr[256];
	U8 id,type;
	U32 len;
	struct jpegtype *jt;
//	tga_printconsole(acon,"<<<<<<<<<<<<<<<<<<<<<<<<< jpeg '%s' >>>>>>>>>>>>>>>>>>>>>>>",fname);
	perf_start(JPEG);
//	logger("<<<<<<<<<<<<<<<<<<<<<<<<< jpeg  '%s' >>>>>>>>>>>>>>>>>>>>>>>\n",getfilename());
	initdct();
//	fileopen(fname,READ);
//	fh=fopen2(fname,"rb");
	ajpeginfo.filesize=getfilesize(fh);
	// read in ff and SOI
	id=filereadU8(fh);
	type=filereadU8(fh);
	if (id!=0xff || type !=0xd8)
		errorexit("jpeg error : no soi");
	// read diff segments
	perf_start(JPEG1);
	while(1) {
		// get id and type
		id=filereadU8(fh);
		type=filereadU8(fh);
		if (id!=0xff) {
			logger("bad id %02x, aborting...\n",id);
			break;
		}
		// see what we have
		jt=&jpegtypes[type];
		if (jt->haslen) {
			len=filereadU16BE(fh)-2;	// big endian
			curpos=ftell(fh); //(0,FILE_CURRENT);
//			logger("------------------------------ id %02x, type '%s', len %d ------------------------------------\n",id,jt->name,len);
		} else {
			len=0;
			curpos=ftell(fh);//fileskip(0,FILE_CURRENT);
//			logger("----------------------------- id %02x, type '%s', nolen ----------------------------------\n",id,jt->name);
		}
		if (jt->iserror)
			errorexit("jpeg error : getjpeg");
//		if (id==0xd9) // EOI
//			break;
		if (jt->func)
			(jt->func)(len);
		if (type==0xda)
			break; // SOS
//		fileskip(curpos+len,FILE_START);
		fseek(fh,curpos+len,SEEK_SET);
	}
	// check jpeginfo for errors
	perf_end(JPEG1);
	ajpeginfo.curinterval=ajpeginfo.interval;
	ajpeginfo.intval=0xd0;
	if (ajpeginfo.height==0 || ajpeginfo.width==0) // width and height
		errorexit("jpeg error : width or height == 0  : sof0");
	if (ajpeginfo.nsoscomps!=ajpeginfo.ncomps) // components
		errorexit("jpeg : error no components : getjpeg");
	if (ajpeginfo.ncomps!=3 && ajpeginfo.ncomps!=1)
		errorexit("jpeg error : ncomponents not a 1 or 3 : sof0");
	for (i=0;i<ajpeginfo.ncomps;i++) { // component id
		if (ajpeginfo.comps[i].cid!=i+1 || ajpeginfo.comps[i].cid!=ajpeginfo.soscomps[i].cid)
			errorexit("jpeg : error merging components : getjpeg");
		if (ajpeginfo.comps[i].cid<1 || ajpeginfo.comps[i].cid>3) {
			sprintf(errstr,"bad cid $%02x",ajpeginfo.comps[i].cid);
			errorexit("jpeg error : sof0");
		}
		if (!ajpeginfo.hasqt[ajpeginfo.comps[i].qtn])
			errorexit("jpeg error : missing quant table : getjpeg");
		if (!ajpeginfo.hashuff[DC][ajpeginfo.soscomps[i].dc])
			errorexit("jpeg error : missing dc hufftable : getjpeg");
		if (!ajpeginfo.hashuff[AC][ajpeginfo.soscomps[i].ac]) {
			errorexit("jpeg error : missing ac hufftable : getjpeg");
//			if (!ajpeginfo.hashuff[DC][ajpeginfo.soscomps[i].ac])
//				ErrorExit("jpeg error","missing ac hufftable and no dc hufftable found","getjpeg");
//			ajpeginfo.soscomps[i].acusedc=1;
		}
	}
	scanffs();
//	fileclose();
	fclose(fh);
//	jshowbits();
	rettga=bitmap32alloc(ajpeginfo.width,ajpeginfo.height,C32BLUE);
	perf_start(JPEG5);
	procjpeg(rettga);
	perf_end(JPEG5);
	// update console
//	clipblit24(tga_getbitmap(acon),&winglue.tv,0,0,0,0,WX,WY);
	// reset jpeginfo
	if (ajpeginfo.data)
//		memfree(ajpeginfo.data);
		delete[] ajpeginfo.data;
	memset(&ajpeginfo,0xff,sizeof(ajpeginfo));
//	tga_outtextxy(&rettga,ajpeginfo.width/2-4*strlen(fname),0,fname,rgbwhite);
//	memset(&ajpeginfo,0,sizeof(ajpeginfo));
//	clear32((void *)&ajpeginfo,sizeof(ajpeginfo),0);
	ajpeginfo.filesize=0;
	ajpeginfo.data=0;
	ajpeginfo.datasize=0;
	ajpeginfo.hasqt[0]=ajpeginfo.hasqt[1]=ajpeginfo.hasqt[2]=ajpeginfo.hasqt[3]=0;
	ajpeginfo.hashuff[0][0]=ajpeginfo.hashuff[0][1]=ajpeginfo.hashuff[0][2]=ajpeginfo.hashuff[0][3]=0;
	ajpeginfo.hashuff[1][0]=ajpeginfo.hashuff[1][1]=ajpeginfo.hashuff[1][2]=ajpeginfo.hashuff[1][3]=0;
	ajpeginfo.ncomps=ajpeginfo.nsoscomps=0;
	ajpeginfo.interval=ajpeginfo.curinterval=ajpeginfo.done=0;
	ajpeginfo.intval=0;
	perf_end(JPEG);
	colorkeyinfo.lasthascolorkey=0;
	return rettga;
}

struct bitmap32* gfxread32_jpg(const C8* fname)
{
	fnamec=fname;
	FILE* fp=fopen2(fname,"rb");
	if (!fp)
		logger("WARNING: can't read jpeg '%s'\n",fname);
	bitmap32* ret=jpegread(fp);
	fclose(fp);
	return ret;
}
