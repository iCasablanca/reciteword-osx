#ifndef __RW_RECORD_H__
#define __RW_RECORD_H__

#include <gtk/gtk.h>
#include "widget.h"

class CRecord
{	
public:
	gint showing;
	GtkWidget *window,*fixed,*text_label;
	tPixButton exit_button;

	CRecord ();
	void init ();
	void show ();
	void close ();
	~CRecord ();

};


#endif
