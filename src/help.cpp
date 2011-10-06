#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "help.h"
#include "reciteword.h"
#include <glib/gi18n.h>
#include "skin.h"
#include <cstdio>
#include <sys/stat.h>
#include <gdk/gdkkeysyms.h>


extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;


static void
on_exit_button_clicked (gpointer data)
{
	g_pReciteWord->help_window->close ();
}

static gboolean
on_window_key_press(GtkWidget * window, GdkEventKey * event ,CHelp * parent)
{
	switch (event->keyval)
	{
		case GDK_Return:
		case GDK_Escape:
			parent->exit_button.do_clicked();
			break;
	}
	return true;
}

static gint
on_window_delete_event (GtkWidget * window,GdkEvent *event, CHelp * parent)
{
	parent->close();
	return true;
}

static void
on_help_wordlistadjust_valuechanged (GtkAdjustment * adj,
				      CHelp * parent)
{
	int newvalue = ((gint) adj->value);
	gtk_fixed_move(GTK_FIXED(parent->text_area),parent->text_label,0,-newvalue);
}

static gboolean
on_text_area_scroll (GtkWidget * window, GdkEventScroll * event ,CHelp * parent)
{
	GtkAdjustment *adj = (GtkAdjustment *)parent->wordlist_adjust;
	gdouble new_value = adj->value + ((event->direction == GDK_SCROLL_UP ||
                                         event->direction == GDK_SCROLL_LEFT) ? 
					-adj->page_increment / 2: 
					adj->page_increment / 2);
	if (new_value<0)
		new_value=0;
	else if (new_value>(adj->upper-adj->page_size))
		new_value = (adj->upper-adj->page_size);
	if (new_value!=adj->value)
	{
		adj->value = new_value;
		g_signal_emit_by_name (parent->wordlist_adjust, "changed");
		on_help_wordlistadjust_valuechanged(adj,parent);
	}
	return true;
}

static void
on_help_robutton_selected(const gpointer data)
{
	gchar *filename = g_build_filename(reciteword_data_dir, "modules", (gchar *)data, NULL);
	FILE *file;
	gchar *buffer;
	struct stat stats;
	if (stat (filename, &stats))
	{
		buffer = g_strdup_printf(_("File \"%s\" not found!"),filename);
	}
	else // file exists
	{
		if (!(file = fopen (filename, "rb")))
		{
			buffer = g_strdup_printf(_("Open file \"%s\" failed!"),filename);
		}
		else
		{
			buffer = (gchar *) g_malloc (stats.st_size + 1);
			fread (buffer, 1, stats.st_size, file);
			fclose (file);
			buffer[stats.st_size] = '\0';
		}
	}
	g_pReciteWord->help_window->set_text(buffer);
	g_free(buffer);
	g_free(filename);
}

CHelp::CHelp ()
{
	skin_load_help (g_pReciteWord->window);
}

CHelp::~CHelp ()
{
}

void
CHelp::init ()
{
	g_pReciteWord->help_button.set_enable (FALSE);
}

void
CHelp::show ()
{
	init ();

	showing = TRUE;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (window, Skin->help.help.w, Skin->help.help.h);
	gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
	gtk_widget_set_app_paintable(window,TRUE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), _("Help"));
	gtk_window_set_transient_for (GTK_WINDOW (window),
				      GTK_WINDOW (g_pReciteWord->window));
	gtk_widget_realize (window);

	fixed = gtk_fixed_new ();
	//gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_container_add (GTK_CONTAINER (window), fixed);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->help.help.p[0]);

	overview_robutton.create (NULL, fixed, Skin->help.overview_robutton.x[0], Skin->help.overview_robutton.y[0],
			      Skin->help.overview_robutton.p[0], NULL, Skin->help.overview_robutton.p[1], NULL, on_help_robutton_selected,(gpointer) _("overview.txt"));
	firstrecite_robutton.create (overview_robutton.group, fixed, Skin->help.firstrecite_robutton.x[0], Skin->help.firstrecite_robutton.y[0],
			      Skin->help.firstrecite_robutton.p[0], NULL, Skin->help.firstrecite_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("firstrecite.txt"));
	revise_robutton.create (firstrecite_robutton.group, fixed, Skin->help.revise_robutton.x[0], Skin->help.revise_robutton.y[0],
			      Skin->help.revise_robutton.p[0], NULL, Skin->help.revise_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("revise.txt"));
	shooting_robutton.create (revise_robutton.group, fixed, Skin->help.shooting_robutton.x[0], Skin->help.shooting_robutton.y[0],
			      Skin->help.shooting_robutton.p[0], NULL, Skin->help.shooting_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("shooting.txt"));
	hearing_robutton.create (shooting_robutton.group, fixed, Skin->help.hearing_robutton.x[0], Skin->help.hearing_robutton.y[0],
			      Skin->help.hearing_robutton.p[0], NULL, Skin->help.hearing_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("hearing.txt"));
	typing_robutton.create (hearing_robutton.group, fixed, Skin->help.typing_robutton.x[0], Skin->help.typing_robutton.y[0],
			      Skin->help.typing_robutton.p[0], NULL, Skin->help.typing_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("typing.txt"));
	rest_robutton.create (typing_robutton.group, fixed, Skin->help.rest_robutton.x[0], Skin->help.rest_robutton.y[0],
			      Skin->help.rest_robutton.p[0], NULL, Skin->help.rest_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("rest.txt"));
	choosebook_robutton.create (rest_robutton.group, fixed, Skin->help.choosebook_robutton.x[0], Skin->help.choosebook_robutton.y[0],
			      Skin->help.choosebook_robutton.p[0], NULL, Skin->help.choosebook_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("choosebook.txt"));
	filling_robutton.create (choosebook_robutton.group, fixed, Skin->help.filling_robutton.x[0], Skin->help.filling_robutton.y[0],
			      Skin->help.filling_robutton.p[0], NULL, Skin->help.filling_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("filling.txt"));
	know_robutton.create (filling_robutton.group, fixed, Skin->help.know_robutton.x[0], Skin->help.know_robutton.y[0],
			      Skin->help.know_robutton.p[0], NULL, Skin->help.know_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("know.txt"));
	wordedit_robutton.create (know_robutton.group, fixed, Skin->help.wordedit_robutton.x[0], Skin->help.wordedit_robutton.y[0],
			      Skin->help.wordedit_robutton.p[0], NULL, Skin->help.wordedit_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("wordedit.txt"));
	bookedit_robutton.create (wordedit_robutton.group, fixed, Skin->help.bookedit_robutton.x[0], Skin->help.bookedit_robutton.y[0],
			      Skin->help.bookedit_robutton.p[0], NULL, Skin->help.bookedit_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("bookedit.txt"));
	sentence_robutton.create (bookedit_robutton.group, fixed, Skin->help.sentence_robutton.x[0], Skin->help.sentence_robutton.y[0],
			      Skin->help.sentence_robutton.p[0], NULL, Skin->help.sentence_robutton.p[1], NULL, on_help_robutton_selected,(gpointer)_("sentence.txt"));
				  

	exit_button.create (fixed, Skin->help.exit_button.x[0],
			    Skin->help.exit_button.y[0],
			    Skin->help.exit_button.p[0],
			    Skin->help.exit_button.p[1], NULL,
			    on_exit_button_clicked, NULL);
	
	text_area = gtk_fixed_new();
	gtk_fixed_set_has_window(GTK_FIXED(text_area),TRUE);
	gtk_widget_set_events(text_area,GDK_SCROLL_MASK);
	gtk_widget_set_size_request (text_area, Skin->help.text_area.w, Skin->help.text_area.h);
	gtk_fixed_put(GTK_FIXED(fixed),text_area,Skin->help.text_area.x[0],Skin->help.text_area.y[0]);
	gtk_widget_show (text_area);
	SetBackPixmap (text_area, Skin->help.text_area.p[0]);
	g_signal_connect (G_OBJECT (text_area), "scroll_event",
			  G_CALLBACK (on_text_area_scroll), this);
	text_label = gtk_label_new("");
	gtk_widget_set_name(text_label,"help-text_label");
	gtk_widget_set_size_request (text_label, Skin->help.text_area.w, -1);
	gtk_label_set_line_wrap (GTK_LABEL (text_label),true);
	gtk_label_set_selectable(GTK_LABEL (text_label),true);
	gtk_fixed_put(GTK_FIXED(text_area),text_label,0,0);
	gtk_widget_show(text_label);
	
	wordlist_adjust = gtk_adjustment_new (0, 0, 0, 1, Skin->help.text_area.h, Skin->help.text_area.h);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK (on_help_wordlistadjust_valuechanged),
			  this);
	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll, Skin->help.wordlist_vscroll.w,
			      Skin->help.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (fixed), wordlist_vscroll,
		       Skin->help.wordlist_vscroll.x[0],
		       Skin->help.wordlist_vscroll.y[0]);

	g_signal_connect (G_OBJECT (window), "delete_event",
			  G_CALLBACK (on_window_delete_event), this);
	g_signal_connect (G_OBJECT (window), "key_press_event",
			  G_CALLBACK (on_window_key_press), this);
			  
	switch (g_pReciteWord->status)
	{
		case STATUS_NORMAL:
			overview_robutton.do_clicked();
			break;
		case STATUS_CHOOSEBOOK:
			choosebook_robutton.do_clicked();
			break;
		case STATUS_FIRSTRECITE_GROUP:
		case STATUS_FIRSTRECITE_SKIM:
		case STATUS_FIRSTRECITE_TEST:
			firstrecite_robutton.do_clicked();
			break;
		case STATUS_REVISE_GROUP:
		case STATUS_REVISE_TEST:
		case STATUS_REVISE_SKIM:
			revise_robutton.do_clicked();
			break;
		case STATUS_REST:
			rest_robutton.do_clicked();
			break;
		case STATUS_KNOW:
			know_robutton.do_clicked();
			break;
		case STATUS_SHOOTING:
			shooting_robutton.do_clicked();
			break;
		case STATUS_TYPING:
			typing_robutton.do_clicked();
			break;
		default:
			overview_robutton.do_clicked();
			break;
	}
	playsound(SND_WIND);		
	gtk_widget_show (window);
}

void
CHelp::set_text (gchar *text)
{
	gtk_label_set_text(GTK_LABEL(text_label),text);
	gint h;
	pango_layout_get_pixel_size(gtk_label_get_layout(GTK_LABEL(text_label)),NULL,&h);	
	((GtkAdjustment *) wordlist_adjust)->value = 0;
	((GtkAdjustment *) wordlist_adjust)->upper = h;
	g_signal_emit_by_name (wordlist_adjust, "changed");
	on_help_wordlistadjust_valuechanged(((GtkAdjustment *) wordlist_adjust),this);
}

void
CHelp::close ()
{
	playsound(SND_WIND);
	overview_robutton.destroy();
	firstrecite_robutton.destroy();
	revise_robutton.destroy();
	shooting_robutton.destroy();
	hearing_robutton.destroy();
	typing_robutton.destroy();
	rest_robutton.destroy();
	choosebook_robutton.destroy();
	filling_robutton.destroy();
	know_robutton.destroy();
	wordedit_robutton.destroy();
	bookedit_robutton.destroy();
	sentence_robutton.destroy();
	
	exit_button.destroy();
	gtk_widget_destroy (window);
	
	showing = FALSE;
	g_pReciteWord->help_button.set_enable (TRUE);
}
