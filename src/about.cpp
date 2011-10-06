#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "about.h"
#include "reciteword.h"
#include <glib/gi18n.h>
#include "skin.h"
#include <gdk/gdkkeysyms.h>

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;

static void
on_exit_button_clicked (gpointer data)
{
	g_pReciteWord->about_window->close ();
}

static gboolean
on_window_key_press(GtkWidget * window, GdkEventKey * event ,tAbout * parent)
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
on_window_delete_event (GtkWidget * window, GdkEvent * event ,tAbout * parent)
{
	parent->close();
	return true;
}

tAbout::tAbout ()
{
	skin_load_about (g_pReciteWord->window);
}

tAbout::~tAbout ()
{
}

void
tAbout::init ()
{
	g_pReciteWord->about_button.set_enable (FALSE);
}

void
tAbout::show ()
{
	init ();
	playsound(SND_WIND);
	
	GtkWidget *fixed;
	showing = TRUE;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (window, Skin->about.about.w, Skin->about.about.h);
	gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
	gtk_widget_set_app_paintable(window,TRUE);
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_title (GTK_WINDOW (window), _("About"));
	gtk_window_set_transient_for (GTK_WINDOW (window),
				      GTK_WINDOW (g_pReciteWord->window));
	gtk_widget_realize(window);
	fixed = gtk_fixed_new ();
	//gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_container_add (GTK_CONTAINER (window), fixed);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->about.about.p[0]);

	exit_button.create (fixed, Skin->about.exit_button.x[0],
			    Skin->about.exit_button.y[0],
			    Skin->about.exit_button.p[0],
			    Skin->about.exit_button.p[1], NULL,
			    on_exit_button_clicked, NULL);

	g_signal_connect (G_OBJECT (window), "delete_event",
			  G_CALLBACK (on_window_delete_event), this);
	g_signal_connect (G_OBJECT (window), "key_press_event",
			  G_CALLBACK (on_window_key_press), this);
	gtk_widget_show (window);
}

void
tAbout::close ()
{
	playsound(SND_WIND);
	exit_button.destroy();
	gtk_widget_destroy (window);

	showing = FALSE;
	g_pReciteWord->about_button.set_enable (TRUE);
}
