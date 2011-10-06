#ifndef __RW_COLLECTWORD_H__
#define __RW_COLLECTWORD_H__

#include <gtk/gtk.h>
#include "widget.h"

class CCollectword
{
	public:
	
	GtkWidget *window;
	tPixButton exit_button;
	gchar **w,**m;
	gint wordcount;
	GtkWidget *wordlist_label_w[9],*wordlist_label_m[9];
	GtkWidget *wordlist_vscroll;
	GtkObject *wordlist_adjust;

	void init ();
	void show ();
	void set_wordlist_label(gint index);
	void close ();

	CCollectword ();
	~CCollectword ();
};

#endif
