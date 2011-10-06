#ifndef __RW_HELP_H__
#define __RW_HELP_H__

#include <gtk/gtk.h>
#include "widget.h"

class CHelp
{	
public:
	gint showing;
	GtkWidget *window;
	tRadioButton overview_robutton,firstrecite_robutton,revise_robutton,shooting_robutton,hearing_robutton,typing_robutton,rest_robutton,
	             choosebook_robutton,filling_robutton,know_robutton,wordedit_robutton,bookedit_robutton,sentence_robutton;
	tPixButton exit_button;
	GtkWidget *fixed, *wordlist_vscroll, *text_area, *text_label;
	GtkObject *wordlist_adjust;

	CHelp ();
	void init ();
	void show ();
	void set_text(gchar *text);
	void close ();
	~CHelp ();

};


#endif
