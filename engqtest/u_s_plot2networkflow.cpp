// plotter2 runs in FPU control high precision
#include <m_eng.h>

#include "u_plotter2.h"

using namespace u_plotter2;

namespace networkflowp2 {
	struct node {
		pointf2 loc;
		C8* label;
		//kind kind;
		float inFlow;
		float outFlow;
		vector<S32> next;
		vector<S32> nextIndex; // TODO: populate these
		vector<S32> prev;
		vector<S32> prevIndex;
	};

	// define the network
	node nodes[] = {
		{{-.5f, 1}, "IN0", 1},
		{{-.5f, 0}, "IN1", 1},
		{{.5, .5}, "NODE0"},
		{{1.5f, 1}, "OUT0", 0, 1},
		{{1.5f, 0}, "OUT1", 0, 1},
	};
	S32 numnodes = NUMELEMENTS(nodes);

	S32 edges[][2] = {
		{0, 2},
		{1, 2},
		{2, 3},
		{2, 4},
	};
	S32 numedges = NUMELEMENTS(edges);

	// for debvars
	struct menuvar networkflowdv[] = {
		{"@green@--- PLOTTER2 USER VARS ---",NULL,D_VOID,0},
		{"IN0",&nodes[0].inFlow,D_FLOAT,FLOATUP / 8},
		{"IN1",&nodes[1].inFlow,D_FLOAT,FLOATUP / 8},
		{"OUT0",&nodes[3].outFlow,D_FLOAT,FLOATUP / 8},
		{"OUT1",&nodes[4].outFlow,D_FLOAT,FLOATUP / 8},
	};
	const int nnetworkflowdv = NUMELEMENTS(networkflowdv);

} // end namespace networkflowp2

using namespace networkflowp2;

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
		n0.next.push_back(e1);
		n1.prev.push_back(e0);
	}
}

void plot2networkflowproc()
{
	// interact with graph paper
	plotter2proc();

	// range check values
	nodes[0].inFlow = range(0.0f, nodes[0].inFlow, 1.0f);
	nodes[1].inFlow = range(0.0f, nodes[1].inFlow, 1.0f);
	nodes[3].outFlow = range(0.0f, nodes[3].outFlow, 1.0f);
	nodes[4].outFlow = range(0.0f, nodes[4].outFlow, 1.0f);

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
		C8* label = anode.label ? anode.label : "";
		C8* comp;
		if (label) {
			if (anode.prev.empty()) { // source
				comp = ">=";
			} else if (anode.next.empty()) { // sink
				comp = "<=";
			} else {
				comp = "==";
			}
			outtextxybf32(B32, pi.x - 20, pi.y + 16, C32BLACK, C32WHITE, "%s, [%2.2f %s %2.2f]", label, anode.inFlow, comp, anode.outFlow);
		}
	}
}

void plot2networkflowexit()
{
	// free debvars, write out colors to plotter.bin
	plotter2exit();
	removedebvars("networkflow");
}
