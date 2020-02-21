#include <m_eng.h>
#include <l_misclibm.h>

#include "enums.h"
#include "utils/json.h"
#include "carclass.h"
#include "hash.h"
#include "timewarp.h"
#include "avaj.h"

// json stuff
// avatar
const C8* avaj::botnames[]={"bill","darci","jr","landslide","power","quantum","shifter","tim"};
const S32 avaj::nbotnames=sizeof(botnames)/sizeof(botnames[0]);

avaj::avaj(const json& js) : paint(0),decal(0),ownerid(-1)
{
	js.get_object_string("playername",playername);
	js.get_object_string("carbodyname",carbodyname);
	js.get_object_int("paint",paint);
	js.get_object_int("decal",decal);
	js.get_object_array_string("weaps",weaps);
	js.get_object_array_int("energies",energies);
	js.get_object_int("ownerid",ownerid);
}

json avaj::save() const
{
	json ret=json::create_object();
	ret.insert_string("playername",playername);
	ret.insert_string("carbodyname",carbodyname);
	ret.insert_int("paint",paint);
	ret.insert_int("decal",decal);
	ret.insert_array_string("weaps",weaps);
	ret.insert_array_int("energies",energies);
	ret.insert_int("ownerid",ownerid);
	return ret;
}

void avaj::randombot()
{
	S32 paintlo=1,painthi=22;
	S32 decallo=0,decalhi=35;
	S32 k=mt_random(nbotnames);
	playername=botnames[k];
	script* sccarnames=n_carclass::getcarlist();
	S32 nsccarnames=sccarnames->num();
	carbodyname=sccarnames->idx(mt_random(nsccarnames));
	delete sccarnames;
	paint=mt_random(painthi-paintlo+1)+paintlo;
	decal=mt_random(decalhi-decallo+1)+decallo;
	ownerid=-1;
}
