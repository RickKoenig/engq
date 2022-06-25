#include <m_eng.h>

#include "m_perf.h"

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
	rad = sqrt(width * width + height * height) * .5f;
	speed = 0;
	acceleration = .2f;
	this->maxSpeed = maxSpeed;
	friction = .05f;
	angle = 0;
	damaged = false;
	sensor = nullptr;
	brain = nullptr;
	polygon = vector < pointf2>(carSides);
	useBrain = controlType == Controls::ControlType::AI;
	if (controlType != Controls::ControlType::DUMMY) {
		sensor = new Sensor(this);
		offsets.resize(sensor->rayCount);
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
	perf_start(TEST7);
	if (!damaged) {
		move();
		createPolygon();
		if (useBrain) {
			damaged = assessDamage(roadBorders, traffic);
		}
	}
	perf_end(TEST7);
#if 0
	perf_start(TEST10);
	perf_end(TEST10);
	perf_start(TEST11);
	perf_end(TEST11);
#endif
	if (sensor) {
		perf_start(TEST8);
		sensor->update(roadBorders, traffic);
		perf_end(TEST8);

		for (auto i = 0U; i < offsets.size(); ++i) {
			offsets[i] = 1 - sensor->readings[i].z;
		}
		perf_start(TEST9);
		const vector<float>& outputs = brain->feedForward(offsets);
		perf_end(TEST9);
		if (useBrain) {
#if 1
			controls.forward = outputs[0];
			controls.left = outputs[1];
			controls.right = outputs[2];
			controls.reverse = outputs[3];
#endif
		}
	}
}

void Car::draw(C32 color, bool drawSensor)
{
	perf_start(TEST6);
	if (damaged) {
		color = C32DARKGRAY;
	}
	for (U32 i = 0; i < polygon.size(); ++i) {
		auto j = (i + 1) % polygon.size();
		for (auto n = -carLineWidth; n <= carLineWidth; ++n) {
			for (auto m = -carLineWidth; m <= carLineWidth; ++m) {
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
	perf_end(TEST6);
}

bool Car::assessDamage(const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic)
{
#if 1
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
#endif
	return false;
}

void Car::createPolygon() {
	const float rad = hypot(width, height) / 2;
	const float alpha = atan2(width, height);
	polygon[0] = {
		x - sin(angle - alpha) * rad,
		y - cos(angle - alpha) * rad
		};
	polygon[1] = {
		x - sin(angle + alpha) * rad,
		y - cos(angle + alpha) * rad
		};
	polygon[2] = {
		x - sin(PI + angle - alpha) * rad,
		y - cos(PI + angle - alpha) * rad
		};
	polygon[3] = {
		x - sin(PI + angle + alpha) * rad,
		y - cos(PI + angle + alpha) * rad
		};
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
