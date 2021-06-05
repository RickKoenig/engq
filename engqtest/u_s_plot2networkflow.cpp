// plotter2 runs in FPU control high precision
#include <m_eng.h>

#include "u_plotter2.h"

//#define USENAMESPACE

using namespace u_plotter2;

#ifdef USENAMESPACE
namespace networkflowp2 {
#endif
	struct pipe {
		pipe() : otherNodeIndex(10), otherEdgeIndex(20), value(3.1f) {}
		// point to other side of pipe
		S32 otherNodeIndex;
		S32 otherEdgeIndex;
		// value on this side
		float value;
	};

	struct node {
		//node() : label(nullptr), wantedInFlow(3.4f), wantedOutFlow(4.6f) {}
		// keep in this order
		C8* label; // name
		pointf2 loc; // where on screen
		//kind kind;
		float wantedInFlow;
		float wantedOutFlow;
		//float inFlowSum;
		//float outFlowSum;
		// end keep in this order
		vector<pipe> prevPipes;
		vector<pipe> nextPipes;
#if 0
		vector<float> inFlow;
		vector<float> outFlow;
		vector<S32> next;
		vector<S32> nextIndex;
		vector<S32> prev;
		vector<S32> prevIndex;
#endif
	};

//#define SUPER_SIMPLE
#define SIMPLE
//#define SMALL
//#define MISSING // big with 2 node splitters removed
//#define BIG

#ifdef SUPER_SIMPLE
	// define the network
	node nodes[] = {
		{"NODE0",{.5, .5}},
	};
	S32 numnodes = NUMELEMENTS(nodes);

	// connect the nodes
	S32** edges = nullptr;
	S32 numedges = 0;
#endif

#ifdef SIMPLE
	// define the network
	node nodes[] = {
		{"IN0"	,{-.5f, .5f},  1},
		{"NODE0",{.5, .5}},
		{"OUT0"	,{1.5f, .5f},  0, 1},
	};
	S32 numnodes = NUMELEMENTS(nodes);

	// connect the nodes
	S32 edges[][2] = {
		{0, 1},
		{1, 2},
	};
	S32 numedges = NUMELEMENTS(edges);
#endif

#ifdef SMALL
	// define the network
	node nodes[] = {
		{"IN0"	,{-.5f, 1},  1},
		{"IN1"	,{-.5f, 0},  1},
		{"NODE0",{.5, .5}},
		{"OUT0"	,{1.5f, 1},  0, 1},
		{"OUT1"	,{1.5f, 0},  0, 1},
	};
	S32 numnodes = NUMELEMENTS(nodes);

	// connect the nodes
	S32 edges[][2] = {
		{0, 2},
		{1, 2},
		{2, 3},
		{2, 4},
	};
	S32 numedges = NUMELEMENTS(edges);
#endif

#ifdef MISSING
#define OE 1.0/8.0
#define SE 7.0/8.0
	// define the network
	node nodes[] = {
		{"IN0"	,{-.5f, 1},  1},
		{"IN1"	,{-.5f, .75},  .75f},
		{"IN2"	,{-.5f, .25},  1},
		{"IN3"	,{-.5f, 0},  1},
		{"NODE0",{0, SE}},
		{"NODE1",{0, OE}},
		{"NODE2",{.5, SE}},
		{"NODE3",{.5, OE}},
//		{"NODE4",{1.0, SE}},
//		{"NODE5",{1.0, OE}},
		{"OUT0"	,{1.5f, 1},  0, 1},
		{"OUT1"	,{1.5f, .75},  0, 1},
		{"OUT2"	,{1.5f, .25},  0, .25f},
		{"OUT3"	,{1.5f, 0},  0, 1},
	};
	const S32 numnodes = NUMELEMENTS(nodes);

	// connect the nodes
	S32 edges[][2] = {
		{0, 4},
		{1, 4},
		{2, 5},
		{3, 5},
		{4, 6},
		{4, 7},
		{5, 6},
		{5, 7},
		{6, 8},
		{6, 9},
		{7, 10},
		{7, 11},
	};
	const S32 numedges = NUMELEMENTS(edges);
#endif

#ifdef BIG
#define OE 1.0/8.0
#define SE 7.0/8.0
	// define the network
	node nodes[] = {
		{"IN0"	,{-.5f, 1},  1},
		{"IN1"	,{-.5f, .75},  .75f},
		{"IN2"	,{-.5f, .25},  1},
		{"IN3"	,{-.5f, 0},  1},
		{"NODE0",{0, SE}},
		{"NODE1",{0, OE}},
		{"NODE2",{.5, SE}},
		{"NODE3",{.5, OE}},
		{"NODE4",{1.0, SE}},
		{"NODE5",{1.0, OE}},
		{"OUT0"	,{1.5f, 1},  0, 1},
		{"OUT1"	,{1.5f, .75},  0, 1},
		{"OUT2"	,{1.5f, .25},  0, .25f},
		{"OUT3"	,{1.5f, 0},  0, 1},
	};
	const S32 numnodes = NUMELEMENTS(nodes);

	// connect the nodes
	S32 edges[][2] = {
		{0, 4},
		{1, 4},
		{2, 5},
		{3, 5},
		{4, 6},
		{4, 7},
		{5, 6},
		{5, 7},
		{6, 8},
		{6, 9},
		{7, 8},
		{7, 9},
		{8, 10},
		{8, 11},
		{9, 12},
		{9, 13},
	};
	const S32 numedges = NUMELEMENTS(edges);
#endif

	// for debvars
	struct menuvar networkflowdv[] = {
		{"@green@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
#ifdef SUPER_SIMPLE
		{"IN0",&nodes[0].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[0].wantedOutFlow,D_FLOAT,FLOATUP / 8},
#endif
#ifdef SIMPLE
		{"IN0",&nodes[0].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[2].wantedOutFlow,D_FLOAT,FLOATUP / 8},
#endif
#ifdef SMALL
		{"IN0",&nodes[0].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[3].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[4].wantedOutFlow,D_FLOAT,FLOATUP / 8},
#endif
#ifdef MISSING
		{"IN0",&nodes[0].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN2",&nodes[2].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN3",&nodes[3].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[8].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[9].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT2",&nodes[10].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT3",&nodes[11].wantedOutFlow,D_FLOAT,FLOATUP / 8},
#endif
#ifdef BIG
		{"IN0",&nodes[0].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN2",&nodes[2].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"IN3",&nodes[3].wantedInFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[10].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[11].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT2",&nodes[12].wantedOutFlow,D_FLOAT,FLOATUP / 8},
		{"OUT3",&nodes[13].wantedOutFlow,D_FLOAT,FLOATUP / 8},
#endif
	};
	const int nnetworkflowdv = NUMELEMENTS(networkflowdv);

#ifdef USENAMESPACE
} // end namespace networkflowp2
using namespace networkflowp2;
#endif

void plot2networkflowinit()
{
	adddebvars("networkflow", networkflowdv, nnetworkflowdv);
	plotter2init();

	// calc prev and next from edges
	for (S32 i = 0; i < numedges; ++i) {
		S32* edge = edges[i];
		S32 e0 = edge[0];
		S32 e1 = edge[1];
		node& n0 = nodes[e0];
		node& n1 = nodes[e1];
		pipe nextPipe;
		pipe prevPipe;
		nextPipe.otherEdgeIndex = n1.prevPipes.size();
		nextPipe.otherNodeIndex = e1;
		prevPipe.otherEdgeIndex = n0.nextPipes.size();
		prevPipe.otherNodeIndex = e0;
		n0.nextPipes.push_back(nextPipe);
		n1.prevPipes.push_back(prevPipe);
#if 0
		S32 n0size = n0.next.size();
		S32 n1size = n1.prev.size();
		n0.next.push_back(e1);
		n0.nextIndex.push_back(n1size);
		n1.prev.push_back(e0);
		n1.prevIndex.push_back(n0size);
#endif
	}
}

void plot2networkflowproc()
{
	// interact with graph paper
	plotter2proc();

	// range check values
#ifdef SUPER_SIMPLE
	nodes[0].wantedInFlow = range(0.0f, nodes[0].wantedInFlow, 1.0f);
	nodes[0].wantedOutFlow = range(0.0f, nodes[0].wantedOutFlow, 1.0f);
#endif
#ifdef SIMPLE
	nodes[0].wantedInFlow = range(0.0f, nodes[0].wantedInFlow, 1.0f);
	nodes[2].wantedOutFlow = range(0.0f, nodes[2].wantedOutFlow, 1.0f);
#endif
#ifdef SMALL
	nodes[0].wantedInFlow = range(0.0f, nodes[0].wantedInFlow, 1.0f);
	nodes[1].wantedInFlow = range(0.0f, nodes[1].wantedInFlow, 1.0f);
	nodes[3].wantedOutFlow = range(0.0f, nodes[3].wantedOutFlow, 1.0f);
	nodes[4].wantedOutFlow = range(0.0f, nodes[4].wantedOutFlow, 1.0f);
#endif
#ifdef MISSING
	nodes[0].wantedInFlow = range(0.0f, nodes[0].wantedInFlow, 1.0f);
	nodes[1].wantedInFlow = range(0.0f, nodes[1].wantedInFlow, 1.0f);
	nodes[2].wantedInFlow = range(0.0f, nodes[2].wantedInFlow, 1.0f);
	nodes[3].wantedInFlow = range(0.0f, nodes[3].wantedInFlow, 1.0f);
	nodes[8].wantedOutFlow = range(0.0f, nodes[8].wantedOutFlow, 1.0f);
	nodes[9].wantedOutFlow = range(0.0f, nodes[9].wantedOutFlow, 1.0f);
	nodes[10].wantedOutFlow = range(0.0f, nodes[10].wantedOutFlow, 1.0f);
	nodes[11].wantedOutFlow = range(0.0f, nodes[11].wantedOutFlow, 1.0f);
#endif
#ifdef BIG
	nodes[0].wantedInFlow = range(0.0f, nodes[0].wantedInFlow, 1.0f);
	nodes[1].wantedInFlow = range(0.0f, nodes[1].wantedInFlow, 1.0f);
	nodes[2].wantedInFlow = range(0.0f, nodes[2].wantedInFlow, 1.0f);
	nodes[3].wantedInFlow = range(0.0f, nodes[3].wantedInFlow, 1.0f);
	nodes[10].wantedOutFlow = range(0.0f, nodes[10].wantedOutFlow, 1.0f);
	nodes[11].wantedOutFlow = range(0.0f, nodes[11].wantedOutFlow, 1.0f);
	nodes[12].wantedOutFlow = range(0.0f, nodes[12].wantedOutFlow, 1.0f);
	nodes[13].wantedOutFlow = range(0.0f, nodes[13].wantedOutFlow, 1.0f);
#endif

	// re adjust network

}

void plot2networkflowdraw2d()
{
	// draw graph paper
	plotter2draw2d();

	// draw edges
	for (S32 i = 0; i < numedges; ++i) {
		S32* edge = edges[i];
		drawfline(nodes[edge[0]].loc, nodes[edge[1]].loc, C32BLACK);
	}

	// draw nodes and node labels
	for (S32 i = 0; i < numnodes; ++i) {
		node& anode = nodes[i];
		drawfpoint(anode.loc,C32BLACK);
		pointi2 pi = math2screen(anode.loc);
		const C8* label = anode.label ? anode.label : "";
		C8* comp;
		if (label) {
			if (anode.prevPipes.empty()) { // maybe source 
				if (anode.nextPipes.empty()) { // just a single isolated node
					comp = "<>";
				} else { // source
					comp = ">=";
				}
			} else { // not a source
				if (anode.nextPipes.empty()) { // sink
					comp = "<=";
				} else { // a middle node
					comp = "==";
				}
			}
			SMALLFONT->outtextxybf32(B32, pi.x - 30, pi.y - 24, C32BLACK, C32WHITE, "%s", label);
			SMALLFONT->outtextxybf32(B32, pi.x - 50, pi.y - 12, C32BLACK, C32WHITE, "[%4.2f%s%4.2f]", anode.wantedInFlow, comp, anode.wantedOutFlow);
		}
	}
	// leave this here for now
	LARGEFONT->outtextxybf32(B32, WX / 2,  20, C32GREEN, C32BLACK, "%s", "LARGE FONT");
	MEDIUMFONT->outtextxybf32(B32, WX / 2,  60, C32GREEN, C32BLACK, "%s", "MEDIUM FONT");
	SMALLFONT->outtextxybf32(B32, WX / 2, 100, C32GREEN, C32BLACK, "%s", "SMALL FONT");
}

void plot2networkflowexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("networkflow");
}
