#ifndef __TYPING_H__
#define __TYPING_H__

#include <gtk/gtk.h>
#include "widget.h"

struct tLabel
{
	GtkWidget *draw_area;
	GdkPixmap *draw_pixmap;
	GdkGC *gc;
	gchar text_w[256], text_m[256];
	PangoLayout *layout_w,*layout_m;
	gint width, height;
	gint show_line;

	  tLabel ();
	void init ();
	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h);
	void set_label (gchar * str_w, gchar * str_m);
	void set_showline (gint a);
	void draw ();
	void destroy ();
	 ~tLabel ();
};

class CTyping
{
	GtkWidget *wordlist_vscroll;

	gint key_pixmap_o_a, key_pixmap_o_b;
	gint key_pixmap_s_a, key_pixmap_s_b;


      public:

	  gint status;
	tPixButton2 return_button;
	tPixButton2 start_button, pause_button;
	tCheckButton readword_ckbutton,phonetic_ckbutton,meaning_ckbutton,sndeffect_ckbutton,fingering_ckbutton;
	tPixButton readword_button,chooseword_button;
	
	tEntry wordtest_entry;
	gint pressanykey_status;
	GtkWidget *pressanykey_tip, *speed_info_pixmap, *speed_info_bar, *speed_info_bar_event_box, *key_pixmap_a, *key_pixmap_b;
	GtkWidget *speed_ld_pixmap, *speed_rd_pixmap, *speed_lm_pixmap,
		*speed_rm_pixmap, *speed_lu_pixmap, *speed_ru_pixmap;
	GtkWidget *speed_number_a, *speed_number_b, *speed_number_c;
	gint speed_timer;
	GtkWidget *wordtest_label_1,*recited_word_count_label,*word_count_label;
	tLabel wordlist_item[6];
	GtkObject *wordlist_adjust;
	gint selected_wordlist_item;
	gint started;
	gint start_pause_button_status;

	gint speed_time, speed_key, speed_value, speed_tmpvalue;	//speed_tmpvalue 's the last 6 second's speed. 
	gint speed_oldkey[30], speed_turn;

	  CTyping ();
	 ~CTyping ();

	void init ();
	void init_2 ();
	void show ();
	void show_2 ();
	void set_wordlist_item (gint index);
	void set_wordtest_label (gint index);
	void set_key_pixmap (gchar ch);
	void set_speed_number (gint speed);
	void set_speed_pixmap (gint speed);
	void do_scroll(GdkEventScroll * event);
	void close ();
};


#endif
