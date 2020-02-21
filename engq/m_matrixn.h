// general purpose matrix of any size
struct matn {
	S32 order;
	float *data;
};

struct matn* matncreate(S32 ord); // returns an ident
void matnfree(struct matn*);
void matnsetcolrow(struct matn*,S32 col,S32 row,float val);
float matngetcolrow(const struct matn*,S32 col,S32 row);
void matnmult(const struct matn* a,const struct matn* b,struct matn* prod); // a*b=c assume same size
S32 matninverse(const struct matn* a,struct matn* i); // returns true if invertable, assume same size
void matnlogger(const struct matn* a,const C8* desc);
void matncopy(const struct matn* a,struct matn* b); // copy a to b, assume same size
void matnident(struct matn* a); // make a an ident
void matnzero(struct matn* a);
float matndet(const struct matn* a);

struct vecn {
	S32 order;
	float* data;
};

struct vecn* vecncreate(S32 ord); // returns a zero vector
void vecnfree(struct vecn*);
void vecnsetele(struct vecn*,S32 ele,float val);
float vecngetele(const struct vecn*,S32 ele);
void vecnmult(const struct matn* a,const struct vecn* b,const struct vecn* prod); // a*b=c assume same size
void vecnlogger(const struct vecn* a,const C8* desc);
void vecncopy(const struct vecn* a,struct vecn* b); // copy a to b, assume same size
void vecnzero(struct vecn* a);

S32 matnsolve(const struct matn* bu2li,const struct vecn* li,struct vecn* bu); // returns 0 no, 1 one, 2 many
void matnvecnlogger(const struct matn* m,const struct vecn* v,const C8* desc);

// special binary matrix 
struct matb {
	S32 order;
	C8 *data;
};

struct matb* matbcreate(S32 ord); // returns an ident
void matbfree(struct matb*);
void matbsetcolrow(struct matb*,S32 col,S32 row,S32 val);
S32 matbgetcolrow(const struct matb*,S32 col,S32 row);
void matbmult(const struct matb* a,const struct matb* b,struct matb* prod); // a*b=c assume same size
S32 matbinverse(const struct matb* a,struct matb* i); // returns true if invertable, assume same size
void matblogger(const struct matb* a,const C8* desc);
void matbcopy(const struct matb* a,struct matb* b); // copy a to b, assume same size
void matbident(struct matb* a); // make a an ident
void matbzero(struct matb* a);
S32 matbdet(const struct matb* a);
struct vecb {
	S32 order;
	C8* data;
};

S32 vecbcmp(const struct vecb* a,const struct vecb* b); // returns 0 if equal
struct vecb* vecbcreate(S32 ord); // returns a zero vector
void vecbfree(struct vecb*);
void vecbsetele(struct vecb*,S32 ele,S32 val);
S32 vecbgetele(const struct vecb*,S32 ele);
void vecbmult(const struct matb* a,const struct vecb* b,struct vecb* prod); // a*b=c assume same size
void vecblogger(const struct vecb* a,const C8* desc);
void vecbcopy(const struct vecb* a,struct vecb* b); // copy a to b, assume same size
void vecbzero(struct vecb* a);
S32 matbsolve(const struct matb* bu2li,const struct vecb* li,struct vecb* bu); // returns 0 no, 1 one, 2 many

extern struct pointf3 zerov;
