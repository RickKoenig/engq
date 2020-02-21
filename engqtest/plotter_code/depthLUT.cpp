#define DEPTHLUT
void do_depthlut()
{
	logger("doing depthlut!\n");
	FILE *fp = fopen("depthLUT_module66_500lux_100cm.txt","r");
    if(!fp)
		return;
	const S32 MAXLINE = 1024;
	C8 line[MAXLINE];
	//S32 i = 0;
	vector<float> lut;
	while(fgets(line,1024,fp)) {
        //logger("line %d = '%s'\n",i++,line);
		lut.push_back((float)atof(line));
    }
	fclose(fp);
	vector<float> lutf(256);
	S32 i;
	for (i=0;i<256;++i) {
		S32 j = i>>1;
		if (j < 25) {
			lutf[i] = 10000.0f;
		} else {
			lutf[i] = 22.605717f/(j - 24);
		}
	}
	float totalerr = 0.0f;
	for (i=0;i<256;++i) {
		float err = lut[i] - lutf[i];
		logger("line %d = %f, %f, %f\n",i,lut[i],lutf[i],err);
		totalerr += fabs(err);
	}
	logger("total error = %f\n",totalerr);
	logger("loading a depth map\n");
	bitmap32* b = gfxread32("scene2.jpg.depth.jpg");
	S32 p = b->size.x*b->size.y;
	S32 hist[256];
	::fill(hist,hist+256,0);
	C32* data = b->data;
	for (i=0;i<p;++i) {
		++hist[data[i].g];
	}
	for (i=0;i<256;++i)
		if (hist[i])
			logger("hist depth %3d = %d\n",i,hist[i]);
	bitmap32free(b);
}
