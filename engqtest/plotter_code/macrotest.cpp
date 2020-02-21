#define MACROTEST
#ifdef MACROTEST

#define LOGNEW(format,...) do { \
	logger(format,__VA_ARGS__); \
	logger("%s\n",format #__VA_ARGS__); \
	logger("'%s' %d : " format,__FILE__,__LINE__,__VA_ARGS__); \
	logger( __FILE__  " %d : " format,__LINE__,__VA_ARGS__); \
} while(0)

#define TWOVAR(a,b) a ## b

#define SHOWTWOVAR(a,b) do { \
    logger("variable name = " #a #b ); \
    logger("variable value = %d\n", a ## b); \
} while(0)

void macrotest()
{
	logger("doing macrotest\n");
	int a = 3;
	int b = 4;
	LOGNEW("%d times %d = %d\n",a,b,a*b);
    int TWOVAR(var,34);
    TWOVAR(var,34) = 69;
    SHOWTWOVAR(var,34);
}
#endif
