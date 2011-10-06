#ifndef __SHOOTING_H__
#define __SHOOTING_H__

#include <gtk/gtk.h>
#include "widget.h"

struct tLabelItem
{
	GtkWidget *draw_area;
	GdkPixmap *draw_pixmap;
	gchar text[256];
	PangoLayout *layout;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_active;
	gint width, height;
	int mousein;
	int enable;
	int active;
	void (*runfunc1) (glong);
	void (*runfunc2) (glong);
	gint funcdata;

	  tLabelItem ();
	void init ();
	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		     GdkPixmap * pix_1, GdkPixmap * pix_2, gchar * str,
		     void (*func1) (glong), void (*func2) (glong), gint data);
	void set_label (gchar * str);
	void set_enable (gint a);
	void draw ();
	void destroy ();
	 ~tLabelItem ();
};

class CShooting
{
      public:
	  gint status;
	tPixButton2 start_button, pause_button;
	tRadioButton entocn_robutton;
	tRadioButton cntoen_robutton;
	tCheckButton readword_ckbutton,phonetic_ckbutton,meaning_ckbutton,word_ckbutton,shotsnd_ckbutton;
	tPixButton readword_button,chooseword_button;
	tPixButton2 return_button;

	tLabelItem wordlist_item[4];
	GtkWidget *tank_pixmap, *shoot_pixmap[4], *right_pixmap[4];
	tPixButton wordlist_head[4];
	GtkWidget *word_label,*meaning_label,*recited_word_count_label,*word_count_label;
	gint started;
	gint start_pause_button_status;

	gint method;
	gint *tested_wordlist_item;
	gint recited_word_count;
	gint testing_word_index;
	gint active_wordlist_item;

	gint shoot_pixmap_timer;
	gint timer_type;
	gint timer_count;

	  CShooting ();
	 ~CShooting ();

	void init ();
	void init_2 ();
	void show ();
	void show_2 ();
	void set_wordlist_label (gint index);
	void set_wordview_label (gint index);
	void close ();
	void do_up();
	void do_down();
	void do_pageup();
	void do_pagedown();
	void do_fire();
};

#endif
