#include <m_eng.h>

#define binary(a,b,c,d,e,f,g,h)	\
	(C8)((a&1)*128+(b&1)*64+(c&1)*32+(d&1)*16+(e&1)*8+(f&1)*4+(g&1)*2+(h&1))

#include "u_stoid_data.h"
#ifdef DONAMESPACE
namespace stoid {
#endif

const C8 *levelstrs[] = {
"The Wall",
"Think about it!",
"The Tube",
"Dead End",
"Walls of Death",
"Zig Zag Man!",
"Fort Knox",
"San Quentin",
"ZigZag II",
"The Grand Finale" };

#define x 2
const char level0[] = {
binary(1,1,1,1,1,1,1,1),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char level1[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(1,x,0,0,0,0,0,0),
binary(0,x,1,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,1,x,0,0,0,0),
binary(0,0,0,x,1,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char level2[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(1,x,1,0,0,0,0,0),
binary(1,x,1,0,0,0,0,0),
binary(1,x,1,0,0,0,0,0),
binary(1,x,1,0,0,0,0,0),
binary(0,1,x,1,0,0,0,0),
binary(0,1,x,1,0,0,0,0),
binary(0,1,x,1,0,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,0,1,x,1,0),
binary(0,0,0,0,1,x,1,0),
binary(0,0,0,0,1,x,1,0),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,0,1,x)
};

const char level3[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(1,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(x,1,1,1,1,1,1,1),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(1,1,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,1,x,x,0,1,0,0),
binary(0,1,x,1,0,1,0,0),
binary(1,1,x,1,0,1,0,0),
binary(0,x,1,1,0,x,0,0),
binary(0,0,x,1,1,0,x,0),
binary(0,0,0,x,1,1,1,1),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char level4[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(1,1,1,1,1,1,x,1),
binary(0,0,0,0,0,x,0,1),
binary(1,1,0,0,x,0,0,1),
binary(0,0,0,x,0,0,0,1),
binary(1,1,x,0,0,0,0,1),
binary(0,x,1,1,1,1,1,1),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(1,1,1,1,1,1,x,1),
binary(0,0,0,0,0,x,0,1),
binary(0,0,0,0,x,0,0,1),
binary(1,1,1,x,0,0,0,1),
binary(1,0,x,0,0,0,0,1),
binary(1,x,1,1,1,1,1,1),
binary(1,1,x,1,1,1,1,1),
binary(1,1,1,x,1,1,1,1),
binary(1,1,1,1,x,1,1,1),
binary(1,1,1,1,1,x,1,1),
binary(1,1,1,1,1,1,x,1),
binary(1,1,1,1,1,1,1,x)
};

const char level5[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(1,1,1,1,1,1,x,1),
binary(0,0,0,0,0,x,0,1),
binary(0,0,0,0,x,0,0,1),
binary(1,1,1,x,0,0,1,1),
binary(1,0,x,0,0,0,0,0),
binary(0,x,1,1,1,1,1,1),
binary(0,0,x,0,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,1,0,1,0,x,0,0),
binary(0,1,0,1,1,1,x,0),
binary(0,1,1,1,0,x,0,1),
binary(0,1,0,0,x,0,0,1),
binary(0,1,0,x,0,0,1,1),
binary(0,0,x,1,1,1,0,1),
binary(0,0,0,x,0,1,1,1),
binary(0,0,0,0,x,0,0,0),
binary(0,1,1,1,1,x,0,0),
binary(0,1,0,0,x,0,0,0),
binary(0,1,0,x,0,0,0,1),
binary(0,0,x,1,1,1,1,1),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,1,1,1,1,x,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,x,1,1,1,1),
binary(0,0,0,0,x,0,0,0),
binary(0,1,1,1,1,x,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,x,1,1,1,1),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,1,1,x,0,0),
binary(0,0,0,0,x,1,1,0),
binary(0,0,0,1,1,x,0,0),
binary(0,0,0,0,x,1,1,0),
binary(0,0,0,1,1,x,0,0),
binary(0,0,0,0,x,1,1,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char level6[] = {
binary(1,1,1,1,1,1,1,1),
binary(1,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(1,1,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(1,1,1,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(0,0,0,0,0,0,0,0),
binary(1,1,1,0,0,0,0,0),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(1,1,1,x,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(1,1,1,0,x,1,1,1),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,1,x,1),
binary(1,1,1,1,1,1,x,1),
binary(1,0,0,0,0,x,0,1),
binary(1,0,0,0,x,0,1,1),
binary(1,0,0,x,0,0,1,0),
binary(1,0,x,0,0,0,1,0),
binary(1,x,1,0,0,0,1,0),
binary(1,x,1,0,1,0,1,0),
binary(1,x,1,0,1,0,1,0),
binary(1,x,1,0,1,0,1,0),
binary(1,x,1,0,1,0,1,0),
binary(1,x,1,0,1,0,1,0),
binary(1,x,1,1,1,0,1,0),
binary(1,x,0,0,0,0,1,0),
binary(1,0,x,0,0,0,1,0),
binary(1,1,1,x,1,1,1,0),
binary(1,0,1,x,1,0,0,0),
binary(1,0,1,x,1,0,0,0),
binary(0,0,1,x,1,0,0,0),
binary(0,1,x,1,0,1,0,0),
binary(0,1,x,1,0,1,0,0),
binary(0,1,x,1,1,1,0,0),
binary(1,0,1,x,1,0,1,0),
binary(1,0,1,x,1,0,1,0),
binary(1,1,1,1,x,1,1,0),
binary(0,0,0,1,1,x,1,1),
binary(0,0,0,0,1,1,x,1),
binary(0,0,0,0,0,1,1,x)
};

const char level7[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,1,x,0,0,0,0,0),
binary(0,1,x,0,0,0,0,0),
binary(0,1,x,1,0,0,0,0),
binary(0,1,x,1,0,0,0,0),
binary(0,1,x,1,0,1,0,0),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,0,1,0,1),
binary(0,1,x,1,1,1,0,1),
binary(0,0,x,1,0,1,1,1),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,1,x,0,0,0),
binary(0,0,0,1,x,0,0,0),
binary(0,0,0,1,x,1,0,0),
binary(1,1,1,1,x,1,0,1),
binary(1,0,0,x,0,1,0,1),
binary(0,0,x,1,1,1,1,1),
binary(0,0,0,x,0,0,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,1,x,0),
binary(0,0,0,1,1,1,x,1),
binary(0,0,0,1,0,x,0,1),
binary(0,0,0,1,x,1,0,1),
binary(0,0,0,1,x,1,0,1),
binary(0,0,0,1,x,1,1,1),
binary(0,0,0,1,1,x,1,0),
binary(0,0,0,0,1,x,1,0),
binary(0,0,0,0,1,x,1,0),
binary(0,0,1,1,1,x,1,0),
binary(0,0,1,0,x,0,1,0),
binary(0,0,1,x,1,0,1,0),
binary(0,0,1,x,1,0,1,0),
binary(0,0,1,x,1,0,1,0),
binary(0,0,1,x,1,1,1,0),
binary(1,1,1,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,1,x,0),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,1,x,1),
binary(0,0,0,0,0,x,1,1),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char level8[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,0),
binary(1,1,1,x,0,0,0,0),
binary(1,0,x,0,0,0,0,0),
binary(0,x,1,1,1,1,0,0),
binary(0,0,x,0,0,0,0,0),
binary(1,1,1,x,0,0,0,0),
binary(1,0,x,0,0,0,0,0),
binary(0,x,1,1,1,1,0,0),
binary(0,0,x,0,0,0,0,0),
binary(0,1,1,x,0,0,0,0),
binary(0,0,x,1,1,0,0,0),
binary(0,1,1,x,0,0,0,0),
binary(0,0,x,1,1,0,0,0),
binary(0,0,0,x,0,0,0,0),
binary(0,0,1,1,x,0,0,0),
binary(0,0,0,x,1,1,0,0),
binary(0,0,1,1,x,0,0,0),
binary(0,0,0,x,1,1,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,1,1,x,0,0),
binary(0,0,0,0,x,1,1,0),
binary(0,0,0,1,1,x,0,0),
binary(0,0,0,0,x,1,1,0),
binary(1,0,0,0,0,x,0,0),
binary(1,0,1,1,1,1,x,0),
binary(1,1,1,0,0,x,0,1),
binary(1,0,0,0,x,0,0,1),
binary(1,0,0,x,0,1,1,1),
binary(1,0,x,1,0,0,0,0),
binary(1,x,1,1,1,0,0,0),
binary(1,1,x,1,1,0,1,1),
binary(1,1,1,x,1,1,0,0),
binary(1,1,1,1,x,1,1,1),
binary(1,1,1,1,1,x,1,1),
binary(1,1,1,1,1,1,x,1),
binary(1,1,1,1,1,1,1,x)
};

const char level9[] = {
binary(1,1,1,1,1,1,1,1),
binary(x,0,0,0,0,0,0,0),
binary(0,x,0,0,0,0,0,0),
binary(0,0,x,0,0,0,0,1),
binary(0,0,0,x,0,0,0,0),
binary(0,1,1,1,x,1,0,0),
binary(0,1,0,0,x,1,0,1),
binary(0,1,0,x,0,1,0,0),
binary(0,1,x,1,1,1,0,0),
binary(0,1,1,x,1,1,0,0),
binary(1,1,1,1,x,1,0,1),
binary(0,1,1,x,0,1,0,0),
binary(1,1,x,1,0,1,0,0),
binary(1,x,1,1,0,1,0,0),
binary(1,x,1,0,0,0,0,1),
binary(1,x,1,0,0,0,0,0),
binary(1,x,1,0,0,0,0,1),
binary(0,x,1,1,0,0,0,1),
binary(1,1,x,1,0,0,0,1),
binary(0,0,x,1,0,0,1,1),
binary(0,0,x,1,1,0,1,0),
binary(1,1,1,x,1,0,1,0),
binary(0,0,0,x,0,0,0,0),
binary(0,1,x,1,1,1,1,1),
binary(0,1,1,x,1,1,0,0),
binary(0,1,1,1,x,1,0,0),
binary(0,1,1,1,1,x,0,0),
binary(0,1,1,1,x,1,0,0),
binary(0,1,1,x,1,1,1,1),
binary(0,0,0,1,x,1,0,0),
binary(0,0,0,1,1,x,0,0),
binary(1,1,1,1,x,1,0,0),
binary(0,0,0,x,1,1,1,1),
binary(0,0,0,1,x,1,1,1),
binary(0,0,0,1,1,x,0,1),
binary(1,1,1,1,x,0,0,1),
binary(1,0,0,x,0,0,0,1),
binary(1,0,x,0,0,0,0,1),
binary(1,x,1,1,1,1,1,1),
binary(1,0,x,0,0,0,0,1),
binary(1,0,0,x,0,0,0,1),
binary(1,0,1,1,x,0,0,1),
binary(1,0,0,x,1,1,0,1),
binary(1,0,0,0,x,0,0,1),
binary(1,0,0,0,0,x,0,0),
binary(1,1,1,1,1,1,x,1),
binary(0,0,0,0,0,x,0,1),
binary(0,0,0,0,x,1,0,1),
binary(0,0,0,0,x,1,1,1),
binary(0,1,1,1,1,x,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,x,1,1,1,1),
binary(0,0,1,1,x,0,0,0),
binary(0,0,0,x,1,1,0,0),
binary(0,0,0,0,x,0,0,0),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,1,1,x,0),
binary(0,0,0,0,0,x,1,1),
binary(0,0,1,1,1,1,x,0),
binary(0,0,0,0,0,x,0,1),
binary(0,0,0,0,x,1,1,1),
binary(0,0,0,0,0,x,0,0),
binary(0,0,0,0,0,0,x,0),
binary(0,0,0,0,0,0,0,x)
};

const char *levels[] = {
level0,level1,level2,level3,level4,level5,level6,level7,level8,level9 };

const char levelsizes[] = {
sizeof(level0),sizeof(level1),sizeof(level2),sizeof(level3),sizeof(level4),
sizeof(level5),sizeof(level6),sizeof(level7),sizeof(level8),sizeof(level9) };

const S32 LAST_LEVEL = sizeof(levelstrs) / sizeof(char *);

#ifdef DONAMESPACE
} // end namespace stoid

using namespace stoid;
#endif
