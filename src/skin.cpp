#include "skin.h"
#include "configfile.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;
extern gchar reciteword_data_dir[256];

_Skin_cursor::_Skin_cursor()
{
	cursor=NULL;
}

_Skin_cursor::~_Skin_cursor()
{
	if (cursor)
		gdk_cursor_unref(cursor);
}

_Skin_pixbuf_1::_Skin_pixbuf_1()
{
	for (int i=0;i<1;i++)
		p[i]=NULL;
}

_Skin_pixbuf_1::~_Skin_pixbuf_1()
{
	for (int i=0;i<1;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_1::_Skin_pixmap_1()
{
	for (int i=0;i<1;i++)
		p[i]=NULL;
}

_Skin_pixmap_1::~_Skin_pixmap_1()
{
	for (int i=0;i<1;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_2::_Skin_pixmap_2()
{
	for (int i=0;i<2;i++)
		p[i]=NULL;
}

_Skin_pixmap_2::~_Skin_pixmap_2()
{
	for (int i=0;i<2;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_3::_Skin_pixmap_3()
{
	for (int i=0;i<3;i++)
		p[i]=NULL;
}

_Skin_pixmap_3::~_Skin_pixmap_3()
{
	for (int i=0;i<3;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_4::_Skin_pixmap_4()
{
	for (int i=0;i<4;i++)
		p[i]=NULL;
}

_Skin_pixmap_4::~_Skin_pixmap_4()
{
	for (int i=0;i<4;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_6::_Skin_pixmap_6()
{
	for (int i=0;i<6;i++)
		p[i]=NULL;
}

_Skin_pixmap_6::~_Skin_pixmap_6()
{
	for (int i=0;i<6;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_10::_Skin_pixmap_10()
{
	for (int i=0;i<10;i++)
		p[i]=NULL;
}

_Skin_pixmap_10::~_Skin_pixmap_10()
{
	for (int i=0;i<10;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_pixmap_14::_Skin_pixmap_14()
{
	for (int i=0;i<14;i++)
		p[i]=NULL;
}

_Skin_pixmap_14::~_Skin_pixmap_14()
{
	for (int i=0;i<14;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_phonetic::_Skin_phonetic()
{
	for (int i=0;i<43;i++)
		p[i]=NULL;
}

_Skin_phonetic::~_Skin_phonetic()
{
	for (int i=0;i<43;i++)
	{
		if (p[i])
			g_object_unref(p[i]);
	}
}

_Skin_reciteword::_Skin_reciteword()
{
	n_font=NULL;
	b_font=NULL;
	normal_english_font=NULL;
	big_english_font=NULL;
}

_Skin_reciteword::~_Skin_reciteword()
{
	if (n_font)
	{
		pango_font_description_free(n_font);
	}
	if (b_font)
	{
		pango_font_description_free(b_font);
	}
	if (normal_english_font)
		g_free(normal_english_font);
	if (big_english_font)
		g_free(big_english_font);
}

static void
skin_read_info (ConfigFile * cfgfile, char *section, char *key,
			  int deep, int x[],int y[], int *w, int *h)
{
	gchar *ab, *a, *b;
	if (rw_cfg_read_string (cfgfile, section, key, &ab))
	{
		a = ab;
		b = strchr (a, ',');
		x[0] = atoi (a);
		a = b + 1;
		b = strchr (a, ',');
		y[0] = atoi (a);
		if (w && h)
		{
			a = b + 1;
			b = strchr (a, ',');
			*w = atoi (a);
			a = b + 1;
			b = strchr (a, ',');
			*h = atoi (a);
		}
		gint i;
		for (i = 1; i < deep; i++)
		{
			a = b + 1;
			b = strchr (a, ',');
			x[i] = atoi (a);
			a = b + 1;
			b = strchr (a, ',');
			y[i] = atoi (a);
		}
		g_free (ab);
	}
}

static void
skin_read_info_widget_1 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_widget_1 *button)
{
	skin_read_info(cfgfile,section,key,1,button->x,button->y,NULL,NULL);
}

static void
skin_read_info_widget_2 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_widget_2 *button)
{
	skin_read_info(cfgfile,section,key,1,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixbuf_1 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixbuf_1 *button)
{
	skin_read_info(cfgfile,section,key,2,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_1 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixmap_1 *button)
{
	skin_read_info(cfgfile,section,key,2,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_2 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixmap_2 *button)
{
	skin_read_info(cfgfile,section,key,3,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_3 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixmap_3 *button)
{
	skin_read_info(cfgfile,section,key,4,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_4 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixmap_4 *button)
{
	skin_read_info(cfgfile,section,key,5,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_6 (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_pixmap_6 *button)
{
	skin_read_info(cfgfile,section,key,7,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_10 (ConfigFile * cfgfile, char *section, char *key,
			  struct _Skin_pixmap_10 *button)
{
	skin_read_info(cfgfile,section,key,11,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_pixmap_14 (ConfigFile * cfgfile, char *section, char *key,
			  struct _Skin_pixmap_14 *button)
{
	skin_read_info(cfgfile,section,key,15,button->x,button->y,&(button->w),&(button->h));
}

static void
skin_read_info_phonetic (ConfigFile * cfgfile, char *section, char *key,
			 struct _Skin_phonetic *button)
{
	gchar *ab, *a, *b;
	if (rw_cfg_read_string (cfgfile, section, key, &ab))
	{
		a = ab;
		b = strchr (ab, ',');
		button->x[0] = atoi (a);
		a = b + 1;
		b = strchr (a, ',');
		button->y[0] = atoi (a);
		a = b + 1;
		b = strchr (a, ',');
		button->h = atoi (a);
		gint i;
		for (i = 0; i < 43; i++)
		{
			a = b + 1;
			b = strchr (a, ',');
			button->w[i] = atoi (a);
		}
		g_free (ab);
	}
}

gint
skin_load_info (gchar * skin_dir)
{
	ConfigFile *cfgfile;

	gchar *skin_file;
	skin_file = g_build_filename(reciteword_data_dir, "skins", skin_dir, "skin.inf", NULL);
	cfgfile = rw_cfg_open_file (skin_file);
	g_free(skin_file);
	if (!cfgfile)
	{
#ifdef G_OS_WIN32
#else
		g_print ("load skin \"%s\" error!\n", skin_dir);
#endif
		return TRUE;
	}
	gint i, j;
	gchar tmpstr[256];

// reciteword
	rw_cfg_read_int (cfgfile, "reciteword", "width",
			 &(Skin->reciteword.width));
	rw_cfg_read_int (cfgfile, "reciteword", "height",
			 &(Skin->reciteword.height));
	rw_cfg_read_int (cfgfile, "reciteword", "titlebar_height",
			 &(Skin->reciteword.titlebar_height));
	rw_cfg_read_string (cfgfile, "reciteword", "normal_english_font", &(Skin->reciteword.normal_english_font));
	rw_cfg_read_string (cfgfile, "reciteword", "big_english_font", &(Skin->reciteword.big_english_font));

// face
	skin_read_info_pixmap_1 (cfgfile, "face", "face", &(Skin->face.face));
	skin_read_info_pixbuf_1 (cfgfile, "face", "icon", &(Skin->face.icon));
	skin_read_info_pixmap_1 (cfgfile, "face", "menu", &(Skin->face.menu));
	skin_read_info_pixmap_1 (cfgfile, "face", "menubar",
				 &(Skin->face.menubar));
	skin_read_info_pixmap_2 (cfgfile, "face", "menu_exit_button",
				 &(Skin->face.menu_exit_button));
	skin_read_info_pixmap_3 (cfgfile, "face", "help_button",
				 &(Skin->face.help_button));
	skin_read_info_pixmap_3 (cfgfile, "face", "record_button",
				 &(Skin->face.record_button));
	skin_read_info_pixmap_3 (cfgfile, "face", "option_button",
				 &(Skin->face.option_button));
	skin_read_info_pixmap_3 (cfgfile, "face", "dict_button",
				 &(Skin->face.dict_button));
	skin_read_info_pixmap_3 (cfgfile, "face", "about_button",
				 &(Skin->face.about_button));
 	skin_read_info_pixmap_4 (cfgfile, "face", "choosebook_button",
				 &(Skin->face.choosebook_button));
	skin_read_info_pixmap_4 (cfgfile, "face", "firstrecite_button",
				 &(Skin->face.firstrecite_button));
	skin_read_info_pixmap_4 (cfgfile, "face", "revise_button",
				 &(Skin->face.revise_button));
 	skin_read_info_pixmap_4 (cfgfile, "face", "rest_button",
				 &(Skin->face.rest_button));
	skin_read_info_pixmap_4 (cfgfile, "face", "know_button",
				 &(Skin->face.know_button));
	skin_read_info_pixmap_4 (cfgfile, "face", "shooting_button",
				 &(Skin->face.shooting_button));
	skin_read_info_pixmap_4 (cfgfile, "face", "typing_button",
				 &(Skin->face.typing_button));

// choosebook
	
	skin_read_info_pixmap_1(cfgfile,"choosebook","choosebook", &(Skin->choosebook.choosebook));
	
	skin_read_info_pixbuf_1(cfgfile,"choosebook","dir", &(Skin->choosebook.dir));
	skin_read_info_pixmap_3(cfgfile,"choosebook","ok_button", &(Skin->choosebook.ok_button));
	skin_read_info_pixmap_3(cfgfile,"choosebook","return_button", &(Skin->choosebook.return_button));
	skin_read_info_widget_1(cfgfile,"choosebook","book_index_label", &(Skin->choosebook.book_index_label));
	skin_read_info_widget_1(cfgfile,"choosebook","book_count_label", &(Skin->choosebook.book_count_label));
	skin_read_info_widget_1(cfgfile,"choosebook","word_count_label", &(Skin->choosebook.word_count_label));
	skin_read_info_widget_1(cfgfile,"choosebook","book_filename_label", &(Skin->choosebook.book_filename_label));
	skin_read_info_widget_2(cfgfile,"choosebook","book_tree", &(Skin->choosebook.book_tree));
	skin_read_info_widget_2(cfgfile,"choosebook","book_list", &(Skin->choosebook.book_list));

// group

	skin_read_info_pixmap_1 (cfgfile, "group", "group",
				 &(Skin->group.group));
	skin_read_info_widget_1 (cfgfile, "group", "book_label",
				 &(Skin->group.book_label));
	skin_read_info_widget_1 (cfgfile, "group", "zu_label",
				 &(Skin->group.zu_label));
	skin_read_info_widget_1 (cfgfile, "group", "zu_wordcount_label",
				 &(Skin->group.zu_wordcount_label));
	skin_read_info_widget_1 (cfgfile, "group", "wordlist_scrollvalue_label",
				 &(Skin->group.wordlist_scrollvalue_label));
	skin_read_info_widget_1 (cfgfile, "group", "book_wordcount_label",
				 &(Skin->group.book_wordcount_label));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_area",
				 &(Skin->group.wordlist_area));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_label_1",
				 &(Skin->group.wordlist_label_1));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_label_2",
				 &(Skin->group.wordlist_label_2));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_label_3",
				 &(Skin->group.wordlist_label_3));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_label_4",
				 &(Skin->group.wordlist_label_4));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_label_5",
				 &(Skin->group.wordlist_label_5));
	skin_read_info_widget_2 (cfgfile, "group", "zu_entry",
				 &(Skin->group.zu_entry));
	skin_read_info_widget_2 (cfgfile, "group", "wordlist_vscroll",
				 &(Skin->group.wordlist_vscroll));
	skin_read_info_pixmap_3 (cfgfile, "group", "ok_button",
				 &(Skin->group.ok_button));
	skin_read_info_pixmap_3 (cfgfile, "group", "cancel_button",
				 &(Skin->group.cancel_button));
	skin_read_info_pixmap_3 (cfgfile, "group", "add_button",
				 &(Skin->group.add_button));
	skin_read_info_pixmap_3 (cfgfile, "group", "reduce_button",
				 &(Skin->group.reduce_button));
	skin_read_info_pixmap_3 (cfgfile, "group", "class_button",
				 &(Skin->group.class_button));
	skin_read_info_pixmap_3 (cfgfile, "group", "return_button",
				 &(Skin->group.return_button));
	skin_read_info_pixmap_4 (cfgfile, "group", "order_ckbutton",
				 &(Skin->group.order_ckbutton));

// skim
	skin_read_info_pixmap_1 (cfgfile, "skim", "skim", &(Skin->skim.skim));
	skin_read_info_pixmap_3 (cfgfile, "skim", "start_button",
				 &(Skin->skim.start_button));
	skin_read_info_pixmap_4 (cfgfile, "skim", "stop_button",
				 &(Skin->skim.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "skim", "pause_button",
				 &(Skin->skim.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "skim", "continue_button",
				 &(Skin->skim.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "skim", "group_button",
				 &(Skin->skim.group_button));
	skin_read_info_pixmap_3 (cfgfile, "skim", "test_button",
				 &(Skin->skim.test_button));
	skin_read_info_pixmap_3 (cfgfile, "skim", "return_button",
				 &(Skin->skim.return_button));
	skin_read_info_pixmap_4 (cfgfile, "skim", "readword_ckbutton",
				 &(Skin->skim.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "skim", "phonetic_ckbutton",
				 &(Skin->skim.phonetic_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "skim", "showmeanlater_ckbutton",
				 &(Skin->skim.showmeanlater_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "skim", "upcase_ckbutton",
				 &(Skin->skim.upcase_ckbutton));
	skin_read_info_pixmap_3 (cfgfile, "skim", "readword_button",
				 &(Skin->skim.readword_button));
	skin_read_info_widget_1 (cfgfile, "skim", "wordlist_scrollvalue_label",
				 &(Skin->skim.wordlist_scrollvalue_label));
	skin_read_info_widget_1 (cfgfile, "skim", "word_index_label",
				 &(Skin->skim.word_index_label));
	skin_read_info_widget_1 (cfgfile, "skim", "word_count_label",
				 &(Skin->skim.word_count_label));
	skin_read_info_widget_2 (cfgfile, "skim", "wordlist_vscroll",
				 &(Skin->skim.wordlist_vscroll));
	skin_read_info_widget_2 (cfgfile, "skim", "sentencelist_vscroll",
				 &(Skin->skim.sentencelist_vscroll));
	skin_read_info_widget_1 (cfgfile, "skim", "wordview_label_1",
				 &(Skin->skim.wordview_label_1));
	skin_read_info_widget_1 (cfgfile, "skim", "phonetic_label",
				 &(Skin->skim.phonetic_label));
	skin_read_info_widget_1 (cfgfile, "skim", "wordview_label_3",
				 &(Skin->skim.wordview_label_3));
	skin_read_info_widget_2 (cfgfile, "skim", "wordlist_area",
				 &(Skin->skim.wordlist_area));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_1",
				 &(Skin->skim.wordlist_item_1));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_2",
				 &(Skin->skim.wordlist_item_2));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_3",
				 &(Skin->skim.wordlist_item_3));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_4",
				 &(Skin->skim.wordlist_item_4));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_5",
				 &(Skin->skim.wordlist_item_5));
	skin_read_info_pixmap_2 (cfgfile, "skim", "wordlist_item_6",
				 &(Skin->skim.wordlist_item_6));
	skin_read_info_pixmap_1 (cfgfile, "skim", "dialog",
				 &(Skin->skim.dialog));
	skin_read_info_pixmap_4 (cfgfile, "skim", "dialog_test_robutton",
				 &(Skin->skim.dialog_test_robutton));
	skin_read_info_pixmap_4 (cfgfile, "skim", "dialog_skim_robutton",
				 &(Skin->skim.dialog_skim_robutton));
	skin_read_info_pixmap_4 (cfgfile, "skim", "dialog_return_robutton",
				 &(Skin->skim.dialog_return_robutton));
	skin_read_info_pixmap_2 (cfgfile, "skim", "dialog_ok_button",
				 &(Skin->skim.dialog_ok_button));


// test
	skin_read_info_pixmap_1 (cfgfile, "test", "test", &(Skin->test.test));
	skin_read_info_pixmap_3 (cfgfile, "test", "start_button",
				 &(Skin->test.start_button));
	skin_read_info_pixmap_4 (cfgfile, "test", "stop_button",
				 &(Skin->test.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "test", "pause_button",
				 &(Skin->test.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "test", "continue_button",
				 &(Skin->test.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "test", "group_button",
				 &(Skin->test.group_button));
	skin_read_info_pixmap_3 (cfgfile, "test", "skim_button",
				 &(Skin->test.skim_button));
	skin_read_info_pixmap_3 (cfgfile, "test", "return_button",
				 &(Skin->test.return_button));
	skin_read_info_pixmap_4 (cfgfile, "test", "meaning_ckbutton",
				 &(Skin->test.meaning_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "readword_ckbutton",
				 &(Skin->test.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "phonetic_ckbutton",
				 &(Skin->test.phonetic_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "length_ckbutton",
				 &(Skin->test.length_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "headchar_ckbutton",
				 &(Skin->test.headchar_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "sndeffect_ckbutton",
				 &(Skin->test.sndeffect_ckbutton));
	skin_read_info_pixmap_3 (cfgfile, "test", "readword_button",
				 &(Skin->test.readword_button));
	skin_read_info_widget_1 (cfgfile, "test", "wordlist_scrollvalue_label",
				 &(Skin->test.wordlist_scrollvalue_label));
	skin_read_info_widget_1 (cfgfile, "test", "word_index_label",
				 &(Skin->test.word_index_label));
	skin_read_info_widget_1 (cfgfile, "test", "recited_word_count_label",
				 &(Skin->test.recited_word_count_label));
	skin_read_info_widget_1 (cfgfile, "test", "word_count_label",
				 &(Skin->test.word_count_label));
	skin_read_info_widget_1 (cfgfile, "test", "wordtest_label_1",
				 &(Skin->test.wordtest_label_1));
	skin_read_info_widget_1 (cfgfile, "test", "phonetic_label",
				 &(Skin->test.phonetic_label));
	skin_read_info_widget_1 (cfgfile, "test", "wordtest_line",
				 &(Skin->test.wordtest_line));
	skin_read_info_widget_2 (cfgfile, "test", "wordtest_entry",
				 &(Skin->test.wordtest_entry));
	skin_read_info_widget_2 (cfgfile, "test", "wordlist_vscroll",
				 &(Skin->test.wordlist_vscroll));
	skin_read_info_widget_2 (cfgfile, "test", "sentencelist_vscroll",
				 &(Skin->test.sentencelist_vscroll));
	skin_read_info_widget_2 (cfgfile, "test", "wordlist_area",
				 &(Skin->test.wordlist_area));
	skin_read_info_pixmap_3 (cfgfile, "test", "wordlist_item_1",
				 &(Skin->test.wordlist_item_1));
	skin_read_info_pixmap_3 (cfgfile, "test", "wordlist_item_2",
				 &(Skin->test.wordlist_item_2));
	skin_read_info_pixmap_3 (cfgfile, "test", "wordlist_item_3",
				 &(Skin->test.wordlist_item_3));
	skin_read_info_pixmap_3 (cfgfile, "test", "wordlist_item_4",
				 &(Skin->test.wordlist_item_4));
	skin_read_info_pixmap_3 (cfgfile, "test", "wordlist_item_5",
				 &(Skin->test.wordlist_item_5));
	skin_read_info_pixmap_1 (cfgfile, "test", "wrongright_tip",
				 &(Skin->test.wrongright_tip));
	skin_read_info_widget_1 (cfgfile, "test", "right_label",
				 &(Skin->test.right_label));
	skin_read_info_widget_1 (cfgfile, "test", "headchar_label",
				 &(Skin->test.headchar_label));
	skin_read_info_pixmap_1 (cfgfile, "test", "pressanykey_tip",
				 &(Skin->test.pressanykey_tip));
	skin_read_info_pixmap_1 (cfgfile, "test", "dialog1",
				 &(Skin->test.dialog1));
	skin_read_info_pixmap_1 (cfgfile, "test", "dialog2",
				 &(Skin->test.dialog2));
	skin_read_info_pixmap_4 (cfgfile, "test", "dialog_testagain_group_robutton",
				 &(Skin->test.dialog_testagain_group_robutton));
	skin_read_info_pixmap_4 (cfgfile, "test", "dialog_testover_robutton",
				 &(Skin->test.dialog_testover_robutton));
	skin_read_info_pixmap_2 (cfgfile, "test", "dialog_ok_button",
				 &(Skin->test.dialog_ok_button));

// about
	skin_read_info_pixmap_1 (cfgfile, "about", "about",
				 &(Skin->about.about));
	skin_read_info_pixmap_2 (cfgfile, "about", "exit_button",
				 &(Skin->about.exit_button));

// dict
	skin_read_info_pixmap_1 (cfgfile, "dict", "dict", &(Skin->dict.dict));
	skin_read_info_pixbuf_1 (cfgfile, "dict", "icon", &(Skin->dict.icon));
	skin_read_info_pixmap_3 (cfgfile, "dict", "exit_button",
				 &(Skin->dict.exit_button));
	skin_read_info_pixmap_3 (cfgfile, "dict", "search_button",
				 &(Skin->dict.search_button));
	skin_read_info_pixmap_3 (cfgfile, "dict", "sound_button",
				 &(Skin->dict.sound_button));
	skin_read_info_pixmap_3 (cfgfile, "dict", "card_button",
				 &(Skin->dict.card_button));
	skin_read_info_pixmap_4 (cfgfile, "dict", "search_ckbutton",
				 &(Skin->dict.search_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "dict", "sound_ckbutton",
				 &(Skin->dict.sound_ckbutton));
	skin_read_info_widget_2 (cfgfile, "dict", "word_entry",
				 &(Skin->dict.word_entry));
	skin_read_info_widget_2 (cfgfile, "dict", "word_vscroll",
				 &(Skin->dict.word_vscroll));
	skin_read_info_widget_1 (cfgfile, "dict", "wordlist_dict_label[0]",
				 &(Skin->dict.wordlist_dict_label[0]));
	skin_read_info_widget_1 (cfgfile, "dict", "wordlist_dict_label[1]",
				 &(Skin->dict.wordlist_dict_label[1]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[2]", &(Skin->dict.wordlist_dict_label[2]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[3]", &(Skin->dict.wordlist_dict_label[3]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[4]", &(Skin->dict.wordlist_dict_label[4]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[5]", &(Skin->dict.wordlist_dict_label[5]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[6]", &(Skin->dict.wordlist_dict_label[6]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[7]", &(Skin->dict.wordlist_dict_label[7]));
	//skin_read_info_widget_1(cfgfile,"dict","wordlist_dict_label[8]", &(Skin->dict.wordlist_dict_label[8]));
	//skin_read_info_pixmap_4(cfgfile,"dict","ying_button",&(Skin->dict.ying_button));
	//skin_read_info_pixmap_4(cfgfile,"dict","han_button",&(Skin->dict.han_button));

// help
	skin_read_info_pixmap_1 (cfgfile, "help", "help",
				 &(Skin->help.help));
	skin_read_info_pixmap_1 (cfgfile, "help", "text_area", &(Skin->help.text_area));
	skin_read_info_pixmap_2 (cfgfile, "help", "exit_button",
				 &(Skin->help.exit_button));
	skin_read_info_widget_2 (cfgfile, "help", "wordlist_vscroll", &(Skin->help.wordlist_vscroll));
	skin_read_info_pixmap_2 (cfgfile, "help", "overview_robutton", &(Skin->help.overview_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "firstrecite_robutton", &(Skin->help.firstrecite_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "revise_robutton", &(Skin->help.revise_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "shooting_robutton", &(Skin->help.shooting_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "hearing_robutton", &(Skin->help.hearing_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "typing_robutton", &(Skin->help.typing_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "rest_robutton", &(Skin->help.rest_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "choosebook_robutton", &(Skin->help.choosebook_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "filling_robutton", &(Skin->help.filling_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "know_robutton", &(Skin->help.know_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "wordedit_robutton", &(Skin->help.wordedit_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "bookedit_robutton", &(Skin->help.bookedit_robutton));
	skin_read_info_pixmap_2 (cfgfile, "help", "sentence_robutton", &(Skin->help.sentence_robutton));

// record
	skin_read_info_pixmap_1 (cfgfile, "record", "record",
				 &(Skin->record.record));
	skin_read_info_pixmap_2 (cfgfile, "record", "exit_button",
				 &(Skin->record.exit_button));
	skin_read_info_widget_2 (cfgfile, "record", "text_label",
				 &(Skin->record.text_label));
// option

// text
	skin_read_info_pixmap_1 (cfgfile, "text", "entry_pen",
				 &(Skin->text.entry_pen));
	skin_read_info_pixmap_1 (cfgfile, "text", "entry_right",
				 &(Skin->text.entry_right));
	skin_read_info_pixmap_1 (cfgfile, "text", "entry_wrong",
				 &(Skin->text.entry_wrong));
	skin_read_info_pixmap_1 (cfgfile, "text", "yinbiao",
				 &(Skin->text.yinbiao));
	skin_read_info_widget_1 (cfgfile, "text", "entry_text",
				 &(Skin->text.entry_text));
	skin_read_info_widget_2 (cfgfile, "text", "entry_cursor",
				 &(Skin->text.entry_cursor));
	skin_read_info_widget_2 (cfgfile, "text", "entry_underline",
				 &(Skin->text.entry_underline));
	
	skin_read_info_phonetic (cfgfile, "text", "b_phonetic",
				 &(Skin->text.b_phonetic));

// revise_group
	skin_read_info_pixmap_1 (cfgfile, "revise_group", "revise_group",
				 &(Skin->revise_group.revise_group));
	skin_read_info_pixmap_3 (cfgfile, "revise_group", "return_button",
				 &(Skin->revise_group.return_button));
 	skin_read_info_pixmap_3 (cfgfile, "revise_group", "chooseword_button",
				 &(Skin->revise_group.chooseword_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_group", "order_ckbutton",
				 &(Skin->revise_group.order_ckbutton));
	skin_read_info_pixmap_2 (cfgfile, "revise_group", "plane_button_p",
				 &(Skin->revise_group.plane_button_p));
	skin_read_info_widget_1 (cfgfile, "revise_group", "plane_button_text_1",
				 &(Skin->revise_group.plane_button_text_1));
	skin_read_info_widget_1 (cfgfile, "revise_group", "plane_button_text_2",
				 &(Skin->revise_group.plane_button_text_2));
	skin_read_info_widget_1 (cfgfile, "revise_group", "total_groups_label",
				 &(Skin->revise_group.total_groups_label));
	for (i = 0; i < 5; i++)
	{
		sprintf (tmpstr, "plane_button_%d", i + 1);
		skin_read_info_widget_1 (cfgfile, "revise_group", tmpstr,
					 &(Skin->revise_group.
					   plane_button[i]));
	}
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			sprintf (tmpstr, "plane_h_%d_%d", i + 1, j + 1);
			skin_read_info_pixmap_1 (cfgfile, "revise_group",
						 tmpstr,
						 &(Skin->revise_group.
						   plane_h[i][j]));
			sprintf (tmpstr, "plane_%d_%d", i + 1, j + 1);
			skin_read_info_pixmap_2 (cfgfile, "revise_group",
						 tmpstr,
						 &(Skin->revise_group.
						   plane[i][j]));
		}
	}

// revise_test
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "revise_test",
				 &(Skin->revise_test.revise_test));
	skin_read_info_widget_1 (cfgfile, "revise_test",
				 "wordlist_item_label_1",
				 &(Skin->revise_test.wordlist_item_label_1));
	skin_read_info_widget_1 (cfgfile, "revise_test",
				 "wordlist_item_label_2",
				 &(Skin->revise_test.wordlist_item_label_2));
	skin_read_info_pixbuf_1 (cfgfile, "revise_test",
				 "wordlist_item_normal",
				 &(Skin->revise_test.wordlist_item_normal));
	skin_read_info_pixbuf_1 (cfgfile, "revise_test",
				 "wordlist_item_right",
				 &(Skin->revise_test.wordlist_item_right));
	skin_read_info_pixbuf_1 (cfgfile, "revise_test",
				 "wordlist_item_wrong",
				 &(Skin->revise_test.wordlist_item_wrong));
	skin_read_info_pixbuf_1 (cfgfile, "revise_test", "wordlist_item_ok",
				 &(Skin->revise_test.wordlist_item_ok));
	skin_read_info_widget_2 (cfgfile, "revise_test", "wordlist_area",
				 &(Skin->revise_test.wordlist_area));
	skin_read_info_pixmap_2 (cfgfile, "revise_test", "wordlist_item_1",
				 &(Skin->revise_test.wordlist_item_1));
	skin_read_info_pixmap_2 (cfgfile, "revise_test", "wordlist_item_2",
				 &(Skin->revise_test.wordlist_item_2));
	skin_read_info_pixmap_2 (cfgfile, "revise_test", "wordlist_item_3",
				 &(Skin->revise_test.wordlist_item_3));
	skin_read_info_widget_2 (cfgfile, "revise_test", "wordlist_vscroll",
				 &(Skin->revise_test.wordlist_vscroll));
	skin_read_info_widget_1 (cfgfile, "revise_test", "wordtest_label_1",
				 &(Skin->revise_test.wordtest_label_1));
	skin_read_info_widget_1 (cfgfile, "revise_test", "wordlist_scrollvalue_label",
				 &(Skin->revise_test.wordlist_scrollvalue_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "word_index_label",
				 &(Skin->revise_test.word_index_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "recited_word_count_label",
				 &(Skin->revise_test.recited_word_count_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "word_count_label",
				 &(Skin->revise_test.word_count_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "phonetic_label",
				 &(Skin->revise_test.phonetic_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "right_label",
				 &(Skin->revise_test.right_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "headchar_label",
				 &(Skin->revise_test.headchar_label));
	skin_read_info_widget_2 (cfgfile, "revise_test", "wordtest_entry",
				 &(Skin->revise_test.wordtest_entry));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "start_button",
				 &(Skin->revise_test.start_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "stop_button",
				 &(Skin->revise_test.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "pause_button",
				 &(Skin->revise_test.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "continue_button",
				 &(Skin->revise_test.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "group_button",
				 &(Skin->revise_test.group_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "skim_button",
				 &(Skin->revise_test.skim_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "return_button",
				 &(Skin->revise_test.return_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "handinpaper_button",
				 &(Skin->revise_test.handinpaper_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "readword_button",
				 &(Skin->revise_test.readword_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_test", "chooseword_button",
				 &(Skin->revise_test.chooseword_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "meaning_ckbutton",
				 &(Skin->revise_test.meaning_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "readword_ckbutton",
				 &(Skin->revise_test.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "phonetic_ckbutton",
				 &(Skin->revise_test.phonetic_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "length_ckbutton",
				 &(Skin->revise_test.length_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "headchar_ckbutton",
				 &(Skin->revise_test.headchar_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test", "sndeffect_ckbutton",
				 &(Skin->revise_test.sndeffect_ckbutton));
	skin_read_info_widget_1 (cfgfile, "revise_test", "group_id_label", &(Skin->revise_test.group_id_label));
	skin_read_info_widget_1 (cfgfile, "revise_test", "group_info_label", &(Skin->revise_test.group_info_label));
	for (i = 0; i < 5; i++)
	{
		sprintf (tmpstr, "record_label_%d", i + 1);
		skin_read_info_widget_1 (cfgfile, "revise_test", tmpstr, &(Skin->revise_test.record_label[i]));
		sprintf (tmpstr, "revise_%d_times_pix", i + 1);
		skin_read_info_pixmap_1 (cfgfile, "revise_test", tmpstr, &(Skin->revise_test.revise_times_pix[i]));
	}
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "wordtest_tip", &(Skin->revise_test.wordtest_tip));
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "handinpaper_tip", &(Skin->revise_test.handinpaper_tip));
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "firsttest_tip", &(Skin->revise_test.firsttest_tip));
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "secondtest_tip", &(Skin->revise_test.secondtest_tip));
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "pressanykey_tip", &(Skin->revise_test.pressanykey_tip));
	skin_read_info_pixmap_1 (cfgfile, "revise_test", "right_tip", &(Skin->revise_test.right_tip));

	skin_read_info_pixmap_1 (cfgfile, "revise_test", "dialog",
				 &(Skin->revise_test.dialog));
	skin_read_info_pixmap_4 (cfgfile, "revise_test",
				 "dialog_skim_robutton",
				 &(Skin->revise_test.dialog_skim_robutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test",
				 "dialog_group_robutton",
				 &(Skin->revise_test.dialog_group_robutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_test",
				 "dialog_testover_robutton",
				 &(Skin->revise_test.
				   dialog_testover_robutton));
	skin_read_info_pixmap_2 (cfgfile, "revise_test", "dialog_ok_button",
				 &(Skin->revise_test.dialog_ok_button));

// revise_skim
	skin_read_info_pixmap_1 (cfgfile, "revise_skim", "revise_skim",
				 &(Skin->revise_skim.revise_skim));
	skin_read_info_widget_2 (cfgfile, "revise_skim", "viewer",
				 &(Skin->revise_skim.viewer));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "start_button",
				 &(Skin->revise_skim.start_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_skim", "stop_button",
				 &(Skin->revise_skim.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_skim", "pause_button",
				 &(Skin->revise_skim.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "continue_button",
				 &(Skin->revise_skim.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "group_button",
				 &(Skin->revise_skim.group_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "test_button",
				 &(Skin->revise_skim.test_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "return_button",
				 &(Skin->revise_skim.return_button));
	skin_read_info_widget_1 (cfgfile, "revise_skim", "word_index_label",
				 &(Skin->revise_skim.word_index_label));
	skin_read_info_widget_1 (cfgfile, "revise_skim", "word_count_label",
				 &(Skin->revise_skim.word_count_label));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "readword_button",
				 &(Skin->revise_skim.readword_button));
	skin_read_info_pixmap_3 (cfgfile, "revise_skim", "chooseword_button",
				 &(Skin->revise_skim.chooseword_button));
	skin_read_info_pixmap_4 (cfgfile, "revise_skim", "readword_ckbutton",
				 &(Skin->revise_skim.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_skim", "meaning_ckbutton",
				 &(Skin->revise_skim.meaning_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "revise_skim", "animate_ckbutton",
				 &(Skin->revise_skim.animate_ckbutton));
				 

// rest
	skin_read_info_pixmap_1 (cfgfile, "rest", "rest",
				 &(Skin->rest.rest));
	skin_read_info_pixmap_3 (cfgfile, "rest", "start_button",
				 &(Skin->rest.start_button));
	skin_read_info_pixmap_4 (cfgfile, "rest", "stop_button",
				 &(Skin->rest.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "rest", "pause_button",
				 &(Skin->rest.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "rest", "continue_button",
				 &(Skin->rest.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "rest", "return_button",
				 &(Skin->rest.return_button));

// know
	skin_read_info_pixmap_1 (cfgfile, "know", "know",
				 &(Skin->know.know));
	skin_read_info_pixmap_3 (cfgfile, "know", "return_button",
				 &(Skin->know.return_button));

// shooting
	skin_read_info_pixmap_1 (cfgfile, "shooting", "shooting",
				 &(Skin->shooting.shooting));
	skin_read_info_pixmap_3 (cfgfile, "shooting", "start_button",
				 &(Skin->shooting.start_button));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "stop_button",
				 &(Skin->shooting.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "pause_button",
				 &(Skin->shooting.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "shooting", "continue_button",
				 &(Skin->shooting.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "shooting", "return_button",
				 &(Skin->shooting.return_button));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "readword_ckbutton",
				 &(Skin->shooting.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "phonetic_ckbutton",
				 &(Skin->shooting.phonetic_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "meaning_ckbutton",
				 &(Skin->shooting.meaning_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "word_ckbutton",
				 &(Skin->shooting.word_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "shotsnd_ckbutton",
				 &(Skin->shooting.shotsnd_ckbutton));
	skin_read_info_pixmap_3 (cfgfile, "shooting", "readword_button",
				 &(Skin->shooting.readword_button));
	skin_read_info_pixmap_3 (cfgfile, "shooting", "chooseword_button",
				 &(Skin->shooting.chooseword_button));
	skin_read_info_pixmap_1 (cfgfile, "shooting", "wordlist_head_1",
				 &(Skin->shooting.wordlist_head[0]));
	skin_read_info_pixmap_1 (cfgfile, "shooting", "wordlist_head_2",
				 &(Skin->shooting.wordlist_head[1]));
	skin_read_info_pixmap_1 (cfgfile, "shooting", "wordlist_head_3",
				 &(Skin->shooting.wordlist_head[2]));
	skin_read_info_pixmap_1 (cfgfile, "shooting", "wordlist_head_4",
				 &(Skin->shooting.wordlist_head[3]));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "wordlist_item_1",
				 &(Skin->shooting.wordlist_item_1));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "wordlist_item_2",
				 &(Skin->shooting.wordlist_item_2));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "wordlist_item_3",
				 &(Skin->shooting.wordlist_item_3));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "wordlist_item_4",
				 &(Skin->shooting.wordlist_item_4));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "entocn_robutton",
				 &(Skin->shooting.entocn_robutton));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "cntoen_robutton",
				 &(Skin->shooting.cntoen_robutton));
	skin_read_info_widget_1 (cfgfile, "shooting", "word_label",
				 &(Skin->shooting.word_label));
	skin_read_info_widget_1 (cfgfile, "shooting", "meaning_label",
				 &(Skin->shooting.meaning_label));
	skin_read_info_widget_1 (cfgfile, "shooting", "recited_word_count_label",
				 &(Skin->shooting.recited_word_count_label));
	skin_read_info_widget_1 (cfgfile, "shooting", "word_count_label",
				 &(Skin->shooting.word_count_label));
	skin_read_info_pixmap_4 (cfgfile, "shooting", "tank",
				 &(Skin->shooting.tank));
	skin_read_info_pixmap_14 (cfgfile, "shooting", "shoot_pixmap_1",
				  &(Skin->shooting.shoot_pixmap[0]));
	skin_read_info_pixmap_14 (cfgfile, "shooting", "shoot_pixmap_2",
				  &(Skin->shooting.shoot_pixmap[1]));
	skin_read_info_pixmap_14 (cfgfile, "shooting", "shoot_pixmap_3",
				  &(Skin->shooting.shoot_pixmap[2]));
	skin_read_info_pixmap_14 (cfgfile, "shooting", "shoot_pixmap_4",
				  &(Skin->shooting.shoot_pixmap[3]));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "right_pixmap_1",
				 &(Skin->shooting.right_pixmap[0]));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "right_pixmap_2",
				 &(Skin->shooting.right_pixmap[1]));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "right_pixmap_3",
				 &(Skin->shooting.right_pixmap[2]));
	skin_read_info_pixmap_2 (cfgfile, "shooting", "right_pixmap_4",
				 &(Skin->shooting.right_pixmap[3]));

// typing
	skin_read_info_pixmap_1 (cfgfile, "typing", "typing",
				 &(Skin->typing.typing));
	skin_read_info_pixmap_3 (cfgfile, "typing", "start_button",
				 &(Skin->typing.start_button));
	skin_read_info_pixmap_4 (cfgfile, "typing", "stop_button",
				 &(Skin->typing.stop_button));
	skin_read_info_pixmap_4 (cfgfile, "typing", "pause_button",
				 &(Skin->typing.pause_button));
	skin_read_info_pixmap_3 (cfgfile, "typing", "continue_button",
				 &(Skin->typing.continue_button));
	skin_read_info_pixmap_3 (cfgfile, "typing", "return_button",
				 &(Skin->typing.return_button));
	skin_read_info_pixmap_4 (cfgfile, "typing", "readword_ckbutton",
				 &(Skin->typing.readword_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "typing", "phonetic_ckbutton",
				 &(Skin->typing.phonetic_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "typing", "meaning_ckbutton",
				 &(Skin->typing.meaning_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "typing", "sndeffect_ckbutton",
				 &(Skin->typing.sndeffect_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "typing", "fingering_ckbutton",
				 &(Skin->typing.fingering_ckbutton));
	skin_read_info_pixmap_3 (cfgfile, "typing", "readword_button",
				 &(Skin->typing.readword_button));
	skin_read_info_pixmap_3 (cfgfile, "typing", "chooseword_button",
				 &(Skin->typing.chooseword_button));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_area",
				 &(Skin->typing.wordlist_area));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_1",
				 &(Skin->typing.wordlist_item[0]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_2",
				 &(Skin->typing.wordlist_item[1]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_3",
				 &(Skin->typing.wordlist_item[2]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_4",
				 &(Skin->typing.wordlist_item[3]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_5",
				 &(Skin->typing.wordlist_item[4]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_item_6",
				 &(Skin->typing.wordlist_item[5]));
	skin_read_info_widget_2 (cfgfile, "typing", "wordlist_vscroll",
				 &(Skin->typing.wordlist_vscroll));
	skin_read_info_widget_2 (cfgfile, "typing", "wordtest_entry",
				 &(Skin->typing.wordtest_entry));
	skin_read_info_widget_1 (cfgfile, "typing", "wordtest_label_1",
				 &(Skin->typing.wordtest_label_1));
	skin_read_info_widget_1 (cfgfile, "typing", "recited_word_count_label",
				 &(Skin->typing.recited_word_count_label));
	skin_read_info_widget_1 (cfgfile, "typing", "word_count_label",
				 &(Skin->typing.word_count_label));
	skin_read_info_pixmap_1 (cfgfile, "typing", "pressanykey_tip",
				 &(Skin->typing.pressanykey_tip));
	skin_read_info_pixmap_1 (cfgfile, "typing", "speed_info_pixmap",
				 &(Skin->typing.speed_info_pixmap));
	skin_read_info_pixmap_1 (cfgfile, "typing", "speed_info_bar",
				 &(Skin->typing.speed_info_bar));
	for (i = 0; i < 55; i++)
	{
		sprintf (tmpstr, "key_pixmap_%d", i + 1);
		skin_read_info_pixmap_1 (cfgfile, "typing", tmpstr,
					 &(Skin->typing.key_pixmap[i]));
	}
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_ld_pixmap",
				 &(Skin->typing.speed_ld_pixmap));
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_rd_pixmap",
				 &(Skin->typing.speed_rd_pixmap));
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_lm_pixmap",
				 &(Skin->typing.speed_lm_pixmap));
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_rm_pixmap",
				 &(Skin->typing.speed_rm_pixmap));
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_lu_pixmap",
				 &(Skin->typing.speed_lu_pixmap));
	skin_read_info_pixmap_6 (cfgfile, "typing", "speed_ru_pixmap",
				 &(Skin->typing.speed_ru_pixmap));
	skin_read_info_pixmap_10 (cfgfile, "typing", "number_pixmap",
				  &(Skin->typing.number_pixmap));

// chooseword
	skin_read_info_pixmap_1 (cfgfile, "chooseword", "chooseword",
				 &(Skin->chooseword.chooseword));
	skin_read_info_pixmap_2 (cfgfile, "chooseword", "ok_button",
				 &(Skin->chooseword.ok_button));
	skin_read_info_pixmap_2 (cfgfile, "chooseword", "cancel_button",
				 &(Skin->chooseword.cancel_button));
	skin_read_info_pixmap_4 (cfgfile, "chooseword", "order_ckbutton",
				 &(Skin->chooseword.order_ckbutton));
	skin_read_info_pixmap_4 (cfgfile, "chooseword", "bygroup_robutton",
				 &(Skin->chooseword.bygroup_robutton));
	skin_read_info_pixmap_4 (cfgfile, "chooseword", "byscope_robutton",
				 &(Skin->chooseword.byscope_robutton));
	skin_read_info_pixmap_4 (cfgfile, "chooseword", "byclass_robutton",
				 &(Skin->chooseword.byclass_robutton));
	skin_read_info_widget_2 (cfgfile, "chooseword", "group_entry",
				 &(Skin->chooseword.group_entry));
	skin_read_info_widget_2 (cfgfile, "chooseword", "scope_entry",
				 &(Skin->chooseword.scope_entry));
	skin_read_info_widget_2 (cfgfile, "chooseword", "scopecount_entry",
				 &(Skin->chooseword.scopecount_entry));
	skin_read_info_widget_2 (cfgfile, "chooseword", "class_entry",
				 &(Skin->chooseword.class_entry));
	skin_read_info_widget_1 (cfgfile, "chooseword", "group_s_label",
				 &(Skin->chooseword.group_s_label));
	skin_read_info_widget_1 (cfgfile, "chooseword", "scope_s_label",
				 &(Skin->chooseword.scope_s_label));
	skin_read_info_widget_1 (cfgfile, "chooseword", "class_s_label",
				 &(Skin->chooseword.class_s_label));
	skin_read_info_pixmap_1 (cfgfile, "chooseword", "noclass_tip",
				 &(Skin->chooseword.noclass_tip));
	skin_read_info_pixmap_1 (cfgfile, "chooseword", "scopeerr_tip",
				 &(Skin->chooseword.scopeerr_tip));

// collectword
	skin_read_info_pixmap_1 (cfgfile, "collectword", "collectword",
				 &(Skin->collectword.collectword));
	skin_read_info_pixmap_2 (cfgfile, "collectword", "exit_button",
				 &(Skin->collectword.exit_button));
	skin_read_info_widget_2 (cfgfile, "collectword", "wordlist_vscroll",
				 &(Skin->collectword.wordlist_vscroll));
	for (i = 0; i < 9; i++)
	{
		sprintf (tmpstr, "wordlist_label_%d", i+1);
		skin_read_info_widget_2 (cfgfile, "collectword", tmpstr,
					 &(Skin->collectword.
					   wordlist_label[i]));
	}

// mark
	skin_read_info_pixmap_1 (cfgfile, "mark", "mark", &(Skin->mark.mark));
	skin_read_info_pixmap_3 (cfgfile, "mark", "ok_button", &(Skin->mark.ok_button));
	skin_read_info_pixmap_1 (cfgfile, "mark", "typing_pixmap", &(Skin->mark.typing_pixmap));
	skin_read_info_widget_1 (cfgfile, "mark", "revise_test_mark_label", &(Skin->mark.revise_test_mark_label));
	skin_read_info_widget_1 (cfgfile, "mark", "revise_test_time_label", &(Skin->mark.revise_test_time_label));
	skin_read_info_widget_1 (cfgfile, "mark", "typing_mark_label", &(Skin->mark.typing_mark_label));

// end
	rw_cfg_free (cfgfile);
	g_free (cfgfile);

	sprintf (Skin->dir, "%s/skins/%s", reciteword_data_dir, skin_dir);  //load info success,set the skin dir.
	
	return FALSE;
}

static void
skin_load_pixbuf (GdkPixbuf * tmp_pixbuf, int deep,int x[],int y[],int w, int h, GdkPixbuf* p[])
{
	for (int i=0; i< deep; i++)
	{
		p[i]=gdk_pixbuf_new_subpixbuf(tmp_pixbuf, x[i+1], y[i+1], w, h);
	}
}

void
skin_load_pixbuf_1 (GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixbuf_1 *image)
{
	if (image->w == 0)
		return;
	skin_load_pixbuf (tmp_pixbuf, 1, image->x, image->y, image->w, image->h, image->p);
}

static void
skin_load_pixmap (GtkWidget *window, GdkPixbuf * tmp_pixbuf, int deep,int x[],int y[],int w, int h, GdkPixmap* p[])
{
	for (int i=0; i< deep; i++)
	{
		p[i] = gdk_pixmap_new (window->window, w, h, -1);
		gdk_draw_pixbuf(p[i],
				       window->style->fg_gc[GTK_STATE_NORMAL],
				tmp_pixbuf, 
				       x[i+1], y[i+1], 0, 0, w,
				       h, GDK_RGB_DITHER_NORMAL, 0, 0);
		/*gdk_pixbuf_render_to_drawable (tmp_pixbuf, p[i],
				       window->style->fg_gc[GTK_STATE_NORMAL],
				       x[i+1], y[i+1], 0, 0, w,
				       h, GDK_RGB_DITHER_NORMAL, 0, 0);*/
	}
}

void
skin_load_pixmap_1 (GtkWidget *window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixmap_1 *image)
{
	if (image->w == 0)
		return;
	skin_load_pixmap (window, tmp_pixbuf, 1, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_2 (GtkWidget *window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixmap_2 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 2, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_3 (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixmap_3 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 3, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_4 (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixmap_4 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 4, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_6 (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_pixmap_6 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 6, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_10 (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		     struct _Skin_pixmap_10 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 10, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_pixmap_14 (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		     struct _Skin_pixmap_14 *image)
{
	skin_load_pixmap (window, tmp_pixbuf, 14, image->x, image->y, image->w, image->h, image->p);
}

void
skin_load_phonetic (GtkWidget * window, GdkPixbuf * tmp_pixbuf,
		    struct _Skin_phonetic *button)
{
	gint i, x;
	x = button->x[0];
	for (i = 0; i < 43; i++)
	{
		button->p[i]=gdk_pixbuf_new_subpixbuf(tmp_pixbuf, x, button->y[0], button->w[i], button->h);		
		x += button->w[i];
	}
}

void
skin_load_reciteword (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;

	Skin->reciteword.normal_cursor.cursor=(GdkCursor *)NULL; // NULL is the default cursor.
	Skin->reciteword.disable_cursor.cursor= gdk_cursor_new(GDK_X_CURSOR);
	Skin->reciteword.hand_cursor.cursor= gdk_cursor_new(GDK_HAND2);
	Skin->reciteword.gun_cursor.cursor= gdk_cursor_new(GDK_PLUS);

	gboolean use_custom_font;
	rw_cfg_read_boolean (usercfgfile, "reciteword", "use_custom_font", &use_custom_font);
	if (use_custom_font)
	{
		gchar *bb;
		if (rw_cfg_read_string (usercfgfile, "reciteword", "normal_english_font", &bb))
		{
			Skin->reciteword.n_font = pango_font_description_from_string(bb);
			g_free(bb);
		}
		else
		{
			Skin->reciteword.n_font = pango_font_description_from_string(Skin->reciteword.normal_english_font);
		}
		if (rw_cfg_read_string (usercfgfile, "reciteword", "big_english_font", &bb))
		{
			Skin->reciteword.b_font = pango_font_description_from_string(bb);
			g_free(bb);
		}
		else
		{
			Skin->reciteword.b_font = pango_font_description_from_string(Skin->reciteword.big_english_font);
		}
	}
	else
	{
		Skin->reciteword.n_font = pango_font_description_from_string(Skin->reciteword.normal_english_font);
		Skin->reciteword.b_font = pango_font_description_from_string(Skin->reciteword.big_english_font);
	}
}

void
skin_load_face (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;

	GdkPixbuf *tmp_pixbuf;
	char skin_file[256];

	sprintf (skin_file, "%s/face.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->face.face));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->face.menubar));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/facer.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixbuf_1 (tmp_pixbuf, &(Skin->face.icon));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->face.menu));

	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.choosebook_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->face.firstrecite_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.revise_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.rest_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.know_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.shooting_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->face.typing_button));

	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->face.menu_exit_button));

	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->face.help_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->face.record_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->face.option_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->face.dict_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->face.about_button));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_about (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;

	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];
	
	sprintf (skin_file, "%s/about.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->about.about));
	g_object_unref (tmp_pixbuf);
	sprintf (skin_file, "%s/aboutr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->about.exit_button));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_dict (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/dict.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->dict.dict));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/dictr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixbuf_1 (tmp_pixbuf, &(Skin->dict.icon));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->dict.exit_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->dict.search_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->dict.card_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->dict.sound_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->dict.search_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->dict.sound_ckbutton));
	//skin_load_pixmap_4(window,tmp_pixbuf,&(Skin->dict.ying_button));
	//skin_load_pixmap_4(window,tmp_pixbuf,&(Skin->dict.han_button));
	g_object_unref (tmp_pixbuf);

}

void
skin_load_help (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;

	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];
	sprintf (skin_file, "%s/help.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->help.help));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->help.text_area));
	g_object_unref (tmp_pixbuf);
	sprintf (skin_file, "%s/helpr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.exit_button));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.overview_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.firstrecite_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.revise_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.shooting_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.hearing_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.typing_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.rest_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.choosebook_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.filling_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.know_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.wordedit_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.bookedit_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->help.sentence_robutton));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_record (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;

	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];
	sprintf (skin_file, "%s/record.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->record.record));
	g_object_unref (tmp_pixbuf);
	sprintf (skin_file, "%s/recordr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->record.exit_button));
	g_object_unref (tmp_pixbuf);
}

void skin_load_choosebook(GtkWidget * window)
{
	static int loaded=0;
	if (loaded)
		return;
	loaded=1;
	

	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];
	
	sprintf (skin_file, "%s/choosebook.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file(skin_file,NULL);
	skin_load_pixmap_1(window,tmp_pixbuf,&(Skin->choosebook.choosebook));
	g_object_unref(tmp_pixbuf);
	
	sprintf (skin_file, "%s/choosebookr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file(skin_file,NULL);
	skin_load_pixbuf_1(tmp_pixbuf,&(Skin->choosebook.dir));
	skin_load_pixmap_3(window,tmp_pixbuf,&(Skin->choosebook.ok_button));
	skin_load_pixmap_3(window,tmp_pixbuf,&(Skin->choosebook.return_button));
	g_object_unref(tmp_pixbuf);
}

void
skin_load_group (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/group.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->group.group));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/groupr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.ok_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.cancel_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.add_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.reduce_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.class_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->group.return_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->group.order_ckbutton));

	g_object_unref (tmp_pixbuf);

}

void
skin_load_skim (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/skim.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->skim.skim));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/skimr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.group_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.test_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.return_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.phonetic_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.showmeanlater_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->skim.upcase_ckbutton));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->skim.readword_button));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_1));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_2));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_3));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_4));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_5));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->skim.wordlist_item_6));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->skim.dialog));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->skim.dialog_test_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->skim.dialog_skim_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->skim.dialog_return_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->skim.dialog_ok_button));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_test (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/test.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->test.test));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/testr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.group_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.skim_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.return_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.meaning_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.phonetic_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.length_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.headchar_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->test.sndeffect_ckbutton));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.readword_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.wordlist_item_1));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.wordlist_item_2));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.wordlist_item_3));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.wordlist_item_4));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->test.wordlist_item_5));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->test.wrongright_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->test.pressanykey_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->test.dialog1));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->test.dialog2));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->test.dialog_testagain_group_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->test.dialog_testover_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->test.dialog_ok_button));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_text (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	char skin_file[256];

	sprintf (skin_file, "%s/text.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->text.entry_pen));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->text.entry_right));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->text.entry_wrong));
	skin_load_phonetic (window, tmp_pixbuf, &(Skin->text.b_phonetic));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_revise_group (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/revise_group.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_group.revise_group));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/revise_groupr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_group.return_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_group.chooseword_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_group.order_ckbutton));

	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->revise_group.plane_button_p));
	gint i, j;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			skin_load_pixmap_1 (window, tmp_pixbuf,
					    &(Skin->revise_group.
					      plane_h[j][i]));
			skin_load_pixmap_2 (window, tmp_pixbuf,
					    &(Skin->revise_group.plane[j][i]));
		}
	}
	g_object_unref (tmp_pixbuf);
}

void
skin_load_revise_test (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/revise_test.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.revise_test));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/revise_testr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixbuf_1 (tmp_pixbuf,
			      &(Skin->revise_test.wordlist_item_normal));
	skin_load_pixbuf_1 (tmp_pixbuf,
			      &(Skin->revise_test.wordlist_item_right));
	skin_load_pixbuf_1 (tmp_pixbuf,
			      &(Skin->revise_test.wordlist_item_wrong));
	skin_load_pixbuf_1 (tmp_pixbuf,
			      &(Skin->revise_test.wordlist_item_ok));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->revise_test.wordlist_item_1));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->revise_test.wordlist_item_2));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->revise_test.wordlist_item_3));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.group_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.skim_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.return_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.handinpaper_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.readword_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_test.chooseword_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.meaning_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.phonetic_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.length_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.headchar_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.sndeffect_ckbutton));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.revise_times_pix[0]));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.revise_times_pix[1]));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.revise_times_pix[2]));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.revise_times_pix[3]));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.revise_times_pix[4]));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.wordtest_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.handinpaper_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.firsttest_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.secondtest_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_test.pressanykey_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.right_tip));

	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->revise_test.dialog));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.dialog_skim_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.dialog_group_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_test.dialog_testover_robutton));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->revise_test.dialog_ok_button));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_revise_skim (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/revise_skim.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->revise_skim.revise_skim));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/revise_skimr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_skim.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_skim.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.group_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.test_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.return_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.readword_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->revise_skim.chooseword_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_skim.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_skim.meaning_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->revise_skim.animate_ckbutton));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_rest (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/rest.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->rest.rest));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/restr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->rest.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->rest.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->rest.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->rest.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->rest.return_button));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_know (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/know.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->know.know));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/knowr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->know.return_button));
	g_object_unref (tmp_pixbuf);
}

void
skin_load_shooting (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/shooting.png", Skin->dir);
	tmp_pixbuf =
		gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->shooting.shooting));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/shootingr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->shooting.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->shooting.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->shooting.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->shooting.return_button));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_head[0]));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_head[1]));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_head[2]));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_head[3]));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_item_1));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_item_2));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_item_3));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.wordlist_item_4));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.entocn_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.cntoen_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.phonetic_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.meaning_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.word_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->shooting.shotsnd_ckbutton));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->shooting.readword_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->shooting.chooseword_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->shooting.tank));
	skin_load_pixmap_14 (window, tmp_pixbuf,
			     &(Skin->shooting.shoot_pixmap[0]));
	skin_load_pixmap_14 (window, tmp_pixbuf,
			     &(Skin->shooting.shoot_pixmap[1]));
	skin_load_pixmap_14 (window, tmp_pixbuf,
			     &(Skin->shooting.shoot_pixmap[2]));
	skin_load_pixmap_14 (window, tmp_pixbuf,
			     &(Skin->shooting.shoot_pixmap[3]));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.right_pixmap[0]));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.right_pixmap[1]));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.right_pixmap[2]));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->shooting.right_pixmap[3]));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_typing (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/typing.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->typing.typing));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->typing.speed_info_bar));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/typingr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->typing.start_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.stop_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.pause_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->typing.continue_button));
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->typing.return_button));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.readword_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.phonetic_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.meaning_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.sndeffect_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf, &(Skin->typing.fingering_ckbutton));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->typing.readword_button));
	skin_load_pixmap_3 (window, tmp_pixbuf,
			    &(Skin->typing.chooseword_button));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->typing.pressanykey_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->typing.speed_info_pixmap));
	gint i;
	for (i = 0; i < 55; i++)
		skin_load_pixmap_1 (window, tmp_pixbuf,
				    &(Skin->typing.key_pixmap[i]));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_ld_pixmap));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_rd_pixmap));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_lm_pixmap));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_rm_pixmap));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_lu_pixmap));
	skin_load_pixmap_6 (window, tmp_pixbuf,
			    &(Skin->typing.speed_ru_pixmap));
	skin_load_pixmap_10 (window, tmp_pixbuf,
			     &(Skin->typing.number_pixmap));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_chooseword (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/chooseword.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->chooseword.chooseword));
	skin_load_pixmap_2 (window, tmp_pixbuf, &(Skin->chooseword.ok_button));
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->chooseword.cancel_button));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->chooseword.order_ckbutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->chooseword.bygroup_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->chooseword.byscope_robutton));
	skin_load_pixmap_4 (window, tmp_pixbuf,
			    &(Skin->chooseword.byclass_robutton));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->chooseword.noclass_tip));
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->chooseword.scopeerr_tip));

	g_object_unref (tmp_pixbuf);
}

void
skin_load_collectword (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/collectword.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->collectword.collectword));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/collectwordr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_2 (window, tmp_pixbuf,
			    &(Skin->collectword.exit_button));
	g_object_unref (tmp_pixbuf);

}

void
skin_load_mark (GtkWidget * window)
{
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;


	GdkPixbuf *tmp_pixbuf;
	gchar skin_file[256];

	sprintf (skin_file, "%s/mark.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_1 (window, tmp_pixbuf,
			    &(Skin->mark.mark));
	g_object_unref (tmp_pixbuf);

	sprintf (skin_file, "%s/markr.png", Skin->dir);
	tmp_pixbuf = gdk_pixbuf_new_from_file (skin_file, NULL);
	skin_load_pixmap_3 (window, tmp_pixbuf, &(Skin->mark.ok_button));
	skin_load_pixmap_1 (window, tmp_pixbuf, &(Skin->mark.typing_pixmap));
	g_object_unref (tmp_pixbuf);
}

int
SetBackPixmap (GtkWidget * widget, GdkPixmap * pixmap)
{
	gdk_window_set_back_pixmap (widget->window, pixmap, FALSE);
	//gtk_widget_queue_draw (widget);	
	gdk_window_clear (widget->window);
	return 0;
}

void
SetFont (GtkWidget *widget, gboolean is_big)
{
	if (is_big)
		gtk_widget_modify_font(widget,Skin->reciteword.b_font);
	else
		gtk_widget_modify_font(widget,Skin->reciteword.n_font);
}
