void loadrematch(); // off by default
void freerematch();
void drawrematch();
void procrematch();
void rematchon();
void rematchoff();
bool isrematchon();
enum rematch{NONE,YES,NO};
rematch getrematchbuts();
void setrematchbuts(rematch rm);
