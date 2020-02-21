void load_device(); // loads device.txt into wanted drivers, then sets drivers with wanted
void set_device();
void save_device(); // saves drivers
U32 changeglobalxyres(S32 direction); // increase / decrease globalxres, globalyres
U32 setglobalxyres(S32 idx); // set globalxyres index, calcs globalxres,globalyres
#define GX videoinfo.gamexy[videoinfo.gamexyidx].x
#define GY videoinfo.gamexy[videoinfo.gamexyidx].y
