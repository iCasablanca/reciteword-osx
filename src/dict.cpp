#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

# include "dict.h"
#include "rwconfig.h"
#include "reciteword.h"
# include "skin.h"
#include <glib/gi18n.h>
#include "readword.h"
#include "configfile.h"
#include <gdk/gdkkeysyms.h>


extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;

static gint
on_window_delete_event (GtkWidget * window, GdkEvent *event , CDict * parent)
{
	parent->close();
	return true;
}

static gboolean
on_window_key_press(GtkWidget * window, GdkEventKey * event ,CDict * parent)
{
	gboolean return_val=true;
	const char* text;
	switch (event->keyval)
	{
		case GDK_Escape:
			text = gtk_entry_get_text(GTK_ENTRY(parent->word_entry));
			if (text && text[0]!='\0')
			{
				gtk_entry_set_text(GTK_ENTRY(parent->word_entry),"");
			}
			else
			{
				parent->exit_button.do_clicked();
			}
			break;
		default:
			return_val=false;
	}
	return return_val;
}

static void
on_exit_clicked (gpointer data)
{
	CDict * parent = (CDict *)data;
	parent->close ();
}

static void
on_search_clicked (gpointer data)
{	
	CDict * parent = (CDict *)data;
	if (!parent->search_ckbutton.status)
	{
		if (!(parent->word_text))
			return;
		if (parent->word_text[0] == '\0')
			return;
		if (parent->ecdict->working)
		{
			parent->ecdict->query (parent->word_text);
			parent->set_wordlist_label (1);
		}		
	}
	if (parent->sound_ckbutton.status)
		readword(parent->word_text);
}
static void
on_sound_clicked (gpointer data)
{
	CDict * parent = (CDict *)data;
	readword(parent->word_text);
}
static void
on_card_clicked (gpointer data)
{
}

static void
on_dict_word_entry_activate (GtkWidget * widget, CDict * parent)
{
	on_search_clicked (parent);
}
static void
on_dict_word_entry_changed (GtkWidget * widget, CDict * parent)
{
	parent->word_text =	gtk_entry_get_text (GTK_ENTRY (widget));
	if (parent->search_ckbutton.status)
	{		
		parent->word_text =	gtk_entry_get_text (GTK_ENTRY (widget));
		if (!(parent->word_text))
			return;
		if (parent->word_text[0] == '\0')
			return;
		if (parent->ecdict->working)
		{
			//BUG, when query "sud", the memory will mess up as the value of "parent" will change.
			parent->ecdict->query (parent->word_text);
			parent->set_wordlist_label (1);
		}
	}
}

CDict::CDict ()
{
	onlyshow=FALSE;
	if (g_pReciteWord->window)
		skin_load_dict (g_pReciteWord->window);
	gchar *filename;
	filename = g_build_filename(reciteword_data_dir, "dicts", NULL);
	ecdict = new mydict (filename);
	g_free(filename);
}

CDict::~CDict ()
{
	delete ecdict;
}

void
CDict::init ()
{
	if (!onlyshow)
		g_pReciteWord->dict_button.set_enable (FALSE);
	word_text = NULL;
}

void
CDict::show ()
{
	init ();
	playsound(SND_WIND);
	
	GtkWidget *fixed;
	//GtkWidget *event_box;
	//GtkWidget *pixmap_widget;
	//gchar str[256];
	
	showing = TRUE;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (window, Skin->dict.dict.w, Skin->dict.dict.h);
	gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
	gtk_widget_set_app_paintable(window,TRUE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), _("Dict"));
	gtk_widget_realize (window);
	if (onlyshow)
	{
		skin_load_reciteword (window);
		skin_load_dict (window);
	}
	else
	{
		gtk_window_set_transient_for (GTK_WINDOW (window),
				      GTK_WINDOW (g_pReciteWord->window));
	}
	gtk_window_set_icon (GTK_WINDOW(window),Skin->dict.icon.p[0]);
	
	g_signal_connect (G_OBJECT (window), "delete_event",
			  G_CALLBACK (on_window_delete_event), this);
	g_signal_connect (G_OBJECT (window), "key_press_event",
			  G_CALLBACK (on_window_key_press), this);


	fixed = gtk_fixed_new ();
	//gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_container_add (GTK_CONTAINER (window), fixed);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->dict.dict.p[0]);

	exit_button.create (fixed, Skin->dict.exit_button.x[0],
			    Skin->dict.exit_button.y[0],
			    Skin->dict.exit_button.p[0],
			    Skin->dict.exit_button.p[1],
			    Skin->dict.exit_button.p[2], on_exit_clicked, this);
	search_button.create (fixed, Skin->dict.search_button.x[0],
			      Skin->dict.search_button.y[0],
			      Skin->dict.search_button.p[0],
			      Skin->dict.search_button.p[1],
			      Skin->dict.search_button.p[2], on_search_clicked,
			      this);
	sound_button.create (fixed, Skin->dict.sound_button.x[0],
			     Skin->dict.sound_button.y[0],
			     Skin->dict.sound_button.p[0],
			     Skin->dict.sound_button.p[1],
			     Skin->dict.sound_button.p[2], on_sound_clicked,
			     this);
	card_button.create (fixed, Skin->dict.card_button.x[0],
			    Skin->dict.card_button.y[0], Skin->dict.card_button.p[0],
			    Skin->dict.card_button.p[1],
			    Skin->dict.card_button.p[2], on_card_clicked, NULL);
	search_ckbutton.create (fixed, Skin->dict.search_ckbutton.x[0],
				Skin->dict.search_ckbutton.y[0],
				Skin->dict.search_ckbutton.p[0],
				Skin->dict.search_ckbutton.p[1],
				Skin->dict.search_ckbutton.p[2],
				Skin->dict.search_ckbutton.p[3], NULL);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "dict", "rtsearch",
			     &tmp_b);
	search_ckbutton.set_status(tmp_b);

	sound_ckbutton.create (fixed, Skin->dict.sound_ckbutton.x[0],
			       Skin->dict.sound_ckbutton.y[0],
			       Skin->dict.sound_ckbutton.p[0],
			       Skin->dict.sound_ckbutton.p[1],
			       Skin->dict.sound_ckbutton.p[2],
			       Skin->dict.sound_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "dict", "readword",
			     &tmp_b);
	sound_ckbutton.set_status(tmp_b);
	word_entry = gtk_entry_new ();
	gtk_entry_set_has_frame(GTK_ENTRY(word_entry),false);
	gtk_widget_set_size_request (word_entry, Skin->dict.word_entry.w,
			      Skin->dict.word_entry.h);
	gtk_fixed_put (GTK_FIXED (fixed), word_entry,
		       Skin->dict.word_entry.x[0], Skin->dict.word_entry.y[0]);
	gtk_entry_set_text (GTK_ENTRY (word_entry), "");

	g_signal_connect (G_OBJECT (word_entry), "activate",
			    G_CALLBACK (on_dict_word_entry_activate),
			    this);
	g_signal_connect (G_OBJECT (word_entry), "changed",
			    G_CALLBACK (on_dict_word_entry_changed),
			    this);
	gtk_widget_show (word_entry);
	gtk_widget_grab_focus (word_entry);
	// word_adjust = gtk_adjustment_new (0, 0,20, 1, 5, 5);
	//gtk_signal_connect (GTK_OBJECT (word_adjust), "value",
	//           GTK_SIGNAL_FUNC (on_type_wordlistadjust_value), this);

	word_vscroll = gtk_vscrollbar_new ((GtkAdjustment *) NULL);
	gtk_widget_set_size_request (word_vscroll, Skin->dict.word_vscroll.w,
			      Skin->dict.word_vscroll.h);
	gtk_fixed_put (GTK_FIXED (fixed), word_vscroll,
		       Skin->dict.word_vscroll.x[0], Skin->dict.word_vscroll.y[0]);
	gtk_widget_show (word_vscroll);
	/*ying_button.create (NULL,fixed,
	 * Skin->dict.ying_button.x[0],
	 * Skin->dict.ying_button.y[0],
	 * Skin->dict.ying_button.p[0],
	 * Skin->dict.ying_button.p[1],
	 * Skin->dict.ying_button.p[2],
	 * Skin->dict.ying_button.p[3], NULL);
	 * han_button.create (ying_button.group,fixed,
	 * Skin->dict.han_button.x[0],
	 * Skin->dict.han_button.y[0],
	 * Skin->dict.han_button.p[0],
	 * Skin->dict.han_button.p[1],
	 * Skin->dict.han_button.p[2],
	 * Skin->dict.han_button.p[3], NULL); */
	wordlist_dict_label[0] = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_dict_label[0]),true);

	gtk_fixed_put (GTK_FIXED (fixed),
		       wordlist_dict_label[0],
		       Skin->dict.wordlist_dict_label[0].x[0],
		       Skin->dict.wordlist_dict_label[0].y[0]);

	gtk_widget_show (wordlist_dict_label[0]);
	wordlist_dict_label[1] = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_dict_label[1]),true);
	gtk_widget_set_size_request (wordlist_dict_label[1], 355, -1);
	gtk_label_set_line_wrap (GTK_LABEL (wordlist_dict_label[1]),
				  TRUE);
	gtk_label_set_justify (GTK_LABEL (wordlist_dict_label[1]),
			       GTK_JUSTIFY_LEFT);
	gtk_fixed_put (GTK_FIXED (fixed), wordlist_dict_label[1],
		       Skin->dict.wordlist_dict_label[1].x[0],
		       Skin->dict.wordlist_dict_label[1].y[0]);
	gtk_widget_show (wordlist_dict_label[1]);
	if (!ecdict->working)
	{
		gtk_label_set_text(GTK_LABEL(wordlist_dict_label[1]),_("Dict initialization failed,Dict data files not found!"));
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(wordlist_dict_label[1]),_("Type ESC to quit.\nType Ctrl-backspace to remove the typed word in entry."));
	}
	gtk_widget_show (window);
}

void
CDict::set_wordlist_label (int index)
{
	const gchar *tmp;
	gchar *tmp1;
	tmp=ecdict->getNearEnglishItem (index);
	if (tmp)
	{
		tmp1 = g_convert(tmp,-1,"UTF-8","GBK",NULL,NULL,NULL); //most time,tmp should be English word,but some time it may contain error char.
		if (tmp1)
		{
			gtk_label_set_text (GTK_LABEL (wordlist_dict_label[0]),tmp1);
			g_free(tmp1);
		}
	}
	tmp=ecdict->getNearChineseItem (index);
	if (tmp)
	{
		tmp1 = g_convert(tmp,-1,"UTF-8","GBK",NULL,NULL,NULL);
		//sprintf (str, "%s", ecdict->getNearChineseItem (index));
		if (tmp1)
		{
			gtk_label_set_text (GTK_LABEL (wordlist_dict_label[1]), tmp1);
			g_free(tmp1);
		}
	}
}

void
CDict::close ()
{
	playsound(SND_WIND);
	
	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "dict", "rtsearch",
			     &tmp_b);
	if (tmp_b!=search_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "dict", "rtsearch",
			     search_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "dict", "readword",
			     &tmp_b);
	if (tmp_b!=sound_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "dict", "readword",
			     sound_ckbutton.status);
	}
	search_button.destroy ();
	sound_button.destroy ();
	card_button.destroy ();
	exit_button.destroy ();
	search_ckbutton.destroy ();
	sound_ckbutton.destroy ();
	gtk_widget_destroy (wordlist_dict_label[0]);
	gtk_widget_destroy (wordlist_dict_label[1]);
	gtk_widget_destroy (word_entry);
	gtk_widget_destroy (word_vscroll);
	gtk_widget_destroy (window);
	
	if (onlyshow)
	{
		conf_save_usr();
		if (usercfgfile)
		{
			rw_cfg_free (usercfgfile);
			g_free (usercfgfile);
			usercfgfile = NULL;
		}
		gtk_main_quit ();
	}
	else
	{
		showing = FALSE;
		g_pReciteWord->dict_button.set_enable (TRUE);		
	}
}

void
CDict::create()
{
	onlyshow=TRUE;
	show();
	gdk_threads_enter();
	gtk_main ();
	gdk_threads_leave();
}
