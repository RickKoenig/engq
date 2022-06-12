#include <m_eng.h>

#include "u_s_selfdriving.h"

#ifdef DONAMESPACE
namespace selfdriving {
#endif

#include "u_utils.h"
#include "u_road.h"

Road::Road(float xCenter, float width, S32 laneCount) 
{
	float x = xCenter;
	fullWidth = 2 * xCenter;
	this->width = width;
	this->laneCount = laneCount;
	left = x - width / 2;
	right = x + width / 2;
	const float inf = 1'000'000; // for borders
	top = -inf;
	bot = inf;
	const vector<pointf2> leftSide = { {left, top}, {left, bot } };
	const vector<pointf2> rightSide = { {right, top}, {right, bot } };
	borders.push_back(leftSide);
	borders.push_back(rightSide);
}

float Road::getLaneCenter(S32 laneIndex) 
{
	const float laneWidth = width / laneCount;
	return left + laneWidth / 2
		+ min(laneCount - 1, laneIndex)*laneWidth;
}

void Road::draw()
{
	// background
	cliprect32(B32
		, static_cast<S32>(-carCamera.x)
		, static_cast<S32>(top)
		, static_cast<S32>(fullWidth)
		, static_cast<S32>(bot - top)
		, C32LIGHTGRAY);
	float lineLen = static_cast<float>(WY) / lineDash;
	float roadOffsetY = static_cast<float>(fmod(-carCamera.y, lineLen));
	roadOffsetY -= lineLen;
	// lanes
	for (auto i = 1; i <= laneCount; ++i) {
		const float x = lerp(
			left,
			right,
			static_cast<float>(i) / laneCount
		);
		clipline32dash(B32
			, static_cast<S32>(-carCamera.x + x)
			, static_cast<S32>(roadOffsetY)
			, static_cast<S32>(-carCamera.x + x)
			, static_cast<S32>(WY + roadOffsetY)
			, C32WHITE, lineDash);
	}
	// borders
	for (const auto& border : borders) {
		const pointf2& b0 = border[0];
		const pointf2& b1 = border[1];
		clipline32(B32
			, static_cast<S32>(-carCamera.x + b0.x)
			, static_cast<S32>(b0.y)
			, static_cast<S32>(-carCamera.x + b1.x)
			, static_cast<S32>(b1.y)
			,C32WHITE);
	}
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif

