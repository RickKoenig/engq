#define CUBIC2
#ifdef CUBIC2
float cubeeq(float x)
{
	return x*x*x - 3*x - 1;
}

void docubic()
{
	logger("doing cubic\n");
	S32 i;
	vector<S32> angs;
	angs.push_back(1);
	angs.push_back(5);
	angs.push_back(7);
	S32 n = angs.size();
	float x,y;
	float s1 = 0,s2 = 0,p1 = 1;
	for(i=0;i<n;++i) {
		x = 2*cosf(2*PI*angs[i]/18);
		y = cubeeq(x);
		logger("rat = %2d, x = %f, y = %f\n",angs[i],x,y);
		float x1 = 2*cosf(2*PI*angs[(i+1)%n]/18);
		s1 += x*x*x1;
		s2 += x1*x1*x;
		p1 *= x-x1;
	}
	logger("s1 = %f, s2 = %f, p1 = %f\n",s1,s2,p1);
}
#endif
