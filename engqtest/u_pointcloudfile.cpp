// engq API
#include <m_eng.h>

#include "u_pointcloudkdtree.h"
// read data in from a .bin2 file
#include "u_pointcloudfile.h"


// used by constructor
string pointcloudfile::gettextline()
{
	C8 parsestr[maxstrsize];
	U32 textlen = 0;

	while(true) {
		C8 curchar;
		U32 bytesread = fileread(fr,&curchar,sizeof(curchar));
		if (curchar == '\n' || !bytesread)
			break;
		parsestr[textlen++] = curchar;
	}
	//strcpy(parsestr,"123");
	parsestr[textlen] = '\0';
	return string(parsestr);
}

U32 pointcloudfile::gettextnumber()
{
	string textnumber = gettextline();
	U32 pos = textnumber.find('=');
	return atoi(textnumber.c_str() + pos + 1);
}

pointcloudfile::pointcloudfile(const C8* fname)
{
	fr = fopen2(fname,"rb");
	U32 fs = getfilesize(fr); // 4 Gigs or less, (32 bit)
	U32 version;
	fileread(fr,&version,sizeof(version));
	logger("\t.bin2 name = '%s', size = %u\n",fname,fs);
	logger("\t\tversion = %u\n",version);
	frame = -1;
	pckd = 0;
}

U32 pointcloudfile::getnextframe(const pointf3** pp,const pointf3** pc)
{
	regionSizes.clear();
	regionOffsets.clear();
	curRegion = -1;
	++frame;
	C8 chr;
	U32 ret = fileread(fr,&chr,sizeof(chr)); // skip first /n
	if (!ret) {
		fseek(fr,5,SEEK_SET); // end of file, rewind to start skipping version and first '\n'
		frame = 0;
	}
	U32 npoints = gettextnumber(); // npoints
	U32 npixels = gettextnumber(); // npixels
	U32 naudios = gettextnumber(); // naudio
	U32 timestamp = gettextnumber(); // timestamp
	logger("\n\t\tnpoints = '%u', npixels = '%u', naudios = '%u', timestamp = '%u'\n",npoints,npixels,naudios,timestamp);

//	points.resize(npoints);
//	colors.resize(npoints);
	points.clear();
	backupColors.clear();
	//colors.clear();
	U32 i;
	// read points
	pointf3 pf;
	pf.w = 1.0f; // a point, not a vector
//#define LOGPOINTS // too slow, too much !!!
#ifdef LOGPOINTS
	logger("point cloud for frame %d\n",frame);
#endif
//#define ZEROZ
	for (i=0;i<npoints;++i) {
		S16 pp[3];
		fileread(fr,pp,sizeof(pp));
		pf.x = .001f*pp[0];// + .25f*i;
		pf.y = .001f*pp[1];
#ifdef ZEROZ
		pf.z = 0; // RHC to LHC ?
#else
		pf.z = -.001f*pp[2]; // RHC to LHC ?
		//pf.z -= i*.000001f;
#endif
#ifdef LOGPOINTS
		logger("(%8.5f,%8.5f,%8.5f)\n",pf.x,pf.y,pf.z);
#endif
//		points[i] = pf;
		points.push_back(pf);
/*		pf.x = -pf.x;
		points.push_back(pf);
		pf.x = pf.x;
		pf.y = - pf.y;
		points.push_back(pf);
		pf.x = -pf.x;
		points.push_back(pf); */
	}
	// read colors
	pointf3 cf;
	cf.w = 1.0f; // opaque, not transparent
	for (i=0;i<npoints;++i) {
		U16 cp;
		//fileread(fr,&cp,sizeof(cp));
		cp = filereadU16LE(fr);
		U16 b = 0x1f&(cp>>10);
		U16 g = 0x3f&(cp>>5);
		U16 r = 0x1f&cp;
		cf.x = r * (1.0f/32.0f);
		cf.y = g * (1.0f/64.0f);
		cf.z = b * (1.0f/32.0f);
		cf.w = 1;//.06f;
		//colors[i] = cf;
		backupColors.push_back(cf);
/*		colors.push_back(cf);
		colors.push_back(cf);
		colors.push_back(cf); */
	}
	colors = backupColors;
	// skip audio
	fseek(fr,naudios,SEEK_CUR);
	U32 nextts;
	fileread(fr,&nextts,sizeof(nextts));
	U32 pos = ftell(fr);
	logger("\t\tframe %u, seek pos after reading a block = 0x%08x, nexts = %u\n",frame,pos,nextts);


	//points.resize(5);
	//colors.resize(5);
	if (points.empty()) {
		*pp = 0;
		*pc = 0;
	} else {
		*pp = &points[0];
		*pc = &colors[0];
	}
	if (frame == 0) {
		studypoints();
#define KDTREE
#ifdef KDTREE
		logger("\n in kdtree\n");
		delete pckd;
		pckd = new pc_kdtree3d(points);
	} else {
		delete pckd;
		pckd = 0;
	}
		

#endif
	return points.size();
}
	
U32 pointcloudfile::getnexthilit(const pointf3** pc,bool back)
{
	if (colors.empty()) {
		*pc = 0;
		return 0;
	}
	if (back) {
		if (curRegion == -1) {
			curRegion = regionSizes.size() - 1;
		} else {
			--curRegion;
		}
	} else {
		++curRegion;
		if (curRegion == regionSizes.size())
			curRegion = -1;
	}
	colors = backupColors;
	if (curRegion >= 0) {
		U32 i = regionOffsets[curRegion];
		U32 n = i + regionSizes[curRegion];
		for (;i<n;++i) {
			colors[i] = F32WHITE;
		}
	}
	*pc = &colors[0];
	return colors.size();
}

pointcloudfile::~pointcloudfile()
{
	fclose(fr);
	delete pckd;
}

void pointcloudfile::studypoints()
{
	logger("=== study points ===\n");
	logger("frame '0' points are size = %u\n",points.size());

	// bounding box
	pointf3x boxmin( 10000, 10000, 10000);
	pointf3x boxmax(-10000,-10000,-10000);
	U32 i,n = points.size();
	U32 select = 0;
	pointf3x lastpoint;
	U32 lasti = 0;
	for (i=0;i<n;++i) {
		if (points[i].x < boxmin.x)
			boxmin.x = points[i].x;
		if (points[i].x > boxmax.x)
			boxmax.x = points[i].x;
		if (points[i].y < boxmin.y)
			boxmin.y = points[i].y;
		if (points[i].y > boxmax.y)
			boxmax.y = points[i].y;
		if (points[i].z < boxmin.z)
			boxmin.z = points[i].z;
		if (points[i].z > boxmax.z)
			boxmax.z = points[i].z;
	}

//#define SUBPOINTS
#ifdef SUBPOINTS
	for (i=0;i<n;++i) {
		if ((points[i].y >= -.015f && points[i].y <= .015f) &&
			(points[i].x >= -.015f && points[i].x <= .015f)) {
			colors[i] = F32WHITE;
			logger("select point from %7u, (%8.3f,%8.3f,%8.3f)",i,points[i].x,points[i].y,points[i].z);
			logger("\t\tdeltas %7u, (%8.3f,%8.3f,%8.3f)\n",i - lasti,points[i].x - lastpoint.x,points[i].y - lastpoint.y,points[i].z - lastpoint.z);
			lasti = i;
			lastpoint = points[i];
			++select;
		}
	}
#endif

#define SCANPOINTS
#ifdef SCANPOINTS
	U32 numInCol = 0;
	lastpoint = points[0];
	//vector<U32> colhist;
	for (i=0;i<n;++i) {
		pointf3 delta;
		delta.x = points[i].x - lastpoint.x;
		delta.y = points[i].y - lastpoint.y;
		delta.z = points[i].z - lastpoint.z;
		if (delta.y > .025f) { // move to next column
			if (numInCol > 0)
				regionSizes.push_back(numInCol);
			numInCol = 1; // first point in new column
		} else { // stay in current column
			++numInCol;
		}
		lastpoint = points[i];
	}
	regionSizes.push_back(numInCol);
	U32 np = 0;
	n = regionSizes.size();
	for (i=0;i<n;++i) {
		regionOffsets.push_back(np);
		logger("\t\tcolumn %5u : offset = %6u, size = %5u\n",i,regionOffsets[i],regionSizes[i]);
		np += regionSizes[i];
	}
	logger("\ttotal columns = %u, count total points in columns = %u",regionSizes.size(),np);
#if 0
	if ((points[i].y >= -.015f && points[i].y <= .015f) &&
			(points[i].x >= -.015f && points[i].x <= .015f)) {
			colors[i] = F32WHITE;
			logger("select point from %7u, (%8.3f,%8.3f,%8.3f)",i,points[i].x,points[i].y,points[i].z);
			logger("\t\tdeltas %7u, (%8.3f,%8.3f,%8.3f)\n",i - lasti,points[i].x - lastpoint.x,points[i].y - lastpoint.y,points[i].z - lastpoint.z);
			lasti = i;
			lastpoint = points[i];
			++select;
		}
	}
#endif
#endif

	logger("\nselect points = %u\tboxmin = (%8.3f,%8.3f,%8.3f), boxmax = (%8.3f,%8.3f,%8.3f)\n",select,boxmin.x,boxmin.y,boxmin.z,boxmax.x,boxmax.y,boxmax.z);

}




// for reference
#if 0
Hi again Rick,

Sending some recordings via WeTransfer.  The reference on is a static dummy with a calibration pattern.  
The one and 2 camera ones are a different dummy (me, haha).  
I'm thinking that the technology should make the one and 2 camera ones the same, just more points.
They were captured on a RealSense at 848 x 420, 30hz I think.

File format:

U32 FileVersion = 1 (not byte)
string NewLine
string "n_points= nPoints" NewLine
string "n_pixels= nPixels" NewLine
string "n_audiodata= nAudioBytes" NewLine
string "frame_timestamps= FrameTimeInMs" NewLine
for each of nPoints -> 3 * sizeof(short) for compressed X,Y,Z coordinates of the point. To uncompress to float multiply X,Y,Z by 0.001f
for each of nPoints -> 2 * sizeof(byte) for a 16-bit color of the point
for each of nAudioBytes -> byte for captured audio
int NextFrameTimeInMs
string NewLine
string "n_points= nPoints" NewLine
string "n_pixels= nPixels" NewLine
string "n_audiodata= nAudioBytes" NewLine
string "frame_timestamps= TimeInMs" NewLine
.......

and so on until the file ends


#endif