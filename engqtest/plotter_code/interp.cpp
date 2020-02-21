//#define INTERP
#ifdef INTERP
float a,b,c;
float x1=2,x2=4,x3=3;
float poly(float x)
{
	return (a*x+b)*x+c;
}

void calcpoly()
{
	a = .5f*x3 - x2 + .5f*x1;
	b = -1.5f*x3 + 4*x2 - 2.5f*x1;
	c = x3 - 3*x2 + 3*x1;
#if 0
	a = 2;
	b = 4;
	c = -15;
#endif
}
void drawinterp()
{
	drawfunction(poly);
	drawfpoint(pointf2x(1.0f,x1),C32BLACK);
	drawfpoint(pointf2x(2.0f,x2),C32BLACK);
	drawfpoint(pointf2x(3.0f,x3),C32BLACK);
}

#endif
