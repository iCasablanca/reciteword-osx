#include "sound.h"
#include "reciteword.h"
#include <string>
using namespace std;

extern CReciteWord* g_pReciteWord;

CSound::CSound ()
{
	gchar *filename;
	filename = g_build_filename(reciteword_data_dir, "modules", NULL);
	canplay = g_file_test(filename, G_FILE_TEST_EXISTS);
	g_free(filename);
	if (!canplay)
		g_print("The \"modules\" directory not found!\n");
}

void
CSound::play (SOUND_EVENTS event,PLAY_METHOD method)
{
	if (!canplay)
		return;
	gchar *filename;
	switch (event)
	{
		case SND_BUTTONDOWN:
			filename = "buttondown.wav";
			break;
		case SND_BUTTONACTIVE:
			filename = "buttonactive.wav";
			break;
		case SND_BUTTONUP:
			filename = "buttonup.wav";
			break;
		case SND_MENUSHOW:
			filename = "menushow.wav";
			break;
		case SND_ANSWEROK:
			filename = "answerok.wav";
			break;
		case SND_ANSWERNO:
			filename = "answerno.wav";
			break;
		case SND_ANSWERNO2:
			filename = "answerno2.wav";
			break;
		case SND_CHORD:
			filename = "chord.wav";
			break;
		case SND_DING:
			filename = "ding.wav";
			break;
		case SND_GUNOK:
			filename = "gunok.wav";
			break;
		case SND_GUNNO:
			filename = "gunno.wav";
			break;
		case SND_TYPE:
			filename = "type.wav";
			break;
		case SND_BACK:
			filename = "back.wav";
			break;
		case SND_WIND:
			filename = "wind.wav";
			break;
		default:
			filename = "";
			break;
	}
	filename = g_build_filename(reciteword_data_dir, "modules", filename, NULL);
	play_file(filename,method);
	g_free(filename);
}

CSound::~CSound ()
{
}


//static CSound soundevent;

void
playsound (SOUND_EVENTS event,PLAY_METHOD method)
{
	if (g_pReciteWord->soundevent.disable)
		return;
	if (event==SND_NULL)
		return;
	g_pReciteWord->soundevent.play (event,method);
}
