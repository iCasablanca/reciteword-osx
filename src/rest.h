#ifndef __REST_H__
#define __REST_H__

#include <gtk/gtk.h>
#include "widget.h"


class CRest
{
      public:
	  gint started;

	tPixButton2 start_button;
	tPixButton2 pause_button;
	tPixButton2 return_button;

	  CRest ();
	 ~CRest ();

	void init ();
	void show ();
	void close ();
};

#endif
