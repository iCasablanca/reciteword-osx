#ifndef __FIRSTRECITE_TEST_H__
#define __FIRSTRECITE_TEST_H__

#include <gtk/gtk.h>
#include "widget.h"

class CTestDialog
{
      public:

	GtkWidget *fixed;
	
	tRadioButton testagain_group_robutton;
	tRadioButton testover_robutton;
	tPixButton ok_button;


	CTestDialog ();
	~CTestDialog ();
	void show ();
	void close ();
	void do_up();
	void do_down();
};

struct tListItem2
{
	GtkWidget *fixed, *label;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_finished;
	GdkPixmap *pix_selected;
	int selected;
	GSList *group;

	  tListItem2 ();
	void init ();
	void create (GSList * group, GtkWidget * pfixed, gint x, gint y,
		     gint w, gint h, GdkPixmap * pix_1, GdkPixmap * pix_2,
		     GdkPixmap * pix_3);
	void clean_selected ();
	void set_selected (int a);
	void set_group (GSList * group);
	void set_label (gchar * str);
	void destroy ();
	 ~tListItem2 ();
};

enum FIRSTRECITE_TEST_STATUS {FRTS_FIRSTTIME,FRTS_FIRSTTIMEOVER,FRTS_SECONDTIME};

class CFirstRecite_test
{
	GtkWidget *wordlist_vscroll, *sentencelist_vscroll;
	GtkWidget *wordlist_scrollvalue_label,*word_index_label,*word_count_label;

      public:
	tPixButton readword_button;
	CPhoneticLabel phonetic_label;
	tCheckButton meaning_ckbutton,readword_ckbutton,phonetic_ckbutton,length_ckbutton,headchar_ckbutton,sndeffect_ckbutton;
	tPixButton2 group_button;
	tPixButton2 skim_button;
	tPixButton2 return_button;
	tPixButton2 start_button;
	tPixButton2 pause_button;
	tEntry wordtest_entry;
	GtkObject *wordlist_adjust;
	tListItem2 wordlist_item[5];
	GtkWidget *pressanykey_tip;
	GtkWidget *wordtest_label_1,*wrongright_tip, *right_label, *headchar_label,*recited_word_count_label;
	int headchar_len;
	int selected_wordlist_item;
	int *tested_wordlist_item;
	int recited_word_count;
	gint started;
	FIRSTRECITE_TEST_STATUS status;
	gint pressanykey_status;
	CTestDialog dialog;

	  CFirstRecite_test ();
	 ~CFirstRecite_test ();

	void init ();
	void show ();
	void set_wordtest_label (gint i);
	void set_wordlist_item (int index);
	void save_setting();
	void do_scroll(GdkEventScroll * event);
	void do_up();
	void do_down();
	void do_pageup();
	void do_pagedown();
	void close ();

};

#endif
