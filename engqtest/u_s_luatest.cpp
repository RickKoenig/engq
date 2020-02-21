#define INCLUDE_WINDOWS
#define D2_3D
#define INCLUDE_LUA
#include <m_eng.h>

namespace { // everything inside anonymous namespace is like static

// on the C side
lua_State* L; // master state
S32 mainhandle; //mainhandle[10];	// main handle

// simple NPC manager
struct npc {
	S32 x,y,r;
	string text;
	S32 isvisible;
};
list<npc*> npclist;

// simple dialog manager
struct dialogstrings {
	string line;
	S32 x,y;
	C32 color;
};

struct dialog {
	S32 x,y,xs,ys;
	list<dialogstrings> textlist;
};
list<dialog*> dialoglist;

//S32 timmer;
// on the LUA side
#if 0
table luarefs; 	// array of luascript's init/proc/exit functions
				// built with lua_ref
#endif

// function names to move from the objluascripts global space into the luarefs array
const C8* objfunctions[]={
	"init",
	"proc",
	"exit"
};
#define NOBJFUNCTIONS (sizeof(objfunctions)/sizeof(objfunctions[0]))

// lua cfunctions
int OS_Beep(lua_State* ls)
{
    if (lua_gettop(ls)!=2)
		errorexit("wrong # of args to beep");
	Beep((int)luaL_checknumber(ls, 1), (int)luaL_checknumber(ls, 2));
    return(0);
}

int OS_Logger(lua_State* ls)
{
    if (lua_gettop(ls)<1)
		errorexit("wrong # of args to logger");
	const char* str=lua_tostring (ls, 1);
//	logger("%s",str);
    return 0;
}

int OS_ErrorExitString(lua_State* ls)
{
    if (lua_gettop(ls)<1)
		errorexit("wrong # of args to errorexit");
	const char* str=lua_tostring (ls, 1);
	errorexit("%s",str);
    return 0;
}

int OS_Logger_Indent(lua_State* ls)
{
// 	logger_indent();
    return 0;
}

int OS_Logger_UnIndent(lua_State* ls)
{
 //	logger_unindent();
    return 0;
}

S32 LuaAddScript(lua_State* ls,const C8* scriptname);
int OS_AddScript(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to addscript");
	const C8* sname=luaL_checkstring(ls,1);
	S32 key=LuaAddScript(ls,sname);
	lua_pushnumber(ls,key);
	return 1;
}

void LuaRemoveScript(lua_State* ls,S32 handle);
int OS_RemoveScript(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to removescript");
	S32 key=(S32)luaL_checknumber(ls,1);
	LuaRemoveScript(ls,key);
	return 0;
}

npc* npcadd(S32 x,S32 y,S32 r,const C8* texta);
int OS_NpcAdd(lua_State* ls)
{
    if (lua_gettop(ls)!=4)
		errorexit("addnpc: wrong # of args to addscript");
	npc* anpc=npcadd(	(S32)luaL_checknumber(ls,1),
						(S32)luaL_checknumber(ls,2),
						(S32)luaL_checknumber(ls,3),
						luaL_checkstring(ls,4));
	lua_pushlightuserdata(ls,anpc);
	return 1;
}

npc* npcclicked();
int OS_NpcClicked(lua_State* ls)
{
    if (lua_gettop(ls)!=0)
		errorexit("addnpc: wrong # of args to addscript");
	npc* anpc=npcclicked();
	if (anpc) {
		lua_pushlightuserdata(ls,anpc);
		return 1;
	}
	return 0;
}

void npcremove(npc* handl);
int OS_NpcRemove(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to removescript");
	npc* handl=(npc*)lua_touserdata(ls,1);
	if (!handl)
		errorexit("removenpc: bad handle to free");
	npcremove(handl);
	return 0;
}

void npcsetvis(npc* handl,S32 vis);
int OS_NpcSetVis(lua_State* ls)
{
    if (lua_gettop(ls)!=2)
		errorexit("wrong # of args to npcsetvis");
	npc* handl=(npc*)lua_touserdata(ls,1);
	S32 vis=(S32)lua_tonumber(ls,2);
	if (!handl)
		errorexit("removenpc: bad handle to npcsetvis");
	npcsetvis(handl,vis);
	return 0;
}

dialog* dialogadd(S32 x,S32 y,S32 xs,S32 ys);
int OS_DialogAdd(lua_State* ls)
{
    if (lua_gettop(ls)!=4)
		errorexit("addnpc: wrong # of args to addscript");
	dialog* adial=dialogadd((S32)luaL_checknumber(ls,1),
						(S32)luaL_checknumber(ls,2),
						(S32)luaL_checknumber(ls,3),
						(S32)luaL_checknumber(ls,4));
	lua_pushlightuserdata(ls,adial);
	return 1;
}

void dialogremove(dialog* handl);
int OS_DialogRemove(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to dialogremove");
	dialog* handl=(dialog*)lua_touserdata(ls,1);
	if (!handl)
		errorexit("dialogremove: bad handle to free");
	dialogremove(handl);
	return 0;
}

void dialogaddstr(dialog* handl,S32 x,S32 y,const C8* texta,const pointf3* color);
int OS_DialogAddStr(lua_State* ls)
{
    pointf3 fcolor;
	pointf3* pc;
	if (lua_gettop(ls)==4) {
		pc=0;
	} else if (lua_gettop(ls)==5) {
		lua_pushnumber(ls,1);
		lua_gettable(ls,5);
		fcolor.x=(float)luaL_checknumber(ls,-1);
		lua_pop(ls,1);
		lua_pushnumber(ls,2);
		lua_gettable(ls,5);
		fcolor.y=(float)luaL_checknumber(ls,-1);
		lua_pop(ls,1);
		lua_pushnumber(ls,3);
		lua_gettable(ls,5);
		fcolor.z=(float)luaL_checknumber(ls,-1);
		lua_pop(ls,1);
		pc=&fcolor;
	} else {
		pc=0;
		errorexit("addnpc: wrong # of args to dialogaddstr");
	}
	dialog* handl=(dialog*)lua_touserdata(ls,1);
	dialogaddstr(handl,(S32)luaL_checknumber(ls,2),
						(S32)luaL_checknumber(ls,3),
						luaL_checkstring(ls,4),pc);
	return 0;
}

void dialogclear(dialog* handl);
int OS_DialogClear(lua_State* ls)
{
     if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to dialogclear");
	dialog* handl=(dialog*)lua_touserdata(ls,1);
	if (!handl)
		errorexit("dialogclear: bad handle to clear");
	dialogclear(handl);
	return 0;
}

int OS_DialogMouseInfo(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to dialogmouseinfo");
	dialog* handl=(dialog*)lua_touserdata(ls,1);
	if (MX>=handl->x && MY>=handl->y && MX<handl->x+handl->xs && MY<handl->y+handl->ys) {
		lua_pushinteger(ls,MX-handl->x);
		lua_pushinteger(ls,MY-handl->y);
		lua_pushinteger(ls,wininfo.mleftclicks);
	} else {
		lua_pushinteger(ls,0);
		lua_pushinteger(ls,0);
		lua_pushinteger(ls,0);
	}
 	return 3;
}

int OS_MouseInfo(lua_State* ls)
{
	lua_pushinteger(ls,MX);
	lua_pushinteger(ls,MY);
	lua_pushinteger(ls,wininfo.mleftclicks);
 	return 3;
}

int OS_WaveLoad(lua_State* ls)
{
    const C8* wavename;
	if (lua_gettop(ls)!=1)
		errorexit("addnpc: wrong # of args to waveload");
	wavename=luaL_checkstring(ls,1);
	wavehandle* aw=wave_load(wavename,0);
	if (!aw)
		errorexit("can't find wave to load '%s'",wavename);
	lua_pushlightuserdata(ls,aw);
	return 1;
}

int OS_WaveUnLoad(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to waveunload");
	wavehandle* handl=(wavehandle*)lua_touserdata(ls,1);
	if (!handl)
		errorexit("waveunload: bad wavehandle to free");
	wave_unload(handl);
	return 0;
}

int OS_PlayASound(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to playasound");
	wavehandle* handl=(wavehandle*)lua_touserdata(ls,1);
	if (!handl)
		errorexit("waveunload: bad wavehandle");
	playasound(handl);
	return 0;
}

int OS_WavePlay(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to playasound");
	wavehandle* wh=(wavehandle*)lua_touserdata(ls,1);
	if (!wh)
		errorexit("waveplay: bad wavehandle");
	soundhandle* sh=wave_play(wh,1);
	if (!sh)
		errorexit("waveplay: bad soundhandle");
	lua_pushlightuserdata(ls,sh);
	return 1;
}

int OS_SoundStatus(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to soundstatus");
	soundhandle* shandl=(soundhandle*)lua_touserdata(ls,1);
	if (!shandl)
		errorexit("soundstatus: bad soundhandle");
	S32 ret=sound_status(shandl);
	lua_pushinteger(ls,ret);
	return 1;
}

int OS_SoundFree(lua_State* ls)
{
    if (lua_gettop(ls)!=1)
		errorexit("wrong # of args to soundfree");
	soundhandle* shandl=(soundhandle*)lua_touserdata(ls,1);
	if (!shandl)
		errorexit("soundfree: bad soundhandle");
	sound_free(shandl);
	return 0;
}

const luaL_reg OS_Lib[] =
{
    {"logstring",		OS_Logger},
	{"loggerindent",	OS_Logger_Indent},
	{"loggerunindent",	OS_Logger_UnIndent},
	{"errorexitstring",	OS_ErrorExitString},
    {"addscript",		OS_AddScript},
    {"removescript",	OS_RemoveScript},
    {"npcadd",			OS_NpcAdd},
    {"npcremove",		OS_NpcRemove},
    {"npcsetvis",		OS_NpcSetVis},
	{"npcclicked",		OS_NpcClicked},
	{"dialogadd",		OS_DialogAdd},
    {"dialogremove",	OS_DialogRemove},
	{"dialogaddstr",	OS_DialogAddStr},
	{"dialogclear",		OS_DialogClear},
	{"dialogmouseinfo",	OS_DialogMouseInfo},
	{"mouseinfo",		OS_MouseInfo},
	{"waveload",		OS_WaveLoad},
	{"waveunload",		OS_WaveUnLoad},
	{"playasound",		OS_PlayASound},
	{"waveplay",		OS_WavePlay},
	{"soundstatus",		OS_SoundStatus},
	{"soundfree",		OS_SoundFree},
	{NULL, NULL}
};

void LuaOpenCLib(lua_State* ls)
{
    luaL_register(ls,"OS",OS_Lib);
	lua_settop(L,0); // clean up stack after a luaL_openlib
}

S32 luaPanic(lua_State* ls)
{
	const char* str=lua_tostring (ls, 1);
	errorexit("%s",str);
	return 0;
}

void LuaOpenLuaLib(lua_State* ls)
{
 	script sc("lualibs.txt");
	while(1) {
		const C8* str=sc.read();
		if (!str || !str[0])
			break;
		S32 status = luaL_dofile(L,str);
		if (status)
			errorexit("syntax error in '%s'\n",str);
		lua_settop (L,0); // clean up stack after a luaL_openlib
	}
}

// builds the object table that holds the init/proc/exit structures(table)
void LuaInitManager(lua_State* ls)
{
	lua_newtable(ls); // luarefs
	lua_setglobal(ls,"luarefs"); // copy table to luarefs
}

void LuaExitManager(lua_State* ls)
{
//	logger("in LuaExitManager\n");
// check to see if table 'luarefs' is empty
	lua_getglobal(ls,"luarefs"); 	// luarefs
	lua_pushnil(ls);  				// luarefs,firstkey
    S32 isempty=1;
	while (lua_next(ls,-2)) {
// uses 'key' (at index -2) and 'value' (at index -1)
		logger("not empty %s - %s\n",lua_typename(ls,lua_type(ls,-2)),lua_typename(ls,lua_type(ls,-1)));
		if (lua_type(ls,-2)==LUA_TNUMBER && lua_type(ls,-1)==LUA_TNUMBER)
			logger("%f - %f\n",lua_tonumber(ls,-2),lua_tonumber(ls,-1));
		else if (lua_type(ls,-2)==LUA_TNUMBER)
			logger("%f - %s\n",lua_tonumber(ls,-2),lua_typename(ls,lua_type(ls,-1)));
		lua_pop(L, 1);
		isempty=0;
	}
	if (isempty)
		logger("is empty!\n");
	lua_pop(ls,1);
	logger("LuaProcScripts stack %d\n",lua_gettop(ls));
	lua_pushnil(ls);
	lua_setglobal(ls,"luarefs");
	logger("LuaExitManager stack %d\n",lua_gettop(ls));
}

// call luarefs[handle]'s named function if it exists
void LuaCallObjectHandle(lua_State* ls,S32 handle,const C8* funname)
{
//	logger("in LuaCallObjecthandle %d '%s'\n",handle,funname);
	lua_getglobal(ls,"luarefs"); // luarefs
	lua_pushnumber(ls,handle); // luarefs, *it
	lua_gettable(ls,-2); // luarefs, luarefs[*it]
	if (!lua_istable(ls,-1)) {
		logger("bad lua object handle %d detected in LuaCallObjecthandle\n",handle);
		lua_pop(ls,2); // luarefs
		logger("LuaCallObjecthandle stack %d\n",lua_gettop(ls));
		return;
	}
	lua_pushstring(ls,funname); // luarefs, luarefs[*it], 'name proc'
	lua_gettable(ls,-2); // luarefs, luarefs[*it], function proc
	if (lua_isfunction(ls,-1)) { // call function
//		lua_pushlightuserdata(ls,(void*)handle);
		lua_call(ls, 0, 1);     // luarefs, luarefs[*it], result
	}
	S32 avar=(S32)lua_tonumber (ls,-1); // luarefs, luarefs[*it], result
	lua_pop(ls,2); // luarefs
	logger("return value from luarefs[%d].%s is %d\n",handle,funname,avar);
	lua_pop(ls,1); // empty
//	logger("LuaCallObjecthandle stack %d\n",lua_gettop(ls));
}

S32 LuaAddScript(lua_State* ls,const C8* scriptname)
{
	S32 status = luaL_dofile(ls,scriptname);
	if (status)
		errorexit("syntax error in '%s'\n",scriptname);
	lua_getglobal(ls,"luarefs"); // luarefs
	lua_newtable(ls);	// luarefs, luarefs[key]
	U32 i;
	for (i=0;i<NOBJFUNCTIONS;++i) { // get all functions copied over to luarefs (OOP)
		lua_pushstring(L,objfunctions[i]); // luarefs,  luarefs[key], function name
		lua_getglobal(ls,objfunctions[i]); // luarefs,  luarefs[key], function name, actual function
		lua_settable(ls,-3);	// luarefs,  luarefs[key]
// init/proc/exit=nil // wipe out old copy of function (for cleanliness)
		lua_pushnil(ls); // luarefs,  luarefs[key], nil
		lua_setglobal(ls,objfunctions[i]); // luarefs, luarefs[key]
	}
	S32 key=luaL_ref(ls,-2); // luarefs
	if (key<0)
		errorexit("bad lual ref '%s' %d\n",scriptname,key);
	lua_pop(ls,1); // empty
//	logger("gettop at end of initlevel is %d, key is %d\n",lua_gettop(ls),key);
	LuaCallObjectHandle(ls,key,"init");
	return key;
}

void LuaRemoveScript(lua_State* ls,S32 handle)
{
//	logger("in LuaRemoveScript with handle %d\n",handle);
	LuaCallObjectHandle(ls,handle,"exit"); // call the exit function if it exists
	lua_getglobal(ls,"luarefs"); // luarefs
	lua_pushnumber(ls,handle); // luarefs, handle
	lua_gettable(ls,-2); // luarefs, luarefs[handle]
	if (!lua_istable(ls,-1)) {
		logger("bad lua object handle %d detected in LuaRemoveScript\n",handle);
		logger("LuaRemoveScript stack %d\n",lua_gettop(ls));
		lua_pop(ls,2);
		return;
	}
	lua_pop(ls,1); // luarefs
	luaL_unref(ls,-1,handle); // remove handle from the 'luarefs' table
	lua_pop(ls,1);
//	logger("LuaRemoveScript stack %d\n",lua_gettop(ls));
}

void LuaProcScripts(lua_State* ls,const C8* funname="proc")
{
//	logger("in LuaProcScripts '%s'\n",funname);
	logger_indent();
	lua_getglobal(ls,"luarefs"); 	// luarefs
	lua_pushnil(ls);  				// luarefs,firstkey
    while (lua_next(ls,-2)) { // walk thru the table 'luarefs', call funct if table found
// uses 'key' (at index -2) and 'value' (at index -1)
/*		logger("%s - %s\n",lua_typename(ls,lua_type(ls,-2)),lua_typename(ls,lua_type(ls,-1)));
		if (lua_type(ls,-2)==LUA_TNUMBER && lua_type(ls,-1)==LUA_TNUMBER)
			logger("%f - %f\n",lua_tonumber(ls,-2),lua_tonumber(ls,-1));
		else if (lua_type(ls,-2)==LUA_TNUMBER)
			logger("%f - %s\n",lua_tonumber(ls,-2),lua_typename(ls,lua_type(ls,-1)));
*/
//		void* hnd=(void*)lua_tointeger(ls,-2);
		if (lua_type(ls,-1)==LUA_TTABLE) { // if value is a table, I bet it has init/proc/exit

			lua_pushstring(ls,funname); // luarefs, walk, luarefs[walk], 'name proc'
			lua_gettable(ls,-2); // luarefs, walk, luarefs[walk], function proc
			if (lua_isfunction(ls,-1)) { // call function
//				lua_pushlightuserdata(ls,hnd );
//				logger("push for proc '%d'\n",hnd);
				lua_call(ls, 0, 1);     // luarefs, walk, luarefs[walk], result
			}
//			S32 avar=(S32)lua_tonumber (ls,-1); // luarefs, walk, luarefs[walk], result
			lua_pop(ls,1); // luarefs, walk, luarefs[walk]
//			logger("return value from luarefs[%f].%s is %d\n",lua_tonumber(ls,-2),funname,avar);
		}
		lua_pop(L, 1); // luarefs, walk
	}
	lua_pop(ls,1);
	logger_unindent();
//	logger("LuaProcScripts stack %d\n",lua_gettop(ls));
}

/*void arrtest(lua_State* ls)
{
	logger("in arrtest\n");
	lua_getglobal(ls,"testnpcnames"); 	// testnpcnames
	lua_pushnil(ls);  				// testnpcnames,firstkey
    while (lua_next(ls,-2)) { // walk thru the table 'luarefs', call funct if table found
// uses 'key' (at index -2) and 'value' (at index -1)
		logger("%s - %s\n",lua_typename(ls,lua_type(ls,-2)),lua_typename(ls,lua_type(ls,-1)));
		if (lua_type(ls,-2)==LUA_TNUMBER && lua_type(ls,-1)==LUA_TNUMBER)
			logger("%f - %f\n",lua_tonumber(ls,-2),lua_tonumber(ls,-1));
		else if (lua_type(ls,-2)==LUA_TNUMBER && lua_type(ls,-1)==LUA_TSTRING)
			logger("%f - '%s'\n",lua_tonumber(ls,-2),lua_tostring(ls,-1));
		else if (lua_type(ls,-2)==LUA_TNUMBER)
			logger("%f - %s\n",lua_tonumber(ls,-2),lua_typename(ls,lua_type(ls,-1)));
		lua_pop(L, 1); // testnpcnames, walk
	}
	lua_pop(ls,1);
	logger("arrtest stack %d\n",lua_gettop(ls));
}
*/
void npcinit()
{
	if (!npclist.empty())
		errorexit("npcinit: npclist not empty\n");
}

void npcexit()
{
	if (!npclist.empty())
		errorexit("npcinit: npclist not empty\n");
}

npc* npcadd(S32 x,S32 y,S32 r,const C8* texta)
{
	npc* anpc=new npc;
	anpc->x=x;
	anpc->y=y;
	anpc->r=r;
	anpc->text=texta;
	anpc->isvisible=0;
	npclist.push_back(anpc);
	return anpc;
}

void npcremove(npc* handl)
{
	npclist.remove(handl);
	delete handl;
}

void npcsetvis(npc* handl,S32 vis)
{
	handl->isvisible=vis;
}

void npcdraw()
{
	list<npc*>::iterator it=npclist.begin(),e=npclist.end();
	while(it!=e) {
		const npc& rnpc=**it;
		if (rnpc.isvisible) {
			clipcircle32(B32,rnpc.x,rnpc.y,rnpc.r,C32BLACK);
			S32 len=rnpc.text.size()<<2;
			outtextxy32(B32,rnpc.x-len,rnpc.y-4,C32WHITE,rnpc.text.c_str());
		}
		++it;
	}
}

npc* npcclicked()
{
	if (!wininfo.mleftclicks)
		return 0;
	list<npc*>::iterator it=npclist.begin(),e=npclist.end();
	while(it!=e) {
		npc* anpc=*it;
		if (anpc->isvisible) {
			S32 dx=MX-anpc->x;
			S32 dy=MY-anpc->y;
			if (dx*dx+dy*dy<=anpc->r*anpc->r)
				return anpc;
		}
		++it;
	}
	return 0;
}

// simple dialog manager
void dialoginit()
{
	if (!dialoglist.empty())
		errorexit("dialoginit: dialoglist not empty\n");
}

void dialogexit()
{
	if (!dialoglist.empty())
		errorexit("dialoginit: dialoglist not empty\n");
}

dialog* dialogadd(S32 x,S32 y,S32 xs,S32 ys)
{
//	logger("in dialog add\n");
	dialog* adialog=new dialog;
	adialog->x=x;
	adialog->y=y;
	adialog->xs=xs;
	adialog->ys=ys;
	dialoglist.push_back(adialog);
	return adialog;
}

void dialogremove(dialog* handl)
{
//	logger("in dialog remove\n");
	dialoglist.remove(handl);
	delete handl;
}

void dialogaddstr(dialog* handl,S32 x,S32 y,const C8* str,const pointf3* color)
{
	dialogstrings ds;
	ds.x=x;
	ds.y=y;
	ds.line=str;
	if (color)
		ds.color=pointf3toC32(color);
	else
		ds.color=C32WHITE;
	handl->textlist.push_back(ds);
}

void dialogclear(dialog* handl)
{
	handl->textlist.clear();
}

void dialogdraw()
{
	list<dialog*>::iterator it=dialoglist.begin(),e=dialoglist.end();
	while(it!=e) {
		const dialog& rdialog=**it;
		cliprect32(B32,rdialog.x,rdialog.y,rdialog.xs,rdialog.ys,C32BLUE);
		list<dialogstrings>::const_iterator it2=rdialog.textlist.begin(),e2=rdialog.textlist.end();
		while(it2!=e2) {
			const dialogstrings& rdialog2=*it2;
			outtextxy32(B32,rdialog.x+rdialog2.x,rdialog.y+rdialog2.y,rdialog2.color,rdialog2.line.c_str());
			++it2;
		}
		++it;
	}
}

// some test npc handles
//npc* npchandles[10];

} // end anonymous namespace

void luatestinit()
{
// init C state
	video_setupwindow(512,384);
//	wininfo.fpswanted=5;
//	logger("start test lua\n");
	logger_indent();
// init npc system
	npcinit();
	dialoginit();
// init lua system
	L = luaL_newstate(); 		// luastate
	lua_atpanic (L,luaPanic);	// panic function
 	pushandsetdir("luascripts");
   if (L) {
		luaL_openlibs(L);	// system libs
        LuaOpenCLib(L);		// user c libs
        LuaOpenLuaLib(L);	// user lua libs
	}
// init lua manager
	LuaInitManager(L);	// creates the 'luarefs' table
// init game
/*	S32 i;
	for (i=0;i<10;++i)
		mainhandle[i]=LuaAddScript(L,"default.lua");
	for (i=0;i<10;++i)
		LuaRemoveScript(L,mainhandle[i]);	// should clear out everything, mainhandle's 'exit' function will do it
	for (i=0;i<10;++i)
		mainhandle[i]=LuaAddScript(L,"default.lua");
	for (i=0;i<10;++i)
		LuaRemoveScript(L,mainhandle[i]);	// should clear out everything, mainhandle's 'exit' function will do it
	for (i=0;i<10;++i)
		mainhandle[i]=LuaAddScript(L,"default.lua"); */
	mainhandle=LuaAddScript(L,"default.lua");
//	arrtest(L);
	logger_unindent();
//	timmer=0;
/*	S32 i;
	for (i=0;i<10;++i) {
		C8 sd[50];
		sprintf(sd,"npc%d",i+1);
		npchandles[i]=npcadd(mt_random(WX),mt_random(WY),20,sd);
	} */
}

void luatestproc()
{
//	logger_indent();
// abort game
	if (KEY==K_ESCAPE)
		popstate();
//	if (timmer==50) {
//		popstate();
//		return;
//	}
// run game
	LuaProcScripts(L);
}

void luatestdraw2d()
{
// show game
//	video_lock();
	clipclear32(B32,C32GREEN);
	outtextxy32(B32,130,10,C32WHITE,"Lua test, look in logfile");
	npcdraw();
	dialogdraw();
//	video_unlock();
//	logger_unindent();
//	++timmer;
}

void luatestexit()
{
//	logger_indent();
// destroy game
	LuaRemoveScript(L,mainhandle);
/*	S32 i;
	for (i=0;i<10;++i)
		npcremove(npchandles[i]); */
	LuaExitManager(L); // check to see if table is empty and sets table to nil
// destroy lua system
	lua_close(L);
// destroy npc system
	npcexit();
	dialogexit();
// exit C state
//	logger_unindent();
	popdir();
	logger("end test lua\n");
}




////////////// for reference /////////////////////////////
#if 0
#if 0
S32 LuaInitObjectHandle(lua_State* ls,const C8* scriptname)
{
	S32 status = luaL_dofile(ls,scriptname);
	if (status)
		errorexit("syntax error in '%s'\n",scriptname);
// luarefs[nluarefs+1]=init/proc/exit
//		lua_getglobal(ls,"luarefs");	// luarefs (1 on stack) luarefs, a table
		lua_pushnumber(ls,nluarefs+1); // luarefs, i+1, luarefs[i+1]
		lua_newtable(ls);	// luarefs, i+1, luarefs[i+1]
		U32 i;
		for (i=0;i<NOBJFUNCTIONS;++i) { // get all functions copied over to luarefs (OOP)
			lua_pushstring(L,objfunctions[i]); // luarefs, i+1, luarefs[i+1], function name
			lua_getglobal(ls,objfunctions[i]); // luarefs, i+1, luarefs[i+1], function name, actual function
			lua_settable(ls,-3);	// luarefs, i+1, luarefs[i+1]
// init/proc/exit=nil // wipe out old copy of function (for cleanliness)
			lua_pushnil(ls); // luarefs, i+1, luarefs[i+1], nil
			lua_setglobal(ls,objfunctions[i]); // luarefs, i+1, luarefs[i+1]
		}
		++nluarefs;
		lua_settable(ls,-3);
	}
	lua_setglobal(ls,"luarefs"); // copy table to luarefs
	logger("gettop at end of initlevel is %d\n",lua_gettop(ls));
	LuaCallObject(ls,"init");
}
// load up all objects, and transfer functions into an array,
// ( probably can be done a better way, but hey if it works... )
void LuaInitObject(lua_State* ls,const C8* levellist)
{
	lua_newtable(ls); // luarefs
	script sc(levellist);
	while(1) {
		const C8* str=sc.read();
		if (!str || !str[0])
			break;
		S32 hnd=LuaInitObjectHandle(ls,str)
	}
	lua_setglobal(ls,"luarefs"); // copy table to luarefs
	logger("gettop at end of initlevel is %d\n",lua_gettop(ls));
	LuaCallObject(ls,"init");
}
#endif
#if 0
// load up all objects, and transfer functions into an array,
// ( probably can be done a better way, but hey if it works... )
void LuaInitObject(lua_State* ls,const C8* levellist)
{
	lua_newtable(ls); // luarefs
	script sc(levellist);
	while(1) {
		const C8* str=sc.read();
		if (!str || !str[0])
			break;
		S32 status = luaL_dofile(ls,str);
		if (status)
			errorexit("syntax error in '%s'\n",str);
// luarefs[nluarefs+1]=init/proc/exit
//		lua_getglobal(ls,"luarefs");	// luarefs (1 on stack) luarefs, a table
		lua_pushnumber(ls,nluarefs+1); // luarefs, i+1, luarefs[i+1]
		lua_newtable(ls);	// luarefs, i+1, luarefs[i+1]
		U32 i;
		for (i=0;i<NOBJFUNCTIONS;++i) { // get all functions copied over to luarefs (OOP)
			lua_pushstring(L,objfunctions[i]); // luarefs, i+1, luarefs[i+1], function name
			lua_getglobal(ls,objfunctions[i]); // luarefs, i+1, luarefs[i+1], function name, actual function
			lua_settable(ls,-3);	// luarefs, i+1, luarefs[i+1]
// init/proc/exit=nil // wipe out old copy of function (for cleanliness)
			lua_pushnil(ls); // luarefs, i+1, luarefs[i+1], nil
			lua_setglobal(ls,objfunctions[i]); // luarefs, i+1, luarefs[i+1]
		}
		++nluarefs;
		lua_settable(ls,-3);
	}
	lua_setglobal(ls,"luarefs"); // copy table to luarefs
	LuaCallObject(ls,"init");
	logger("gettop at end of initlevel is %d\n",lua_gettop(ls));
}
#endif

#if 0
// proc all objects
void LuaProcObject(lua_State* ls)
{
	LuaCallObject(ls,"proc");
}

// destroy game
void LuaDestroyObject(lua_State* ls)
{
// call all objects 'exit' function
	LuaCallObject(ls,"exit");
// destroy lua array of objects
	lua_pushnil(ls);
	lua_setglobal(ls,"luarefs");
// destroy c array of object references
	luarefs.clear();
}
#endif

#endif
