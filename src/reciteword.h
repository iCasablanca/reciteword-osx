#ifndef __RECITEWORD_H__
#define __RECITEWORD_H__

#include <unistd.h>
#include "bookfile.h"
#include "widget.h"
#include "help.h"
#include "about.h"
#include "option.h"
#include "dict.h"
#include "choosebook.h"
#include "firstrecite.h"
#include "firstrecite_test.h"
#include "readword.h"
#include "record.h"
#include "revise.h"
#include "revise_test.h"
#include "rest.h"
#include "sndserv.h"
#include "sound.h"
#include "know.h"
#include "shooting.h"
#include "typing.h"
#include "chooseword.h"
#include "collectword.h"
#include "mark.h"
#include "menu.h"
#include <gtk/gtk.h>

enum Rw_Status
{
	STATUS_NORMAL,
	STATUS_CHOOSEBOOK,
	STATUS_FIRSTRECITE_GROUP,
	STATUS_FIRSTRECITE_SKIM,
	STATUS_FIRSTRECITE_TEST,
	STATUS_REVISE_GROUP,
	STATUS_REVISE_TEST,
	STATUS_REVISE_SKIM,
	STATUS_SHOOTING,
	STATUS_TYPING,
	STATUS_REST,
	STATUS_KNOW
};

struct chooseword_info
{
	gint by; // 0 -> by_group; 1-> by_scope;
	gint zu_index; // when by group
	gint scope_start,scope_count; // when by scope
};

class CReciteWord
{
      protected:

      public:

	Cwyabdc wyabdc;
	CSound soundevent;
	CSndserv sndserv;
	
	gchar userpath[256]; //current user's path,eg, "~/.reciteword" or "users/default"
	
	Rw_Status status;

	BookFile *now_book;
	gint max_zu_index;
	gint next_zu_index;	//firstrecite_group
	Rw_Word now_zu_words;
	gint now_zu_index;  // used in firstrecite_test and revise_test,to record zu's information need to know now is which zu.
	gint now_zu_wordcount;
	
	struct chooseword_info revise_test_chooseword_info;
	struct chooseword_info revise_skim_chooseword_info;
	struct chooseword_info shooting_chooseword_info;
	struct chooseword_info typing_chooseword_info;

	tPixButton help_button;
	tPixButton record_button;
	tPixButton option_button;
	tPixButton dict_button;
	tPixButton about_button;

	CMenu *menu;
	tAbout *about_window;
	CDict *dict_window;
	CHelp *help_window;
	CRecord *record_window;
	COption *option_window;
	
	CChoosebook *choosebook;
	CFirstRecite_group *firstrecite_group;
	CFirstRecite_skim *firstrecite_skim;
	CFirstRecite_test *firstrecite_test;
	CRevise_group *revise_group;
	CRevise_test *revise_test;
	CRevise_skim *revise_skim;
	CRest *rest;
	CKnow *know;
	CShooting *shooting;
	CTyping *typing;
	CChooseword *chooseword;
	CCollectword *collectword;
	CMark *mark;

	GtkWidget *window;
	GtkWidget *fixed;
	GtkTooltips *tooltips;

	  CReciteWord ();

	void init ();
	void create ();
	void show ();
	void close ();
	void clean ();
	void destroy ();
	static void PopupAbout (gpointer data);
	static void PopupDict (gpointer data);
	static void PopupHelp (gpointer data);
	static void PopupRecord (gpointer data);
	static void PopupOption (gpointer data);
	static void PopupChooseword (void (*ok_func) (), void (*cancel_func) (), struct chooseword_info *info);
	static void PopupMark ();
	static void PopupCollectword ();
	static void ShowChoosebook ();
	static void ShowFirstRecite_group ();
	static void ShowFirstRecite_skim ();
	static void ShowFirstRecite_test ();
	static void ShowRevise_group ();
	static void ShowRevise_test ();
	static void ShowRevise_skim ();
	static void ShowRest ();
	static void ShowKnow ();
	static void ShowShooting ();
	static void ShowTyping ();

	void load_book(gchar *filename);
	void close_book();
	void set_now_zu_words(gint zu_index,gint zu_wordcount,gchar **zu_words_w,gchar **zu_words_t,gchar **zu_words_m,gboolean disorder);
	 ~CReciteWord ();
};

extern gchar reciteword_data_dir[256];

#endif // __RECITEWORD_H__
