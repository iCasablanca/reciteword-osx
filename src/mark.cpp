#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "mark.h"
#include <glib/gi18n.h>
#include "reciteword.h"
#include "skin.h"
#include "configfile.h"

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;

void
on_mark_ok_clicked ()
{
	g_pReciteWord->mark->close ();
}

CMark::CMark ()
{
	skin_load_mark (g_pReciteWord->window);
}

CMark::~CMark ()
{
}

void
CMark::init ()
{
}

void
CMark::show ()
{
	init ();

	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_size_request (fixed, Skin->mark.mark.w,
			      Skin->mark.mark.h);
	if (g_pReciteWord->status==STATUS_REVISE_TEST)
	{
		gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed, Skin->mark.mark.x[0], Skin->mark.mark.y[0]);
	}
	else
	{
		gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,
		       (Skin->reciteword.width - Skin->mark.mark.w) / 2,
		       (Skin->reciteword.height -
			Skin->mark.mark.h) / 2);
	}
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->mark.mark.p[0]);

	ok_button.create (fixed, Skin->mark.ok_button.x[0],
			  Skin->mark.ok_button.y[0],
			  Skin->mark.ok_button.p[0],
			  Skin->mark.ok_button.p[1],
			  Skin->mark.ok_button.p[2],
			  NULL, on_mark_ok_clicked);
	gchar tmpstr[256];
	if (g_pReciteWord->status==STATUS_REVISE_TEST)
	{
		gint wrongcount=0;
		for (int i = 0;i < g_pReciteWord->now_zu_wordcount; i++)
		{
			if (g_pReciteWord->revise_test->tested_wordlist_item_status[i] == S_right_2)
			{
				wrongcount++;
			}
		}		
		gint score=(((g_pReciteWord->now_zu_wordcount-wrongcount)*100)/g_pReciteWord->now_zu_wordcount);
		sprintf(tmpstr,"%2d",score);

		mark_label = gtk_label_new (tmpstr);
		gtk_widget_set_name (mark_label, "mark-mark_label");
		gtk_fixed_put (GTK_FIXED (fixed), mark_label,
		       Skin->mark.revise_test_mark_label.x[0],
		       Skin->mark.revise_test_mark_label.y[0]);
		gtk_widget_show (mark_label);
		sprintf(tmpstr,_("%d minutes and %d seconds"),(g_pReciteWord->revise_test->timecount / 60),(g_pReciteWord->revise_test->timecount % 60));
		info_label = gtk_label_new (tmpstr);
		gtk_fixed_put (GTK_FIXED (fixed), info_label,
		       Skin->mark.revise_test_time_label.x[0],
		       Skin->mark.revise_test_time_label.y[0]);
		gtk_widget_show (info_label);
	}
	else if (g_pReciteWord->status==STATUS_TYPING)
	{
		typing_pixmap = gtk_image_new_from_pixmap (Skin->mark.typing_pixmap.p[0], NULL);
		gtk_fixed_put (GTK_FIXED (fixed), typing_pixmap,
		       Skin->mark.typing_pixmap.x[0],
		       Skin->mark.typing_pixmap.y[0]);
		gtk_widget_show(typing_pixmap);
		
		sprintf(tmpstr,"%3d",g_pReciteWord->typing->speed_value);
		mark_label = gtk_label_new (tmpstr);
		gtk_widget_set_name (mark_label, "mark-mark_label");
		gtk_fixed_put (GTK_FIXED (fixed), mark_label,
		       Skin->mark.typing_mark_label.x[0],
		       Skin->mark.typing_mark_label.y[0]);
		gtk_widget_show (mark_label);
	}

}

void
CMark::close ()
{
	ok_button.destroy ();

	gtk_widget_destroy (fixed);
	fixed = NULL;
}
