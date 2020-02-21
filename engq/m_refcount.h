void checkheaps();
template <typename T>
struct referencecountelement {
	S32 refcount;
	T* rdata;
	referencecountelement(S32 counta,T* dataa) : refcount(counta),rdata(dataa) {}
};

template <typename T>
struct referencecount {
	vector<referencecountelement<T> > rlist;
	S32 getrc(T* t)
	{
		S32 i=t->refidx;
		if (i<0) {
//			logger("getrc: refidx = %d\n",i);
			return 0;
		}
		referencecountelement<T>& r=rlist[i];
//		logger("getrc: refcount = %d\n",r.refcount);
		return r.refcount;
	}
	T* newrc(T* orig)
	{
		if (orig==0)
			return 0;
		S32 i=orig->refidx;
		if (i<0)
			errorexit("dupref: '%s' not in the list",orig->name.c_str());
		referencecountelement<T>& r=rlist[i];
		++r.refcount;
		return orig;
	}
	T* newrc(const C8* name)
	{
		if (name==0)
			return 0;
		T* ret;
		S32 i,n=rlist.size();
		for (i=0;i<n;++i)
			if (!my_stricmp(rlist[i].rdata->name.c_str(),name))
				break;
		if (i==n) {
			ret=new T(name);
			ret->refidx=n;
			referencecountelement<T> r(1,ret);
			rlist.push_back(r);
		} else {
			referencecountelement<T>& r=rlist[i];
			++r.refcount;
			ret=r.rdata;
		}
		return ret;
	}
	template <typename C>
	T* newrc(const C8* name)
	{
		if (name==0)
			return 0;
		T* ret;
		S32 i,n=rlist.size();
		for (i=0;i<n;++i)
			if (!my_stricmp(rlist[i].rdata->name.c_str(),name))
				break;
		if (i==n) {
			ret=new C(name);
			ret->refidx=n;
			referencecountelement<T> r(1,ret);
			rlist.push_back(r);
		} else {
			referencecountelement<T>& r=rlist[i];
			++r.refcount;
			ret=r.rdata;
		}
		return ret;
	}
	void deleterc(T* t)
	{
		if (!t)
			return;
		if (t->refidx<0) {
			delete t;
			return;
		}
		S32 i,n=rlist.size();
		i=t->refidx;
		referencecountelement<T>& r=rlist[i];
		T* dt=r.rdata;
		--r.refcount;
		if (r.refcount==0) {
			--n;
			if (i<n) {
				r=rlist[n];
				r.rdata->refidx=i;
			}
			rlist.pop_back();
			if (rlist.size()==0)
				logger("'%s' RCLIST now at %d\n",typeid(T).name(),rlist.size());
			delete dt;
		}
	}
	void showrc()
	{
		S32 i,n=rlist.size();
		logger("showref\n");
		for (i=0;i<n;++i) {
		    logger_indent();
            logger_disableindent();
			logger("%3d[%3d] : refcount %3d, ptr %08x, name '%-12s' ",
				i,rlist[i].rdata->refidx,rlist[i].refcount,rlist[i].rdata,rlist[i].rdata->name.c_str());
			rlist[i].rdata->showline();
            logger("\n");
            logger_enableindent();
            logger_unindent();
		}
	}
	S32 rlistsize()
	{
		return rlist.size();
	}
};
