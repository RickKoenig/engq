class Car {
public:
	float x;
	float y;
	float width;
	float height;
	float speed;
	float acceleration;
	float maxSpeed;
	float friction;
	float angle;
	bool damaged;
	vector<pointf2> polygon;
	class Controls controls;
	Sensor* sensor;
	bool useBrain;
	NeuralNetwork* brain;

public:
	Car(float x, float y, float width, float height, Controls::ControlType controlType, float maxSpeed = 3);
	~Car();
	void update(const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic);
	void draw(C32 color, bool drawSensor = false);
private:
	vector<pointf2> createPolygon();
	void move();
	bool assessDamage(const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic);
};
