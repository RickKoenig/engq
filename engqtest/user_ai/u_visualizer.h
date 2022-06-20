class Visualizer {
	static const float nodeRadius;
public:
	static void drawNetwork(const NeuralNetwork* network, const vector<float>& sensorReadings);
	static void drawLevel(const Level& level, float left, float top, float width, float height, const C8* outputLabels);
};
