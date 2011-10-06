#ifndef __CHOOSEBOOK_H__
#define __CHOOSEBOOK_H__

#include <gtk/gtk.h>
#include "widget.h"

class CChoosebook
{
protected:
	GtkWidget *tree,*list;
	
public:

	//gboolean omit_first_signal;
	//gchar nowdir[256];
	tPixButton2 ok_button;
	tPixButton2 return_button;

	GtkWidget *book_index_label,*book_count_label,*word_count_label,*book_filename_label;
	GtkWidget *list_view,*tree_view;
	gint book_count;
	
	CChoosebook();
	~CChoosebook();
	
	void init();
	void show();
	void create_tree_model(gchar *dir,GtkTreeStore *model, GtkTreeIter *iter);
	GtkWidget *create_tree();
	void add_to_list(gchar *dir,GtkListStore *model);
	GtkWidget *create_list();
	void close();
	
};



#endif
