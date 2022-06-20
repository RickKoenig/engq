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
#include "u_utils.h"

Sensor::Sensor(class Car* car)
	: car(car)
	, rayCount(5)
	, rayLength(150)
	, raySpread(PI / 2)
	, rays(rayCount, vector<pointf2>(2))
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
#if 1
	perf_start(TEST1);
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
	perf_end(TEST1);

	perf_start(TEST2);
	for (auto i = 0U; i < traffic.size(); ++i) {
		++sensorCount;
		const Car* c = traffic[i];
#if 1
		const float rayLenPlusCarRad = rayLength + c->rad;
		const float delx = c->x - ray[0].x;
		const float dely = c->y - ray[0].y;
		const float dist2 = delx * delx + dely * dely;
		const pointf2& trafficCarPos = pointf2{ c->x, c->y };
		//const float farDist2 = c->width * c->width + c->height * c->height;
		if (rayLenPlusCarRad * rayLenPlusCarRad < dist2) {
			++sensorEarlyOut;
			continue;
		}
#endif

		const auto& poly = c->polygon;
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
	perf_end(TEST2);
#endif
	return bestTouch;
}

void Sensor::castRays()
{
	float x = car->x;
	float y = car->y;
	float angle = car->angle;
//	rays.clear();
	for (auto i = 0; i < rayCount; ++i) {

#if 0
		const float rayAngle = lerp(
			raySpread / 2,
			-raySpread / 2,
			rayCount == 1 ? .5f : static_cast<float>(i) / (rayCount - 1)
		) + angle;
#else
		const float rayAngle = lerpStep(
			rayCount,
			i,
			raySpread / 2,
			-raySpread / 2
		) + angle;
#endif

		const pointf2 start = { x, y };
		const pointf2 end = {
			x - sin(rayAngle) * rayLength,
			y - cos(rayAngle) * rayLength
		};
		//vector<pointf2> ray{ start, end };
		vector<pointf2>& aRay = rays[i];
		aRay[0] = start;
		aRay[1] = end;
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

S32 Sensor::sensorCount;
S32 Sensor::sensorEarlyOut;


#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
