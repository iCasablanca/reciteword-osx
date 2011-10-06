#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "collectword.h"
#include "reciteword.h"
#include "skin.h"
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>


extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;

static void
on_exit_button_clicked (gpointer data)
{
	g_pReciteWord->collectword->close ();
}

static gboolean
on_window_key_press (GtkWidget * window, GdkEventKey * event,
		     CCollectword * parent)
{
	switch (event->keyval)
	{
	case GDK_space:
	case GDK_Return:
	case GDK_Escape:
		parent->exit_button.do_clicked ();
		break;
	}
	return true;
}

static gint
on_window_delete_event (GtkWidget * window, GdkEvent * event,
			CCollectword * parent)
{
	parent->close ();
	return true;
}

static void
on_collectword_wordlistadjust_valuechanged (GtkAdjustment * adj,
					    CCollectword * parent)
{
	int newvalue = ((gint) adj->value);

	parent->set_wordlist_label (newvalue);
}

CCollectword::CCollectword ()
{
	skin_load_collectword (g_pReciteWord->window);
	window = NULL;
}

CCollectword::~CCollectword ()
{
}

void
CCollectword::init ()
{
	int i;
	wordcount = 0;
	for (i = 0; i < g_pReciteWord->now_zu_wordcount; i++)
	{
		if (g_pReciteWord->revise_test->
		    tested_wordlist_item_status[i] == S_right_2)
		{
			wordcount++;
		}
	}
	w = (gchar **) g_malloc ((wordcount) * sizeof (gchar *));
	m = (gchar **) g_malloc ((wordcount) * sizeof (gchar *));
	int j = 0;
	for (i = 0; i < g_pReciteWord->now_zu_wordcount; i++)
	{
		if (g_pReciteWord->revise_test->
		    tested_wordlist_item_status[i] == S_right_2)
		{
			w[j] = g_pReciteWord->now_zu_words.w[i];
			m[j] = g_pReciteWord->now_zu_words.m[i];
			j++;
		}
	}
}

void
CCollectword::set_wordlist_label (gint index)
{
	int i;
	gchar tmpstr[256];
	for (i=0;i<9;i++)
	{
		if (i+index < wordcount)
		{
			sprintf(tmpstr,"%3d    %s",i+index+1,w[i + index]);
			gtk_label_set_text (GTK_LABEL (wordlist_label_w[i]),
					tmpstr);
			gtk_label_set_text (GTK_LABEL (wordlist_label_m[i]),
					m[i + index]);
		}
		else
		{
			gtk_label_set_text(GTK_LABEL (wordlist_label_w[i]),"");
			gtk_label_set_text(GTK_LABEL (wordlist_label_m[i]),"");
		}
	}
}

void
CCollectword::show ()
{
	//collectword will only show after revise_test is over and not all word is right at first time.
	init ();

	GtkWidget *fixed;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (window,
				     Skin->collectword.collectword.w,
				     Skin->collectword.collectword.h);
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_set_app_paintable (window, TRUE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), _("Collect my hard words"));
	gtk_window_set_transient_for (GTK_WINDOW (window),
				      GTK_WINDOW (g_pReciteWord->window));
	gtk_widget_realize (window);

	fixed = gtk_fixed_new ();
	//gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_container_add (GTK_CONTAINER (window), fixed);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->collectword.collectword.p[0]);

	exit_button.create (fixed, Skin->collectword.exit_button.x[0],
			    Skin->collectword.exit_button.y[0],
			    Skin->collectword.exit_button.p[0],
			    Skin->collectword.exit_button.p[1], NULL,
			    on_exit_button_clicked, NULL);
	GtkWidget *hbox;
	for (gint i = 0; i < 9; i++)
	{
		hbox = gtk_hbox_new (FALSE, 10);
		wordlist_label_w[i] = gtk_label_new ("");
		gtk_label_set_selectable(GTK_LABEL(wordlist_label_w[i]),true);
		gtk_widget_set_name (wordlist_label_w[i], "collectword-word_label");
		SetFont(wordlist_label_w[i],false);
		gtk_misc_set_alignment (GTK_MISC
					(wordlist_label_w[i]), 0, .5);
		wordlist_label_m[i] = gtk_label_new ("");
		gtk_label_set_selectable(GTK_LABEL(wordlist_label_m[i]),true);
		gtk_widget_set_name (wordlist_label_m[i], "collectword-meaning_label");
		gtk_misc_set_alignment (GTK_MISC
					(wordlist_label_m[i]), 0, .5);
		gtk_widget_show (wordlist_label_w[i]);
		gtk_widget_show (wordlist_label_m[i]);
		gtk_box_pack_start (GTK_BOX (hbox),
				    wordlist_label_w[i], FALSE, FALSE, FALSE);
		gtk_box_pack_start (GTK_BOX (hbox),
				    wordlist_label_m[i], FALSE, FALSE, FALSE);
		gtk_widget_set_size_request (hbox,
					     Skin->collectword.
					     wordlist_label[i].w,
					     Skin->collectword.
					     wordlist_label[i].h);
		gtk_fixed_put (GTK_FIXED (fixed), hbox,
			       Skin->collectword.wordlist_label[i].x[0],
			       Skin->collectword.wordlist_label[i].y[0]);
		gtk_widget_show (hbox);
	}

	wordlist_adjust = gtk_adjustment_new (0, 0, wordcount, 1, 9, 9);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK
			  (on_collectword_wordlistadjust_valuechanged), this);
	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll,
				     Skin->collectword.
				     wordlist_vscroll.w,
				     Skin->collectword.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (fixed),
		       wordlist_vscroll,
		       Skin->collectword.wordlist_vscroll.x[0],
		       Skin->collectword.wordlist_vscroll.y[0]);

	g_signal_connect (G_OBJECT (window), "delete_event",
			  G_CALLBACK (on_window_delete_event), this);
	g_signal_connect (G_OBJECT (window), "key_press_event",
			  G_CALLBACK (on_window_key_press), this);

	gtk_widget_show (window);
	set_wordlist_label (0);
}

void
CCollectword::close ()
{
	exit_button.destroy ();

	gtk_widget_destroy (window);

	g_free (w);
	g_free (m);
	window = NULL;
}
