//#define __GNUC__  // mingw
//#define NDEBUG    // release mode
//#define C_IS_CPP  // .c code compiles as .cpp code

// mingw studio version of the engine, call it engm after engine minimalistic mingw
// now works in devstud
// use only 32 bit color for simplicity, see engine1 for 8, 16(555 and 565), 24, and 32 bit color depths.
// only 32 bit color operations implemented. (and some very basic 8bit stuff for the .pcx's)
// uses rtti, doesn't use exceptions

//#define INCLUDE_WINDOWS, put this in your code to pull in windows
//#define D2_3D, brings in device independent 3D support
//#define D2_DX9, brings in hardware direct3d support

// for the middle button of the  mouse
#define _WIN32_WINNT 0x0500
#define _CRT_SECURE_NO_DEPRECATE // get rid of security warnings like sprintf,strcpy etc.
// incase INCLUDE_WINDOWS not defined
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
// basic template library stuff
#include <vector>
#include <list>
#include <deque>
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <set>
#include <algorithm>
#include <complex> // new addition 2018_2_7
#include <typeinfo> // rtti
#include <string.h>
using namespace std;
#ifdef INCLUDE_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <cmath>

// engine
#include "m_type.h"
#include "m_asmmacros.h"
#include "m_tm.h"
#include "m_sysdebvars.h"
#include "m_state.h"
#include "m_utils.h"
#include "m_mem.h"
#include "m_err.h"
#include "m_dir.h"
#include "m_mt_rand.h"
#include "m_wh.h"
// lossless decompression (inflate)
#include "n_deflatecpp.h"
//#ifdef INCLUDE_WINDOWS
#include "m_win.h"
//#endif
#include "m_file.h"
#include "m_refcount.h"
#include "m_script.h"
#include "m_debp.h"
#include "m_device.h"
// math
#include "m_numath.h"
#include "m_matrix.h"
#include "m_matrixn.h"
#include "m_quat.h"
#include "m_floatconv.h"
// input
#include "m_keyboard.h"
#ifdef INCLUDE_WINDOWS
#include "m_input.h"
#endif
// 2d graphics
#include "m_stdcolors.h"
#include "m_bitmap8.h"
#include "m_bitmap16.h"
#include "m_bitmap24.h"
#include "m_bitmap32.h"
#include "m_txt32.h"
// read write 2d graphics
#include "m_gfx.h" // m_gfx.cpp will include m_gfx_tga.h, m_gfx_pcx.h and m_gfx_jpg.h
// 3d graphics
//#ifdef D2_3D // help out 'intellisense', 'tree2' undefined otherwise
#include "d_floattri.h"
#include "d2_texman.h"
#include "d2_model.h"
#include "d2_tree.h"
#include "d2_keyf.h"
#include "d2_viewport.h"
#include "d2_vid3d.h"	// device independent 3d support (software/hardware)
						// d2_vid3d.cpp will include d2_software.h and d2_dx9.h
#include "d2_light.h"
#include "d2_bwo.h"
#include "d2_bws.h"

#include "d2_sprite.h"
#include "d2_fontq.h"
#include "m_chunk.h"

//#endif
// video hardware
#include "m_vid.h" // m_vid.cpp will include m_vid_gdi.h m_vid_ddraw.h and m_vid_dx9.h
// audio
#include "m_sagger.h"
#include "m_midi.h"
#include "m_audio.h"
//hey
// lua
#ifdef INCLUDE_LUA
#ifndef C_IS_CPP
extern "C" {
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifndef C_IS_CPP
}
#endif
#endif
// lib theora
#include "m_movie.h"
