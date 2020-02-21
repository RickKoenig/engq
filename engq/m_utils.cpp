#include <m_eng.h>

// 'to_string' doesn't work in codeblocks for some reason
// so we introduce 'to_string2' which does the same thing
string to_string2(S32 val) {
	const S32 MAXBUFF{ 100 }; // plenty
	C8 buff[100];
	sprintf(buff, "%d", val);
	return string(buff);
}
