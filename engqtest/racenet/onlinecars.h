void onlinecars_init(),onlinecars_proc(),onlinecars_exit();
//TREE *loadonlinecarbin(char *mname,char *tname); // format used in stuntol
//TREE *loadonlinecarena(char *mname,char *tname); // newer format with more children, ref points etc.
TREE *loadnewjrms(char *carnamearg,char *textnamearg);
TREE *loadnewjrms2(char *carname,int paint,int decal);
TREE *loadshinycar(char *name);

#define MAXLIST 6
extern char excludelist[MAXLIST][80];
