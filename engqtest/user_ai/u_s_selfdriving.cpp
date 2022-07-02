// self driving car
#include <m_eng.h>
#include <l_misclibm.h>
#include "../u_states.h"
#include "m_perf.h"

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
#include "u_trafficdata.h"

// UI
shape *rl, *focus, *oldfocus;
// user UI
hscroll* hslidesimspeed;
text* textsimspeed;

hscroll* hslidecarlinewidth;
text* textcarlinewidth;

hscroll* hslidenumaicars;
text* textnumaicars;

hscroll* hslidemutation;
text* textmutation;

pbut* pbutreset, *pbutquit, *pbuthumancamera;
pbut* pbutsave, *pbutdiscard, *pbutdefaulttraining;

// higher frame rate, was 30 now 60, TODO: should make everything at least 60 ...
S32 fpsSave;

// self driving
// parameters
S32 carCanvasWidth = 200;
S32 networkCanvasWidth = 250;

S32 numAICars;
S32 newNumAICars = 50;
const S32 maxAICars = 2500;

S32 carLineWidth = 1;
const S32 maxCarLineWidth = 8;

const bool humanAICar = true;

S32 mutateRate;
S32 newMutateRate = 10;
S32 mutateDen = 100;

S32 simSpeed = 1;
S32 maxSimSpeed = 20;

const C8* brainFile{ "brainHardCoded.brn" };
const C8* defaultBrainFile{ "brainHardCodedDefaultTrained.brn" };
bool useDefaultBrainFile;

const float trafficSpeed = 2;

const float carYStart = -100;
const float carYStep = -200;


// graphics
pointf2 carCamera; // global
S32 humanCamera = 0;
pointf2 visualizerCamera; // global

// classes
Road* road;
Car* myCar;
Car* bestCar;
vector<Car*> traffic;
vector<Car*> noTraffic;
vector<Car*> aiCars;

// for debvars
struct menuvar selfdrivingdv[] = {
	{"@green@--- self driving ---",NULL,D_VOID,0},
	{"humanCamera",&humanCamera,D_INT,1},
	{"carCameraX",&carCamera.x,D_FLOAT, FLOATUP * 16},
	{"carCameraY",&carCamera.y,D_FLOAT, FLOATUP * 16},
	{"visualizerCameraX",&visualizerCamera.x,D_FLOAT, FLOATUP * 16},
	{"visualizerCameraY",&visualizerCamera.y,D_FLOAT, FLOATUP * 16},
	{"simSpeed",&simSpeed,D_INT,1},
	{"fpswanted",&wininfo.fpswanted,D_INT,1},
	{"fpscurrent",&wininfo.fpscurrent,D_FLOAT | D_RDONLY,FLOATUP},
	{"fpsavg",&wininfo.fpsavg,D_FLOAT | D_RDONLY},
	{"intsectCount",&intsectCount,D_INT | D_RDONLY},
	{"intsectEarlyOut",&intsectEarlyOut,D_INT | D_RDONLY},
	{"sensorCount",&Sensor::sensorCount,D_INT | D_RDONLY},
	{"sensorEarlyOut",&Sensor::sensorEarlyOut,D_INT | D_RDONLY},
};
const int nselfdrivingdv = NUMELEMENTS(selfdrivingdv);

vector<Car*> generateTraffic(const vector<vector<S32>>& trafficData) {
	vector<Car*> traffic;
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
	simSpeed = hslidesimspeed->getidx();
	sprintf(str, "Simulation Speed = %d", simSpeed);
	textsimspeed->settname(str);

	carLineWidth = hslidecarlinewidth->getidx();
	sprintf(str, "Car Line Width = %d", carLineWidth);
	textcarlinewidth->settname(str);

	newNumAICars = hslidenumaicars->getidx();
	sprintf(str, "Num AI Cars %d ==> %d, '*'", newNumAICars, numAICars);
	textnumaicars->settname(str);

	newMutateRate = hslidemutation->getidx();
	sprintf(str, "Mutate Rate %5.2f ==> %5.2f, '*'", static_cast<float>(newMutateRate) / mutateDen, static_cast<float>(mutateRate) / mutateDen);
	textmutation->settname(str);
}

vector<Car*> generateAICars(S32 numAICars) 
{
	vector<Car*> cars;
	for (auto i = 0; i < numAICars; ++i) {
		cars.push_back(new Car(road->getLaneCenter(1), 100, 30, 50, Controls::ControlType::AI));
	}
	return cars;
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

	// pbuts
	pbutreset = rl->find<pbut>("PBUTRESET");
	pbutquit = rl->find<pbut>("PBUTQUIT");
	pbuthumancamera = rl->find<pbut>("PBUTHUMANCAMERA");
	pbutsave = rl->find<pbut>("PBUTSAVE");
	pbutdiscard = rl->find<pbut>("PBUTDISCARD");
	pbutdefaulttraining = rl->find<pbut>("PBUTDEFAULTTRAINING");
	// simspeed
	if (!simSpeed) {
		simSpeed = 1;
	}
	hslidesimspeed = rl->find<hscroll>("HSCROLLSIMSPEED");
	hslidesimspeed->setminmaxval(0, maxSimSpeed);
	hslidesimspeed->setidx(simSpeed);
	textsimspeed = rl->find<text>("TEXTSIMSPEED");

	// carlinewidth
	hslidecarlinewidth = rl->find<hscroll>("HSCROLLCARLINEWIDTH");
	hslidecarlinewidth->setminmaxval(0, maxCarLineWidth);
	hslidecarlinewidth->setidx(carLineWidth);
	textcarlinewidth = rl->find<text>("TEXTCARLINEWIDTH");

	// numaicars
	hslidenumaicars = rl->find<hscroll>("HSCROLLNUMAICARS");
	hslidenumaicars->setminmaxval(0, maxAICars);
	hslidenumaicars->setidx(newNumAICars);
	textnumaicars = rl->find<text>("TEXTNUMAICARS");

	// mutate
	hslidemutation = rl->find<hscroll>("HSCROLLMUTATION");
	hslidemutation->setminmaxval(0, 100);
	hslidemutation->setidx(newMutateRate);
	textmutation = rl->find<text>("TEXTMUTATION");

	numAICars = newNumAICars;
	mutateRate = newMutateRate;
	updateUI();
	// rest of UI setup
	focus = oldfocus = 0;

	// faster frame rate
	fpsSave = wininfo.fpswanted;
	wininfo.fpswanted = 60;
	adddebvars("selfdriving", selfdrivingdv, nselfdrivingdv);

	// self driving
	mt_setseed(getmicrosec());
	carCamera = { -3.0f * WX / 4.0f, 0 };
	visualizerCamera = { -450, 0 };
	road = new Road(carCanvasWidth / 2.0f, carCanvasWidth * .9f);
	if (humanAICar) {
		myCar = new Car(road->getLaneCenter(1), 100, 30, 50, Controls::ControlType::HUMAN);
	}
	aiCars = generateAICars(numAICars);
	bestCar = nullptr;
	if (numAICars) {
		bestCar = aiCars[0];
	}
	const C8* bf = useDefaultBrainFile ? defaultBrainFile : brainFile;
	useDefaultBrainFile = false;
	if (fileexist(bf)) {
		NeuralNetwork masterBrain(bf);
		for (auto i = 0U; i < aiCars.size(); ++i) {
			if (aiCars[i]->brain) { // TODO: look into unique_ptr
				delete (aiCars[i]->brain);
			}
			aiCars[i]->brain = new NeuralNetwork(masterBrain);
			if (i != 0) {
				aiCars[i]->brain->mutate(static_cast<float>(mutateRate) / mutateDen);
			}
		}
		if (myCar) {
			if (myCar->brain) {
				delete myCar->brain;
			}
			myCar->brain = new NeuralNetwork(masterBrain);
		}
	}
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
	// sliders
	if (focus == hslidesimspeed 
		|| focus == hslidenumaicars 
		|| focus == hslidemutation 
		|| focus == hslidecarlinewidth) {
		updateUI();
	// buttons
	} else if (ret == 1 || ret == 2) {
		if (focus == pbutquit) {
			poporchangestate(STATE_MAINMENU);
		} else if (focus == pbutreset) {
			changestate(STATE_SELFDRIVING);
		} else if (focus == pbuthumancamera) {
			humanCamera = !humanCamera;
		} else if (focus == pbutsave) {
			if (bestCar) {
				bestCar->brain->save(brainFile);
			}
		} else if (focus == pbutdefaulttraining) {
			useDefaultBrainFile = true; // one shot
		} else if (focus == pbutdiscard) {
			remove(brainFile);
		}
	}

	// self driving
	intsectCount = 0;
	intsectEarlyOut = 0;
	Sensor::sensorCount = 0;
	Sensor::sensorEarlyOut = 0;
	humanCamera = range(0, humanCamera, 1);
	simSpeed = range(0, simSpeed, maxSimSpeed);
	for (auto k = 0; k < simSpeed; ++k) {
		for (auto i = 0U; i < traffic.size(); ++i) {
			traffic[i]->update(road->borders, noTraffic);
		}
		for (auto i = 0U; i < aiCars.size(); ++i) {
			aiCars[i]->update(road->borders, traffic);
		}
		if (myCar) {
			myCar->update(road->borders, traffic);
		}

		if (aiCars.size()) {
			S32 bestIdx = 0;
			float bestY = aiCars[bestIdx]->y;
			for (auto i = 1U; i < aiCars.size(); ++i) {
				float y = aiCars[i]->y;
				if (y < bestY) {
					bestY = y;
					bestIdx = i;
				}
			}
			bestCar = aiCars[bestIdx];
		}
	}

	// position the camera on either the best car or the human car if it exists
	if (!myCar && !bestCar) {
		carCamera.y = 100; // nothing to show
		carCamera.y -= WY * .7f;
		return;
	}
	if (humanCamera && myCar) {
		carCamera.y = myCar->y;
	} else if (bestCar) {
		if (myCar) {
			carCamera.y = min(bestCar->y, myCar->y);
		} else {
			carCamera.y = bestCar->y;
		}
	} else {
		carCamera.y = myCar->y;
	}
	carCamera.y -= WY * .7f;
}

void selfdrivingdraw2d()
{
	C32 col;
	clipclear32(B32, C32(0, 0, 255));

	// visualizer
	if (myCar && humanCamera) {
		Visualizer::drawNetwork(myCar->brain, myCar->offsets);
	} else if (bestCar && bestCar->brain) {
		Visualizer::drawNetwork(bestCar->brain, bestCar->offsets);
	} else if (myCar) {
		Visualizer::drawNetwork(myCar->brain, myCar->offsets);
	}

	// self driving
	road->draw();

	for (auto i = 0U; i < traffic.size(); ++i) {
		traffic[i]->draw(C32RED); // traffic in red
	}
	//carCtx.globalAlpha = .1;
	for (auto i = 0U; i < aiCars.size(); ++i) {
		aiCars[i]->draw(C32(0,0,170,25)); // other ai cars in blue
	}
	// my human car in yellow
	if (myCar) {
		myCar->draw(C32YELLOW, true);
	}

	// draw front runner with sensors
	if (bestCar) {
		bestCar->draw(C32MAGENTA, true); // lead car in purple and draw sensors
	}

	// if car 0 is the same as lead car then draw cyan for car 0
	if (aiCars.size()) {
		aiCars[0]->draw(aiCars[0] == bestCar ? C32LIGHTCYAN : C32GREEN, true); // car 0 in green and draw sensors
	}

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

	for (auto car : aiCars) {
		delete car;
	}
	aiCars.clear();

	delete rl;
	popdir();
	removedebvars("selfdriving");
	wininfo.fpswanted = fpsSave;
}