void inittracklist();
const char *uniquetrackname(const char *trkname);
const char *hastrackhash(unsigned char hash[NUMHASHBYTES]); // name of track with this hash else NULL
void freetracklist();
int tracklistnum();
