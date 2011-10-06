#ifndef __SKIN_H__
#define __SKIN_H__

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

struct _Skin_cursor
{
	GdkCursor *cursor;
	_Skin_cursor();
	~_Skin_cursor();
};

struct _Skin_widget_1
{
	int x[1], y[1];
};

struct _Skin_widget_2
{
	int x[1], y[1], w, h;
};

struct _Skin_pixbuf_1
{
	int x[2], y[2], w, h;
	GdkPixbuf *p[1];
	_Skin_pixbuf_1();
	~_Skin_pixbuf_1();
};

struct _Skin_pixmap_1
{
	int x[2], y[2], w, h;
	GdkPixmap *p[1];
	_Skin_pixmap_1();
	~_Skin_pixmap_1();
};

struct _Skin_pixmap_2
{
	int x[3], y[3], w, h;
	GdkPixmap *p[2];
	_Skin_pixmap_2();
	~_Skin_pixmap_2();
};

struct _Skin_pixmap_3
{
	int x[4], y[4], w, h;
	GdkPixmap *p[3];
	_Skin_pixmap_3();
	~_Skin_pixmap_3();
};

struct _Skin_pixmap_4
{
	int x[5], y[5], w, h;
	GdkPixmap *p[4];
	_Skin_pixmap_4();
	~_Skin_pixmap_4();
};


struct _Skin_pixmap_6
{
	int x[7], y[7], w, h;
	GdkPixmap *p[6];
	_Skin_pixmap_6();
	~_Skin_pixmap_6();
};

struct _Skin_pixmap_10
{
	int x[11], y[11], w, h;
	GdkPixmap *p[10];
	_Skin_pixmap_10();
	~_Skin_pixmap_10();
};

struct _Skin_pixmap_14
{
	int x[15], y[15], w, h;
	GdkPixmap *p[14];	//shooting 's pixmap
	_Skin_pixmap_14();
	~_Skin_pixmap_14();
};

struct _Skin_phonetic
{
	int x[1], y[1], h, w[43];
	GdkPixbuf *p[43];
	_Skin_phonetic();
	~_Skin_phonetic();
};

struct _Skin_reciteword
{
	int width,height;
	int titlebar_height;
	struct _Skin_cursor normal_cursor;
	struct _Skin_cursor disable_cursor;
	struct _Skin_cursor hand_cursor;
	struct _Skin_cursor gun_cursor;
	gchar *normal_english_font,*big_english_font;
	PangoFontDescription *n_font,*b_font;
	
	_Skin_reciteword();
	~_Skin_reciteword();
};

struct _Skin_face
{
	struct _Skin_pixmap_1 face, menu, menubar;
	struct _Skin_pixbuf_1 icon;
	struct _Skin_pixmap_2 menu_exit_button;
	struct _Skin_pixmap_3 help_button, record_button, option_button,
		dict_button, about_button;
	struct _Skin_pixmap_4 choosebook_button,firstrecite_button, revise_button,
		rest_button,know_button,shooting_button, typing_button;
};

struct _Skin_about
{
	struct _Skin_pixmap_1 about;
	struct _Skin_pixmap_2 exit_button;
};

struct _Skin_dict
{
	struct _Skin_pixmap_1 dict;
	struct _Skin_pixbuf_1 icon;
	struct _Skin_pixmap_3 exit_button, search_button, sound_button,
		card_button;
	struct _Skin_pixmap_4 search_ckbutton, sound_ckbutton;
	struct _Skin_widget_2 word_entry;
	struct _Skin_widget_2 word_vscroll;
	struct _Skin_widget_1 wordlist_dict_label[2];
	//struct _Skin_pixmap_4 ying_button,han_button;
};

struct _Skin_help
{
	struct _Skin_pixmap_1 help,text_area;
	struct _Skin_pixmap_2 exit_button;
	struct _Skin_pixmap_2 overview_robutton,firstrecite_robutton,revise_robutton,shooting_robutton,hearing_robutton,typing_robutton,rest_robutton,
	             choosebook_robutton,filling_robutton,know_robutton,wordedit_robutton,bookedit_robutton,sentence_robutton;
	struct _Skin_widget_2 wordlist_vscroll;
};

struct _Skin_record
{
	struct _Skin_pixmap_1 record;
	struct _Skin_pixmap_2 exit_button;
	struct _Skin_widget_2 text_label;
};

struct _Skin_choosebook
{
	struct _Skin_pixmap_1 choosebook;
		
	struct _Skin_pixbuf_1 dir;
	struct _Skin_widget_1 book_index_label,book_count_label,word_count_label,book_filename_label;
	struct _Skin_pixmap_3 ok_button,return_button;
	struct _Skin_widget_2 book_tree,book_list;
};

struct _Skin_group
{
	struct _Skin_pixmap_1 group;
	struct _Skin_pixmap_3 ok_button, cancel_button, add_button,
		reduce_button, class_button, return_button;
	struct _Skin_pixmap_4 order_ckbutton;
	struct _Skin_widget_1 book_label, zu_label, zu_wordcount_label,
		wordlist_scrollvalue_label,book_wordcount_label;
	struct _Skin_widget_2 wordlist_area, wordlist_label_1, wordlist_label_2,
		wordlist_label_3, wordlist_label_4, wordlist_label_5;
	struct _Skin_widget_2 zu_entry, wordlist_vscroll;
};

struct _Skin_skim
{
	struct _Skin_pixmap_1 skim;
	struct _Skin_widget_2 wordlist_vscroll, sentencelist_vscroll;
	struct _Skin_pixmap_3 start_button, return_button, group_button,
		test_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_pixmap_4 readword_ckbutton,phonetic_ckbutton,showmeanlater_ckbutton,upcase_ckbutton;
	struct _Skin_pixmap_3 readword_button;
	struct _Skin_widget_1 wordlist_scrollvalue_label,word_index_label,word_count_label;
	struct _Skin_widget_1 wordview_label_1, wordview_label_3;
	struct _Skin_widget_1 phonetic_label;
	struct _Skin_widget_2 wordlist_area;
	struct _Skin_pixmap_2 wordlist_item_1, wordlist_item_2,
		wordlist_item_3, wordlist_item_4, wordlist_item_5,
		wordlist_item_6;

	struct _Skin_pixmap_1 dialog;
	struct _Skin_pixmap_4 dialog_test_robutton, dialog_skim_robutton,
		dialog_return_robutton;
	struct _Skin_pixmap_2 dialog_ok_button;
};

struct _Skin_test
{
	struct _Skin_pixmap_1 test;

	struct _Skin_pixmap_3 start_button, return_button, group_button,
		skim_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_pixmap_4 meaning_ckbutton,readword_ckbutton,phonetic_ckbutton,length_ckbutton,headchar_ckbutton,sndeffect_ckbutton;
	struct _Skin_pixmap_3 readword_button;
	struct _Skin_widget_1 wordlist_scrollvalue_label,word_index_label,recited_word_count_label,word_count_label;
	struct _Skin_widget_1 wordtest_label_1, wordtest_label_2,
		wordtest_line, right_label, headchar_label;
	struct _Skin_widget_1 phonetic_label;
	struct _Skin_widget_2 wordtest_entry, wordlist_vscroll,
		sentencelist_vscroll;
	struct _Skin_widget_2 wordlist_area;
	struct _Skin_pixmap_3 wordlist_item_1, wordlist_item_2,
		wordlist_item_3, wordlist_item_4, wordlist_item_5,
		wordlist_item_6;
	struct _Skin_pixmap_1 wrongright_tip, pressanykey_tip;

	struct _Skin_pixmap_1 dialog1,dialog2;
	struct _Skin_pixmap_4 dialog_testagain_group_robutton,
		dialog_testover_robutton;
	struct _Skin_pixmap_2 dialog_ok_button;
};

struct _Skin_text
{
	struct _Skin_pixmap_1 entry_pen, yinbiao, entry_wrong, entry_right;
	struct _Skin_widget_2 entry_cursor, entry_underline;
	struct _Skin_widget_1 entry_text;
	struct _Skin_phonetic b_phonetic;
};

struct _Skin_revise_group
{
	struct _Skin_pixmap_1 revise_group;
	struct _Skin_pixmap_3 return_button, chooseword_button;
	struct _Skin_pixmap_4 order_ckbutton;
	struct _Skin_pixmap_1 plane_h[5][5];
	struct _Skin_pixmap_2 plane[5][5];
	struct _Skin_pixmap_2 plane_button_p;
	struct _Skin_widget_1 plane_button_text_1;
	struct _Skin_widget_1 plane_button_text_2;
	struct _Skin_widget_1 plane_button[5];
	struct _Skin_widget_1 total_groups_label;
};

struct _Skin_revise_test
{
	struct _Skin_pixmap_1 revise_test;
	struct _Skin_widget_1 wordtest_label_1,wordlist_scrollvalue_label,word_index_label,recited_word_count_label,word_count_label;
	struct _Skin_widget_1 phonetic_label;
	struct _Skin_widget_2 wordtest_entry;
	struct _Skin_pixmap_3 start_button, return_button, group_button,
		skim_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_pixmap_4 meaning_ckbutton,readword_ckbutton,phonetic_ckbutton,length_ckbutton,headchar_ckbutton,sndeffect_ckbutton;	
	struct _Skin_pixbuf_1 wordlist_item_normal, wordlist_item_right,
		wordlist_item_wrong, wordlist_item_ok;
	struct _Skin_widget_2 wordlist_area;
	struct _Skin_pixmap_2 wordlist_item_1, wordlist_item_2,
		wordlist_item_3;
	struct _Skin_widget_1 wordlist_item_label_1, wordlist_item_label_2;
	struct _Skin_widget_1 right_label,headchar_label;
	struct _Skin_widget_2 wordlist_vscroll;
	struct _Skin_pixmap_3 handinpaper_button,readword_button,chooseword_button;
	struct _Skin_widget_1 group_id_label, group_info_label;
	struct _Skin_widget_1 record_label[5];
	struct _Skin_pixmap_1 revise_times_pix[5];
	struct _Skin_pixmap_1 wordtest_tip, handinpaper_tip, firsttest_tip,
		secondtest_tip, pressanykey_tip, right_tip;

	struct _Skin_pixmap_1 dialog;
	struct _Skin_pixmap_4 dialog_skim_robutton, dialog_group_robutton, dialog_testover_robutton;
	struct _Skin_pixmap_2 dialog_ok_button;

};

struct _Skin_revise_skim
{
	struct _Skin_pixmap_1 revise_skim;
	struct _Skin_pixmap_3 start_button, return_button, group_button,
		test_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_widget_1 word_index_label,word_count_label;
	struct _Skin_pixmap_3 readword_button,chooseword_button;
	struct _Skin_pixmap_4 readword_ckbutton, meaning_ckbutton,animate_ckbutton;
	struct _Skin_widget_2 viewer;
};

struct _Skin_rest
{
	struct _Skin_pixmap_1 rest;
	struct _Skin_pixmap_3 start_button, return_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
};

struct _Skin_know
{
	struct _Skin_pixmap_1 know;
	struct _Skin_pixmap_3 return_button;
};

struct _Skin_shooting
{
	struct _Skin_pixmap_1 shooting;
	struct _Skin_pixmap_3 start_button, return_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_pixmap_1 wordlist_head[4];
	struct _Skin_pixmap_2 wordlist_item_1, wordlist_item_2,
		wordlist_item_3, wordlist_item_4;
	struct _Skin_pixmap_4 entocn_robutton, cntoen_robutton;
	struct _Skin_widget_1 word_label,meaning_label,recited_word_count_label,word_count_label;
	struct _Skin_pixmap_4 readword_ckbutton,phonetic_ckbutton,meaning_ckbutton,word_ckbutton,shotsnd_ckbutton;
	struct _Skin_pixmap_3 readword_button,chooseword_button;
	struct _Skin_pixmap_4 tank;
	struct _Skin_pixmap_14 shoot_pixmap[4];
	struct _Skin_pixmap_2 right_pixmap[4];
};

struct _Skin_typing
{
	struct _Skin_pixmap_1 typing;
	struct _Skin_pixmap_3 start_button, return_button, continue_button;
	struct _Skin_pixmap_4 stop_button, pause_button;
	struct _Skin_pixmap_4 readword_ckbutton,phonetic_ckbutton,meaning_ckbutton,sndeffect_ckbutton,fingering_ckbutton;
	struct _Skin_pixmap_3 readword_button,chooseword_button;
	struct _Skin_widget_2 wordlist_area, wordlist_item[6];
	struct _Skin_widget_2 wordlist_vscroll;
	struct _Skin_widget_2 wordtest_entry;
	struct _Skin_widget_1 wordtest_label_1,recited_word_count_label,word_count_label;
	struct _Skin_pixmap_1 pressanykey_tip, speed_info_pixmap, speed_info_bar;
	struct _Skin_pixmap_1 key_pixmap[55];
	struct _Skin_pixmap_6 speed_ld_pixmap, speed_rd_pixmap,
		speed_lm_pixmap, speed_rm_pixmap, speed_lu_pixmap,
		speed_ru_pixmap;
	struct _Skin_pixmap_10 number_pixmap;
};

struct _Skin_chooseword
{
	struct _Skin_pixmap_1 chooseword;
	struct _Skin_pixmap_2 ok_button, cancel_button;
	struct _Skin_pixmap_4 order_ckbutton, bygroup_robutton,
		byscope_robutton, byclass_robutton;
	struct _Skin_widget_2 group_entry, scope_entry, scopecount_entry,
		class_entry;
	struct _Skin_widget_1 group_s_label, scope_s_label, class_s_label;
	struct _Skin_pixmap_1 noclass_tip, scopeerr_tip;
};

struct _Skin_collectword
{
	struct _Skin_pixmap_1 collectword;
	struct _Skin_pixmap_2 exit_button;
	struct _Skin_widget_2 wordlist_label[9],wordlist_vscroll;
};

struct _Skin_mark
{
	struct _Skin_pixmap_1 mark;
	struct _Skin_pixmap_1 typing_pixmap;
	struct _Skin_pixmap_3 ok_button;
	struct _Skin_widget_1 revise_test_mark_label, revise_test_time_label, typing_mark_label;
};


struct _Skin
{
	gchar dir[256];
	struct _Skin_reciteword reciteword;
	struct _Skin_face face;
	struct _Skin_about about;
	struct _Skin_dict dict;
	struct _Skin_help help;
	struct _Skin_record record;
	struct _Skin_choosebook choosebook;
	struct _Skin_group group;
	struct _Skin_skim skim;
	struct _Skin_test test;
	struct _Skin_text text;
	struct _Skin_revise_group revise_group;
	struct _Skin_revise_test revise_test;
	struct _Skin_revise_skim revise_skim;
	struct _Skin_rest rest;
	struct _Skin_know know;
	struct _Skin_shooting shooting;
	struct _Skin_typing typing;
	struct _Skin_chooseword chooseword;
	struct _Skin_collectword collectword;
	struct _Skin_mark mark;
};

gint skin_load_info (gchar * skin_dir);
void skin_load_reciteword (GtkWidget * window);
void skin_load_face (GtkWidget * window);
void skin_load_about (GtkWidget * window);
void skin_load_dict (GtkWidget * window);
void skin_load_help (GtkWidget * window);
void skin_load_record (GtkWidget * window);
void skin_load_choosebook (GtkWidget * window);
void skin_load_group (GtkWidget * window);
void skin_load_skim (GtkWidget * window);
void skin_load_test (GtkWidget * window);
void skin_load_text (GtkWidget * window);
void skin_load_revise_group (GtkWidget * window);
void skin_load_revise_test (GtkWidget * window);
void skin_load_revise_skim (GtkWidget * window);
void skin_load_rest(GtkWidget * window);
void skin_load_know(GtkWidget * window);
void skin_load_shooting (GtkWidget * window);
void skin_load_typing (GtkWidget * window);
void skin_load_chooseword (GtkWidget * window);
void skin_load_collectword (GtkWidget * window);
void skin_load_mark (GtkWidget * window);


int SetBackPixmap (GtkWidget * widget, GdkPixmap * pixmap);
void SetFont (GtkWidget * widget, gboolean is_big);

#endif
