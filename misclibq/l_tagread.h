struct tag {
	int ntag;
	struct tagheader *th;
};

tag *tagread(char *fname);
tag *newtagread(char *fname);
wavehandle *taggetwhbyidx(tag *,int idx);
wavehandle *taggetwhbyname(tag *,char *name);
int taggetnum(tag *);
char *taggetname(tag *,int idx);
void tagfree(tag *);
tag *vagread(char **names,int num);
