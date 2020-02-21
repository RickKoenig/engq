/*struct aclass {
	void amethod()
	{
		logger("amethod\n");
	}
};*/
//#define WRITEBIN
#ifdef WRITEBIN
void writebin()
{
	U8 data[] = {0xff,0xbb,0x77,0x33};
	FILE* fp=fopen2("testbinbig.bin","wb");
	int i,j;
	for (j=0;j<256;++j) {
		U8 uj = 255 - j;
		for (i=0;i<256;++i) {
			U8 ui = 255 - i;
			fwrite( &uj,1,sizeof(uj),fp);
			fwrite( &ui,1,sizeof(ui),fp);
		}
	}
	for (j=0;j<256;++j) {
		U8 uj = 255 - j;
		for (i=0;i<256;++i) {
			U8 ui = 255 - i;
			fwrite( &ui,1,sizeof(ui),fp);
			fwrite( &uj,1,sizeof(uj),fp);
		}
	}
	fclose(fp);
}
#endif
