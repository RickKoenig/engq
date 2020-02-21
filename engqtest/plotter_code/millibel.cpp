//#define MILLIBEL
#ifdef MILLIBEL
void millibel()
{
	logger("in millibel\n");
	float powers[5] = {1.0f,.1f,.01f,.001f,.00001f};
	int i;
	for (i=0;i<5;++i) {
		float p = powers[i];
		float mb = 1000.0f*log10f(p);
		logger("power = %f, mb = %f\n",p,mb);
	}
}
#endif


