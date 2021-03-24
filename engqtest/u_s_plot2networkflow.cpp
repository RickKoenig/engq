// plotter2 runs in FPU control high precision
#include <m_eng.h>

#include "u_plotter2.h"

#define USENAMESPACE

using namespace u_plotter2;

#ifdef USENAMESPACE
namespace networkflowp2 {
#endif

	struct node {
		C8* label; // name
		pointf2 loc; // where on screen
		//kind kind;
		float inFlow;
		float outFlow;
		float actualInFlow;
		float actualOutFlow;
		vector<S32> next;
		vector<S32> nextIndex;
		vector<S32> prev;
		vector<S32> prevIndex;
	};

//#define SMALL
#define BIG
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
#ifdef BIG
#define OE 1.0/8.0
#define SE 7.0/8.0
	// define the network
	node nodes[] = {
		{"IN0"	,{-.5f, 1},  1},
		{"IN1"	,{-.5f, .75},  1},
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
		{"OUT2"	,{1.5f, .25},  0, 1},
		{"OUT3"	,{1.5f, 0},  0, 1},
	};
	S32 numnodes = NUMELEMENTS(nodes);

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
	S32 numedges = NUMELEMENTS(edges);
#endif
	// for debvars
	struct menuvar networkflowdv[] = {
		{"@green@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
#ifdef SMALL
		{"IN0",&nodes[0].inFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].inFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[3].outFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[4].outFlow,D_FLOAT,FLOATUP / 8},
#endif
#ifdef BIG
		{"IN0",&nodes[0].inFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].inFlow,D_FLOAT,FLOATUP / 8},
		{"IN2",&nodes[2].inFlow,D_FLOAT,FLOATUP / 8},
		{"IN3",&nodes[3].inFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[10].outFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[11].outFlow,D_FLOAT,FLOATUP / 8},
		{"OUT2",&nodes[12].outFlow,D_FLOAT,FLOATUP / 8},
		{"OUT3",&nodes[13].outFlow,D_FLOAT,FLOATUP / 8},
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
		S32 n0size = n0.next.size();
		node& n1 = nodes[e1];
		S32 n1size = n1.prev.size();
		n0.next.push_back(e1);
		n0.nextIndex.push_back(n1size);
		n1.prev.push_back(e0);
		n1.prevIndex.push_back(n0size);
	}
}

void plot2networkflowproc()
{
	// interact with graph paper
	plotter2proc();

	// range check values
#if 0
	nodes[0].inFlow = range(0.0f, nodes[0].inFlow, 1.0f);
	nodes[1].inFlow = range(0.0f, nodes[1].inFlow, 1.0f);
	nodes[3].outFlow = range(0.0f, nodes[3].outFlow, 1.0f);
	nodes[4].outFlow = range(0.0f, nodes[4].outFlow, 1.0f);
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
			if (anode.prev.empty()) { // source
				comp = ">=";
			} else if (anode.next.empty()) { // sink
				comp = "<=";
			} else {
				comp = "==";
			}
			SMALLFONT->outtextxybf32(B32, pi.x - 30, pi.y - 24, C32BLACK, C32WHITE, "%s", label);
			SMALLFONT->outtextxybf32(B32, pi.x - 50, pi.y - 12, C32BLACK, C32WHITE, "[%2.2f%s%2.2f]", anode.inFlow, comp, anode.outFlow);
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
