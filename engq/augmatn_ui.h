// augmented matrices ui
template <typename T>
class augmatn_ui : public augmatn<T>  {
public:

	static const S32 startx = 10;
	static const S32 sizex = 100;
	static const S32 sepx = 120;
	static const S32 starty = 45;
	static const S32 sizey = 20;
	static const S32 sepy = 40;
private:
	vector<vector<shape*> > pbutshapes;
public:
	augmatn_ui<T>(S32 neqa,S32 nvara,shape* rla);
	bool isfocus(shape* s,S32& ri,S32& ci);
	void dofocus(S32 cr,S32 cc,U32 butval);
};

template <typename T>
augmatn_ui<T>::augmatn_ui (S32 neqa,S32 nvara,shape* rla) : augmatn<T>(neqa,nvara)
{
	S32 i,j;
//	if (nvara>5 || neqa > 5 || nvara<1 || neqa<1)
//		errorexit("autmatn: bad args for constructor");
	static const C8* varnames= "XYZWAB";
	pbutshapes.resize(augmatn<T>::nr);
	for (i=0;i<augmatn<T>::nc;++i) {
		stringstream ss;
		ss << varnames[i];
		string s = ss.str();
		text* atext=new text("var",startx+sepx*i,starty-sepy,100,20,s.c_str());
		rla->addchild(atext);
	}
	text* atext=new text("var",startx+sepx*augmatn<T>::nc,starty-sepy,100,20,"Q");
	rla->addchild(atext);
	for (j=0;j<augmatn<T>::nr;++j) { // row
		pbutshapes[j].resize(augmatn<T>::nc+1);
		for (i=0;i<=augmatn<T>::nc;++i) { // column, include augment column
			stringstream ss;
			ss << (j+1) << " " << (i+1);
			string name = ss.str();
			stringstream ss2;
			ss2 << augmatn<T>::vals[j][i];
			string vs = ss2.str();
			pbut* abut=new pbut(name.c_str(),startx+sepx*i,starty+sepy*j,100,20,vs.c_str());
			pbutshapes[j][i] = abut;
			rla->addchild(abut);
		}
	}
}

template <typename T>
bool augmatn_ui<T>::isfocus(shape* s,S32& ridx,S32& cidx)
{
	S32 i,j;
	for (j=0;j<augmatn<T>::nr;++j) { // row
		for (i=0;i<=augmatn<T>::nc;++i) { // column, include augment column
			if (pbutshapes[j][i] == s) {
				ridx = j;
				cidx = i;
				return true;
			}
		}
	}
	return false;
}

template <typename T>
void augmatn_ui<T>::dofocus(S32 cr,S32 cc,U32 butval) // after isfocus returns true
{
	pbut* pb = dynamic_cast<pbut*>(pbutshapes[cr][cc]);
	if (pb) {
		stringstream ss2;
		T& vr = augmatn<T>::vals[cr][cc];
		if (butval==1) {
			--vr;
		} else if (butval==2) {
			++vr;
		}
		ss2 << augmatn<T>::vals[cr][cc];
		string vs = ss2.str();
		pb->settname(vs.c_str());
	}
}
