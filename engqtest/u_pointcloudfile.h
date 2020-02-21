class pointcloudfile {
	FILE* fr;
	S32 frame;
	static const S32 maxstrsize = 500; // TODO: lack of security, should check maxstrsize
	vector<pointf3> points;
	vector<pointf3> colors;
	// vertical strips
	vector<pointf3> backupColors;
	vector<U32> regionSizes;
	vector<U32> regionOffsets;
	S32 curRegion;
	pc_kdtree3d* pckd;
	// read .bin2 file helpers
	string gettextline();
	U32 gettextnumber();
public:
	pointcloudfile(const C8* fname);
	U32 getnextframe(const pointf3** points,const pointf3** colors);
	U32 getnexthilit(const pointf3** colors,bool back = false);
	~pointcloudfile();

	void studypoints();
};