#ifndef __RW_MARK_H__
#define __RW_MARK_H__

#include <gtk/gtk.h>
#include "widget.h"

class CMark
{
      public:

	GtkWidget * fixed;
	tPixButton2 ok_button;
	GtkWidget *mark_label,*info_label,*typing_pixmap;

	  CMark ();
	 ~CMark ();
	void init ();
	void show ();
	void close ();
};


#endif
