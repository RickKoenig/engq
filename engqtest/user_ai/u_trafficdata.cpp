#include <m_eng.h>

#include "u_s_selfdriving.h"
#ifdef DONAMESPACE
namespace selfdriving {
#endif

#include "u_trafficdata.h"

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
#if 1
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
#endif
};

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif

