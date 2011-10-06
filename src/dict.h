#ifndef __DICT_H__
#define __DICT_H__

#include <gtk/gtk.h>
#include "mydict.h"
#include "widget.h"


class CDict
{
	
      public:
	gboolean onlyshow;
	gint showing;
	GtkWidget *window;
	tPixButton exit_button, search_button, sound_button, card_button;
	tCheckButton search_ckbutton, sound_ckbutton;
	//tRadioButton ying_button,han_button;
	GtkWidget *word_entry, *word_vscroll;
	GtkObject *word_adjust;
	GtkWidget *wordlist_dict_label[2];
	//GtkWidget *word_dict_clist;
	
	mydict *ecdict;

      public:
	const gchar *word_text;
	void set_wordlist_label (int index);

	  CDict ();
	void init ();
	void show ();
	void close ();
	void create(); // used when only show the dict.
	 ~CDict ();

};

#endif
