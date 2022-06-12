float lerp(float A, float B, float t);
bool getIntersection(const pointf2& A, const pointf2& B, const pointf2& C, const pointf2& D, pointf3* I);
bool polysIntersect(const vector<pointf2>& poly1, const vector<pointf2>& poly2);
void clipline32dash(const struct bitmap32* b32, S32 x0, S32 y0, S32 x1, S32 y1, C32 color, S32 repeat);
C32 getRGBA(float value); // return a nice color for values from -1 to +1
