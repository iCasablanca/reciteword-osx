#ifndef __ABOUT_H__
#define __ABOUT_H__

#include <gtk/gtk.h>
#include "widget.h"

struct tAbout
{
	gint showing;
	GtkWidget *window;
	tPixButton exit_button;

	  tAbout ();
	void init ();
	void show ();
	void close ();
	 ~tAbout ();
};

#endif
