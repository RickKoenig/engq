#define DDS2PNG // convert all .dds's to .png's for htdocs
#ifdef DDS2PNG
//#define REDUCERES 128
#define REDUCERES 8192
void dds2pngrec() {
	script sc = scriptdir(0);
	int i;
	for (i=0;i<sc.num();++i) {
		string e = sc.idx(i);
		logger("file '%s'\n",e.c_str());
		C8 ext[100];
		mgetext(e.c_str(),ext,sizeof(ext));
		C8 name[MAX_PATH];
		mgetname(e.c_str(),name);
		if (!my_stricmp(ext,"dds") || !my_stricmp(ext,"tga")) {
			string ne = string(name) + ".png";
			logger("converting '%s' to '%s'\n",e.c_str(),ne.c_str());
			bitmap32* bm = gfxread32(e.c_str());
			if (bm->size.x > REDUCERES || bm->size.y > REDUCERES) {
				int bmsx = min(bm->size.x,REDUCERES);
				int bmsy = min(bm->size.y,REDUCERES);
				bitmap32* bms = bitmap32alloc(bmsx,bmsy,C32BLACK);
				clipscaleblit32(bm,bms);
				gfxwrite32(ne.c_str(),bms);
				bitmap32free(bms);
				bitmap32free(bm);
			} else {
				gfxwrite32(ne.c_str(),bm);
				bitmap32free(bm);
			}
		}
	}
	// now do subdirectories
	sc = scriptdir(1);
	for (i=0;i<sc.num();++i) {
		string de = sc.idx(i);
		if (de == ".metadata") // skip this folder
			continue;
		logger("dir '%s'\n",de.c_str());
		pushandsetdirdown(de.c_str());
		logger_indent();
		dds2pngrec();
		logger_unindent();
		popdir();
	}
}

void dds2png() {
	logger("doing dds2png\n");
//	pushandsetdir("c:/nodejs/engw");
//	pushandsetdir("c:/xampp/htdocs");
	pushandsetdir("C:/srcw32/android_studio/OpenGLGallery/app/src/main/assets/physics3d");
	dds2pngrec();
	popdir();
}
#endif

