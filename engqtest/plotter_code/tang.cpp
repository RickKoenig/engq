//#define TANG
#ifdef TANG
pointf2x vr(4.0f,-1.0f); 
pointf2x nrm(0.0f,1.0f);
pointf2 tang;

void drawtang()
{
	drawflinec(pointf2x(0,0),vr,C32GREEN);
	drawflinec(pointf2x(0,0),nrm,C32RED);
	drawflinec(pointf2x(0,0),tang,C32MAGENTA);
}
#endif
