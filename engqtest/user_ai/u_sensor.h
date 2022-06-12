class Sensor {
	class Car* car;
public:
	S32 rayCount;
private:
	float rayLength;
	float raySpread;
	vector<vector<pointf2>> rays;
	vector<pointf3> readings;
public:
	Sensor(class Car* car);
	void update(const vector<vector<pointf2>>& roadBorders, const vector<Car*>&  traffic);

private:
	pointf3 getReading(const vector<pointf2> &ray, const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic);
	void castRays();
public:
	void draw();
};
