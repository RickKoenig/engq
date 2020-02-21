#define LINE2SPHERE
// ray
pointf2 rs = {0.0f,0.0f};
pointf2 rd = {1.0f,1.0f};
float extend = 10.0f;
// circle
pointf2 pos = {4.0f,3.0f};
float radius = 2.0f;
// solution
float t0,t1;
pointf2 p0,p1;

pointf2 ray(const pointf2& rs,const pointf2& rd,float t)
{
	pointf2 r;
	r.x = rs.x + t*rd.x;
	r.y = rs.y + t*rd.y;
	return r;
}

void drawline2sphere()
{
	drawfcircle(pos,C32RED,math2screen(radius));
	drawfpoint(rs,C32BLUE);
	drawfpoint(pointf2x(rs.x+rd.x,rs.y+rd.y),C32BLUE);
	drawfline(rs,pointf2x(rs.x+extend*rd.x,rs.y+extend*rd.y),C32GREEN);
	pointf2 D;
	D.x = rs.x - pos.x;
	D.y = rs.y - pos.y;
	float dt = rd.x*D.x + rd.y*D.y;
	float rd2 = rd.x*rd.x + rd.y*rd.y;
	float diff = D.x*D.x + D.y*D.y - radius*radius;
	float radi = sqrtf(dt*dt - rd2*diff);
	t0 = (-dt - radi)/rd2;
	t1 = (-dt + radi)/rd2;
	p0 = ray(rs,rd,t0);
	p1 = ray(rs,rd,t1);
	drawfpoint(p0,C32RED);
	drawfpoint(p1,C32RED);
}
