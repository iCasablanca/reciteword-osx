#ifndef __RW_SOUND_H__
#define __RW_SOUND_H__

#include "sndserv.h"

enum SOUND_EVENTS
{
	SND_NULL,
	SND_BUTTONDOWN,
	SND_BUTTONACTIVE,
	SND_BUTTONUP,
	SND_MENUSHOW,
	SND_ANSWEROK,
	SND_ANSWERNO,
	SND_ANSWERNO2,
	SND_CHORD,
	SND_DING,
	SND_GUNOK,
	SND_GUNNO,
	SND_TYPE,
	SND_BACK,
	SND_WIND,
	SND_LAST
};

class CSound
{
	bool canplay;

      public:
	gboolean disable;
	  CSound ();
	 ~CSound ();
	void play (SOUND_EVENTS event,PLAY_METHOD method);
};

void playsound (SOUND_EVENTS event,PLAY_METHOD method=PM_MIX);

#endif
