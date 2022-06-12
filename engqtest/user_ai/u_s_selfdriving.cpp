// self driving car
#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"

#include "u_s_selfdriving.h"

#ifdef DONAMESPACE
namespace selfdriving {
#endif


#include "u_utils.h"
#include "u_road.h"
#include "u_controls.h"
#include "u_network.h"
#include "u_sensor.h"
#include "u_car.h"
#include "u_visualizer.h"

// UI
shape *rl, *focus, *oldfocus;
// user UI
hscroll* hslideiter;
text* textiter;
pbut* pbutreset, *pbutquit;


// higher frame rate, was 30 now 60, TODO: should make everything at least 60 ...
S32 fpsSave;

// self driving
S32 carCanvasWidth = 200;
S32 networkCanvasWidth = 250;
Road* road;
Car* myCar;
pointf2 carCamera; // global
S32 autoCarCamera = 1;
pointf2 visualizerCamera; // global
vector<Car*> traffic;
S32 simSpeed = 1;
S32 maxSimSpeed = 10;

// for debvars
struct menuvar selfdrivingdv[] = {
	{"@green@--- self driving ---",NULL,D_VOID,0},
	{"autoCarCamera",&autoCarCamera,D_INT,1},
	{"carCameraX",&carCamera.x,D_FLOAT, FLOATUP * 16},
	{"carCameraY",&carCamera.y,D_FLOAT, FLOATUP * 16},
	{"visualizerCameraX",&visualizerCamera.x,D_FLOAT, FLOATUP * 16},
	{"visualizerCameraY",&visualizerCamera.y,D_FLOAT, FLOATUP * 16},
	{"simSpeed",&simSpeed,D_INT,1},
	{"fpswanted",&wininfo.fpswanted,D_INT,1},
	{"fpscurrent",&wininfo.fpscurrent,D_FLOAT | D_RDONLY,FLOATUP},
	{"fpsavg",&wininfo.fpsavg,D_FLOAT | D_RDONLY},
};
const int nselfdrivingdv = NUMELEMENTS(selfdrivingdv);


const vector<vector<S32>> trafficData = {
	// tunnel
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{1, 0, 1},
	{1, 0, 1},
	{1, 0, 1},
	{1, 1, 0},
	{1, 1, 0},
	{1, 1, 0},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},

	// standard
	{0, 1, 0},
	{1, 0, 1},
	{1, 1, 0},
	{0, 1, 1},

	// easy
	{0, 0, 0},
	{0, 0, 0},
	{1, 0, 0},

	{0, 0, 0},
	{0, 0, 0},
	{0, 1, 0},

	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 1},

	// hard
	{1, 1, 0},
	{1, 0, 1},
	{1, 1, 0},
	{1, 0, 1},
	{0, 1, 1},
	{1, 0, 1},
	{0, 1, 1},
	{1, 0, 1},
	{1, 1, 0},
	{1, 0, 1},
	{1, 1, 0},

	// small break
	{0, 0, 0},

	// harder
	{0, 1, 1},
	{1, 1, 0},
	{0, 1, 1},
	{1, 1, 0},

	// tunnel
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{1, 0, 1},
	{1, 0, 1},
	{1, 0, 1},
	{1, 1, 0},
	{1, 1, 0},
	{1, 1, 0},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1}
};

vector<Car*> generateTraffic(const vector<vector<S32>>& trafficData) {
	vector<Car*> traffic;
	//const float trafficSpeed = 2;
	const float trafficSpeed = 0;
	const float carYStart = -100;
	const float carYStep = -200;
	for (auto i = 0U; i < trafficData.size(); ++i) {
		for (auto j = 0U; j < trafficData[i].size(); ++j) {
			if (trafficData[i][j]) {
				const auto car = new Car(
					road->getLaneCenter(j)
					, carYStart + i * carYStep
					, 30, 50, Controls::ControlType::DUMMY, trafficSpeed);
				traffic.push_back(car);
			}
		}
	}
	return traffic;
}

void updateUI()
{
	char str[100];
	//hslideiter->setidx(simSpeed);
	simSpeed = hslideiter->getidx();
	//iter = hslideiter->getidx();
	sprintf(str, "Simulation speed = %d", simSpeed);
	textiter->settname(str);
}


#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif

void selfdrivinginit()
{
	// window
	//changeWindowTitle("this is self driving"); // experimental
	video_setupwindow(1024, 768);

	// UI
	pushandsetdir("selfdriving");
	script sc;
	rl = res_loadfile("selfdrivingres.txt");
	// text
	textiter = rl->find<text>("TEXTSIMSPEED");
	// pbuts
	pbutreset = rl->find<pbut>("PBUTRESET");
	pbutquit = rl->find<pbut>("PBUTQUIT");
	// sliders
	hslideiter = rl->find<hscroll>("HSCROLLSIMSPEED");
	hslideiter->setminmaxval(1, maxSimSpeed);
	hslideiter->setidx(simSpeed);
	updateUI();
	// rest of UI setup
	focus = oldfocus = 0;

	// faster frame rate
	fpsSave = wininfo.fpswanted;
	wininfo.fpswanted = 60;


	// self driving
	mt_setseed(getmicrosec());
	carCamera = { -3.0f * WX / 4.0f, 0 };
	visualizerCamera = { -450, 0 };
	road = new Road(carCanvasWidth / 2.0f, carCanvasWidth * .9f);
	adddebvars("selfdriving", selfdrivingdv, nselfdrivingdv);
	myCar = new Car(road->getLaneCenter(1), 100, 30, 50, Controls::ControlType::HUMAN);
	traffic = generateTraffic(trafficData);
}

void selfdrivingproc()
{
	if (KEY == K_ESCAPE)
		poporchangestate(STATE_MAINMENU);

	// UI
	// handle buttons and sliders
	if (wininfo.mleftclicks || wininfo.mrightclicks) {
		focus = rl->getfocus();
	}
	S32 ret = -1;
	if (focus) {
		ret = focus->proc();
	}
	if (oldfocus && oldfocus != focus) {
		oldfocus->deactivate();
	}
	oldfocus = focus;
	if (focus == pbutquit) {
		if (ret == 1 || ret == 2) {
			poporchangestate(STATE_MAINMENU);
		}
	} else if (focus == pbutreset) {
		if (ret == 1 || ret == 2) {
			changestate(STATE_SELFDRIVING);
		}
	} else if (focus == hslideiter) {
		updateUI();
	}


	// self driving
	autoCarCamera = range(0, autoCarCamera, 1);
	simSpeed = range(0, simSpeed, maxSimSpeed);
	vector<Car*> noTraffic;
	for (auto k = 0; k < simSpeed; ++k) {
		for (auto i = 0U; i < traffic.size(); ++i) {
			traffic[i]->update(road->borders,noTraffic);
		}
		myCar->update(road->borders, traffic);
	}
	if (autoCarCamera) {
		carCamera.y = myCar->y - WY * .7f;
	}
#if 0
	for (let k = 0; k < simSpeed; ++k) {
		for (let i = 0; i < cars.length; ++i) {
			cars[i].update(road.borders, traffic);
		}

		bestCar = cars.find(
			c = > c.y == Math.min(
				...cars.map(c = > c.y)
			));

	}
	carCanvas.height = window.innerHeight;
	networkCanvas.height = window.innerHeight;
	carCtx.save();
	carCtx.translate(0, -bestCar.y + carCanvas.height * .7);
	for (let i = 0; i < traffic.length; ++i) {
		traffic[i].draw(carCtx, "red"); // traffic in red
	}
	carCtx.globalAlpha = .1;
	for (let i = 0; i < cars.length; ++i) {
		cars[i].draw(carCtx, "blue"); // other ai cars in blue
	}
	carCtx.globalAlpha = 1;
	bestCar.draw(carCtx, "purple", true); // lead car in purple and draw sensors

	// if car 0 is the same as lead car then draw cyan for car 0
	cars[0].draw(carCtx, cars[0] == bestCar ? "cyan" : "green", true); // car 0 in green and draw sensors

	carCtx.restore();

	networkCtx.lineDashOffset = -time / 50;
	if (bestCar.brain) {
		Visualizer.drawNetwork(networkCtx, bestCar.brain);
	}
	requestAnimationFrame(animate);
#endif
}

void selfdrivingdraw2d()
{
	C32 col;
	clipclear32(B32, C32(0, 0, 255));

	// visualizer
	if (myCar->brain) {
		Visualizer::drawNetwork(myCar->brain);
	}

	// self driving
	road->draw();
	for (auto i = 0U; i < traffic.size(); ++i) {
		traffic[i]->draw(C32RED); // traffic in red
	}
	myCar->draw(C32GREEN, true);

	// UI
	rl->draw();
}

void selfdrivingexit()
{
	delete road;
	delete myCar;
	for (auto car : traffic) {
		delete car;
	}
	traffic.clear();

	delete rl;
	popdir();
	removedebvars("selfdriving");
	wininfo.fpswanted = fpsSave;
}

#if 0
const carCanvas = document.getElementById("carCanvas");
carCanvas.width = 200;
const networkCanvas = document.getElementById("networkCanvas");
networkCanvas.width = 300;

const carCtx = carCanvas.getContext("2d");
const networkCtx = networkCanvas.getContext("2d");
const road = new Road(carCanvas.width / 2, carCanvas.width*.9);

const N = 250;
const mutateRate = .1;
const simSpeed = 5;

const cars = generateCars(N);
let bestCar = cars[0];
if (localStorage.getItem("bestBrain")) {
	for (let i = 0; i < cars.length; ++i) {
		cars[i].brain = JSON.parse(
			localStorage.getItem("bestBrain")
		);
		if (i != 0) {
			NeuralNetwork.mutate(cars[i].brain, mutateRate);
		}
	}
}

const trafficData = [

	// tunnel
	[0, 1, 1],
		[0, 1, 1],
		[0, 1, 1],
		[1, 0, 1],
		[1, 0, 1],
		[1, 0, 1],
		[1, 1, 0],
		[1, 1, 0],
		[1, 1, 0],
		[0, 1, 1],
		[0, 1, 1],
		[0, 1, 1],

		// standard
		[0, 1, 0],
		[1, 0, 1],
		[1, 1, 0],
		[0, 1, 1],

		// easy
		[0, 0, 0],
		[0, 0, 0],
		[1, 0, 0],

		[0, 0, 0],
		[0, 0, 0],
		[0, 1, 0],

		[0, 0, 0],
		[0, 0, 0],
		[0, 0, 1],

		// hard
		[1, 1, 0],
		[1, 0, 1],
		[1, 1, 0],
		[1, 0, 1],
		[0, 1, 1],
		[1, 0, 1],
		[0, 1, 1],
		[1, 0, 1],
		[1, 1, 0],
		[1, 0, 1],
		[1, 1, 0],

		// small break
		[0, 0, 0],

		// harder
		[0, 1, 1],
		[1, 1, 0],
		[0, 1, 1],
		[1, 1, 0],

		// tunnel
		[0, 1, 1],
		[0, 1, 1],
		[0, 1, 1],
		[1, 0, 1],
		[1, 0, 1],
		[1, 0, 1],
		[1, 1, 0],
		[1, 1, 0],
		[1, 1, 0],
		[0, 1, 1],
		[0, 1, 1],
		[0, 1, 1]
];

function generateTraffic(trafficData) {
	let traffic = [];
	const carYStart = -100;
	const carYStep = -200;
	for (let i = 0; i < trafficData.length; ++i) {
		for (let j = 0; j < trafficData[i].length; ++j) {
			if (trafficData[i][j]) {
				const car = new Car(
					road.getLaneCenter(j)
					, carYStart + i * carYStep
					, 30, 50, "DUMMY", 2);
				traffic.push(car);
			}
		}

	}
	return traffic;
}

const traffic = generateTraffic(trafficData);

animate();

function save() {
	localStorage.setItem("bestBrain",
		JSON.stringify(bestCar.brain)
	);
}

function discard() {
	localStorage.removeItem("bestBrain");
}

function generateCars(N) {
	const cars = [];
	for (let i = 1; i <= N; ++i) {
		cars.push(new Car(road.getLaneCenter(1), 100, 30, 50, "AI"));
	}
	return cars;
}

function animate(time) {
	for (let k = 0; k < simSpeed; ++k) {
		for (let i = 0; i < traffic.length; ++i) {
			traffic[i].update(road.borders, []);
		}
		for (let i = 0; i < cars.length; ++i) {
			cars[i].update(road.borders, traffic);
		}

		bestCar = cars.find(
			c = > c.y == Math.min(
				...cars.map(c = > c.y)
			));

	}
	carCanvas.height = window.innerHeight;
	networkCanvas.height = window.innerHeight;
	carCtx.save();
	carCtx.translate(0, -bestCar.y + carCanvas.height * .7);
	road.draw(carCtx);
	for (let i = 0; i < traffic.length; ++i) {
		traffic[i].draw(carCtx, "red"); // traffic in red
	}
	carCtx.globalAlpha = .1;
	for (let i = 0; i < cars.length; ++i) {
		cars[i].draw(carCtx, "blue"); // other ai cars in blue
	}
	carCtx.globalAlpha = 1;
	bestCar.draw(carCtx, "purple", true); // lead car in purple and draw sensors

	// if car 0 is the same as lead car then draw cyan for car 0
	cars[0].draw(carCtx, cars[0] == bestCar ? "cyan" : "green", true); // car 0 in green and draw sensors

	carCtx.restore();

	networkCtx.lineDashOffset = -time / 50;
	if (bestCar.brain) {
		Visualizer.drawNetwork(networkCtx, bestCar.brain);
	}
	requestAnimationFrame(animate);
}
#endif
