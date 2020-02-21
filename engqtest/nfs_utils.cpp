#include <m_eng.h>

#define USENAMESPACE
#ifdef USENAMESPACE
namespace nfs_utils {
#endif

S32 hiliteff(S32 frm)
{
	if (frm & 8) {
		return (15-frm)&7;
	} else {
		return frm&7;
	}
}
#ifdef USENAMESPACE
} // end namespace nfsshell
using namespace nfs_utils;
#endif

