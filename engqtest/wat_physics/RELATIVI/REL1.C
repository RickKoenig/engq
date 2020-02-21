double newt(),relt();
#define light 186000
main()
{
double rock,bullet;
printf("relativity simulator, everything is miles per second\n");
printf("light travels 186000 miles per second\n");
while(1)
	{
	printf("input rocket speed, 0 to quit ");
	scanf(" %lf",&rock);
	if (rock==0)
		break;
	if (rock>186000)
		{
		printf("rocket can't go faster than the speed of light, adjusting to 185999\n");
		rock=185999;
		}
	printf("input bullet speed relative to rocket ");
	scanf(" %lf",&bullet);
	if (bullet>186000)
		{
		printf("bullet can't go faster than the speed of light, adjusting to 185999\n");
		bullet=185999;
		}
	printf("rocket = %f, bull = %f\n",rock,bullet);
	printf("speed of bullet relative to ground.\n");
	printf("\tNewton says   %f\n",newt(rock,bullet));
	printf("\tEinstein says %f\n",relt(rock,bullet));
	}
}


double newt(r,b)
double r,b;
{
return(r+b);
}

double relt(r,b)
double r,b;
{
return((r+b)/(1+r*b/light/light));
}


