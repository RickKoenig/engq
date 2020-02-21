#define INPUT 6
#define HIDDEN	6
#define OUTPUT 4
#define BIAS 1

#define DELTA .9
#define ERROR .1

#define NUMTESTS 60
#define EXTRACREDIT 4

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double wij[HIDDEN+1][OUTPUT];
double ewij[HIDDEN+1][OUTPUT];

double wki[INPUT+1][HIDDEN];
double ewki[INPUT+1][HIDDEN];

double inputs[INPUT+1];

double hidden[HIDDEN+1];
double eai[HIDDEN+1];
double eii[HIDDEN+1];

double outputs[OUTPUT];
double eaj[OUTPUT];
double eij[OUTPUT];

double desired[OUTPUT];

double sigmoid(double x)
{
return 1/(1+exp(-x));
}

double frand()
{
return rand()/(RAND_MAX+1.0);
}

void runnet()
{
int i,j,k;
for (i=0;i<HIDDEN;i++)
	{
	hidden[i]=0.0;
	for (k=0;k<=INPUT;k++)
		hidden[i]+=wki[k][i]*inputs[k];
	hidden[i]=sigmoid(hidden[i]);
	}
for (j=0;j<OUTPUT;j++)
	{
	outputs[j]=0.0;
	for (i=0;i<=HIDDEN;i++)
		outputs[j]+=wij[i][j]*hidden[i];
	outputs[j]=sigmoid(outputs[j]);
	}
}

void initbias()
{
inputs[INPUT]=1.0;
hidden[HIDDEN]=1.0;
}

void initnetwork()
{
int k,i,j;
for (j=0;j<OUTPUT;j++)
	for (i=0;i<=HIDDEN;i++)
		wij[i][j]=1-2*frand();
for (i=0;i<OUTPUT;i++)
	for (k=0;k<=INPUT;k++)
		wki[k][i]=1-2*frand();
}


void settest(int t)
{
int k;
for (k=0;k<INPUT;k++)
	if (t==k)
		{
		desired[k]=1.0;
		inputs[k]=1.0;
		}
	else
		{
		desired[k]=0.0;
		inputs[k]=0.0;
		}
}

/*
void settest(int t)
{
int k;
int a;
int b;
a=t&7;
b=t>>3;
t=a+b;

inputs[0]=a&1;
a>>=1;
inputs[1]=a&1;
a>>=1;
inputs[2]=a&1;

inputs[3]=b&1;
b>>=1;
inputs[4]=b&1;
b>>=1;
inputs[5]=b&1;

desired[0]=t&1;
t>>=1;
desired[1]=t&1;
t>>=1;
desired[2]=t&1;
t>>=1;
desired[3]=t&1;
}
*/

double checkerr()
{
double error=0;
int j;
for (j=0;j<OUTPUT;j++)
	error+=(outputs[j]-desired[j])*(outputs[j]-desired[j]);
return(error);
}

double checkerrors()
{
int t;
double error=0.0;
for (t=0;t<NUMTESTS;t++)
	{
	settest(t);
	runnet();
	error+=checkerr();
	}
return(error);
}

void train()
{
int i,j,k;
int t;

t=random(NUMTESTS);
settest(t);

runnet();

for (j=0;j<OUTPUT;j++)
	{
	eaj[j]=outputs[j]-desired[j];
	eij[j]=eaj[j]*outputs[j]*(1-outputs[j]);
	for (i=0;i<HIDDEN+1;i++)
		ewij[i][j]=eij[j]*hidden[i];
	}
for (i=0;i<HIDDEN+1;i++)
	{
	eai[i]=0;
	for (j=0;j<OUTPUT;j++)
		eai[i]+=eij[j]*wij[i][j];
	eii[i]=eai[i]*hidden[i]*(1-hidden[i]);
	}
for (k=0;k<INPUT+1;k++)
	for (i=0;i<HIDDEN;i++)
		ewki[k][i]=eii[i]*inputs[k];
for (j=0;j<OUTPUT;j++)
	for (i=0;i<HIDDEN+1;i++)
		wij[i][j]-=DELTA*ewij[i][j];
for (i=0;i<HIDDEN;i++)
	for (k=0;k<INPUT+1;k++)
		wki[k][i]-=DELTA*ewki[k][i];
}


void showweights()
{
int i,j,k;
printf("\n");
for (j=0;j<OUTPUT;j++)
	{
	printf("wij[][%d]=",j);
	for (i=0;i<=HIDDEN;i++)
		printf("   %f",wij[i][j]);
	printf("\n");
	}
for (i=0;i<HIDDEN;i++)
	{
	printf("wki[][%d]=",i);
	for (k=0;k<=INPUT;k++)
		printf("   %f",wki[k][i]);
	printf("\n");
	}
}

void showtests()
{
int t;
int j;
for (t=0;t<NUMTESTS+EXTRACREDIT;t++)
	{
	settest(t);
	runnet();
	printf(" in ");
	for (j=0;j<INPUT;j++)
		printf("%6.3f",inputs[j]);
	printf("  hid ");
	for (j=0;j<HIDDEN;j++)
		printf("%6.3f",hidden[j]);
	printf("  out ");
	for (j=0;j<OUTPUT;j++)
		printf("%6.3f",outputs[j]);
	printf("\n");
	}
}

void main()
{
int count=0;
double error;
printf("Neural Net\n");
randomize();
initbias();
initnetwork();
while(1)
	{
	if (count==500)
		{
		error=checkerrors();
		printf("%10.7f",error);
		if (error<ERROR)
			break;
		count=0;
		}
	count++;
	train();
	}
showweights();
showtests();
}

