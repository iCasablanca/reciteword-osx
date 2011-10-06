#ifndef __REVISE_H__
#define __REVISE_H__

#include <gtk/gtk.h>
#include "widget.h"
#include "bookfile.h"

struct tTextButton
{
	GtkWidget *draw_area;
	GdkPixmap *draw_pixmap;
	gchar text[256];
	PangoLayout *layout;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_down;
	gint width, height, text_x1, text_y1, text_x2, text_y2;
	int mousein;
	gboolean enable;
	void (*runfunc) (gpointer);
	gpointer funcdata;

	  tTextButton ();
	void init ();
	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,gint tx1, gint ty1,gint tx2, gint ty2,
		     GdkPixmap * pix_1, GdkPixmap * pix_2, gchar * str,
		     void (*func) (gpointer), gpointer data);
	void draw (gboolean pressed);
	void set_enable(gboolean e);
	void destroy ();
	 ~tTextButton ();
};

class CRevise_group
{
	GtkWidget *plane_h[5];

      public:
	gint max_revise_times; //the max times of words will be revised

	  gint revise_zu_index[5];

	tPixButton2 return_button;
	tTextButton *plane_button[5];
	tPixButton *plane[5];
	tPixButton chooseword_button;
	tCheckButton order_ckbutton;
	GtkWidget *total_groups_label;
	
	  CRevise_group ();
	 ~CRevise_group ();

	void init ();
	void show ();
	void create_plane (glong x, glong y);
	void close ();
};

struct tViewer
{
	GtkWidget *text_area;
	GdkPixmap *draw_pixmap;
	GdkGC *gc;
	GdkColormap *cmap;
	GdkColor color;
	gchar *str1, *str2, **str_1, **str_2;
	PangoLayout *layout_w,*layout_m;
	gint now_count;
	gint show_meaning;
	gint count;
	gint w_appear_timer, m_show_timer, m_appear_timer, hide_timer,
		disappear_timer;
	gint w_show_time, m_show_time;
	void (*runfunc) ();

	  tViewer ();
	void init ();
	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		     gchar ** str_w, gchar ** str_m, gint _count,
		     void (*func) ());
	void set_words(gchar ** str_w, gchar ** str_m, gint _count);
	void set_text(gchar * str_w, gchar * str_m);
	void start_skim ();
	void stop_skim ();
	void pause_skim ();
	void continue_skim ();
	void draw (gint status);
	void set_show_meaning (int a);
	void refresh ();
	void destroy ();
	 ~tViewer ();
};

class CRevise_skim
{
public:
	gint started;

	tViewer viewer;
	GtkWidget *word_index_label,*word_count_label;
	tCheckButton readword_ckbutton,meaning_ckbutton,animate_ckbutton;
	tPixButton readword_button,chooseword_button;
	tPixButton2 start_button;
	tPixButton2 pause_button;
	tPixButton2 group_button;
	tPixButton2 test_button;
	tPixButton2 return_button;
	gint start_pause_button_status;

	  CRevise_skim ();
	 ~CRevise_skim ();

	void init ();
	void show ();
	void close ();
};

#endif
