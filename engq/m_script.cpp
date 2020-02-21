#define INCLUDE_WINDOWS
#include <m_eng.h>
string script2printc(string s)
{
	string r;
	S32 i,n=s.size();
	for (i=0;i<n;++i)
		if (s[i]=='\\')
			r+="\\\\";
		else if (s[i]=='\"')
			r+="\\\"";
		else
			r+=s[i];
	return r;
}

script::chartype script::getchartype(C8 c)
{
	if (c==' ' || c=='\t')
		return WS;
	if (c=='\n' || c=='\r')
		return CRLF;
	if (c=='#')
		return HASH;
	if (c=='\\')
		return BACKSLASH;
	if (c=='\"')
		return QUOTE;
	if (c==EOF)
		return ISEOF;
	if (c=='/')
		return SLASH;
	if (c=='*')
		return STAR;
	return CHARS;
}

// make a script from a text scriptfile
// supports # as a comment to end of line
// supports " as a way of treating # and whitespace as normal chars
script::script(const C8* fnamea,bool iscstr) : ridx(0)
{
	if (!fnamea)
		return; // create a blank script
//	logger("scriptnoinc constructor with '%s'\n",fname);
	FILE* fp = 0;
	const C8* cp = 0;
	if (iscstr) {
		fname = "_cstr";
		cp = fnamea; // treat the filename as the data if file not found
		inCharSize = strlen(cp);
	} else {
		if (fileexist(fnamea)) {
			fp=fopen2(fnamea,"rb");
			fseek(fp,0L,SEEK_END);
			inCharSize = ftell(fp);
			fseek(fp,0L,SEEK_SET);
			fname=fnamea;
		} else {
			fname = "_not found";
			return;
		}
	}
	string s;
	state curstate=GETCHAR;
//	bool usedchar=true;
	while(curstate!=BAIL) {
		C8 c;
		S32 ret;
		if (fp) {
			ret = fread(&c,1,1,fp);
		} else {
			c = *cp++;
			ret = 1;
		}
		c&=0x7f;
		if (ret==0 || c=='\0')
			c=EOF;
		chartype ct=getchartype(c);
		switch(curstate) {
//// state: normal get C8's
		case BAIL:
			break;
		case GETCHAR:
			switch(ct) {
// look out for a / in /*
			case SLASH:
				curstate=SLASH1;
				break;
// accumulate normal C8 into string
			case STAR:
			case CHARS:
				s+=c;
				break;
// start a quote
			case QUOTE:
				if (s.size())
					addscript(s.c_str());
				s.clear();
				curstate=QUOTEMODE;
				break;
// found a whitespace crlf, add string to script (if necc.), look for more ws
			case WS:
			case CRLF:
				if (s.size())
					addscript(s.c_str());
				s.clear();
				curstate=SKIPWS;
				break;
// found a comment starter, add string to script (if necc.) goto hash state
			case HASH:
				if (s.size())
					addscript(s.c_str());
				s.clear();
				curstate=HASHMODE;
				break;
// end o file, add string to script (if necc.), done
			case ISEOF:
				if (s.size())
					addscript(s.c_str());
				curstate=BAIL;
				break;
			case BACKSLASH:
				curstate=BACKSLASH1;
				break;
			}
			break;
//// state: slash looking for * in  /*
		case SLASH1:
			switch(ct) {
// another to EOL comment
			case SLASH:
				if (s.size())
					addscript(s.c_str());
				s.clear();
				curstate=HASHMODE;
				break;
// start a comment
			case STAR:
				curstate=STAR1;
				break;
// accumulate normal C8 into string
			case CHARS:
				s+='/';
				s+=c;
				curstate=GETCHAR;
				break;
// start a quote
			case QUOTE:
				s+='/';
				addscript(s.c_str());
				s.clear();
				curstate=QUOTEMODE;
				break;
// found a whitespace crlf, add string to script (if necc.), look for more ws
			case WS:
			case CRLF:
				s+='/';
				addscript(s.c_str());
				s.clear();
				curstate=SKIPWS;
				break;
// found a comment starter, add string to script (if necc.) goto hash state
			case HASH:
				s+='/';
				addscript(s.c_str());
				s.clear();
				curstate=HASHMODE;
				break;
// end o file, add string to script (if necc.), done
			case ISEOF:
				s+='/';
				addscript(s.c_str());
				curstate=BAIL;
				break;
			case BACKSLASH:
				s+='/';
				curstate=BACKSLASH1;
				break;
			}
			break;
//// state: comment looking for * in */
		case STAR1:
			switch(ct) {
// ignore
			case SLASH:
				break;
// get to star2 state
			case STAR:
				curstate=STAR2;
				break;
// ignore
			case CHARS:
				break;
// found a quote, ignore
			case QUOTE:
				break;
// found a whitespace, ignore
			case WS:
				break;
// found a crlf, ignore
			case CRLF:
				break;
// found a comment starter, ignore
			case HASH:
				break;
// end o file, done
			case ISEOF:
				errorexit("1missing */ (end comment) for '%s'",fnamea);
				break;
			case BACKSLASH:
				break;
			}
			break;
//// state: looking for / in  */
		case STAR2:
			switch(ct) {
// found / in */ go back to normal
			case SLASH:
				curstate=GETCHAR;
				break;
// ignore, stay in star2 mode, (looking for / in */)
			case STAR:
				break;
// back to star1
			case CHARS:
			case QUOTE:
			case WS:
			case CRLF:
			case HASH:
				curstate=STAR1;
				break;
// end o file, complain
			case ISEOF:
				errorexit("2missing */ (end comment) for '%s'",fnamea);
				break;
			case BACKSLASH:
				curstate=STAR1;
				break;
			}
			break;
//// state: skip over whitepsace
		case SKIPWS:
			switch(ct) {
// start accumulate chars into string again
			case SLASH:
				curstate=SLASH1;
				break;
			case STAR:
			case CHARS:
				s+=c;
				curstate=GETCHAR;
				break;
// start a quote
			case QUOTE:
				curstate=QUOTEMODE;
				break;
// found more whitespace crlf, do nothing
			case WS:
			case CRLF:
				break;
// found a comment starter, enter hasmode
			case HASH:
				curstate=HASHMODE;
				break;
// end o file, done
			case ISEOF:
				curstate=BAIL;
				break;
			case BACKSLASH:
				curstate=BACKSLASH1;
				break;
			}
			break;
//// state: found a hash, skip over comment until crlf
		case HASHMODE:
			switch(ct) {
// ignore
			case SLASH:
			case STAR:
			case CHARS:
				break;
// found a quote, ignore
			case QUOTE:
				break;
// found a whitespace, ignore
			case WS:
				break;
// found a crlf, look for chars again
			case CRLF:
				curstate=GETCHAR;
				break;
// found a comment starter, ignore
			case HASH:
				break;
// end o file, done
			case ISEOF:
				curstate=BAIL;
				break;
			case BACKSLASH:
				break;
			}
			break;
//// state: found a quote, find other quote
		case QUOTEMODE:
			switch(ct) {
// accumulate normal C8 into string, stay in quotemode
			case SLASH:
			case STAR:
			case CHARS:
				s+=c;
				break;
// end a quote, write out string to script (if necc.) and get chars again
			case QUOTE:
//				if (s.size())
					addscript(s.c_str());
				s.clear();
				curstate=GETCHAR;
				break;
// found crlf, ignore, but stay in quotemode
			case CRLF:
				break;
// found a whitespace, add ws to string
			case WS:
// found a comment starter, since it's quoted just add to string
			case HASH:
				s+=c;
				break;
// end o file, complain
			case ISEOF:
				errorexit("missing close quote for '%s'",fnamea);
				break;
			case BACKSLASH:
				curstate=BACKSLASHQ;
				break;
			}
			break;
/// state: backslash
		case BACKSLASH1:
			switch(ct) {
// accumulate normal C8 into string, stay in quotemode
			case SLASH:
			case STAR:
			case CHARS:
			case CRLF:
			case WS:
			case HASH:
				s+='\\';
				s+=c;
				curstate=GETCHAR;
				break;
			case ISEOF:
				errorexit("backslash at eof",fnamea);
				break;
			case BACKSLASH:
			case QUOTE:
				s+=c;
				curstate=GETCHAR;
			}
			break;
/// state: backslash inside quotes
		case BACKSLASHQ:
			switch(ct) {
// accumulate normal C8 into string, stay in quotemode
			case SLASH:
			case STAR:
			case CHARS:
			case CRLF:
			case WS:
			case HASH:
				s+='\\';
				s+=c;
				curstate=QUOTEMODE;
				break;
			case ISEOF:
				errorexit("backslash at eof",fnamea);
				break;
			case BACKSLASH:
			case QUOTE:
				s+=c;
				curstate=QUOTEMODE;
				break;
			}
			break;
		}
	}
//	fileclose();
	if (fp)
		fclose(fp);
}

static bool scripticmp(string a,string b)
{
	S32 r=my_stricmp(a.c_str(),b.c_str());
	return r<0;
}

void script::sort()
{
	::sort(data.begin(),data.end(),scripticmp);
}

//void scriptnoinc::showline()
//{
//	logger("scriptnoinc: numtokens %d",data.size());
//}

scriptinc::scriptinc(const C8* fnamea,const C8* inctweeka) : script(fnamea)
{
	if (!fnamea)
		return;
	static vector<string> includelist;	 // list of filenames included, including self
	vector<script*> sclist; // list of subscripts from includes
	vector<S32> offsets; // list of where subscripts fit into main script
	S32 i;
	includelist.push_back(fnamea);
	for (i=0;i<num()-1;++i) {
		if (!my_stricmp(idx(i).c_str(),"include")) {
			string fn=idx(i+1);
			if (inctweeka) {
				C8 pt[500],nam[500];
				if (mgetpath(fn.c_str(),pt)) {
					if (mgetnameext(fn.c_str(),nam)) {
						string fn2=string(pt) + "/" + inctweeka + nam;
						if (fileexist(fn2.c_str()))
							fn=fn2;
					}
				}
			}
			if (fileexist(fn.c_str())) {
				for (U32 j=0;j<includelist.size();++j)
					if (!my_stricmp(includelist[j].c_str(),fn.c_str()))
						errorexit("'%s': circular include \"%s\"",fnamea,fn.c_str());
				script* isc = new scriptinc(fn.c_str(),inctweeka);
				sclist.push_back(isc);
				offsets.push_back(i);
			}
		}
	}
	includelist.pop_back();
	if (sclist.size()) {
// have includes, merge a new list

		script msc; // new merged script
//		nmsc=nsc;
//		for (i=0;i<nsclist.size();++i)
//			nmsc+=nsclist[i]-2;
//		if (nmsc)
//			msc=(C8**)memalloc(sizeof(C8*)*nmsc);
//		else
//			msc=0;
		U32 k=0; // include file index
//		S32 d=0;
		S32 s=0; // main script index
		while(s<num()) {
			if (k>=offsets.size() || s!=offsets[k])
				msc.addscript(idx(s++).c_str()); // copy string over from main script
			else { // copy strings over from included script
				s+=2; // skip include fnamea
				S32 m;
				for (m=0;m<sclist[k]->num();++m) { // merge
					msc.addscript((*sclist[k]).idx(m).c_str());
				}
				delete(sclist[k]); // free include script
				k++;
			}
		}
		data.clear(); // copy over new script
		for (s=0;s<msc.num();++s)
			addscript(msc.idx(s).c_str());
	}
}

scriptdir::scriptdir(S32 dirs) : script()
{
	WIN32_FIND_DATA fd;
	HANDLE ffh=FindFirstFile("*.*",&fd);
	if (ffh==INVALID_HANDLE_VALUE)
		return;
	do {
		U32 isadir=fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		if ((dirs && isadir) || (!dirs && !isadir)) {
			if (strcmp(fd.cFileName,".") && strcmp(fd.cFileName,"..")) {
				addscript(fd.cFileName);
			}
		}
	} while (FindNextFile(ffh,&fd));
	FindClose(ffh);
}
