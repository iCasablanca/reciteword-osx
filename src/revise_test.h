#ifndef __REVISE_TEST_H__
#define __REVISE_TEST_H__

#include <gtk/gtk.h>
#include "widget.h"

enum TestStatus {S_not_tested,S_right,S_wrong,S_right_1,S_right_2};

struct tListItem3
{
	GtkWidget *fixed, *label1, *label2, *pixmap;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_selected;
	int mousein;
	int can_select;
	int selected;
	GSList *group;
	void (*runfunc) (int);
	int func_data;

	  tListItem3 ();
	void init ();
	void create (GSList * group, GtkWidget * pfixed, gint x, gint y,
		     gint w, gint h, GdkPixmap * pix_1, GdkPixmap * pix_2,
		     gint label_x1, gint label_y1, gint label_x2,
		     gint label_y2, void (*func) (int), int data);
	void clean_selected ();
	void set_can_select (int a);
	void set_selected (int a);
	void set_group (GSList * group);
	void set_label (gint a, gchar * str);
	void set_pixmap (TestStatus a);
	void destroy ();
	 ~tListItem3 ();
};

class CRevise_test_Dialog
{
      public:

	GtkWidget *fixed;
	
	tRadioButton skim_robutton;
	tRadioButton group_robutton;
	tRadioButton testover_robutton;
	tPixButton ok_button;



	  CRevise_test_Dialog ();
	 ~CRevise_test_Dialog ();
	void show ();
	void close ();
	void do_up();
	void do_down();
};

enum REVISE_TEST_STATUS {RTS_ANSWERING,RTS_CHECKING,RTS_TESTING};

class CRevise_test
{
      public:
	gboolean readword_when_answerright;
	  CRevise_test_Dialog dialog;
	REVISE_TEST_STATUS status;
	gint pressanykey_status;
	gint test_status;
	gint timecount;
	gint timecount_timer;
	gint tip_timer;
	gint checknext_timer;
	GtkWidget *wordtest_label_1;
	CPhoneticLabel phonetic_label;
	GtkWidget *wordlist_vscroll;
	GtkObject *wordlist_adjust;
	GtkWidget *wordtest_tip, *handinpaper_tip, *firsttest_tip,
		*secondtest_tip, *pressanykey_tip, *right_tip, *revise_times_pix;
	GtkWidget *right_label, *headchar_label;
	int headchar_len;
	GtkWidget *group_id_label, *group_info_label;
	GtkWidget *record_label[5];
	GtkWidget *wordlist_scrollvalue_label,*word_index_label,*recited_word_count_label,*word_count_label;
	tListItem3 wordlist_item[3];
	tPixButton handinpaper_button,readword_button,chooseword_button;
	tCheckButton meaning_ckbutton,readword_ckbutton,phonetic_ckbutton,length_ckbutton,headchar_ckbutton,sndeffect_ckbutton;
	tPixButton2 start_button;
	tPixButton2 pause_button;
	tPixButton2 group_button;
	tPixButton2 skim_button;
	tPixButton2 return_button;
	tEntry wordtest_entry;
	int selected_wordlist_item;
	gchar **tested_wordlist_item_str;
	gint recited_word_count;
	TestStatus *tested_wordlist_item_status;
	gint started;
	gint start_pause_button_status;
	gboolean save_record;

	  CRevise_test ();
	 ~CRevise_test ();

	void init ();
	void show ();
	void refresh_saverecord ();
	void set_wordtest_label (gint i);
	void set_wordlist_item (gint index);
	void save_hard_words();
	void save_setting();
	void do_scroll(GdkEventScroll * event);
	void close ();
};

#endif
