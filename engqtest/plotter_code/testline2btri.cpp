#define LINE2BTRI
#ifdef LINE2BTRI


void line2btri_draw()
{
	pointf2x v0 = pointf2x(.25f,-.21651f);
	pointf2x v1 = pointf2x(.25f,-.33171f);
	pointf2x v2 = pointf2x(.38302f,-.24620f);
	pointf2x p = pointf2x(.25f,-.24620f);
	drawflinec(v0,v1,C32GREEN);
	drawflinec(v1,v2,C32GREEN);
	drawflinec(v2,v0,C32GREEN);
	drawfpoint(p,C32RED);
}

#endif
