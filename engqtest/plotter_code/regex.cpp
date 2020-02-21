// non determinded finite state machine
#define REGEX

struct pstate {
	C8 chr;
	S32 next1; // -1 for no next
	S32 next2;
};

pstate pstates1[] = {
// (a*b+ac)d
	{'-', 5,-1}, // 0 // enter
	{'a', 2,-1}, // 1
	{'-', 3, 1}, // 2
	{'b', 4,-1}, // 3
	{'-', 8,-1}, // 4
	{'-', 6, 2}, // 5
	{'a', 7,-1}, // 6
	{'c', 8,-1}, // 7
	{'d', 9,-1}, // 8
	{'-',-1,-1}, // 9 // exit
};
//S32 npstates = NUMELEMENTS(pstates);

pstate pstates2[] = {
// (ab+b)*(a+b)
	{'-', 5,-1}, // 0 // enter
	{'-', 2, 4}, // 1
	{'a', 3,-1}, // 2
	{'b', 5,-1}, // 3
	{'b', 5,-1}, // 4
	{'-', 1, 6}, // 5
	{'-', 7, 8}, // 6
	{'a', 9,-1}, // 7
	{'b', 9,-1}, // 8
	{'-',-1,-1}, // 9 // exit
};
//S32 npstates = NUMELEMENTS(pstates);

// return first non matching char idx if matches, else return st if no match
S32 runsim(const pstate* ps,S32 nps,const C8* s,S32 si,bool whole = false)
{
	S32 i;
	bool verbose = true; // verbose
	bool showmachine = true;
	S32 sisv = si;
	if (verbose)
		logger("--------------------------\nrun sim with str '%s', start stridx %d\n",s,si);
	if (showmachine) {
		logger("@@ machine state @@\n");
		for (i=0;i<nps;++i) {
			C8 c = ps[i].chr;
			if (!c)
				c = '0';
			logger("  state %2d, %c %2d %2d\n",i,c,ps[i].next1,ps[i].next2);
		}
	}
	//if (*s == '\0')
	//	return false;
	deque<S32> que;
	que.push_back(0); // 0, start state
	que.push_back(-1); // -1, scan token
	//S32 cur = 0;
	S32 match = si;
	S32 watch = 0;
	const S32 maxwatch = 1000;
	//S32 cc = 0; // current character
	while(true) {
		// show the que
		if (verbose) {
			for (i=0;i<(S32)que.size();++i)
				logger("%2d ",que[i]);
			logger("\n");
		}
		// pop the front to get current state to process
		S32 curstate = que.front(); que.pop_front();
		// check current state
		if (curstate == -1) {
// it's a 'scan' token, not a state	
			if (que.empty()) {
				if (verbose)
					logger("deque empty, bad match, no next states to advance to at stridx %d\n",si);
				break;
			}
			/*if (s[si] =='\0') { // never happens
				if (verbose)
					logger("EOF can't read char during 'scan' at idx %d\n",si);
				break;
			}*/
			if (verbose)
				logger("scanned '%c' at idx %d\n",s[si],si);
			++si; // advance to next char
			que.push_back(-1); // another 'scan' at the end
			continue;
		}
		if (curstate >= nps) {
			logger("array out of bounds for sim, curstate wanted %d, nstates %d, at stridx %d\n",curstate,nps,si);
			break;			
		}
		const pstate& cps = ps[curstate];
		if (cps.next1 == -1) { 
// it's an 'exit' token, no next states
			if (whole) {
				// whole string here
				if (s[si]!='\0') {
					// not the whole string
					if (verbose) {
						logger("match but not whole string ,try other paths, at stridx %d\n",si);
					}
					continue;
				} else {
					match = si; // we made it here
					if (verbose) {
						logger("successful whole match! at stridx %d\n",si);
					}
				}
			} else { 
				// not whole string
				match = si; // we made it here
				if (verbose) {
					logger("successful match! at stridx %d\n",si);
				}
			}
			break;
		}
		if (cps.chr == '-') {
// it's an 'or' token
			// push 1 or more choices to the front
			que.push_front(cps.next1);
			if (cps.next2 != -1)
				que.push_front(cps.next2);
		} else { 
// it's a 'char' token	
#if 0
			if (*s =='\0') {
				logger("EOF can't read char during 'char'\n");
				break;
			}
#endif
			// check for char match, if not then do nothing
			if (s[si] == cps.chr) {
				que.push_back(cps.next1); // after the 'scan'
			}
		}
		++watch;
		if (watch >= maxwatch) {
			logger("error: maxwatch exceeded %d\n",watch);
			break;
		}
	}
	if (!verbose) {
		if (match != sisv)
			if (whole)
				logger("--->>> run sim with str '%s' idx [%d,%d) whole matched!\n",s,sisv,match);
			else
				logger("--->>> run sim with str '%s' idx [%d,%d) matched!\n",s,sisv,match);
	}
	return match;
}

vector<pstate> parseret;
S32 stridx;
const C8* pstr;
S32 curstate;
string err;

bool letter(C8 c)
{
	return c >= 'a' && c<= 'z' || c>='A' && c<='Z';
}

S32 expr();
S32 term();
S32 factor();
#define MAXPARSE 20 // such a hack job
#if 0
// new test version
// term or term+expr
S32 expr()
{
	S32 t1 = term();
	S32 ret = t1;
	if (pstr[stridx] == '+') {
		++stridx;
		++curstate;
		S32 t2 = curstate;
		ret = t2;
		++curstate;
#if 0
		S32 expret = expr();
		pstate ps1 = {'-',curstate,-1};
		parseret[t2-1] = ps1;
		pstate ps2 = {'-',expret,t1};
		parseret[t2] = ps2;
#else
		S32 cs = expr();
		pstate ps2 = {'-',cs,t1};
		parseret[t2] = ps2;
		pstate ps1 = {'-',curstate,-1};
		parseret[t2-1] = ps1;
#endif
	}
	return ret;
}

// factor or factorterm
S32 term()
{
	S32 rt;
	S32 ret = factor();
	if (pstr[stridx] == '(' || letter(pstr[stridx])) {
		// new, fixup next state
		S32 cs = curstate;
		rt = term();
#if 1
		S32 oldrt = parseret[cs-1].next1;
		logger("new, adjusting next1 of %d from %d to %d\n",cs-1,oldrt,rt);
		parseret[cs-1].next1 = rt;
		// adjust in all of the machine
		S32 i;
		for (i=0;i<MAXPARSE;++i) {
			if (parseret[i].next1 == oldrt) {
				logger("new, in the array, adjusting next1 of %d from %d to %d\n",i,oldrt,rt);
				parseret[i].next1 = rt;
			}
		}
#endif
	}
	return ret;
}

// (expr) or char or factor*
S32 factor()
{
	S32 t1 = curstate;
	S32 t2 = -1;
	if (pstr[stridx] == '(') {
		++stridx;
		t2 = expr();
		if (pstr[stridx] == ')') {
			++stridx;
		} else {
			err = "missing )"; // no matching close paren
		}
	} else if (letter(pstr[stridx])) {
		pstate ps = {pstr[stridx],curstate+1,-1};
		++stridx;
		parseret[curstate] = ps;
		t2 = curstate;
		++curstate;
	} else {
		err = "EOF"; // EOF
	}
	S32 ret = t2;
	// look for '*'
	if (pstr[stridx] == '*') {
		++stridx;
		pstate ps = {'-',curstate+1,t2};
		parseret[curstate] = ps;
		ret = curstate;
		parseret[t1-1].next1 = curstate;
		++curstate;
	}
	return ret;
}

vector<pstate> runparse(const C8* str)
{
	logger("runparse with '%s'\n",str);
	err = "none";
	stridx = 0;
	pstr = str;
	curstate = 1;
	parseret.clear();
	// hack
	parseret.resize(MAXPARSE);
	pstate ps1 = {'-',-1,-1}; // enter
	parseret[0] = ps1;
	parseret[0].next1 = expr();
	if (pstr[stridx] != '\0')
		logger("error: still have '%c' to parse\n",pstr[stridx]);
	pstate ps2 = {'-',-1,-1}; // exit
	parseret[curstate] = ps2;
	//if (err.size())
		logger("with error = '%s', final stridx = %d\n",err.c_str(),stridx);
	return parseret;
}
#else
// old working version
// term or term+expr
S32 expr()
{
	S32 t1 = term();
	S32 ret = t1;
	if (pstr[stridx] == '+') {
		++stridx;
		++curstate;
		S32 t2 = curstate;
		ret = t2;
		++curstate;
#if 0
		S32 expret = expr();
		pstate ps1 = {'-',curstate,-1};
		parseret[t2-1] = ps1;
		pstate ps2 = {'-',expret,t1};
		parseret[t2] = ps2;
#else
		S32 cs = expr();
		pstate ps2 = {'-',cs,t1};
		parseret[t2] = ps2;
		pstate ps1 = {'-',curstate,-1};
		parseret[t2-1] = ps1;
#endif
	}
	return ret;
}

// factor or factorterm
S32 term()
{
	S32 rt;
	S32 ret = factor();
	if (pstr[stridx] == '(' || letter(pstr[stridx])) {
		// new, fixup next state
		S32 cs = curstate;
		rt = term();
#if 1
		S32 oldrt = parseret[cs-1].next1;
		logger("new, adjusting next1 of %d from %d to %d\n",cs-1,oldrt,rt);
		parseret[cs-1].next1 = rt;
		// adjust in all of the machine
		S32 i;
		for (i=0;i<MAXPARSE;++i) {
			if (parseret[i].next1 == oldrt) {
				logger("new, in the array, adjusting next1 of %d from %d to %d\n",i,oldrt,rt);
				parseret[i].next1 = rt;
			}
		}
#endif
	}
	return ret;
}

// (expr) or char or factor*
S32 factor()
{
	S32 t1 = curstate;
	S32 t2 = -1;
	if (pstr[stridx] == '(') {
		++stridx;
		t2 = expr();
		if (pstr[stridx] == ')') {
			++stridx;
		} else {
			err = "missing )"; // no matching close paren
		}
	} else if (letter(pstr[stridx])) {
		pstate ps = {pstr[stridx],curstate+1,-1};
		++stridx;
		parseret[curstate] = ps;
		t2 = curstate;
		++curstate;
	} else {
		err = "EOF"; // EOF
	}
	S32 ret = t2;
	// look for '*'
	if (pstr[stridx] == '*') {
		++stridx;
		pstate ps = {'-',curstate+1,t2};
		parseret[curstate] = ps;
		ret = curstate;
		parseret[t1-1].next1 = curstate;
		++curstate;
	}
	return ret;
}

vector<pstate> runparse(const C8* str)
{
	logger("runparse with '%s'\n",str);
	err = "none";
	stridx = 0;
	pstr = str;
	curstate = 1;
	parseret.clear();
	// hack
	parseret.resize(MAXPARSE);
	pstate ps1 = {'-',-1,-1}; // enter
	parseret[0] = ps1;
	parseret[0].next1 = expr();
	if (pstr[stridx] != '\0')
		logger("error: still have '%c' to parse\n",pstr[stridx]);
	pstate ps2 = {'-',-1,-1}; // exit
	parseret[curstate] = ps2;
	//if (err.size())
		logger("with error = '%s', final stridx = %d\n",err.c_str(),stridx);
	return parseret;
}
#endif

void doparsetest()
{
	logger("###### start doing parsetest\n");
	const C8* parsestrs[] = {
		"a*",
		"(ab+b)*(a+b)",
		"a(b+c)",
		"a",
		"(a*b+ac)d",
	};
	const C8* teststrs[] = {
		"a",
		"aba",
		"a",
		"a",
		"aabd",
	};
	S32 i;
	//for (i=0;i<NUMELEMENTS(parsestrs);++i) {
	i=1; {
		vector<pstate> ps = runparse(parsestrs[i]);
		runsim(&ps[0],ps.size(),teststrs[i],0,true);
	}
	logger("###### end doing parsetest\n");
}

void doregextest()
{
	logger("###### start doing regextest\n");
	const bool dowhole = true; // match end of char too.
#if 0
#if 1
	// example regex1
	const pstate* pstates = pstates1;
	const S32 maxchar = 4;
	const S32 maxlen = 3;
#else
	// binary regex2
	const pstate* pstates = pstates2;
	const S32 maxchar = 2;
	const S32 maxlen = 5;
#endif
	S32 i,j,k;
	C8 test2[maxlen+1];
	for (k=0;k<=maxlen;++k) {
		for (i=0;i<k;++i)
			test2[i] = 'a';
		test2[i] = '\0';
		if (k == 0) {
			//logger("test str (zero length) = '%s'\n",test2);
			runsim(pstates,test2,0,dowhole);
		} else {
			j = k - 1;
			while(j>=0) {
				//logger("test str = '%s'\n",test2);
				runsim(pstates,test2,0,dowhole);
				while(j>=0) {
					++test2[j];
					if (test2[j] == 'a' + maxchar) {
						test2[j] = 'a';
						--j;
					} else {
						j = k - 1;
						break;
					}
				}
			}
		}
	}
#endif
	doparsetest();
	C8 test1[] = "aabd";
	//runsim(pstates1,NUMELEMENTS(pstates1),test1,0,dowhole);
	logger("###### end doing regextest\n");
}
