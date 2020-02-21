void meshtestinit(),meshtestproc(),meshtestdraw3d(),meshtestdraw2d(),meshtestexit();

//int (* buildfaces(pointf3 *vrts,int nvrt,int *nfcep))[3];
face* buildfaces(const pointf3* vrts,int nvrt,int *nfcep);

//void buildmtstart(const char *modelname);
tree2* buildmt(const char *modelname,const char *texname,pointf3 *v,uv *uvs,int nv);
//tree2* buildmts(const char *modelname,const char *texname,pointf3 **v,uv **uvs,int *matid,int *nv,int nnv,char *texname2=0);
tree2* buildmts(const char* modelname,const char *texname,const polylist& pl,const char *texname2);
//tree2 *buildmtend();
