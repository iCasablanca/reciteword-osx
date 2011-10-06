#ifndef __RW_SNDSERV_H__
#define __RW_SNDSERV_H__

#include <glib.h>

enum PLAY_METHOD {PM_MIX,PM_STOP_PRE,PM_AFTER_PRE};

class CSndserv
{
	int fd;

      public:
	gboolean disable;
	bool canplay;
	  CSndserv ();
	 ~CSndserv ();
	void play (const gchar *filename);
};

void play_file(const gchar *filename,PLAY_METHOD method);

#endif
