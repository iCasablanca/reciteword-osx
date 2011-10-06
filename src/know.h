#ifndef __KNOW_H__
#define __KNOW_H__

#include <gtk/gtk.h>
#include "widget.h"


class CKnow
{
      public:

	tPixButton2 return_button;
	
	  CKnow ();
	 ~CKnow ();

	void init ();
	void show ();
	void close ();
};

#endif
