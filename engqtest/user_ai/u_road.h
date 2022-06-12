class Road {
public:
	Road(float xCenter, float width, S32 laneCount = 3);
	float getLaneCenter(S32 laneIndex);
	void draw();
private:
	float fullWidth; // including outside of curb
	float width; // curb to curb
	S32 laneCount;
	float left;
	float right;
	float top; // for borders, not drawing
	float bot;
	pointf2 topLeft;
	pointf2 topRight;
	pointf2 botLeft;
	pointf2 botRight;
	const S32 lineDash = 15;
public:
	vector< vector<pointf2>> borders;
};
