class Sensor {
	class Car* car;
public:
	static S32 sensorCount;
	static S32 sensorEarlyOut;
	S32 rayCount;
private:
	float rayLength;
	float raySpread;
	vector<vector<pointf2>> rays;
public:
	vector<pointf3> readings;
	Sensor(class Car* car);
	void update(const vector<vector<pointf2>>& roadBorders, const vector<Car*>&  traffic);

private:
	pointf3 getReading(const vector<pointf2> &ray, const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic);
	void castRays();
public:
	void draw();
};
