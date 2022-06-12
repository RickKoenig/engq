#include <m_eng.h>

#include "u_s_selfdriving.h"
#ifdef DONAMESPACE
namespace selfdriving {
#endif

#include "u_utils.h"
#include "u_controls.h"
#include "u_network.h"
#include "u_sensor.h"
#include "u_car.h"

Car::Car(float x, float y, float width, float height, Controls::ControlType controlType, float maxSpeed)
		: controls(controlType)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	speed = 0;
	acceleration = .2f;
	this->maxSpeed = maxSpeed;
	friction = .05f;
	angle = 0;
	damaged = false;
	sensor = nullptr;
	brain = nullptr;
	useBrain = controlType == Controls::ControlType::AI;
	if (controlType != Controls::ControlType::DUMMY) {
		sensor = new Sensor(this);
		brain = new NeuralNetwork(vector<U32>{static_cast<U32>(sensor->rayCount), 6, 4});
	}
}

Car::~Car()
{
	delete sensor;
	delete brain;
}

void Car::update(const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic)
{
	if (!damaged) {
		move();
		polygon = createPolygon();
		if (useBrain) {
			damaged = assessDamage(roadBorders, traffic);
		}
	}

	if (sensor) {
		sensor->update(roadBorders, traffic);
#if 0
		const offsets = this.sensor.readings.map(
			s = > s == null ? 0 : 1 - s.offset
		);
		const outputs = NeuralNetwork.feedForward(offsets, this.brain);

		if (this.useBrain) {
			this.controls.forward = outputs[0];
			this.controls.left = outputs[1];
			this.controls.right = outputs[2];
			this.controls.reverse = outputs[3];
		}
#endif
	}
}

void Car::draw(C32 color, bool drawSensor)
{
	if (damaged) {
		color = C32DARKGRAY;
	}
	for (U32 i = 0; i < polygon.size(); ++i) {
		auto j = (i + 1) % polygon.size();
		for (auto n = -2; n <= 2; ++n) {
			for (auto m = -2; m <= 2; ++m) {
				clipline32(B32
					, static_cast<S32>(polygon[i].x - carCamera.x + m)
					, static_cast<S32>(polygon[i].y - carCamera.y + n)
					, static_cast<S32>(polygon[j].x - carCamera.x + m)
					, static_cast<S32>(polygon[j].y - carCamera.y + n)
					, color);
			}
		}
	}
	if (sensor && drawSensor) {
		sensor->draw();
	}
}

bool Car::assessDamage(const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic)
{
	for (auto i = 0U; i < roadBorders.size(); ++i) {
		if (polysIntersect(polygon, roadBorders[i])) {
			return true;
		}
	}
	for (auto i = 0U; i < traffic.size(); ++i) {
		if (polysIntersect(polygon, traffic[i]->polygon)) {
			return true;
		}
	}
	return false;
}

vector<pointf2> Car::createPolygon() {
	vector<pointf2> points;
	const float rad = hypot(width, height) / 2;
	const float alpha = atan2(width, height);
	points.push_back({
		x - sin(angle - alpha) * rad,
		y - cos(angle - alpha) * rad
		});
	points.push_back({
		x - sin(angle + alpha) * rad,
		y - cos(angle + alpha) * rad
		});
	points.push_back({
		x - sin(PI + angle - alpha) * rad,
		y - cos(PI + angle - alpha) * rad
		});
	points.push_back({
		x - sin(PI + angle + alpha) * rad,
		y - cos(PI + angle + alpha) * rad
		});
	return points;
}

void Car::move() {
	controls.update();

	if (controls.forward) {
		speed += acceleration;
	}
	if (controls.reverse) {
		speed -= acceleration;
	}

	if (speed > maxSpeed) {
		speed = maxSpeed;
	}
	if (speed < -maxSpeed / 2) {
		speed = -maxSpeed / 2;
	}

	if (speed > 0) {
		speed -= friction;
	}
	if (speed < 0) {
		speed += friction;
	}
	if (abs(speed) < friction) {
		speed = 0;
	}
	if (speed != 0) {
		const float flip = speed > 0 ? 1.0f : -1.0f;
		if (controls.left) {
			angle += .03f * flip;
		}
		if (controls.right) {
			angle -= .03f * flip;
		}
	}

	x -= sin(angle) * speed;
	y -= cos(angle) * speed;
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
