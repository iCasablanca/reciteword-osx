#ifndef __FIRSTRECITE_H__
#define __FIRSTRECITE_H__

#include <gtk/gtk.h>
#include "widget.h"
#include "bookfile.h"

class CFirstRecite_group
{
	GtkWidget *book_label, *book_wordcount_label, *wordlist_vscroll, *wordlist_scrollvalue_label;
	GtkWidget *wordlist_label_1_w, *wordlist_label_2_w,
		*wordlist_label_3_w, *wordlist_label_4_w, *wordlist_label_5_w;
	GtkWidget *wordlist_label_1_m, *wordlist_label_2_m,
		*wordlist_label_3_m, *wordlist_label_4_m, *wordlist_label_5_m;



      public:
	tPixButton class_button;
	tPixButton add_button, reduce_button;
	tPixButton2 return_button;
	tPixButton ok_button,cancel_button;
	tCheckButton order_ckbutton;

	  GtkWidget * zu_label, *zu_entry, *zu_wordcount_label;
	Rw_Word zu_words;
	gint zu_wordcount;
	gint zu_index;
	gint total_wordcount;

	GtkObject *wordlist_adjust;

	  CFirstRecite_group ();
	 ~CFirstRecite_group ();

	void init ();
	void show ();
	void close ();
	void set_wordlist_label (int index);
	void do_up();
	void do_down();
	void do_pageup();
	void do_pagedown();
	void do_scroll(GdkEventScroll * event);
};

class CSkimDialog
{
      public:
	
	GtkWidget *fixed;
	
	tRadioButton test_robutton;
	tRadioButton skim_robutton;
	tRadioButton return_robutton;
	tPixButton ok_button;


	  CSkimDialog ();
	 ~CSkimDialog ();
	void show ();
	void close ();
	void do_up();
	void do_down();
};

struct tListItem
{
	GtkWidget *fixed, *label;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_selected;
	int mousein;
	int selected;
	GSList *group;
	void (*runfunc) (int);
	int func_data;

	  tListItem ();
	void init ();
	void create (GSList * group, GtkWidget * pfixed, gint x, gint y,
		     gint w, gint h, GdkPixmap * pix_1, GdkPixmap * pix_2,
		     void (*func) (int), int data);
	void clean_selected ();
	void set_selected (int a);
	void set_group (GSList * group);
	void set_label (gchar * str);
	void destroy ();
	 ~tListItem ();
};

class CFirstRecite_skim
{
	GtkWidget *wordlist_vscroll, *sentencelist_vscroll;
	GtkWidget *wordview_label_1, *wordview_label_3,*wordlist_scrollvalue_label,*word_index_label,*word_count_label;

      public:
	tPixButton readword_button;
	CPhoneticLabel phonetic_label;
	tCheckButton readword_ckbutton,phonetic_ckbutton,showmeanlater_ckbutton,upcase_ckbutton;
	tPixButton2 start_button;
	tPixButton2 pause_button;
	tPixButton2 group_button;
	tPixButton2 test_button;
	tPixButton2 return_button;
	GtkObject *wordlist_adjust;
	tListItem wordlist_item[6];
	int selected_wordlist_item;
	CSkimDialog dialog;
	gint timeout;
	gint left_time;
	gint wait_time;
	gboolean started;

	  CFirstRecite_skim ();
	 ~CFirstRecite_skim ();

	void init ();
	void show ();
	void set_wordview_label (gint i);
	void set_wordlist_item (int index);
	void do_up();
	void do_down();
	void do_left();
	void do_right();
	void do_pageup();
	void do_pagedown();
	void do_home();
	void do_end();
	void do_scroll(GdkEventScroll * event);
	void close ();

};

#endif
