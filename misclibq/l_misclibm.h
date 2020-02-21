namespace res2d {}
namespace res3d {}

//#define BERK, don't turn on here, turn on in 'l_socker_berk.h'

#ifdef RES3D
using namespace res3d;
#else
using namespace res2d;
#endif
// very handy extensions to engq
#include "l_factory.h" 		// hierarchal polymorphic generic class factory for scripts

#ifdef RES3D
#include "l_res3d.h"		// shape hierarchal polymorphic class for ui in 3d
#else
#include "l_res.h"			// shape hierarchal polymorphic class for ui
#endif
//#ifdef BERK
//#include "l_socker_berk.h"		// winsock tcp and udp implementation wrapper api
//#else
#include "l_socker.h"		// berkeley sockets tcp implementation wrapper api
//#endif
#include "l_vcam.h" 		// video capture, vfw.h is not complete, using msvfw.h, also supposed to be obsolete, doesn't work well anyways

#include "l_tagread.h"		// audio bank of waves
#include "particles.h" // simple particle system
