#ifndef __RW_OPTION_H__
#define __RW_OPTION_H__

#include <gtk/gtk.h>
#include "widget.h"

class COption
{	
public:
	gint showing;
	GtkWidget *window, *categories_tree, *notebook;
	GtkTreeModel *categories_tree_model;
	GtkWidget *misc_sound_ckbutton1,*misc_sound_ckbutton2,*misc_sound_ckbutton3;
	GtkWidget *font_table, *font_button1, *font_button2, *font_button3;

	COption ();
	void init ();
	void show ();
	void close ();
	~COption ();
};


#endif
