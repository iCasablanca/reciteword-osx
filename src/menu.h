#ifndef __MENU_H__
#define __MENU_H__

#include <gtk/gtk.h>
#include "widget.h"

class CMenu
{
      protected:
	tPixButton exit_button;

      public:
	GtkWidget *fixed;

	tPixButton2 choosebook_button;
	tPixButton2 firstrecite_button;
	tPixButton2 revise_button;
	tPixButton2 rest_button;
	tPixButton2 know_button;
	tPixButton2 shooting_button;
	tPixButton2 typing_button;
	gboolean re_continue;


	  CMenu ();
	 ~CMenu ();

	void show ();
	void close ();
	void make_pause();
	void make_continue();
};

#endif
