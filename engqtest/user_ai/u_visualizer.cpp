#include <m_eng.h>

#include "u_s_selfdriving.h"
#ifdef DONAMESPACE
namespace selfdriving {
#endif

#include "u_utils.h"
#include "u_network.h"
#include "u_visualizer.h"
	
const float Visualizer::nodeRadius = 18;

void Visualizer::drawNetwork(const NeuralNetwork* network, const vector<float>& sensorReadings)
{
	const float fullWidth = 300;
	const float fullHeight = static_cast<float>(WY);
	const float margin = 50;
	const float left = margin;
	const float top = margin;
	const float width = fullWidth - margin * 2;
	const float height = fullHeight - margin * 2;
	const float levelHeight = height / network->levels.size();

	// background
	cliprect32(B32
		, static_cast<S32>(-visualizerCamera.x)
		, static_cast<S32>(-visualizerCamera.y)
		, static_cast<S32>(fullWidth), WY, C32BLACK);

	// draw each level, a level has inputs, outputs, weights, biases
	for (S32 i = network->levels.size() - 1; i >= 0; i--) {
		const char controlString[] = "^<>v";
		const float levelTop = top +
			lerpStep(network->levels.size(), i, height - levelHeight, 0);
		drawLevel(network->levels[i]
			, left - visualizerCamera.x, levelTop - visualizerCamera.y
			, width, levelHeight
			, i == network->levels.size() - 1 // last level label controls
				? controlString
				: nullptr
		);

	}
#if 1
	for (auto i = 0U; i < sensorReadings.size(); ++i) {
		const float x = lerpStep(sensorReadings.size(), i, left, left + width);
		clipcircle32(B32
			, static_cast<S32>(x - visualizerCamera.x)
			, static_cast<S32>(top + height - visualizerCamera.y)
			, static_cast<S32>(nodeRadius)
			, C32BLACK);
		clipcircle32(B32
			, static_cast<S32>(x - visualizerCamera.x)
			, static_cast<S32>(top + height - visualizerCamera.y)
			, static_cast<S32>(nodeRadius * .6f)
			, getRGBA(sensorReadings[i]));
	}
#endif
	// test colors value palette
	const U32 numColors = 20;
	for (auto i = 0U; i < numColors; ++i) {
		const float x = lerpStep(numColors, i, left, left + width);
		const float v = lerpStep(numColors, i, -1, 1);
		C32 col = getRGBA(v);
		clipcircle32(B32, static_cast<S32>(x - visualizerCamera.x), static_cast<S32>(top + height + margin / 2 - visualizerCamera.y), 4, col);
	}
}

void Visualizer::drawLevel(const Level& level, float left, float top, float width, float height, const C8* outputLabels)
{
	const float right = left + width;
	const float bottom = top + height;

	//const vector<float>& inputs = level.inputs;
	const vector<float>& outputs = level.outputs;
	const vector<float>& biases = level.biases;
	const vector<vector<float>>& weights = level.weights;

	// draw weights as a line from inputs to outputs
	for (auto i = 0U; i < level.inputSize; ++i) {
		for (auto j = 0U; j < outputs.size(); ++j) {
			clipline32(B32
				, static_cast<S32>(lerpStep(level.inputSize, i, left, right))
				, static_cast<S32>(bottom)
				, static_cast<S32>(lerpStep(outputs.size(), j, left, right))
				, static_cast<S32>(top)
				, getRGBA(weights[i][j]));
		}
	}

	// draw input nodes
#if 0
	if (true) {
		for (auto i = 0U; i < inputs.size(); ++i) {
			const float x = lerpStep(inputs.size(), i, left, right);
			clipcircle32(B32
				, static_cast<S32>(x)
				, static_cast<S32>(bottom)
				, static_cast<S32>(nodeRadius)
				, C32BLACK);
			clipcircle32(B32
				, static_cast<S32>(x)
				, static_cast<S32>(bottom)
				, static_cast<S32>(nodeRadius * .6f)
				, getRGBA(inputs[i]));
		}
	}
#endif
	// draw output nodes and biases
	for (auto i = 0U; i < outputs.size(); ++i) {
		const float x = lerpStep(outputs.size(), i, left, right);
		clipcircle32(B32
			, static_cast<S32>(x)
			, static_cast<S32>(top)
			, static_cast<S32>(nodeRadius)
			, C32BLACK);
		clipcircle32(B32
			, static_cast<S32>(x)
			, static_cast<S32>(top)
			, static_cast<S32>(nodeRadius * .6f)
			, getRGBA(outputs[i]));
		// draw bias for outputs
		clipcircleo32(B32
			, static_cast<S32>(x)
			, static_cast<S32>(top)
			, static_cast<S32>(nodeRadius * .8f)
			, getRGBA(biases[i]));
		// draw labels for last output
		if (outputLabels) {
			outtextxybf32(B32
				, static_cast<S32>(x - 4)
				, static_cast<S32>(top - 4)
				, C32WHITE, C32BLACK, "%c", outputLabels[i]);
		}
	}
}

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
