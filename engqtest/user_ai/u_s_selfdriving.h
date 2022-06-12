void selfdrivinginit(), selfdrivingproc(), selfdrivingdraw2d(), selfdrivingexit();
//#define DONAMESPACE // selfdriving

#ifdef DONAMESPACE
namespace selfdriving {
#endif

extern pointf2 carCamera; // global
extern pointf2 visualizerCamera; // global

#ifdef DONAMESPACE
} // end namespace selfdriving
using namespace selfdriving;
#endif
