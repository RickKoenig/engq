
// game related
#define MAX_PLAYERS 64
#define MAX_GAMEOBJ 400

#define MAX_WEAPCARRY 9

enum {CNTL_NONE,CNTL_HUMAN,CNTL_AI,MAX_CNTL};
extern char *cntl_names[MAX_CNTL];

enum {ENERGY_FUSION,ENERGY_ELECTRICAL,ENERGY_MAGNETIC,ENERGY_PLASMA,ENERGY_ANTIMATTER,ENERGY_CHEMICAL,MAX_ENERGIES};
extern char *energy_names[MAX_ENERGIES];

enum {WHEEL_FL,WHEEL_FR,WHEEL_BL,WHEEL_BR,MAX_WHEELS};
extern char *wheel_names[MAX_WHEELS];

enum {RULE_NONE,RULE_NOWEAP,RULE_COMBAT,RULE_NOWEAPCTF,RULE_COMBATCTF,RULE_STUNT,RULE_NOWEAPBOXES,RULE_COMBATBOXES,MAX_RULES};
extern char *rule_names[MAX_RULES];

enum {GAMENET_BAILED,GAMENET_SERVER,GAMENET_CLIENT,GAMENET_BOTGAME,MAX_GAMENET};
extern char *which_game_names[MAX_GAMENET];

enum {
	REGPOINT_BACK,
	REGPOINT_TIREBR,
	REGPOINT_BOTTOM,
	REGPOINT_TIREBL,
	REGPOINT_TIREFR,
	REGPOINT_TIREFL,
	REGPOINT_FRONT,
	REGPOINT_HOOD,
	REGPOINT_RIGHT,
	REGPOINT_ROOF,
	REGPOINT_LEFT,
	REGPOINT_TRUNK,
	REGPOINT_NREGPOINTS,
};
extern char *regpointnames[REGPOINT_NREGPOINTS];

enum {
	WEAP_LASER, // 0
	WEAP_KLONDYKE,
	WEAP_MISSILES,
	WEAP_OIL,
	WEAP_EMB,
	WEAP_ECTO,
	WEAP_DIELECTRIC,
	WEAP_TRANSDUCER,
	WEAP_PLASMA,
	WEAP_RAMJET,

	WEAP_INTAKE, // 10
	WEAP_SPIKES,
	WEAP_BAINITE,
	WEAP_AERODYNE,
	WEAP_FERRITE,
	WEAP_WEAPBOOST,
	WEAP_RPD,
	WEAP_NANO,
	WEAP_RANDOMIZER,
	WEAP_ICRMS,
// 20 (21!?) new weapons
	WEAP_BIGTIRES, // 20
	WEAP_BUZZ,
//	WEAP_CHEMBOOST,
//	WEAP_CHEMVAC,
	WEAP_ENLEECH,
//	WEAP_ENUPTAKE,
//	WEAP_FSNBOO,
	WEAP_FSNCAN,
//	WEAP_GEOSHIELD,
	WEAP_GRAPPLE,
//	WEAP_HOLO,
	WEAP_MAGNET,
	WEAP_MINES,
//	WEAP_NITRO,
//	WEAP_NRGCON,
//	WEAP_POOLBOO,
	WEAP_POUNCER,
	WEAP_PRISM,
//	WEAP_SHLDBSTR,
	WEAP_SONIC,
//	WEAP_THERMCAN,

// 10 new weapons
	WEAP_SLEDGE, // 30
	WEAP_ENERGYWALL,
	WEAP_STASIS,
	WEAP_SUPAJUMP,
	WEAP_FORKLIFT,
	WEAP_BUMPERCAR,
	WEAP_GYROSCOPE,
	WEAP_GRAVITYBOMB,
	WEAP_FLASHER,
	WEAP_CHARGER,
// 1 extra weapon
	WEAP_SWITCHAROO, // 40
// 2 new ones
	WEAP_MINIGUN, // 41
	WEAP_PARTICLE, // 42

	MAX_WEAPKIND  // 43
};
struct weapinfo {
	char *name,*longname,special;
	int shield;
	int persist;
	float /*deploystart,*/deployend;//,deployloop;
	float actstart,actend,actloop;
	float /*resstart,*/resend;//,resloop;
	int energyusage[MAX_ENERGIES];
};
extern struct weapinfo weapinfos[];

tree2 *findtreestrstrrec(tree2 *t,const char *str); // find partial names
void hideregpoints(tree2 *t);
void findcarwheels(tree2 *t,tree2 **wheels);
void loadenergymatrix();

#define TICKRATE 60

void setnextmaterial(char *matname,char *texname);
void drawlines(viewport2 *vp,modelb *m,C32 c);
void drawpoint(viewport2 *vp,pointf3 *p,C32 c);

int intersectline2d(const pointf3 *p0,const pointf3 *p1,const pointf3 *q0,const pointf3 *q1,pointf3 *i,float *t,float *u);
