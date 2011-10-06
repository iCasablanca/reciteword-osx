#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "record.h"
#include "reciteword.h"
#include "rwconfig.h"
#include <glib/gi18n.h>
#include "skin.h"
#include <gdk/gdkkeysyms.h>


extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;


static void
on_exit_button_clicked (gpointer data)
{
	g_pReciteWord->record_window->close ();
}

static gboolean
on_window_key_press(GtkWidget * window, GdkEventKey * event ,CRecord * parent)
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
on_window_delete_event (GtkWidget * window,GdkEvent *event, CRecord * parent)
{
	parent->close();
	return true;
}

CRecord::CRecord ()
{
	skin_load_record (g_pReciteWord->window);
}

CRecord::~CRecord ()
{
}

void
CRecord::init ()
{
	g_pReciteWord->record_button.set_enable (FALSE);
}

void
CRecord::show ()
{
	init ();
	playsound(SND_WIND);

	showing = TRUE;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (window, Skin->record.record.w, Skin->record.record.h);
	gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
	gtk_widget_set_app_paintable(window,TRUE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), _("Record"));
	gtk_window_set_transient_for (GTK_WINDOW (window),
				      GTK_WINDOW (g_pReciteWord->window));
	gtk_widget_realize (window);

	fixed = gtk_fixed_new ();
	//gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_container_add (GTK_CONTAINER (window), fixed);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->record.record.p[0]);
				  

	exit_button.create (fixed, Skin->record.exit_button.x[0],
			    Skin->record.exit_button.y[0],
			    Skin->record.exit_button.p[0],
			    Skin->record.exit_button.p[1], NULL,
			    on_exit_button_clicked, NULL);

	glong firstrecite_totalwordcount,revise_test_totalwordcount;
	rw_cfg_read_long(usercfgfile,"firstrecite_test","totalwordcount",&firstrecite_totalwordcount);
	rw_cfg_read_long(usercfgfile,"revise_test","totalwordcount",&revise_test_totalwordcount);
	gchar *text;
	text=g_strdup_printf(_("You had firsterecited %ld words,revised %ld words."),firstrecite_totalwordcount,revise_test_totalwordcount);
	text_label=gtk_label_new(text);
	g_free(text);
	gtk_widget_set_name (text_label, "record-text_label");
	gtk_label_set_line_wrap (GTK_LABEL (text_label), TRUE);
	gtk_widget_set_size_request (text_label,
			      Skin->record.text_label.w,
			      Skin->record.text_label.h);
	gtk_fixed_put (GTK_FIXED (fixed), text_label,
		       Skin->record.text_label.x[0],
		       Skin->record.text_label.y[0]);
	gtk_widget_show(text_label);

	g_signal_connect (G_OBJECT (window), "delete_event",
			  G_CALLBACK (on_window_delete_event), this);
	g_signal_connect (G_OBJECT (window), "key_press_event",
			  G_CALLBACK (on_window_key_press), this);
			  		
	gtk_widget_show (window);
}

void
CRecord::close ()
{
	playsound(SND_WIND);
	exit_button.destroy();
	gtk_widget_destroy (text_label);
	gtk_widget_destroy (window);
	
	showing = FALSE;
	g_pReciteWord->record_button.set_enable (TRUE);
}
