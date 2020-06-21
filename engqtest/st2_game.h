namespace st2 {
    void game_init();
void game_proc();
void game_exit();

#define MAXPLAYERS 256
#define MAXWORLDOBJECTS 64
#define CARREV0 19
#define CARREV1 20
#define CARREV2 21
#define CARREV3 22
#define CARREV4 23
#define CARREV5 24
#define CARREV6 25
#define CARREV7 26
#define CARREV8 27
#define CARREV9 28
#define CARREV10 29
#define CARREV11 30
#define CARREV12 31

#define CARTYPE_UNDEFINED  0
#define CARTYPE_CAR        1
#define CARTYPE_BIKE       2
#define CARTYPE_BOAT       3
#define CARTYPE_ULTRALIGHT 4

extern int uup;
extern int udown;
extern int uleft;
extern int uright;
extern int urollleft;
extern int urollright;
extern int uready;

extern int flymode;
extern int startstunt;

extern float wheelaccel;

// use just 8 in x and 8 in z for now
#define COLLGRIDX 16
#define COLLGRIDZ 16
//#define NCOLLLWOS 64
#define MAXMATFUNCS 32

struct CARACTION
{
  TREE *sitnull;
  int drivemode;
  int uup;
  int udown;
  int uleft;
  int uright;
  int flymode;
  int ldown;
  int lup;
  int urollright;
  int urollleft;
  int uready;
  int startstunt;
  int lleft;
  int lright;
  int lrollleft;
  int lrollright;
  int airtime;
  int cbairtime;
  int ufliptime;
  int cantstartdrive;
  int flyaccel;
  int revflyaccel;
  int doacrashreset;
  int cantdrive;
  int caractive;
  int stunttime;
  int lastairtime;
  int udidroll;
  int udidpitch;
  int udidyaw;
  int cartype;
  float uextraheading;
  float crashresetdir;
  float debmoi;
  float debmass;
  VEC ulastpos;
  VEC ulastloc;
  VEC carstuntrpy;
  VEC ucarrotvel;
  VEC crashresetloc;
  VEC landspot;
  VEC qcamdesired;
  VEC camdesired;
};

typedef struct {
  char *lwsname;
  TREE *bulletnull;
  TREE *weaponnull;
  TREE *anim;
  TREE *lowerabs;
  int isai;
  int repeat;
  int numlws;
  int mangrab;
  int manfire;
  int manleft;
  int manright;
  int manup;
  int mandown;
  int manjump;
  int lastfalling;
  int falling;
  int curseq;
  int incar;
  int fireframe;
  int inwater;
  int havegun;
  int incarnum;
  int curgun;
  int closecar;
  int controlready;
  int wantincar;
  int chute;
  int hitwall;
  int grabledge;
  int lasthitwall;
  VEC manrot;
  float falldelta;
  float aiclose;
  float walkspeed;
  float bestd;
  float curd;
  float mantogun;
  float bestdist;
  float lastgroundheight;
  float groundheight;
} SEQPROP;

extern int totalcrashs;
}
