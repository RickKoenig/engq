//#define SLERPTEST
#ifdef SLERPTEST
void normquat(pointf3& q) {
	q.w = sqrtf(1.0f-q.x*q.x-q.y*q.y-q.z*q.z);
}

void slerptest()
{
	logger("slerp test\n");
#if 0
	pointf3 qa = {.1f,.2f,.3f};
	pointf3 qb = {.4f,.5f,-.55f};
#elif 0
	pointf3 qa = {0,0,.5f};
	pointf3 qb = {.5f,0,0};
#elif 1
	pointf3 qa = {0,0,1};
	pointf3 qb = {1,0,0};
#endif
	normquat(qa);
	normquat(qb);
	pointf3 qc,qc2;
	float t;
	for (t=0.0f;t<=1.0f;t+=.03125f) {
		quatinterp(&qa,&qb,t,&qc);
		quatinterp2(&qa,&qb,t,&qc2);
		pointf3 d;
		d.x = qc2.x - qc.x;
		d.y = qc2.y - qc.y;
		d.z = qc2.z - qc.z;
		d.w = qc2.w - qc.w;
		//qc = qa;
		//qc2 = qb;
		logger("t = %f, qc = (%f,%f,%f,%f), qc2 = (%f,%f,%f,%f), d = (%g,%g,%g,%g)\n",t,qc.x,qc.y,qc.z,qc.w,qc2.x,qc2.y,qc2.z,qc2.w,d.x,d.y,d.z,d.w);
	}
}
#endif

