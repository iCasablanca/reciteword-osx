#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "choosebook.h"
#include "reciteword.h"
#include "skin.h"
#include <glib/gi18n.h>

#include <stdlib.h>

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;

		  // BUG!!!!!
		  // when click first time,two signal will get,one for root item,one for the item which your clicked.
		  // this make the first chick will delay for a long time,as click root item will scan all the book files's info.
		  // i think this should be gtk's bug,but,it seems it willn't be fixed in gtk2.0.
		  // see http://bugzilla.gnome.org/show_bug.cgi?id=96063
		  // i try to use a variable to just omit the first signal;
		  // this fix leading a bug too,when the first time your just click the root item,it is also be omited :(
		  // then i try to connect to "button_press_event",use
		  // gtk_tree_view_get_path_at_pos to get the click row,but it can't tell
		  // whether it is click on the triangle which use to toggle the row.
		  // so,i just leave this bug along,it will only make the first time 's response is slow.
		  // now i make "myhardwords" to be the first item,this item will be search over soon.


static void on_choosebook_ok_clicked()
{
		GtkTreeSelection *selection;
	  GtkTreeModel *model;
	  GtkTreeIter iter;

		selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(g_pReciteWord->choosebook->list_view));
	  if (gtk_tree_selection_get_selected (selection, &model, &iter))
	  {
		  gchar *filename;
		       gtk_tree_model_get (model, &iter,
                          6, &filename,
                          -1);
		  g_pReciteWord->load_book(filename);
		  g_free(filename);
	  }

	g_pReciteWord->choosebook->close();
	g_pReciteWord->show();
}

static void on_choosebook_return_clicked()
{
	g_pReciteWord->choosebook->close();
	g_pReciteWord->show();
}

/*static gboolean
tree_selection_func(GtkTreeSelection *selection,
                                             GtkTreeModel *model,
                                             GtkTreePath *path,
                                             gboolean path_currently_selected,
                                             gpointer data)
{	
	  if (path_currently_selected)
	  {
		  CChoosebook *parent= (CChoosebook *)data;
		  GtkTreeIter iter;
		  gtk_tree_model_get_iter(model,&iter,path);
		  gchar *dir;
		       gtk_tree_model_get (model, &iter,
                          2, &dir,
                          -1);
		  GtkListStore *list_store=GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(parent->list_view)));
		  gtk_list_store_clear(list_store);
		  parent->book_count=0;
		  parent->add_to_list(dir,list_store);
		  gchar aa[5];
		  sprintf(aa,"%d",parent->book_count);
		  gtk_label_set_text(GTK_LABEL(parent->book_count_label),aa);
		  g_free(dir);
	  }
	  return true;
}*/


static void
on_choosebook_tree_selection_changed(GtkTreeSelection *selection,CChoosebook *parent)
{
	  GtkTreeModel *model;
	  GtkTreeIter iter;

		if (gtk_tree_selection_get_selected (selection, &model, &iter))
		{
			gchar *dir,*tmpstr;
			gchar aa[5];
		    
			gtk_tree_model_get (model, &iter,
                          2, &dir,
                          -1);
			
		  /*if ((parent->omit_first_signal)&&(strcmp(dir,"books")==0)) 
		  {
			  g_free(dir);
			  parent->omit_first_signal= false;
			  return;
		  }*/
			tmpstr = g_filename_to_utf8(dir,-1,NULL,NULL,NULL);
			if (tmpstr)
			{
				gtk_label_set_text(GTK_LABEL(parent->book_filename_label),tmpstr);
				g_free(tmpstr);
		  	}
		  	else
				gtk_label_set_text(GTK_LABEL(parent->book_filename_label),"");
			gtk_label_set_text(GTK_LABEL(parent->word_count_label),"");
		  
			GtkListStore *list_store=GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(parent->list_view)));
			gtk_list_store_clear(list_store);
			parent->book_count=0;
			parent->add_to_list(dir,list_store);
			sprintf(aa,"%d",parent->book_count);
			gtk_label_set_text(GTK_LABEL(parent->book_count_label),aa);
			g_free(dir);
		}
}


static void
on_choosebook_list_selection_changed(GtkTreeSelection *selection,CChoosebook *parent)
{
	  GtkTreeModel *model;
	  GtkTreeIter iter;
	
	  if (gtk_tree_selection_get_selected (selection, &model, &iter))
	  {
		  gchar *filename,*aa,*index,*count;
		  gint countint;
		  
		  gtk_tree_model_get (model, &iter,
                          6, &filename,
                          -1);
		  aa = g_filename_to_utf8(filename,-1,NULL,NULL,NULL);
		  if (aa)
		  {
			  gtk_label_set_text(GTK_LABEL(parent->book_filename_label),aa);
			  g_free(aa);
		  }
		  else
			  gtk_label_set_text(GTK_LABEL(parent->book_filename_label),"");
		  g_free(filename);
		  gtk_tree_model_get (model, &iter,
                          7, &index,
                          -1);
		  gtk_label_set_text(GTK_LABEL(parent->book_index_label),index);
		  g_free(index);
		  gtk_tree_model_get (model, &iter,
                          1, &countint,
                          -1);
		  count = g_strdup_printf("%d",countint);
		  gtk_label_set_text(GTK_LABEL(parent->word_count_label),count);
		  g_free(count);
	  }
}

/*
static gboolean
on_choosebook_tree_button_press(GtkWidget * widget, GdkEventButton * event,
			   CChoosebook * parent)
{
	if ((event->type==GDK_BUTTON_PRESS)&&(event->window==gtk_tree_view_get_bin_window(GTK_TREE_VIEW(parent->tree_view))))
	{
		GtkTreePath *path;
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(parent->tree_view),(gint) event->x,(gint)event->y,&path,NULL,NULL,NULL))
		{
			GtkTreeModel *model=gtk_tree_view_get_model(GTK_TREE_VIEW(parent->tree_view));
			GtkTreeIter iter;
			gtk_tree_model_get_iter(model,&iter,path);
			gtk_tree_path_free(path);
			gchar *dir;
			
			gtk_tree_model_get (model, &iter,
                          2, &dir,
                          -1);
			printf("%s\n",dir);
			if (strcmp(dir,parent->nowdir)==0)
			{
				g_free(dir);
				return false;
			}
			strcpy(parent->nowdir,dir);
		  GtkListStore *list_store=GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(parent->list_view)));
		  gtk_list_store_clear(list_store);
		  parent->book_count=0;
		  parent->add_to_list(dir,list_store);
		  gchar aa[5];
		  sprintf(aa,"%d",parent->book_count);
		  gtk_label_set_text(GTK_LABEL(parent->book_count_label),aa);
		  g_free(dir);
		}
		return false;
	}
	else
	{
		return false;
	}
}
*/

static gboolean
on_choosebook_list_button_press(GtkWidget * widget, GdkEventButton * event,
			   CChoosebook * parent)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		on_choosebook_ok_clicked();
		return true;
	}
	else
	{
		return false;
	}
}

static gboolean    select_dir_func    (GtkTreeModel *model,
                                             GtkTreePath *path,
                                             GtkTreeIter *iter,
                                             gpointer data)
{
			  gchar *dir;
		       gtk_tree_model_get (model, iter,
                          2, &dir,
                          -1);
	if (!strcmp((gchar *) data,dir))
	{
		GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(g_pReciteWord->choosebook->tree_view));
		gtk_tree_selection_select_iter(selection,iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(g_pReciteWord->choosebook->tree_view),path,NULL,TRUE,0.5,0);
	}
	g_free(dir);

	return false;
}

static gboolean    select_book_func    (GtkTreeModel *model,
                                             GtkTreePath *path,
                                             GtkTreeIter *iter,
                                             gpointer data)
{
				  gchar *filename;
		       gtk_tree_model_get (model, iter,
                          6, &filename,
                          -1);
	if (!strcmp((gchar *) data,filename))
	{
		gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(g_pReciteWord->choosebook->list_view)),iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(g_pReciteWord->choosebook->list_view),path,NULL,TRUE,0.5,0);
	}
	g_free(filename);

	return false;
}

CChoosebook::CChoosebook()
{
	skin_load_choosebook(g_pReciteWord->window);
}

CChoosebook::~CChoosebook()
{
}

void
CChoosebook::init()
{
	g_pReciteWord->status=STATUS_CHOOSEBOOK;
	//omit_first_signal = true;
	//nowdir[0]='\0';
}

void
CChoosebook::show()
{
	init();
	
	SetBackPixmap(g_pReciteWord->fixed,Skin->choosebook.choosebook.p[0]);
		
	book_index_label = gtk_label_new(NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), book_index_label,
		       Skin->choosebook.book_index_label.x[0], Skin->choosebook.book_index_label.y[0]);
	gtk_widget_show (book_index_label);
	book_count_label = gtk_label_new(NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), book_count_label,
		       Skin->choosebook.book_count_label.x[0], Skin->choosebook.book_count_label.y[0]);
	gtk_widget_show (book_count_label);
	word_count_label = gtk_label_new(NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->choosebook.word_count_label.x[0], Skin->choosebook.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	book_filename_label = gtk_label_new(NULL);
	gtk_widget_set_size_request(book_filename_label, Skin->reciteword.width - Skin->choosebook.book_filename_label.x[0], -1);
	gtk_label_set_line_wrap(GTK_LABEL(book_filename_label), true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), book_filename_label,
		       Skin->choosebook.book_filename_label.x[0], Skin->choosebook.book_filename_label.y[0]);
	gtk_widget_show (book_filename_label);

	list = create_list();
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), list, Skin->choosebook.book_list.x[0], Skin->choosebook.book_list.y[0]);
	gtk_widget_show(list);
	
	tree = create_tree();
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), tree, Skin->choosebook.book_tree.x[0], Skin->choosebook.book_tree.y[0]);
	gtk_widget_show(tree);

	if (g_pReciteWord->now_book && g_pReciteWord->now_book->bookfilename)
	{
		gchar *a,book_dir[256];
		strcpy(book_dir,g_pReciteWord->now_book->bookfilename);
		a=strrchr(book_dir,G_DIR_SEPARATOR);
		if (a)  // book_dir is not aways separate by G_DIR_SEPARATOR.
			*a='\0';
	
		GtkTreeModel *model;
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		gtk_tree_model_foreach  (model,
                                             select_dir_func,
                                             &book_dir);
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_view));
		gtk_tree_model_foreach  (model,
                                             select_book_func,
                                             g_pReciteWord->now_book->bookfilename);	
	}	
	
	return_button.create(g_pReciteWord->fixed,Skin->choosebook.return_button.x[0],Skin->choosebook.return_button.y[0],Skin->choosebook.return_button.p[0],Skin->choosebook.return_button.p[1],Skin->choosebook.return_button.p[2],(GdkPixmap*) NULL,on_choosebook_return_clicked);
	ok_button.create(g_pReciteWord->fixed,Skin->choosebook.ok_button.x[0],Skin->choosebook.ok_button.y[0],Skin->choosebook.ok_button.p[0],Skin->choosebook.ok_button.p[1],Skin->choosebook.ok_button.p[2],(GdkPixmap*) NULL,on_choosebook_ok_clicked);
	
}

void
CChoosebook::create_tree_model(gchar *dir,GtkTreeStore *model, GtkTreeIter *iter)
{
	GDir *dp;
	if (!(dp = g_dir_open(dir,0,NULL))) return;

	const gchar *item;
	gchar fullname[256], dirnamefile[256],*tmp1;

	while ((item = g_dir_read_name(dp)) != NULL)
	{
		strcpy(fullname, dir);
		if (fullname[strlen(fullname) - 1] != G_DIR_SEPARATOR) strcat(fullname,G_DIR_SEPARATOR_S);
		strcat(fullname, item);
		if (g_file_test(fullname,G_FILE_TEST_IS_DIR))
		{
			GtkTreeIter child_iter;
			gtk_tree_store_append (model, &child_iter,iter);
			sprintf(dirnamefile, "%s/dirname", fullname);
			if (g_file_test(dirnamefile, G_FILE_TEST_EXISTS)) {
				if (g_file_get_contents(dirnamefile, &tmp1, NULL, NULL))
					g_strstrip(tmp1);
				else
					tmp1 = NULL;
			} else {
				tmp1 = g_filename_to_utf8(item,-1,NULL,NULL,NULL);
			}
			gtk_tree_store_set (model, &child_iter,
				  0,Skin->choosebook.dir.p[0],1, tmp1,2,fullname,
				  -1);
			if (tmp1)
				g_free(tmp1);
			create_tree_model(fullname,model, &child_iter);
		}
	}
	g_dir_close(dp);
}

GtkWidget *
CChoosebook::create_tree()
{
	GtkWidget *sw;
	GtkTreeStore *model;
	GtkTreeIter iter;
	gchar *filename;
	
	model = gtk_tree_store_new (3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
	
	filename = g_build_filename(g_pReciteWord->userpath, "hardbooks", NULL);	
	gtk_tree_store_append (model, &iter, NULL);
	gtk_tree_store_set (model, &iter, 0,Skin->choosebook.dir.p[0],1, _("hardbooks"),2,filename, -1);
	create_tree_model(filename,model, &iter);
	g_free(filename);

	filename = g_build_filename(g_pReciteWord->userpath, "books", NULL);
	gtk_tree_store_append (model, &iter, NULL);
	gtk_tree_store_set (model, &iter, 0,Skin->choosebook.dir.p[0],1, _("my books"),2,filename, -1);
	create_tree_model(filename,model, &iter);
	g_free(filename);
	
	gtk_tree_store_append (model, &iter, NULL);
	filename = g_build_filename(reciteword_data_dir, "books", NULL);
	gtk_tree_store_set (model, &iter,
			  0,Skin->choosebook.dir.p[0],1, _("books"),2, filename,
			  -1);
	create_tree_model(filename,model, &iter);
	g_free(filename);

	
	tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree_view), TRUE);
	g_object_unref (model);
	
	
	/*GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	     column = gtk_tree_view_column_new ();

      cell_renderer = gtk_cell_renderer_pixbuf_new ();
      gtk_tree_view_column_pack_start (column,
				       cell_renderer,
				       FALSE);
      cell_renderer = gtk_cell_renderer_text_new ();
      gtk_tree_view_column_pack_start (column,
				       cell_renderer,
				       TRUE);
      gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
				   column);*/
				   
	GtkCellRenderer *renderer_image;
	GtkCellRenderer *renderer_text;
	GtkTreeViewColumn *column;
	renderer_image = gtk_cell_renderer_pixbuf_new();
	renderer_text = gtk_cell_renderer_text_new ();
	//g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,_("Directories"));
	gtk_tree_view_column_pack_start(column,renderer_image,FALSE);
	gtk_tree_view_column_add_attribute(column,
                                     renderer_image,
                                     "pixbuf",0);
	gtk_tree_view_column_pack_end(column,renderer_text,TRUE);
	gtk_tree_view_column_add_attribute(column,
                                     renderer_text,
                                     "text",1);
	gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view),column,-1);
/*	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree_view),
							    -1, "Folder",
							    renderer, "text",
							    0,
							    NULL);
	  column = gtk_tree_view_get_column (GTK_TREE_VIEW (tree_view), col_offset - 1);*/
	  //gtk_tree_view_column_set_visible(GTK_TREE_VIEW_COLUMN (column), FALSE);
	  gtk_tree_view_column_set_clickable (column, TRUE);
	      
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(tree_view));
	//gtk_tree_selection_set_select_function(selection,tree_selection_func,this,NULL);
	g_signal_connect (G_OBJECT (selection),
			"changed",
			G_CALLBACK (on_choosebook_tree_selection_changed),
			this);

	/*g_signal_connect (G_OBJECT (tree_view),
			"button_press_event",
			G_CALLBACK (on_choosebook_tree_button_press),
			this);*/
	
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request(sw,Skin->choosebook.book_tree.w,Skin->choosebook.book_tree.h);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);	
	gtk_container_add (GTK_CONTAINER (sw), tree_view);
	gtk_widget_show(tree_view);
	
	return sw;
}

void
CChoosebook::add_to_list(gchar *dir,GtkListStore *model)
{
	GDir *dp;
	if ((dp = g_dir_open(dir,0,NULL)) == NULL) return;
	const gchar *item;
	gchar fullname[256],index[5];
	GtkTreeIter iter;
	
	while ((item = g_dir_read_name(dp)) != NULL)
	{
		strcpy(fullname, dir);
		if (fullname[strlen(fullname) - 1] != G_DIR_SEPARATOR) strcat(fullname,G_DIR_SEPARATOR_S);
		strcat(fullname, item);		
		if (g_file_test(fullname,G_FILE_TEST_IS_DIR))
		{
			add_to_list(fullname,model);
		}
		else
		{
			int len = strlen(fullname);
			if (len > 4 && g_ascii_strcasecmp(fullname + len -4, ".bok") == 0) {
				gchar* strvalue[6];
				if (rw_book_get_file_info(fullname,&(strvalue[0]),&(strvalue[1]),&(strvalue[2]),&(strvalue[3]),&(strvalue[4]),&(strvalue[5])))
				{
					g_pReciteWord->choosebook->book_count++;
					sprintf(index,"%d",g_pReciteWord->choosebook->book_count);
					gtk_list_store_append (model, &iter);
					gtk_list_store_set (model, &iter,
						  0, strvalue[0],
							1,atoi(strvalue[1]),
							2,strvalue[2],
							3,strvalue[3],
							4,strvalue[4],
							5,strvalue[5],
							6,fullname,
							7,index, -1);
					//gtk_clist_append (GTK_CLIST (g_pReciteWord->choosebook->list), strvalue);
					if (strvalue[0]) g_free(strvalue[0]);
					if (strvalue[1]) g_free(strvalue[1]);
					if (strvalue[2]) g_free(strvalue[2]);
					if (strvalue[3]) g_free(strvalue[3]);
					if (strvalue[4]) g_free(strvalue[4]);
					if (strvalue[5]) g_free(strvalue[5]);
				}
			}
		}
	}
	g_dir_close(dp);
}

GtkWidget *
CChoosebook::create_list()
{
	GtkWidget *tree_view;		
	GtkWidget *sw;
	GtkListStore *model;

	model = gtk_list_store_new (8,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
	tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree_view), TRUE);
	list_view= tree_view;
	g_object_unref (model);
		  
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Book Name"), renderer, "text", 0, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 0);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);
  
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Count"), renderer, "text", 1, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 1);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Author"), renderer, "text", 2, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 2);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Website"), renderer, "text", 3, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 3);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Email"), renderer, "text", 4, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 4);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("About"), renderer, "text", 5, NULL);
	gtk_tree_view_column_set_sort_column_id (column, 5);
	gtk_tree_view_append_column (GTK_TREE_VIEW(tree_view), column);

	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (selection),
			"changed",
			G_CALLBACK (on_choosebook_list_selection_changed),
			this);
	g_signal_connect (G_OBJECT (tree_view),
			"button_press_event",
			G_CALLBACK (on_choosebook_list_button_press),
			this);
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request(sw,Skin->choosebook.book_list.w,Skin->choosebook.book_list.h);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);	
	gtk_container_add (GTK_CONTAINER (sw), tree_view);
	gtk_widget_show(tree_view);
	
	return sw;
}

void
CChoosebook::close()
{
	gtk_widget_destroy(book_index_label);
	gtk_widget_destroy(book_count_label);
	gtk_widget_destroy(word_count_label);
	gtk_widget_destroy(book_filename_label);
	gtk_widget_destroy(tree);
	gtk_widget_destroy(list);
	ok_button.destroy();
	return_button.destroy();
}
