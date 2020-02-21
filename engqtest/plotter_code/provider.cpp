//#define PROVIDER
#ifdef PROVIDER
socker* master,*client;
struct contdata {
	U8 buts[2];
	S16 tilt;
};
contdata ctdata;
void doprovider()
{
	initsocker();
	master = 0;
	client = 0;
	con32_printf(con,"make master on port 2013\n");
}

void procprovider()
	{
	if (!master) {
		int gotmyip=getnmyip();
		if (gotmyip)
			master = tcpmakemaster(0,2013);
	}
	if (!client && master) {
		client = tcpgetaccept(master);
		if (client)
			con32_printf(con,"connected\n");
	}
	if (client) {
		tcpfillreadbuff(client);
		if (client->flags&SF_CLOSED) {
			freesocker(client);
			client = 0;
			con32_printf(con,"closed\n");
		} else {
			int rd;
			do {
				rd = tcpgetbuffdata(client,(C8*)&ctdata,sizeof(ctdata),sizeof(ctdata));
				if (rd>0) {
					con32_printf(con,"c %4d, b1 %d, b2 %d, tilt %d\n",
						cntr,ctdata.buts[0],ctdata.buts[1],ctdata.tilt);
					//con32_printf(con,"c %4d, b %d,%d a %5.2f %5.2f %5.2f\n",
					//	cntr,ctdata.buts[0],ctdata.buts[1],ctdata.accel[0],ctdata.accel[1],ctdata.accel[2]);
				}
			} while(rd > 0);
		}
	}
	if (client)
		tcpsendwritebuff(client);
}

void exitprovider()
{
	freesocker(client);
	freesocker(master);
	uninitsocker();
}
#endif
