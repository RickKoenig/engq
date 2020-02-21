void rotaxis2quat(struct pointf3 *ra,struct pointf3 *q);
void quat2rotaxis(const struct pointf3 *q,struct pointf3 *ra); // angle will be 0 to PI/2
void quat2xform(const struct pointf3 *q,struct mat4 *m); // quat
void quattrans2xform(const struct pointf3 *q,const struct pointf3 *t,struct mat4 *m); // quat
void quat2xformnotrans(const struct pointf3 *q,struct mat4 *m); // leave xform alone
//void quatscalerottrans3d(struct tree *t,struct mat4 *m);
void quatscalerottrans3d(const struct pointf3 *s,const struct pointf3 *q,const struct pointf3 *t,struct mat4 *m);
void quattimes(const struct pointf3 *a,const struct pointf3 *b,struct pointf3 *c);
void quatinverse(const struct pointf3 *a,struct pointf3 *b); // unit quats
void quatneg(const struct pointf3 *a,struct pointf3 *b);
void quatnormalize(const struct pointf3 *a,struct pointf3 *b); // make quat a into quat b
void quat2rpy(const struct pointf3 *q,struct pointf3 *rpy);
void rpy2quat(const struct pointf3 *rpy,struct pointf3 *q);
void quatinterp(const struct pointf3 *a,const struct pointf3 *b,float t,struct pointf3 *c);
void quatinterp2(const struct pointf3 *a,const struct pointf3 *b,float t,struct pointf3 *c); // standard slerp
void quatrot(const struct pointf3 *q,const struct pointf3 *vi,struct pointf3 *vo);
void quatrots(const struct pointf3 *q,const struct pointf3 *vi,struct pointf3 *vo,U32 npnts);

void normal2quat(const pointf3* up,const pointf3* norm,pointf3* quat); // for shadows
void normal2quaty(const pointf3* up,const pointf3* norm,pointf3* quat); // for directions
