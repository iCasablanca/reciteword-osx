#include "reciteword.h"
#include "menu.h"
#include "skin.h"

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;


static gboolean
on_menu_leave_notify (GtkWidget * widget, GdkEventCrossing * event, CMenu * parent)
{
	//GDK_NOTIFY_ANCESTOR enter main window or "button press" on the menu.
	//GDK_NOTIFY_VIRTUAL enter mainwindow from the button which inside the menu or "button press" on the button which inside menu.
	//GDK_NOTIFY_INFERIOR  enter button which inside the menu
	//GDK_NOTIFY_NONLINEAR leave menu and outside the window,or leave menu,into widget which in main window
	//GDK_NOTIFY_NONLINEAR_VIRTUAL leave menu and outside the window from the button which inside the menu,or leave menu,into widget which in main window
	if (event->mode==GDK_CROSSING_NORMAL) //event happen because mouse move,not because button press.
	{
		if (((event->detail==GDK_NOTIFY_ANCESTOR) || (event->detail==GDK_NOTIFY_VIRTUAL))
			||(((event->detail==GDK_NOTIFY_NONLINEAR) || (event->detail==GDK_NOTIFY_NONLINEAR_VIRTUAL))&&(event->x>0))
		)
			parent->close ();
	}
	return true;
}

static void on_menu_choosebook_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowChoosebook ();
}

static void on_menu_firstrecite_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowFirstRecite_group ();
}

static void on_menu_revise_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowRevise_group ();
}

static void on_menu_rest_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowRest ();
}

static void on_menu_know_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowKnow ();
}

static void on_menu_shooting_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowShooting ();
}

static void on_menu_typing_clicked ()
{
	g_pReciteWord->menu->close ();
	g_pReciteWord->clean ();
	g_pReciteWord->ShowTyping ();
}

static void on_menu_exit_clicked (gpointer data)
{
	g_pReciteWord->close ();
}

CMenu::CMenu ()
{
	fixed = NULL;
}

CMenu::~CMenu ()
{

}

void CMenu::close ()
{
	if (!fixed)
		return;
	make_continue();
	choosebook_button.destroy();
	firstrecite_button.destroy();
	revise_button.destroy();
	rest_button.destroy();
	know_button.destroy();
	shooting_button.destroy();
	typing_button.destroy();
	exit_button.destroy();	
	
	gtk_widget_destroy (fixed);
	fixed = NULL;
}

void CMenu::show ()
{

	if (fixed)
		return;
	playsound (SND_MENUSHOW);
	make_pause();
	
	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_events(fixed,GDK_LEAVE_NOTIFY_MASK);
	gtk_widget_set_size_request (fixed, Skin->face.menu.w, Skin->face.menu.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,Skin->face.menu.x[0],Skin->face.menu.y[0]);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->face.menu.p[0]);

	choosebook_button.create (fixed, Skin->face.choosebook_button.x[0],
				   Skin->face.choosebook_button.y[0],
				   Skin->face.choosebook_button.p[0],
				   Skin->face.choosebook_button.p[1],
				   Skin->face.choosebook_button.p[2],
				   Skin->face.choosebook_button.p[3],
				   on_menu_choosebook_clicked);
	firstrecite_button.create (fixed, Skin->face.firstrecite_button.x[0],
				   Skin->face.firstrecite_button.y[0],
				   Skin->face.firstrecite_button.p[0],
				   Skin->face.firstrecite_button.p[1],
				   Skin->face.firstrecite_button.p[2],
				   Skin->face.firstrecite_button.p[3],
				   on_menu_firstrecite_clicked);
	revise_button.create (fixed, Skin->face.revise_button.x[0],
			      Skin->face.revise_button.y[0],
			      Skin->face.revise_button.p[0],
			      Skin->face.revise_button.p[1],
			      Skin->face.revise_button.p[2],
			      Skin->face.revise_button.p[3],
			      on_menu_revise_clicked);
	rest_button.create (fixed, Skin->face.rest_button.x[0],
			      Skin->face.rest_button.y[0],
			      Skin->face.rest_button.p[0],
			      Skin->face.rest_button.p[1],
			      Skin->face.rest_button.p[2],
			      Skin->face.rest_button.p[3],
			      on_menu_rest_clicked);
	know_button.create (fixed, Skin->face.know_button.x[0],
			      Skin->face.know_button.y[0],
			      Skin->face.know_button.p[0],
			      Skin->face.know_button.p[1],
			      Skin->face.know_button.p[2],
			      Skin->face.know_button.p[3],
			      on_menu_know_clicked);
	shooting_button.create (fixed, Skin->face.shooting_button.x[0],
				Skin->face.shooting_button.y[0],
				Skin->face.shooting_button.p[0],
				Skin->face.shooting_button.p[1],
				Skin->face.shooting_button.p[2],
				Skin->face.shooting_button.p[3],
				on_menu_shooting_clicked);
	typing_button.create (fixed, Skin->face.typing_button.x[0],
			      Skin->face.typing_button.y[0],
			      Skin->face.typing_button.p[0],
			      Skin->face.typing_button.p[1],
			      Skin->face.typing_button.p[2],
			      Skin->face.typing_button.p[3],
			      on_menu_typing_clicked);

	exit_button.create (fixed, Skin->face.menu_exit_button.x[0],
			    Skin->face.menu_exit_button.y[0],
			    Skin->face.menu_exit_button.p[0],
			    Skin->face.menu_exit_button.p[1], (GdkPixmap *) NULL,
			    on_menu_exit_clicked, NULL);

	if (g_pReciteWord->now_book)
	{
		switch (g_pReciteWord->status)
		{
		case STATUS_NORMAL:
			break;
		case STATUS_CHOOSEBOOK:
			choosebook_button.set_enable (FALSE);
			break;
		case STATUS_FIRSTRECITE_GROUP:
		case STATUS_FIRSTRECITE_SKIM:
		case STATUS_FIRSTRECITE_TEST:
			firstrecite_button.set_enable (FALSE);
			break;
		case STATUS_REVISE_GROUP:
		case STATUS_REVISE_TEST:
		case STATUS_REVISE_SKIM:
			revise_button.set_enable (FALSE);
			break;
		case STATUS_REST:
			rest_button.set_enable (FALSE);
			break;
		case STATUS_KNOW:
			know_button.set_enable (FALSE);
			break;
		case STATUS_SHOOTING:
			shooting_button.set_enable (FALSE);
			break;
		case STATUS_TYPING:
			typing_button.set_enable (FALSE);
			break;
		default:
			break;
		}
	}
	else
	{
		if (g_pReciteWord->status==STATUS_CHOOSEBOOK)
			choosebook_button.set_enable (FALSE);
		firstrecite_button.set_enable (FALSE);
		revise_button.set_enable (FALSE);
		rest_button.set_enable (FALSE);
		know_button.set_enable (FALSE);
		shooting_button.set_enable (FALSE);
		typing_button.set_enable (FALSE);
	}
	g_signal_connect (G_OBJECT (fixed), "leave_notify_event",
			  G_CALLBACK (on_menu_leave_notify), this);
}

void CMenu::make_pause ()
{
	re_continue=false;
	switch (g_pReciteWord->status)
	{
	case STATUS_FIRSTRECITE_SKIM:
		if (g_pReciteWord->firstrecite_skim->started)
		{
			g_pReciteWord->firstrecite_skim->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	case STATUS_FIRSTRECITE_TEST:
		if (g_pReciteWord->firstrecite_test->started)
		{
			g_pReciteWord->firstrecite_test->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	case STATUS_REVISE_TEST:
		if (g_pReciteWord->revise_test->started)
		{
			g_pReciteWord->revise_test->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	case STATUS_REVISE_SKIM:
		if (g_pReciteWord->revise_skim->started)
		{
			g_pReciteWord->revise_skim->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	case STATUS_SHOOTING:
		if (g_pReciteWord->shooting->started)
		{
			g_pReciteWord->shooting->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	case STATUS_TYPING:
		if (g_pReciteWord->typing->started)
		{
			g_pReciteWord->typing->pause_button.do_clicked (false);
			re_continue=true;
		}
		break;
	default:
		break;
	}
}

void CMenu::make_continue ()
{
	if (!re_continue)
		return;
	switch (g_pReciteWord->status)
	{
	case STATUS_FIRSTRECITE_SKIM:
			g_pReciteWord->firstrecite_skim->pause_button.do_clicked (false);
		break;
	case STATUS_FIRSTRECITE_TEST:
			g_pReciteWord->firstrecite_test->pause_button.do_clicked (false);
		break;
	case STATUS_REVISE_TEST:
			g_pReciteWord->revise_test->pause_button.do_clicked (false);
		break;
	case STATUS_REVISE_SKIM:
			g_pReciteWord->revise_skim->pause_button.do_clicked (false);
		break;
	case STATUS_SHOOTING:
			g_pReciteWord->shooting->pause_button.do_clicked (false);
		break;
	case STATUS_TYPING:
			g_pReciteWord->typing->pause_button.do_clicked (false);
		break;
	default:
		break;
	}
}
