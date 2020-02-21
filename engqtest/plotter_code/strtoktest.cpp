#define STRTOK
#ifdef STRTOK
C8* mystrtok(C8* str,C8* delim)
{
	static C8* p;
	if (str)
		p = str;
	if (!p || !delim)
		return 0;
	// find first non delim
	while(*p) {
		C8* d = delim;
		while(*d) {
			if (*p == *d) {
				break; // found delim, move on
			}
			++d;
		}
		if (!*d) // no delims, in a good spot
			break;
		++p; // in delim, move to next spot
	}
	if (!*p)
		return 0;
	C8* ret = p++;
	// now find first delim
	while(*p) {
		C8* d = delim;
		while(*d) {
			if (*p == *d) {
				break; // found delim, done
			}
			++d;
		}
		if (*d) { // delims, in a good spot
			*p = '\0';
			++p;
			break;
		}
		++p; // in delim, move to next spot
	}
	return ret;
}

void strtoktest()
{
	C8 str[] ="- This, a sample string.";
	C8* pch;
	logger("splitting string \"%s\" into tokens:\n",str);
	pch = mystrtok(str," ,.-");
	while(pch) {
		logger("tok = '%s'\n",pch);
		pch = mystrtok(0," ,.-");
	}
	logger("done splitting string\n");
	// sum of cubes
	S32 i;
	for (i=0;i<1000;++i) {
		S32 j = i;
		S32 d0 = j%10;
		j /= 10;
		S32 d1 = j%10;
		j /= 10;
		S32 d2 = j;
		if (d0*d0*d0+d1*d1*d1+d2*d2*d2 == i)
			logger("cube sum is %d\n",i);
	}
}
#endif
