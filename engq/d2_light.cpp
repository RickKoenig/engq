#define D2_3D
#include <m_eng.h>
#include "m_perf.h"

struct li lightinfo={1,0};

// for now, lights will affect all viewports equally
// global

void addlighttolist(tree2* t)
{
	S32 i;
	if (!(t->flags&TF_ISLIGHT)) {
		logger("WARNING: light '%s' is not a light\n",t->name.c_str());
		return;
	}
	for (i=0;i<lightinfo.numlights;i++)
		if (lightinfo.lightlist[i]==t) {
			logger("WARNING: light '%s' already in list\n",t->name.c_str());
			return;
		}
	if (t->flags&TF_ISAMBLIGHT)
		for (i=0;i<lightinfo.numlights;i++)
			if (lightinfo.lightlist[i]->flags&TF_ISAMBLIGHT) {
				logger("WARNING: amblight '%s' ignored because of amblight '%s'\n",
					lightinfo.lightlist[i]->name.c_str(),
					t->name.c_str());
//				lightinfo.lightlist[i]=t;
				return;
			}
	if (lightinfo.numlights==MAXLIGHTS)
		errorexit("too many lights");
	lightinfo.lightlist[lightinfo.numlights]=t;
	lightinfo.numlights++;
}

void removelightfromlist(tree2* t)
{
	S32 i;
	for (i=0;i<lightinfo.numlights;i++)
		if (lightinfo.lightlist[i]==t) {
			lightinfo.lightlist[i]=lightinfo.lightlist[lightinfo.numlights-1];
//			lightinfo.lightcolors[i]=zerov;
			lightinfo.numlights--;
			if (lightinfo.numlights==0)
				logger("numlights now at 0\n");
			return;
		}
	logger("WARNING ?, light '%s' not in list\n",t->name.c_str());
}

/*static void resetcalclightsonce(struct tree* t)
{
	S32 i,j;
	if (t->mod && t->mod->vertnorms) {
		for (j=0;j<t->mod->nmat;j++) {
			if (t->mod->mats[j].msflags&SMAT_CALCLIGHTS) {
				if (t->flags&TF_CALCLIGHTSONCE)
					t->mod->mats[j].msflags&=~SMAT_CALCLIGHTS;
			}
		}
	}
	for (i=0;i<t->nchildren;i++)
		resetcalclightsonce(t->children[i]);
}
*/

static void dolightssrc()
{
//	struct pointf3 dir;
	S32 i;
	tree2* t;
	if (lightinfo.dodefaultlights || !lightinfo.numlights || !lightinfo.uselights) {
		static bool once;
	    lightinfo.ndirlights=1;
		if (!once) {
            lightinfo.ambcolor.x=.75f;
            lightinfo.ambcolor.y=.75f;
            lightinfo.ambcolor.z=.75f;
            lightinfo.ambcolor.w=1;
            lightinfo.deflightpos[0].x=0;
            lightinfo.deflightpos[0].y=0;
            lightinfo.deflightpos[0].z=0;//-10.0f;
            lightinfo.deflightrot[0].x=0;//PI/4;
            lightinfo.deflightrot[0].y=0;
            lightinfo.deflightrot[0].z=0;
    //		worldlightdirs[0].x=.866f;
    //		worldlightdirs[0].y=.5f;
    //		worldlightdirs[0].z=0;
            lightinfo.lightcolors[0].x=.25f;
            lightinfo.lightcolors[0].y=.25f;
            lightinfo.lightcolors[0].z=.25f;
            lightinfo.lightcolors[0].w=1;
/*            lightinfo.worldlightdirs[1].x=-.433f;
            lightinfo.worldlightdirs[1].y=.5f;
            lightinfo.worldlightdirs[1].z=.75f;
            lightinfo.lightcolors[1].x=.35f;
            lightinfo.lightcolors[1].y=.95f;
            lightinfo.lightcolors[1].z=.35f;
            lightinfo.worldlightdirs[2].x=-.433f;
            lightinfo.worldlightdirs[2].y=.5f;
            lightinfo.worldlightdirs[2].z=-.75f;
            lightinfo.lightcolors[2].x=.35f;
            lightinfo.lightcolors[2].y=.35f;
            lightinfo.lightcolors[2].z=.95f; */
            once=true;
		}
		identmat4(&lightinfo.light2world[0]);
		buildrottrans3d(&lightinfo.deflightrot[0],&lightinfo.deflightpos[0],&lightinfo.light2world[0]);
//		normalize3d(&lightinfo.worldlightdirs[0],&lightinfo.worldlightdirs[0]);
//		normalize3d(&lightinfo.worldlightdirs[1],&lightinfo.worldlightdirs[1]);
//		normalize3d(&lightinfo.worldlightdirs[2],&lightinfo.worldlightdirs[2]);
	} else {
		lightinfo.ndirlights=0;
		lightinfo.ambcolor=zerov;
		bool hasamb=false;
		for (i=0;i<lightinfo.numlights;i++) {
			t=lightinfo.lightlist[i];
			if (t->flags&TF_ISAMBLIGHT && !hasamb) { // first ambient
				lightinfo.ambcolor.x=t->lightcolor.x*t->intensity;
				lightinfo.ambcolor.y=t->lightcolor.y*t->intensity;
				lightinfo.ambcolor.z=t->lightcolor.z*t->intensity;
//				lightinfo.ambcolor.w=t->lightcolor.w;
				hasamb=true;
			} else if (lightinfo.ndirlights==0) { // first directional
				lightinfo.lightcolors[lightinfo.ndirlights].x=t->lightcolor.x*t->intensity;
				lightinfo.lightcolors[lightinfo.ndirlights].y=t->lightcolor.y*t->intensity;
				lightinfo.lightcolors[lightinfo.ndirlights].z=t->lightcolor.z*t->intensity;
//				lightinfo.lightcolors[lightinfo.ndirlights].w=t->lightcolor.w;
//				dir.x=0;
//				dir.y=0;
//				dir.z=1;
//				xformdir(&t->o2w,&dir,&lightinfo.worldlightdirs[lightinfo.ndirlights]);
				lightinfo.light2world[lightinfo.ndirlights]=t->o2w;
				lightinfo.ndirlights++;
			}
		}
	}
}

// static bool didlights;

void dolights()
{
	if (!lightinfo.uselights)
		return;
//	if (didlights)
//		return;
	perf_start(DOLIGHTS);
	dolightssrc();
//	dolightsdestrec(t);
	perf_end(DOLIGHTS);
//	didlights=true;
}

/*void donelights()
{
//	didlights=false;
}
*/
