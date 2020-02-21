// new camera
struct newcams {
// low level
	pointf3 rot,pos;
	float zoom;
// middle level
	float pitch,yaw,dist,centery,centerx;

	int usenewcam,usecoll;
//	float steeryaw;
	float normalpitch;//,normalyaw=0;
	float collpitchrate;
	float collyawrate;
	float pitchlimit;
	float yawlimit;
	float yawsafe,pitchsafe;
	float startcentery,normalcentery,centerratey;
	int colltries;
	float cheadrate;
//	float oldchead;
//	int curleftcount,currightcount;
//	float steeryawlimit,steeryawrate,steeryawrate2;
//	int leftcount,rightcount;
	float yawdrift,pitchdrift;
	float cyaw,cpitch;
	float steeryaw;
	float centerxdrift;
	float ccenterx;
};

extern struct newcams newcamvars;

// end new camera
void new_cam_init();
void new_cam_proc(struct ol_playerdata *op);
//void resetcamera();
