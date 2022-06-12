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
#include "u_utils.h"

Sensor::Sensor(class Car* car)
	: car(car)
	, rayCount(5)
	, rayLength(150)
	, raySpread(PI / 2)
{
}

void Sensor::update(const vector<vector<pointf2>>& roadBorders, const vector<Car*>&  traffic)
{
	castRays();
	readings.clear();
	for (auto i = 0U; i < rays.size(); ++i) {
		readings.push_back(getReading(rays[i], roadBorders, traffic));
	}
}

pointf3 Sensor::getReading(const vector<pointf2>& ray, const vector<vector<pointf2>>& roadBorders, const vector<Car*>& traffic)
{
	pointf3 bestTouch{ray[1].x, ray[1].y, 1};
	for (auto i = 0U; i < roadBorders.size(); ++i) {
		pointf3 touch;
		const bool isTouch = getIntersection(
			ray[0],
			ray[1],
			roadBorders[i][0],
			roadBorders[i][1],
			&touch
		);
		if (isTouch && touch.z < bestTouch.z) {
			bestTouch = touch;
		}
	}

	for (auto i = 0U; i < traffic.size(); ++i) {
		auto poly = traffic[i]->polygon;
		for (auto j = 0U; j < poly.size(); ++j) {
			pointf3 touch;
			const bool isTouch = getIntersection(
				ray[0],
				ray[1],
				poly[j],
				poly[(j + 1) % poly.size()],
				&touch
			);
			if (isTouch && touch.z < bestTouch.z) {
				bestTouch = touch;
			}
		}
	}
	return bestTouch;
}

void Sensor::castRays()
{
	float x = car->x;
	float y = car->y;
	float angle = car->angle;
	rays.clear();
	for (auto i = 0; i < rayCount; ++i) {
		const float rayAngle = lerp(
			raySpread / 2,
			-raySpread / 2,
			rayCount == 1 ? .5f : static_cast<float>(i) / (rayCount - 1)

		) + angle;

		const pointf2 start = { x, y };
		const pointf2 end = {
			x - sin(rayAngle) * rayLength,
			y - cos(rayAngle) * rayLength
		};
		vector<pointf2> ray{ start, end };
		rays.push_back(ray);
	};
}

void Sensor::draw()
{
	for (auto i = 0; i < rayCount; ++i) {
		pointf2 end{ readings[i].x, readings[i].y };

		clipline32(B32
			, static_cast<S32>(-carCamera.x + rays[i][0].x)
			, static_cast<S32>(-carCamera.y + rays[i][0].y)
			, static_cast<S32>(-carCamera.x + end.x)
			, static_cast<S32>(-carCamera.y + end.y)
			, C32YELLOW);
		clipline32(B32
			, static_cast<S32>(-carCamera.x + rays[i][1].x)
			, static_cast<S32>(-carCamera.y + rays[i][1].y)
			, static_cast<S32>(-carCamera.x + end.x)
			, static_cast<S32>(-carCamera.y + end.y)
			, C32BLACK);
	}
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
