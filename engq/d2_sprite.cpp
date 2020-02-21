#define D2_3D // enable 3d subsection of engq
#include <m_eng.h>

namespace {
S32 zs,pcs; // zbuffer save, pers correct save

viewport2 svp;
bool addmode;

tree2* spttreenorm;
tree2* spttreefont;
tree2* spttreetri;

textureb* sptwhitetex;

uv normuv0,normuv1;
uv fontuv0,fontuv1;
float normhandx,normhandy;
float fonthandx,fonthandy;
const float TILETEST=1.0;

pointf3 rectverts[]={
	{0, 0,0},
	{1, 0,0},
	{0,-1,0},
	{1,-1,0},
};

uv rectuvs[4]={
	{SPT_U0,SPT_V0},
	{TILETEST*SPT_U1,SPT_V0},
	{SPT_U0,TILETEST*SPT_V1},
	{TILETEST*SPT_U1,TILETEST*SPT_V1},
};

face rectfaces[]={
	{{0,1,2},0},
	{{2,1,3},0},
};

} // end namespace

void video_sprite_begin(U32 sx,U32 sy,U32 flags,C32 backcolor)
{
	sptwhitetex=texture_create("white.pcx");
	if (texture_getrc(sptwhitetex)==1) {
		pushandsetdir("common");
		sptwhitetex->load();
		popdir();
	}
// setup spt and font vert and uv (cache)
	normuv0.u=SPT_U0;
	normuv0.v=SPT_V0;
	normuv1.u=SPT_U1;
	normuv1.v=SPT_V1;
	fontuv0.u=SPT_U0;
	fontuv0.v=SPT_V0;
	fontuv1.u=SPT_U1;
	fontuv1.v=SPT_V1;
	normhandx=0;
	normhandy=0;
	fonthandx=0;
	fonthandy=0;
// setup sprite viewport
	memset(&svp,0,sizeof(svp));
	svp.backcolor=backcolor;
	svp.flags=flags;
	svp.zfront=-100;
	svp.zback=100;
	svp.xstart=0;
	svp.ystart=0;
	svp.xres=WX;
	svp.yres=WY;
	svp.xsrc=sx;
	svp.ysrc=sy;
	svp.flags=flags;
	svp.isortho=true;
	svp.ortho_size=(float)sy; // ortho doesn't use zoom, it uses ortho_size
	svp.camtrans.x=(float)sx*.5f;
	svp.camtrans.y=-svp.ortho_size*.5f;
// help out software mode, no pers correct, no zbuffer
	zs=video3dinfo.zenable;
	video3dinfo.zenable=0;
	pcs=video3dinfo.perscorrect;
	video3dinfo.perscorrect=0;
	video_setviewport(&svp);
// build trees for 3 types of sprites
	modelb* mod;
// normal
	spttreenorm=new tree2("spttree");
	mod=model_create("sptmod");
	if (model_getrc(mod)==1) {
		mod->copyverts(rectverts,4); 
		mod->copyuvs0(rectuvs,4);
		textureb* texmat=0;//texture_create("maptestnck.tga");
		mod->addmat("tex",SMAT_HASTEX|SMAT_CLAMP,texmat,&F32WHITE,30,2,4);
		mod->addfaces(rectfaces,2,0);
		mod->close();
	}
	spttreenorm->setmodel(mod);
// font
	spttreefont=new tree2("fonttree");
	mod=model_create("fontmod");
	if (model_getrc(mod)==1) {
		mod->copyverts(rectverts,4); 
		mod->copyuvs0(rectuvs,4);
		textureb* texmat=0;//texture_create("maptestnck.tga");
		mod->addmat("texadd",SMAT_HASTEX|SMAT_CLAMP|SMAT_FONT,texmat,&F32WHITE,30,2,4);
		mod->addfaces(rectfaces,2,0);
		mod->close();
	}
	spttreefont->setmodel(mod);
// tri
	spttreetri=new tree2("tritree");
	mod=model_create("trimod");
	if (model_getrc(mod)==1) {
		mod->copyverts(rectverts,3); 
		mod->copyuvs0(rectuvs,3);
		textureb* texmat=0;//texture_create("maptestnck.tga");
		mod->addmat("tex",SMAT_HASTEX|SMAT_CLAMP,texmat,&F32WHITE,30,1,3);
		mod->addfaces(rectfaces,1,0);
		mod->close();
	}
	spttreetri->setmodel(mod);
}

// addmode true is good for some fonts
void video_sprite_setaddmode(bool am)
{
	addmode=am;
}

void video_sprite_draw(textureb* sptex,const pointf3& col,float x0,float y0,
	float xs,float ys,
	float handx,float handy,
	float rot,
	float u0,float v0,
	float u1,float v1)
{
//	logger("in sprite draw with tex name '%s', x %f, y %f, handx %f, handy %f, rot %f, u0 %f, v0 %f, u1 %f, v1 %f\n",
//		sptex->name.c_str(),x0,y0,handx,handy,rot,u0,v0,u1,v1);
	bool notexmode = !sptex;
	if (notexmode)
//		return;
		sptex = sptwhitetex;
	if (!sptex)
		if (!sptex || !sptex->origsize.x || !sptex->origsize.y)
			return;
	if (!xs)
		xs=(float)sptex->origsize.x;
	if (!ys)
		ys=(float)sptex->origsize.y;
	tree2* ttu = addmode ? spttreefont : spttreenorm;
	if (notexmode)
		ttu = spttreefont;
	modelb* ttumod = ttu->mod;
	ttu->trans.x=x0;
	ttu->trans.y=-y0;
	ttu->scale.x=xs;
	ttu->scale.y=ys;
	ttu->scale.z=1;
	ttu->rot.z=rot;

	video_buildworldmats(ttu);
	ttumod->changetex(sptex);
	ttumod->mats[0].color=col;
	bool doupdateuvs=false;
	bool doupdateverts=false;
	if (ttu == spttreefont) {
		if (u0!=fontuv0.u || v0!=fontuv0.v || u1!=fontuv1.u || v1!=fontuv1.v) { // alter the vertex uv data of the model
			doupdateuvs=true;
			fontuv0.u=u0;
			fontuv0.v=v0;
			fontuv1.u=u1;
			fontuv1.v=v1;
		}
		if (handx!=fonthandx || handy!=fonthandy) {
			doupdateverts=true;
			fonthandx=handx;
			fonthandy=handy;
		}
	} else {
		if (u0!=normuv0.u || v0!=normuv0.v || u1!=normuv1.u || v1!=normuv1.v) { // alter the vertex uv data of the model
			doupdateuvs=true;
			normuv0.u=u0;
			normuv0.v=v0;
			normuv1.u=u1;
			normuv1.v=v1;
		}
		if (handx!=normhandx || handy!=normhandy) {
			doupdateverts=true;
			normhandx=handx;
			normhandy=handy;
		}
	}
	if (doupdateuvs) {
		uv newuvs[4];
		newuvs[0].u=u0;
		newuvs[0].v=v0;
		newuvs[1].u=u1;
		newuvs[1].v=v0;
		newuvs[2].u=u0;
		newuvs[2].v=v1;
		newuvs[3].u=u1;
		newuvs[3].v=v1;
		ttumod->copyuvs0(newuvs,4);
	}
	if (doupdateverts) {
		pointf3 newverts[4];
		newverts[0].x=-handx;
		newverts[0].y=handy;
		newverts[0].z=0;
		newverts[1].x=1-handx;
		newverts[1].y=handy;
		newverts[1].z=0;
		newverts[2].x=-handx;
		newverts[2].y=handy-1;
		newverts[2].z=0;
		newverts[3].x=1-handx;
		newverts[3].y=handy-1;
		newverts[3].z=0;
		ttumod->copyverts(newverts,4);
	}
	if (doupdateuvs || doupdateverts)
		ttumod->update();
	video_drawscene(ttu);
}

void video_tri_draw(textureb* sptex,const pointf3& col,const pointf2* xys,const uv* uvs)
{
/*	logger("in tri draw with tex name '%s', "
		"p0x %f, p0y %f, p1x %f, p1y %f, p2x %f, p2y %f, "
		"u0 %f, v0 %f, u1 %f, v1 %f, u2 %f, v2 %f\n",
		sptex->name.c_str(),
		xys[0].x,xys[0].y,xys[1].x,xys[1].y,xys[2].x,xys[2].y,
		uvs[0].u,uvs[0].v,uvs[1].u,uvs[1].v,uvs[2].u,uvs[2].v); */
	if (!sptex || !sptex->origsize.x || !sptex->origsize.y)
		return;
	tree2* ttu = spttreetri;
	modelb* ttumod = ttu->mod;
	pointf3 vts[3];
	S32 i;
	for (i=0;i<3;++i) {
		vts[i].x=xys[i].x;
		vts[i].y=-xys[i].y;
		vts[i].z=0;
		vts[i].w=0;
	}
	ttumod->copyverts(vts,3);
	ttumod->copyuvs0(uvs,3);
	ttumod->update();
	video_buildworldmats(ttu);
	ttumod->changetex(sptex);
	ttumod->mats[0].color=col;
	video_drawscene(ttu);
}

void video_sprite_end()
{
	video3dinfo.zenable=zs;
	video3dinfo.perscorrect=pcs;
	delete spttreenorm;
	delete spttreefont;
	delete spttreetri;
	sptwhitetex->rc.deleterc(sptwhitetex);
	spttreenorm=0;
	spttreefont=0;
	spttreetri=0;
	sptwhitetex=0;
}
