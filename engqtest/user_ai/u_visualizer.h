class Visualizer {
public:
	static void drawNetwork(const NeuralNetwork* network)
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

#if 0
		for (auto i = network->levels.size() - 1; i >= 0; i--) {

			const levelTop = top +
				this.#getNodeX(network.levels, i, height - levelHeight, 0);
			ctx.setLineDash([7, 3]);
			Visualizer.#drawLevel(ctx, network.levels[i],
				left, levelTop,
				width, levelHeight,
				i == network.levels.length - 1
				? ["↑", "←", "→", "↓"]
				: []
			);

		}
#endif
#if 1
		// test colors value palette
		const U32 numColors = 20;
		for (auto i = 0U; i < numColors; ++i) {
#if 1
			const float x = getNodeX(numColors, i, left, left + width);
			const float v = getNodeX(numColors, i, -1, 1);
#endif
#if 0
			ctx.beginPath();
			ctx.arc(x, top + height + margin / 2, 5, 0, Math.PI * 1.75);
			ctx.fillStyle = getRGBA(v);
			ctx.fill();
#endif
			C32 col = getRGBA(v);
			clipcircle32(B32, static_cast<S32>(x - visualizerCamera.x), static_cast<S32>(top + height + margin / 2 - visualizerCamera.y), 4, col);
		}
#endif
	}

	static float getNodeX(U32 len, S32 index, float left, float right) {
		return lerp(
			left,
			right,
			len == 1
			? .5f
			: static_cast<float>(index) / (len - 1)
		);
	}

};

#if 0
class Visualizer {

	static #drawLevel(ctx, level, left, top, width, height, outputLabels) {
		const right = left + width;
		const bottom = top + height;
		const {inputs, outputs, weights, biases} = level;

		for (let i = 0; i < inputs.length; ++i) {
			for (let j = 0; j < outputs.length; ++j) {
				ctx.beginPath();
				ctx.lineTo(
					Visualizer.#getNodeX(inputs, i, left, right),
					bottom
				);
				ctx.lineTo(
					Visualizer.#getNodeX(outputs, j, left, right),
					top
				);
				ctx.lineWidth = 2;
				ctx.strokeStyle = getRGBA(weights[i][j]);
				ctx.stroke();
			}
		}

		const nodeRadius = 18;
		for (let i = 0; i < inputs.length; ++i) {
			const x = Visualizer.#getNodeX(inputs, i, left, right);
			ctx.beginPath();
			ctx.arc(x, bottom, nodeRadius, 0, Math.PI * 2);
			ctx.fillStyle = "black";
			ctx.fill();
			ctx.beginPath();
			ctx.arc(x, bottom, nodeRadius * .6, 0, Math.PI * 2);
			ctx.fillStyle = getRGBA(inputs[i]);
			ctx.fill();
		}

		for (let i = 0; i < outputs.length; ++i) {
			const x = Visualizer.#getNodeX(outputs, i, left, right);
			ctx.beginPath();
			ctx.arc(x, top, nodeRadius, 0, Math.PI * 2);
			ctx.fillStyle = "black";
			ctx.fill();
			ctx.beginPath();
			ctx.arc(x, top, nodeRadius * .6, 0, Math.PI * 2);
			ctx.fillStyle = getRGBA(outputs[i]);
			ctx.fill();

			ctx.beginPath();
			ctx.lineWidth = 2;
			ctx.arc(x, top, nodeRadius * .8, 0, Math.PI * 2);
			ctx.strokeStyle = getRGBA(biases[i]);
			const saveTimeOffset = ctx.lineDashOffset;
			ctx.lineDashOffset = 0;
			ctx.setLineDash([3, 3]);
			ctx.stroke();
			ctx.setLineDash([]);
			ctx.lineDashOffset = saveTimeOffset;

			if (outputLabels[i]) {
				ctx.beginPath();
				ctx.textAlign = "center";
				ctx.textBaseLine = "middle";
				ctx.fillStyle = "black";
				ctx.strokeStyle = "white";
				ctx.font = (nodeRadius * 1.25) + "px Arial";
				const fixCenter = .375;
				ctx.fillText(outputLabels[i], x, top + nodeRadius * fixCenter);
				ctx.lineWidth = .5;
				ctx.strokeText(outputLabels[i], x, top + nodeRadius * fixCenter);

			}
		}
	}


#endif

