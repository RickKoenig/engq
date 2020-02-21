//#define CENTROID
#ifdef CENTROID
void centroid()
{
	float step = .001f;
	float x,y;
	float sx = 0;
	float s = 0;
	for (x=step*.5f;x<3.0f;x+=step) {
		y = x*x*x;
		s += y*step;
		sx += y*x*step;
		//logger("x = %f, y = %f\n",x,y);
	}
	for (x=3.0f+step*.5f;x<30;x+=step) {
		y = 30 - x;
		s += y*step;
		sx += y*x*step;
		//logger("x = %f, y = %f\n",x,y);
	}
	logger("sx = %f, s = %f, centx = %f\n",sx,s,sx/s);
}
#endif
