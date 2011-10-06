#include "firstrecite.h"
#include "reciteword.h"
#include "skin.h"
#include "sound.h"
#include "readword.h"
#include "rwconfig.h"
#include <cstdio>
#include <cstdlib>


extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;


static void
on_group_ok_clicked (gpointer data)
{
	if (g_pReciteWord->firstrecite_group->zu_wordcount < 6)
		return;
	gchar tmpstr[256];

	if (g_pReciteWord->firstrecite_group->zu_index <
	    g_pReciteWord->max_zu_index)
	{
		gint tmpint, i;

		i = g_pReciteWord->firstrecite_group->zu_index;
		while (i >= 0)
		{
			sprintf (tmpstr, "zu%d_wordcount", i);
			if (rw_cfg_read_int
			    (usercfgfile,
			     g_pReciteWord->now_book->bookfilename, tmpstr,
			     &tmpint))
				break;
			i--;
		}
		if (tmpint == 0)
			tmpint = 25;
		if (g_pReciteWord->firstrecite_group->zu_wordcount != tmpint)
		{
#ifdef G_OS_WIN32
#else
			g_print ("it is better not change old record\n");
#endif

			//ask,if sure then clean the afterward's record
			return; //it is better not change old record.
		}
	}
	sprintf (tmpstr, "zu%d_wordcount",
		 g_pReciteWord->firstrecite_group->zu_index);
	rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename,
			  tmpstr,
			  g_pReciteWord->firstrecite_group->zu_wordcount);

	if (g_pReciteWord->firstrecite_group->zu_index >
	    g_pReciteWord->max_zu_index)
		g_pReciteWord->max_zu_index =
			g_pReciteWord->firstrecite_group->zu_index;

	g_pReciteWord->set_now_zu_words(g_pReciteWord->firstrecite_group->zu_index,
					g_pReciteWord->firstrecite_group->zu_wordcount,
					g_pReciteWord->firstrecite_group->zu_words.w,
					g_pReciteWord->firstrecite_group->zu_words.t,
					g_pReciteWord->firstrecite_group->zu_words.m,
					g_pReciteWord->firstrecite_group->order_ckbutton.status);
	
	g_pReciteWord->firstrecite_group->close ();
	g_pReciteWord->ShowFirstRecite_skim ();
}

static void
on_group_return_clicked ()
{
	g_pReciteWord->firstrecite_group->close ();
	g_pReciteWord->show ();
}
static void
on_group_zuentry_activate (GtkWidget * widget, CFirstRecite_group * parent)
{
	on_group_ok_clicked (NULL);
}

static void
on_group_changed (CFirstRecite_group * parent, gint zu_index,
		  gint zu_wordcount)
{
	if (g_pReciteWord->now_book->wordcount < 6)
		return;
	gchar tmpstr[256];
	gint i, total_zucount = 0, max_wordcount = 0, last_zu_wordcount =
		25, tmpint, now_zu_wordcount = 0, now_total_wordcount = 0;

	for (i = 0; i <= g_pReciteWord->max_zu_index; i++)
	{
		sprintf (tmpstr, "zu%d_wordcount", i);
		if (rw_cfg_read_int
		    (usercfgfile, g_pReciteWord->now_book->bookfilename,
		     tmpstr, &tmpint))
			last_zu_wordcount = tmpint;
		if (i == zu_index)
		{
			now_total_wordcount = max_wordcount;
			now_zu_wordcount = last_zu_wordcount;
		}
		max_wordcount += last_zu_wordcount;
	}
	max_wordcount -= last_zu_wordcount;

	if (zu_index > g_pReciteWord->max_zu_index)
	{
		now_total_wordcount =
			max_wordcount + (zu_index -
					 g_pReciteWord->max_zu_index) *
			last_zu_wordcount;
		now_zu_wordcount = last_zu_wordcount;
		total_zucount = zu_index;
	}
	else
		total_zucount = g_pReciteWord->max_zu_index;

	if (zu_wordcount)
	{
		last_zu_wordcount = zu_wordcount;
		now_zu_wordcount = zu_wordcount;
	}

	if (zu_index > g_pReciteWord->max_zu_index)
	{
		if (g_pReciteWord->now_book->wordcount <= now_total_wordcount)
		{
			parent->zu_wordcount = 0;
			return;
		}
		total_zucount +=
			((g_pReciteWord->now_book->wordcount -
			  now_total_wordcount) / last_zu_wordcount);
		tmpint = (g_pReciteWord->now_book->wordcount -
			  now_total_wordcount) % last_zu_wordcount;
	}
	else
	{
		total_zucount +=
			((g_pReciteWord->now_book->wordcount -
			  max_wordcount) / last_zu_wordcount);
		tmpint = (g_pReciteWord->now_book->wordcount -
			  max_wordcount) % last_zu_wordcount;
	}

	if (tmpint!=0)
	{
		if (tmpint>=6)
		{
			total_zucount++;
			if (zu_index == total_zucount - 1)  // if is the last group
				now_zu_wordcount = tmpint;
		}
		else
		{
			if (zu_index == total_zucount - 1)
				now_zu_wordcount = last_zu_wordcount + tmpint;  // one word group at least have 6 words.
		}
	}
	sprintf (tmpstr, "1-%d", total_zucount);
	gtk_label_set_text (GTK_LABEL (parent->zu_label), tmpstr);
	sprintf (tmpstr, "%d", now_zu_wordcount);
	gtk_label_set_text (GTK_LABEL (parent->zu_wordcount_label), tmpstr);

	if ((zu_index >= 0) && (zu_index < total_zucount))
	{
		parent->zu_index = zu_index;
		parent->total_wordcount = now_total_wordcount;
		parent->zu_wordcount = now_zu_wordcount;
		parent->zu_words.w =
			g_pReciteWord->now_book->words.w +
			now_total_wordcount;
		parent->zu_words.t =
			g_pReciteWord->now_book->words.t +
			now_total_wordcount;
		parent->zu_words.m =
			g_pReciteWord->now_book->words.m +
			now_total_wordcount;
		((GtkAdjustment *) g_pReciteWord->firstrecite_group->
		 wordlist_adjust)->value = 0;
		((GtkAdjustment *) g_pReciteWord->firstrecite_group->
		 wordlist_adjust)->upper = now_zu_wordcount;
		g_signal_emit_by_name (G_OBJECT
				       (g_pReciteWord->firstrecite_group->
					wordlist_adjust), "changed");
		parent->set_wordlist_label (0);
	}

}

static void
on_group_zuentry_changed (GtkWidget * widget, CFirstRecite_group * parent)
{
	const gchar *a;
	gint zu_index;

	a = gtk_entry_get_text (GTK_ENTRY (widget));
	zu_index = atoi (a) - 1;

	if (zu_index >= 0)
		on_group_changed (parent, zu_index, 0);
}

static void
on_group_wordlistadjust_valuechanged (GtkAdjustment * adj,
				      CFirstRecite_group * parent)
{
	int newvalue = ((gint) adj->value);

	parent->set_wordlist_label (newvalue);
}

static void
on_group_cancel_clicked (gpointer data)
{
	g_pReciteWord->firstrecite_group->close ();
	g_pReciteWord->show ();
}

static void
on_group_add_clicked (gpointer data)
{
	CFirstRecite_group *parent = g_pReciteWord->firstrecite_group;

	if (parent->total_wordcount + parent->zu_wordcount <
	    g_pReciteWord->now_book->wordcount)
	{
		const gchar *a;
		gint zu_index;

		a = gtk_entry_get_text (GTK_ENTRY (parent->zu_entry));
		zu_index = atoi (a) - 1;
		if (zu_index >= 0)
			on_group_changed (parent, zu_index,
					  parent->zu_wordcount + 1);
	}
}

static void
on_group_reduce_clicked (gpointer data)
{
	CFirstRecite_group *parent = g_pReciteWord->firstrecite_group;
	if (parent->zu_wordcount > 6)
	{
		const gchar *a;
		gint zu_index;

		a = gtk_entry_get_text (GTK_ENTRY (parent->zu_entry));
		zu_index = atoi (a) - 1;
		if (zu_index >= 0)
			on_group_changed (parent, zu_index,
					  parent->zu_wordcount - 1);
	}
}

static void
on_group_class_clicked (gpointer data)
{
}

CFirstRecite_group::CFirstRecite_group ()
{
	skin_load_group (g_pReciteWord->window);
	skin_load_text (g_pReciteWord->window);
}

CFirstRecite_group::~CFirstRecite_group ()
{

}

void
CFirstRecite_group::do_up ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue>0)
	{
		nowvalue--;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_group_wordlistadjust_valuechanged((GtkAdjustment *) wordlist_adjust,this);
	}
}

void
CFirstRecite_group::do_down ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue < ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
	{
		nowvalue++;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_group_wordlistadjust_valuechanged((GtkAdjustment *) wordlist_adjust,this);
	}
}

void
CFirstRecite_group::do_pageup ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue> 0)
	{
		nowvalue-= (gint) ((GtkAdjustment *)wordlist_adjust)->page_increment;
		if (nowvalue < 0)
			nowvalue = 0;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_group_wordlistadjust_valuechanged((GtkAdjustment *) wordlist_adjust,this);
	}
}

void
CFirstRecite_group::do_pagedown ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue < ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
	{
		nowvalue+=(gint) ((GtkAdjustment *)wordlist_adjust)->page_increment;
		if (nowvalue > ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
			nowvalue = ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size));
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_group_wordlistadjust_valuechanged((GtkAdjustment *) wordlist_adjust,this);
	}
}

void
CFirstRecite_group::do_scroll(GdkEventScroll * event)
{
	int x, y;
	gtk_window_get_position (GTK_WINDOW(g_pReciteWord->window), &x, &y);
	if ((event->x_root>=x+Skin->group.wordlist_area.x[0])&&(event->x_root<=x+Skin->group.wordlist_area.x[0]+Skin->group.wordlist_area.w)
		&&(event->y_root>=y+Skin->group.wordlist_area.y[0])&&(event->y_root<=y+Skin->group.wordlist_area.y[0]+Skin->group.wordlist_area.h))
	{
		GtkAdjustment *adj = (GtkAdjustment *)wordlist_adjust;
		gdouble new_value = adj->value + ((event->direction == GDK_SCROLL_UP ||
                                         event->direction == GDK_SCROLL_LEFT) ? 
					-adj->page_increment / 2: 
					adj->page_increment / 2);
		if (new_value>(adj->upper - adj->page_size)) // warning,adj->upper may ==0,this time shouldn't be scrolled.
			new_value = (adj->upper - adj->page_size);
		if (new_value<0)
			new_value=0;
		if (new_value!=adj->value)
		{
			adj->value = new_value;
			g_signal_emit_by_name (wordlist_adjust, "changed");
			on_group_wordlistadjust_valuechanged(adj,this);
		}
	}
}

void
CFirstRecite_group::init ()
{
	g_pReciteWord->status = STATUS_FIRSTRECITE_GROUP;
	zu_wordcount = 0;
	zu_index = 0;
	total_wordcount = 0;
}

void
CFirstRecite_group::show ()
{
	init ();


	gchar tmpstr[256];


	SetBackPixmap (g_pReciteWord->fixed, Skin->group.group.p[0]);
	ok_button.create (g_pReciteWord->fixed, Skin->group.ok_button.x[0],
			  Skin->group.ok_button.y[0], Skin->group.ok_button.p[0],
			  Skin->group.ok_button.p[1], Skin->group.ok_button.p[2],
			  on_group_ok_clicked, NULL);
	cancel_button.create (g_pReciteWord->fixed,
			      Skin->group.cancel_button.x[0],
			      Skin->group.cancel_button.y[0],
			      Skin->group.cancel_button.p[0],
			      Skin->group.cancel_button.p[1],
			      Skin->group.cancel_button.p[2],
			      on_group_cancel_clicked, NULL);
	add_button.create (g_pReciteWord->fixed, Skin->group.add_button.x[0],
			   Skin->group.add_button.y[0], Skin->group.add_button.p[0],
			   Skin->group.add_button.p[1], Skin->group.add_button.p[2],
			   on_group_add_clicked, NULL);
	reduce_button.create (g_pReciteWord->fixed,
			      Skin->group.reduce_button.x[0],
			      Skin->group.reduce_button.y[0],
			      Skin->group.reduce_button.p[0],
			      Skin->group.reduce_button.p[1],
			      Skin->group.reduce_button.p[2],
			      on_group_reduce_clicked, NULL);
	class_button.create (g_pReciteWord->fixed, Skin->group.class_button.x[0],
			     Skin->group.class_button.y[0],
			     Skin->group.class_button.p[0],
			     Skin->group.class_button.p[1],
			     Skin->group.class_button.p[2],
			     on_group_class_clicked, NULL);
	return_button.create (g_pReciteWord->fixed,
			      Skin->group.return_button.x[0],
			      Skin->group.return_button.y[0],
			      Skin->group.return_button.p[0],
			      Skin->group.return_button.p[1],
			      Skin->group.return_button.p[2], (GdkPixmap *) NULL,
			      on_group_return_clicked);
	order_ckbutton.create (g_pReciteWord->fixed,
			       Skin->group.order_ckbutton.x[0],
			       Skin->group.order_ckbutton.y[0],
			       Skin->group.order_ckbutton.p[0],
			       Skin->group.order_ckbutton.p[1],
			       Skin->group.order_ckbutton.p[2],
			       Skin->group.order_ckbutton.p[3], NULL);
   	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_group", "disorder",
			     &order);
	order_ckbutton.set_status(order);

	book_label = gtk_label_new (g_pReciteWord->now_book->name);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), book_label,
		       Skin->group.book_label.x[0], Skin->group.book_label.y[0]);
	gtk_widget_show (book_label);
	zu_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), zu_label,
		       Skin->group.zu_label.x[0], Skin->group.zu_label.y[0]);
	gtk_widget_show (zu_label);
	zu_wordcount_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), zu_wordcount_label,
		       Skin->group.zu_wordcount_label.x[0],
		       Skin->group.zu_wordcount_label.y[0]);
	gtk_widget_show (zu_wordcount_label);
	wordlist_scrollvalue_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_scrollvalue_label,
		       Skin->group.wordlist_scrollvalue_label.x[0],
		       Skin->group.wordlist_scrollvalue_label.y[0]);
	gtk_widget_show (wordlist_scrollvalue_label);
	sprintf (tmpstr, "%d", g_pReciteWord->now_book->wordcount);
	book_wordcount_label = gtk_label_new (tmpstr);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), book_wordcount_label,
		       Skin->group.book_wordcount_label.x[0],
		       Skin->group.book_wordcount_label.y[0]);
	gtk_widget_show (book_wordcount_label);
	GtkWidget *hbox;

	hbox = gtk_hbox_new (FALSE, 10);
	wordlist_label_1_w = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_1_w),true);
	SetFont(wordlist_label_1_w,false);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_1_w), 0, .5);
	wordlist_label_1_m = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_1_m),true);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_1_m), 0, .5);
	gtk_widget_show (wordlist_label_1_w);
	gtk_widget_show (wordlist_label_1_m);
	gtk_widget_set_size_request (hbox, Skin->group.wordlist_label_1.w,
			      Skin->group.wordlist_label_1.h);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_1_w, FALSE, FALSE,
			    FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_1_m, FALSE, FALSE,
			    FALSE);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), hbox,
		       Skin->group.wordlist_label_1.x[0],
		       Skin->group.wordlist_label_1.y[0]);
	gtk_widget_show (hbox);

	hbox = gtk_hbox_new (FALSE, 10);
	wordlist_label_2_w = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_2_w),true);
	SetFont(wordlist_label_2_w,false);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_2_w), 0, .5);
	wordlist_label_2_m = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_2_m),true);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_2_m), 0, .5);
	gtk_widget_show (wordlist_label_2_w);
	gtk_widget_show (wordlist_label_2_m);
	gtk_widget_set_size_request (hbox, Skin->group.wordlist_label_2.w,
			      Skin->group.wordlist_label_2.h);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_2_w, FALSE, FALSE,
			    FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_2_m, FALSE, FALSE,
			    FALSE);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), hbox,
		       Skin->group.wordlist_label_2.x[0],
		       Skin->group.wordlist_label_2.y[0]);
	gtk_widget_show (hbox);

	hbox = gtk_hbox_new (FALSE, 10);
	wordlist_label_3_w = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_3_w),true);
	SetFont(wordlist_label_3_w,false);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_3_w), 0, .5);
	wordlist_label_3_m = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_3_m),true);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_3_m), 0, .5);
	gtk_widget_show (wordlist_label_3_w);
	gtk_widget_show (wordlist_label_3_m);
	gtk_widget_set_size_request (hbox, Skin->group.wordlist_label_3.w,
			      Skin->group.wordlist_label_3.h);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_3_w, FALSE, FALSE,
			    FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_3_m, FALSE, FALSE,
			    FALSE);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), hbox,
		       Skin->group.wordlist_label_3.x[0],
		       Skin->group.wordlist_label_3.y[0]);
	gtk_widget_show (hbox);

	hbox = gtk_hbox_new (FALSE, 10);
	wordlist_label_4_w = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_4_w),true);
	SetFont(wordlist_label_4_w,false);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_4_w), 0, .5);
	wordlist_label_4_m = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_4_m),true);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_4_m), 0, .5);
	gtk_widget_show (wordlist_label_4_w);
	gtk_widget_show (wordlist_label_4_m);
	gtk_widget_set_size_request (hbox, Skin->group.wordlist_label_4.w,
			      Skin->group.wordlist_label_4.h);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_4_w, FALSE, FALSE,
			    FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_4_m, FALSE, FALSE,
			    FALSE);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), hbox,
		       Skin->group.wordlist_label_4.x[0],
		       Skin->group.wordlist_label_4.y[0]);
	gtk_widget_show (hbox);

	hbox = gtk_hbox_new (FALSE, 10);
	wordlist_label_5_w = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_5_w),true);
	SetFont(wordlist_label_5_w,false);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_5_w), 0, .5);
	wordlist_label_5_m = gtk_label_new ("");
	gtk_label_set_selectable(GTK_LABEL(wordlist_label_5_m),true);
	gtk_misc_set_alignment (GTK_MISC (wordlist_label_5_m), 0, .5);
	gtk_widget_show (wordlist_label_5_w);
	gtk_widget_show (wordlist_label_5_m);
	gtk_widget_set_size_request (hbox, Skin->group.wordlist_label_5.w,
			      Skin->group.wordlist_label_5.h);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_5_w, FALSE, FALSE,
			    FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), wordlist_label_5_m, FALSE, FALSE,
			    FALSE);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), hbox,
		       Skin->group.wordlist_label_5.x[0],
		       Skin->group.wordlist_label_5.y[0]);
	gtk_widget_show (hbox);

	zu_entry = gtk_entry_new ();
	gtk_entry_set_has_frame(GTK_ENTRY(zu_entry),false);
	gtk_widget_set_size_request (zu_entry, Skin->group.zu_entry.w,
			      Skin->group.zu_entry.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), zu_entry,
		       Skin->group.zu_entry.x[0], Skin->group.zu_entry.y[0]);
	g_signal_connect (G_OBJECT (zu_entry), "activate",
			  G_CALLBACK (on_group_zuentry_activate), this);
	g_signal_connect (G_OBJECT (zu_entry), "changed",
			  G_CALLBACK (on_group_zuentry_changed), this);
	gtk_widget_show (zu_entry);
	wordlist_adjust = gtk_adjustment_new (0, 0, 0, 1, 5, 5);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK (on_group_wordlistadjust_valuechanged),
			  this);
	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll, Skin->group.wordlist_vscroll.w,
			      Skin->group.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_vscroll,
		       Skin->group.wordlist_vscroll.x[0],
		       Skin->group.wordlist_vscroll.y[0]);

	sprintf (tmpstr, "%d", g_pReciteWord->next_zu_index + 1);
	gtk_entry_set_text (GTK_ENTRY (zu_entry), tmpstr);
}

void
CFirstRecite_group::set_wordlist_label (int index)
{
	gchar aa[5];
	sprintf(aa,"%2d",index+1);
	gtk_label_set_text (GTK_LABEL (wordlist_scrollvalue_label),
			    aa);

	gtk_label_set_text (GTK_LABEL (wordlist_label_1_w),
			    zu_words.w[0 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_1_m),
			    zu_words.m[0 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_2_w),
			    zu_words.w[1 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_2_m),
			    zu_words.m[1 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_3_w),
			    zu_words.w[2 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_3_m),
			    zu_words.m[2 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_4_w),
			    zu_words.w[3 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_4_m),
			    zu_words.m[3 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_5_w),
			    zu_words.w[4 + index]);
	gtk_label_set_text (GTK_LABEL (wordlist_label_5_m),
			    zu_words.m[4 + index]);
}

void
CFirstRecite_group::close ()
{
	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_group", "disorder",
			     &order);
	if (order!=order_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "firstrecite_group", "disorder",
			     order_ckbutton.status);
	}

	ok_button.destroy ();
	cancel_button.destroy ();
	add_button.destroy ();
	reduce_button.destroy ();
	class_button.destroy ();
	return_button.destroy ();
	order_ckbutton.destroy ();

	gtk_widget_destroy (book_label);
	gtk_widget_destroy (book_wordcount_label);
	gtk_widget_destroy (zu_label);
	gtk_widget_destroy (zu_wordcount_label);
	gtk_widget_destroy (wordlist_scrollvalue_label);
	gtk_widget_destroy (wordlist_label_1_w);
	gtk_widget_destroy (wordlist_label_1_m);
	gtk_widget_destroy (wordlist_label_2_w);
	gtk_widget_destroy (wordlist_label_2_m);
	gtk_widget_destroy (wordlist_label_3_w);
	gtk_widget_destroy (wordlist_label_3_m);
	gtk_widget_destroy (wordlist_label_4_w);
	gtk_widget_destroy (wordlist_label_4_m);
	gtk_widget_destroy (wordlist_label_5_w);
	gtk_widget_destroy (wordlist_label_5_m);

	gtk_widget_destroy (zu_entry);
	gtk_widget_destroy (wordlist_vscroll);
}

static void on_skim_start_clicked ();

gint
firstrecite_skim_select_next_word (gpointer data)
{
	g_pReciteWord->firstrecite_skim->left_time -= 250;
	if (g_pReciteWord->firstrecite_skim->left_time>0)
		return true;
	g_pReciteWord->firstrecite_skim->left_time = g_pReciteWord->firstrecite_skim->wait_time;
	
	if (g_pReciteWord->firstrecite_skim->selected_wordlist_item >=
	    g_pReciteWord->now_zu_wordcount - 1)
	{
		on_skim_start_clicked (); //stop
		g_pReciteWord->firstrecite_skim->dialog.show ();
		return FALSE;
	}
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->firstrecite_skim->
		  wordlist_adjust)->value);

	g_pReciteWord->firstrecite_skim->selected_wordlist_item++;
	//if (((g_pReciteWord->firstrecite_skim->selected_wordlist_item)>=nowvalue+6)||((g_pReciteWord->firstrecite_skim->selected_wordlist_item)<nowvalue))
	if ((g_pReciteWord->firstrecite_skim->selected_wordlist_item) ==
	    nowvalue + 6)
	{
		nowvalue =
			g_pReciteWord->firstrecite_skim->
			selected_wordlist_item;
		if (nowvalue > g_pReciteWord->now_zu_wordcount - 6)
			nowvalue = g_pReciteWord->now_zu_wordcount - 6;
		(((GtkAdjustment *) g_pReciteWord->firstrecite_skim->
		  wordlist_adjust)->value = nowvalue);
		g_pReciteWord->firstrecite_skim->set_wordlist_item (nowvalue);
		g_signal_emit_by_name (g_pReciteWord->firstrecite_skim->
					 wordlist_adjust, "changed");
	}
	if (((g_pReciteWord->firstrecite_skim->selected_wordlist_item) >=
	     nowvalue)
	    && ((g_pReciteWord->firstrecite_skim->selected_wordlist_item) <
		(nowvalue + 6)))
		g_pReciteWord->firstrecite_skim->
			wordlist_item[((g_pReciteWord->firstrecite_skim->
					selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
	g_pReciteWord->firstrecite_skim->set_wordview_label (g_pReciteWord->
							     firstrecite_skim->
							     selected_wordlist_item);
	return TRUE;
}

static void
on_skim_start_clicked ()
{
	if (g_pReciteWord->firstrecite_skim->started)
	{
		if (g_pReciteWord->firstrecite_skim->timeout)
		{
			g_source_remove (g_pReciteWord->firstrecite_skim->timeout);
			g_pReciteWord->firstrecite_skim->timeout = 0;
		}
		g_pReciteWord->firstrecite_skim->pause_button.
			set_enable (FALSE);
		g_pReciteWord->firstrecite_skim->start_button.reset (Skin->
								     skim.
								     start_button.
								     p[0],
								     Skin->
								     skim.
								     start_button.
								     p[1],
								     Skin->
								     skim.
								     start_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->firstrecite_skim->started = false;
	}
	else
	{
		if (g_pReciteWord->firstrecite_skim->dialog.fixed)
		{
			g_pReciteWord->firstrecite_skim->dialog.close();
		}
		((GtkAdjustment *) g_pReciteWord->firstrecite_skim->
		 wordlist_adjust)->value = 0;
		g_signal_emit_by_name (g_pReciteWord->firstrecite_skim->
					 wordlist_adjust, "changed");
		g_pReciteWord->firstrecite_skim->selected_wordlist_item = 0;
		g_pReciteWord->firstrecite_skim->wordlist_item[0].
			set_selected (TRUE);
		g_pReciteWord->firstrecite_skim->set_wordlist_item (0);
		g_pReciteWord->firstrecite_skim->set_wordview_label (0);
		g_pReciteWord->firstrecite_skim->pause_button.
			set_enable (TRUE);
		g_pReciteWord->firstrecite_skim->start_button.reset (Skin->
								     skim.
								     stop_button.
								     p[0],
								     Skin->
								     skim.
								     stop_button.
								     p[1],
								     Skin->
								     skim.
								     stop_button.
								     p[2],
								     Skin->
								     skim.
								     stop_button.
								     p[3]);
		g_pReciteWord->firstrecite_skim->left_time = g_pReciteWord->firstrecite_skim->wait_time;
		g_pReciteWord->firstrecite_skim->timeout =
			g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
		g_pReciteWord->firstrecite_skim->started = true;
	}
}

static void
on_skim_pause_clicked ()
{
	if (g_pReciteWord->firstrecite_skim->started)
	{
		g_source_remove (g_pReciteWord->firstrecite_skim->timeout);
		g_pReciteWord->firstrecite_skim->timeout = 0;
		g_pReciteWord->firstrecite_skim->start_button.
			set_enable (FALSE);
		g_pReciteWord->firstrecite_skim->pause_button.reset (Skin->
								     skim.
								     continue_button.
								     p[0],
								     Skin->
								     skim.
								     continue_button.
								     p[1],
								     Skin->
								     skim.
								     continue_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->firstrecite_skim->started = false;
	}
	else
	{
		g_pReciteWord->firstrecite_skim->start_button.
			set_enable (TRUE);
		g_pReciteWord->firstrecite_skim->pause_button.reset (Skin->
								     skim.
								     pause_button.
								     p[0],
								     Skin->
								     skim.
								     pause_button.
								     p[1],
								     Skin->
								     skim.
								     pause_button.
								     p[2],
								     Skin->
								     skim.
								     pause_button.
								     p[3]);
		g_pReciteWord->firstrecite_skim->left_time = g_pReciteWord->firstrecite_skim->wait_time;
		g_pReciteWord->firstrecite_skim->timeout =
			g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
		g_pReciteWord->firstrecite_skim->started = true;
	}
}

static void
on_skim_group_clicked ()
{
	g_pReciteWord->firstrecite_skim->close ();
	g_pReciteWord->ShowFirstRecite_group ();
}

static void
on_skim_test_clicked ()
{
	g_pReciteWord->firstrecite_skim->close ();
	g_pReciteWord->ShowFirstRecite_test ();
}

static void
on_skim_return_clicked ()
{
	g_pReciteWord->firstrecite_skim->close ();
	g_pReciteWord->show ();
}

static void
on_skim_readword_ckbutton_clicked ()
{
}

static void
on_skim_phonetic_ckbutton_clicked ()
{
	if (g_pReciteWord->firstrecite_skim->phonetic_ckbutton.status)
		gtk_widget_show(g_pReciteWord->firstrecite_skim->phonetic_label.hbox);
	else
		gtk_widget_hide(g_pReciteWord->firstrecite_skim->phonetic_label.hbox);
}

static void
on_skim_showmeanlater_ckbutton_clicked ()
{
}

static void
on_skim_upcase_ckbutton_clicked ()
{
}

static void
on_skim_readword_button_clicked (gpointer data)
{
	CFirstRecite_skim *parent = (CFirstRecite_skim *)data;
	if (parent->selected_wordlist_item>=0 && parent->selected_wordlist_item<g_pReciteWord->now_zu_wordcount) {
		if (parent->left_time < 1000)
			parent->left_time = 1000; //delay selecting the next word for 1 second.
		readword(g_pReciteWord->now_zu_words.w[parent->selected_wordlist_item]);
	}
}

static void
on_skim_wordlistadjust_valuechanged (GtkAdjustment * adj,
				     CFirstRecite_skim * parent)
{
	int newvalue = ((gint) adj->value);

	if (((parent->selected_wordlist_item) >= newvalue)
	    && ((parent->selected_wordlist_item) < (newvalue + 6)))
		parent->wordlist_item[((parent->selected_wordlist_item) -
				       newvalue)].set_selected (TRUE);
	else
		parent->wordlist_item[0].clean_selected ();

	g_pReciteWord->firstrecite_skim->set_wordlist_item (newvalue);

}

static void
on_skim_wordlistitem_clicked (int index)
{
	if (g_pReciteWord->firstrecite_skim->timeout)
	{
		g_source_remove(g_pReciteWord->firstrecite_skim->timeout);
		g_pReciteWord->firstrecite_skim->left_time = g_pReciteWord->firstrecite_skim->wait_time;
		g_pReciteWord->firstrecite_skim->timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}

	int i = index +
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->firstrecite_skim->
		  wordlist_adjust)->value);
	g_pReciteWord->firstrecite_skim->selected_wordlist_item = i;
	g_pReciteWord->firstrecite_skim->set_wordview_label (i);
}

CFirstRecite_skim::CFirstRecite_skim ()
{
	skin_load_skim (g_pReciteWord->window);
	rw_cfg_read_int (usercfgfile, "firstrecite_skim", "wait_time", &wait_time);
	if (wait_time <= 0)
		wait_time = 4000; // 4 seconds.
}

CFirstRecite_skim::~CFirstRecite_skim ()
{

}

void
CFirstRecite_skim::init ()
{
	g_pReciteWord->status = STATUS_FIRSTRECITE_SKIM;
	started = false;
	timeout = 0;
	dialog.fixed = NULL;
}

void
CFirstRecite_skim::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->skim.skim.p[0]);
	start_button.create (g_pReciteWord->fixed, Skin->skim.stop_button.x[0],
			     Skin->skim.stop_button.y[0],
			     Skin->skim.stop_button.p[0],
			     Skin->skim.stop_button.p[1],
			     Skin->skim.stop_button.p[2],
			     Skin->skim.stop_button.p[3], on_skim_start_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed, Skin->skim.pause_button.x[0],
			     Skin->skim.pause_button.y[0],
			     Skin->skim.pause_button.p[0],
			     Skin->skim.pause_button.p[1],
			     Skin->skim.pause_button.p[2],
			     Skin->skim.pause_button.p[3],
			     on_skim_pause_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), pause_button.event_box,"Ctrl-P","");
	group_button.create (g_pReciteWord->fixed, Skin->skim.group_button.x[0],
			     Skin->skim.group_button.y[0],
			     Skin->skim.group_button.p[0],
			     Skin->skim.group_button.p[1],
			     Skin->skim.group_button.p[2], (GdkPixmap *) NULL,
			     on_skim_group_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), group_button.event_box,"Ctrl-G","");
	test_button.create (g_pReciteWord->fixed, Skin->skim.test_button.x[0],
			    Skin->skim.test_button.y[0], Skin->skim.test_button.p[0],
			    Skin->skim.test_button.p[1],
			    Skin->skim.test_button.p[2], (GdkPixmap *) NULL,
			    on_skim_test_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), test_button.event_box,"Ctrl-T","");
	return_button.create (g_pReciteWord->fixed, Skin->skim.return_button.x[0],
			      Skin->skim.return_button.y[0],
			      Skin->skim.return_button.p[0],
			      Skin->skim.return_button.p[1],
			      Skin->skim.return_button.p[2], (GdkPixmap *) NULL,
			      on_skim_return_clicked);
	readword_ckbutton.create (g_pReciteWord->fixed, Skin->skim.readword_ckbutton.x[0],
			      Skin->skim.readword_ckbutton.y[0],
			      Skin->skim.readword_ckbutton.p[0],
			      Skin->skim.readword_ckbutton.p[1],
			      Skin->skim.readword_ckbutton.p[2],
				  Skin->skim.readword_ckbutton.p[3],
				  on_skim_readword_ckbutton_clicked);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "readword",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	phonetic_ckbutton.create (g_pReciteWord->fixed, Skin->skim.phonetic_ckbutton.x[0],
			      Skin->skim.phonetic_ckbutton.y[0],
			      Skin->skim.phonetic_ckbutton.p[0],
			      Skin->skim.phonetic_ckbutton.p[1],
			      Skin->skim.phonetic_ckbutton.p[2],
				  Skin->skim.phonetic_ckbutton.p[3],
				  on_skim_phonetic_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "showphonetic",
			     &tmp_b);
	phonetic_ckbutton.set_status(tmp_b);
	showmeanlater_ckbutton.create (g_pReciteWord->fixed, Skin->skim.showmeanlater_ckbutton.x[0],
			      Skin->skim.showmeanlater_ckbutton.y[0],
			      Skin->skim.showmeanlater_ckbutton.p[0],
			      Skin->skim.showmeanlater_ckbutton.p[1],
			      Skin->skim.showmeanlater_ckbutton.p[2],
				  Skin->skim.showmeanlater_ckbutton.p[3],
				  on_skim_showmeanlater_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "showmeanlater",
			     &tmp_b);
	showmeanlater_ckbutton.set_status(tmp_b);
	upcase_ckbutton.create (g_pReciteWord->fixed, Skin->skim.upcase_ckbutton.x[0],
			      Skin->skim.upcase_ckbutton.y[0],
			      Skin->skim.upcase_ckbutton.p[0],
			      Skin->skim.upcase_ckbutton.p[1],
			      Skin->skim.upcase_ckbutton.p[2],
				  Skin->skim.upcase_ckbutton.p[3],
				  on_skim_upcase_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "upcase",
			     &tmp_b);
	upcase_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->skim.readword_button.x[0],
			      Skin->skim.readword_button.y[0],
			      Skin->skim.readword_button.p[0],
			      Skin->skim.readword_button.p[1],
			      Skin->skim.readword_button.p[2],
				  on_skim_readword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), readword_button.event_box,"Tab","");
	sentencelist_vscroll = gtk_vscrollbar_new ((GtkAdjustment *) NULL);
	gtk_widget_set_size_request (sentencelist_vscroll,
			      Skin->skim.sentencelist_vscroll.w,
			      Skin->skim.sentencelist_vscroll.h);
	gtk_widget_show (sentencelist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), sentencelist_vscroll,
		       Skin->skim.sentencelist_vscroll.x[0],
		       Skin->skim.sentencelist_vscroll.y[0]);
	wordview_label_1 = gtk_label_new ("");
	SetFont(wordview_label_1,true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordview_label_1,
		       Skin->skim.wordview_label_1.x[0],
		       Skin->skim.wordview_label_1.y[0]);
	gtk_widget_show (wordview_label_1);
	wordlist_scrollvalue_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_scrollvalue_label,
		       Skin->skim.wordlist_scrollvalue_label.x[0],
		       Skin->skim.wordlist_scrollvalue_label.y[0]);
	gtk_widget_show (wordlist_scrollvalue_label);
	word_index_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_index_label,
		       Skin->skim.word_index_label.x[0],
		       Skin->skim.word_index_label.y[0]);
	gtk_widget_show (word_index_label);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (aa);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->skim.word_count_label.x[0],
		       Skin->skim.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	phonetic_label.create (g_pReciteWord->fixed, 0,
			       Skin->skim.phonetic_label.y[0],
			       &(Skin->text.b_phonetic));
	if (!phonetic_ckbutton.status)
		gtk_widget_hide(phonetic_label.hbox);
	wordview_label_3 = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordview_label_3,
		       Skin->skim.wordview_label_3.x[0],
		       Skin->skim.wordview_label_3.y[0]);
	gtk_widget_show (wordview_label_3);

	wordlist_item[0].create (NULL, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_1.x[0],
				 Skin->skim.wordlist_item_1.y[0],
				 Skin->skim.wordlist_item_1.w,
				 Skin->skim.wordlist_item_1.h,
				 Skin->skim.wordlist_item_1.p[0],
				 Skin->skim.wordlist_item_1.p[1],
				 on_skim_wordlistitem_clicked, 0);
	wordlist_item[1].create (wordlist_item[0].group, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_2.x[0],
				 Skin->skim.wordlist_item_2.y[0],
				 Skin->skim.wordlist_item_2.w,
				 Skin->skim.wordlist_item_2.h,
				 Skin->skim.wordlist_item_2.p[0],
				 Skin->skim.wordlist_item_2.p[1],
				 on_skim_wordlistitem_clicked, 1);
	wordlist_item[2].create (wordlist_item[1].group, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_3.x[0],
				 Skin->skim.wordlist_item_3.y[0],
				 Skin->skim.wordlist_item_3.w,
				 Skin->skim.wordlist_item_3.h,
				 Skin->skim.wordlist_item_3.p[0],
				 Skin->skim.wordlist_item_3.p[1],
				 on_skim_wordlistitem_clicked, 2);
	wordlist_item[3].create (wordlist_item[2].group, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_4.x[0],
				 Skin->skim.wordlist_item_4.y[0],
				 Skin->skim.wordlist_item_4.w,
				 Skin->skim.wordlist_item_4.h,
				 Skin->skim.wordlist_item_4.p[0],
				 Skin->skim.wordlist_item_4.p[1],
				 on_skim_wordlistitem_clicked, 3);
	wordlist_item[4].create (wordlist_item[3].group, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_5.x[0],
				 Skin->skim.wordlist_item_5.y[0],
				 Skin->skim.wordlist_item_5.w,
				 Skin->skim.wordlist_item_5.h,
				 Skin->skim.wordlist_item_5.p[0],
				 Skin->skim.wordlist_item_5.p[1],
				 on_skim_wordlistitem_clicked, 4);
	wordlist_item[5].create (wordlist_item[4].group, g_pReciteWord->fixed,
				 Skin->skim.wordlist_item_6.x[0],
				 Skin->skim.wordlist_item_6.y[0],
				 Skin->skim.wordlist_item_6.w,
				 Skin->skim.wordlist_item_6.h,
				 Skin->skim.wordlist_item_6.p[0],
				 Skin->skim.wordlist_item_6.p[1],
				 on_skim_wordlistitem_clicked, 5);

	wordlist_adjust =
		gtk_adjustment_new (0, 0, g_pReciteWord->now_zu_wordcount, 1,
				    6, 6);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK (on_skim_wordlistadjust_valuechanged),
			  this);

	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll, Skin->skim.wordlist_vscroll.w,
			      Skin->skim.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_vscroll,
		       Skin->skim.wordlist_vscroll.x[0],
		       Skin->skim.wordlist_vscroll.y[0]);

	on_skim_start_clicked ();
}

void
CFirstRecite_skim::close ()
{
	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "readword",
			     &tmp_b);
	if (tmp_b!=readword_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "firstrecite_skim", "readword",
			     readword_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "showphonetic",
			     &tmp_b);
	if (tmp_b!=phonetic_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "firstrecite_skim", "showphonetic",
			     phonetic_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "showmeanlater",
			     &tmp_b);
	if (tmp_b!=showmeanlater_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "firstrecite_skim", "showmeanlater",
			     showmeanlater_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_skim", "upcase",
			     &tmp_b);
	if (tmp_b!=upcase_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "firstrecite_skim", "upcase",
			     upcase_ckbutton.status);
	}

	if (timeout)
	{
		g_source_remove (timeout);
		timeout = 0;
	}

	if (dialog.fixed)
	{
		dialog.close();
	}
	start_button.destroy ();
	pause_button.destroy ();
	group_button.destroy ();
	test_button.destroy ();
	return_button.destroy ();
	readword_ckbutton.destroy ();
	phonetic_ckbutton.destroy ();
	showmeanlater_ckbutton.destroy ();
	upcase_ckbutton.destroy ();
	readword_button.destroy ();
	gtk_widget_destroy (wordlist_vscroll);
	gtk_widget_destroy (sentencelist_vscroll);
	gtk_widget_destroy (wordview_label_1);
	gtk_widget_destroy (wordview_label_3);
	gtk_widget_destroy (wordlist_scrollvalue_label);
	gtk_widget_destroy (word_index_label);
	gtk_widget_destroy (word_count_label);
	phonetic_label.destroy ();
	wordlist_item[0].destroy ();
	wordlist_item[1].destroy ();
	wordlist_item[2].destroy ();
	wordlist_item[3].destroy ();
	wordlist_item[4].destroy ();
	wordlist_item[5].destroy ();
}

void
CFirstRecite_skim::set_wordview_label (gint i)
{
	gtk_label_set_text (GTK_LABEL (wordview_label_1),
			    g_pReciteWord->now_zu_words.w[i]);
	gtk_label_set_text (GTK_LABEL (wordview_label_3),
			    g_pReciteWord->now_zu_words.m[i]);
	gchar aa[5];
	sprintf(aa,"%2d",i+1);
	gtk_label_set_text (GTK_LABEL (word_index_label),
			    aa);
	
	if (phonetic_ckbutton.status)
		gtk_widget_hide(phonetic_label.hbox);
	phonetic_label.set_text (g_pReciteWord->now_zu_words.t[i]);
	gint w;
	pango_layout_get_pixel_size(gtk_label_get_layout(GTK_LABEL(wordview_label_1)),&w,NULL);
	gtk_fixed_move (GTK_FIXED (g_pReciteWord->fixed),
			phonetic_label.hbox,
			Skin->skim.phonetic_label.x[0] +
			Skin->skim.wordview_label_1.x[0] +
			w
	/*gdk_string_width (gtk_style_get_font
					  (wordview_label_1->style),
					  g_pReciteWord->now_zu_words.w[i])*/
	,
			Skin->skim.phonetic_label.y[0]);
	if (phonetic_ckbutton.status)
		gtk_widget_show(phonetic_label.hbox);
	if (readword_ckbutton.status)
		readword(g_pReciteWord->now_zu_words.w[i]);
}

void
CFirstRecite_skim::set_wordlist_item (int index)
{
	gchar aa[5];
	sprintf(aa,"%2d",index+1);
	gtk_label_set_text (GTK_LABEL (wordlist_scrollvalue_label),
			    aa);
	wordlist_item[0].set_label (g_pReciteWord->now_zu_words.w[0 + index]);
	wordlist_item[1].set_label (g_pReciteWord->now_zu_words.w[1 + index]);
	wordlist_item[2].set_label (g_pReciteWord->now_zu_words.w[2 + index]);
	wordlist_item[3].set_label (g_pReciteWord->now_zu_words.w[3 + index]);
	wordlist_item[4].set_label (g_pReciteWord->now_zu_words.w[4 + index]);
	wordlist_item[5].set_label (g_pReciteWord->now_zu_words.w[5 + index]);
}

void
CFirstRecite_skim::do_up ()
{
	if (selected_wordlist_item == 0)
		return;
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	selected_wordlist_item--;
	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		nowvalue = selected_wordlist_item;
		if (nowvalue > g_pReciteWord->now_zu_wordcount - 6)
			nowvalue = g_pReciteWord->now_zu_wordcount - 6;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}

	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
	set_wordview_label (selected_wordlist_item);	
}

void
CFirstRecite_skim::do_down ()
{
	if (selected_wordlist_item >= g_pReciteWord->now_zu_wordcount - 1)
		return;
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	selected_wordlist_item++;
	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		if (selected_wordlist_item<=5)
			nowvalue = 0;
		else
			nowvalue = selected_wordlist_item-5;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}

	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
	set_wordview_label (selected_wordlist_item);
}

void
CFirstRecite_skim::do_left ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue>0)
	{
		nowvalue--;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_skim_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_skim::do_right ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue < ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
	{
		nowvalue++;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_skim_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_skim::do_pageup ()
{
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		nowvalue = selected_wordlist_item;
		if (nowvalue > g_pReciteWord->now_zu_wordcount - 6)
			nowvalue = g_pReciteWord->now_zu_wordcount - 6;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
		return;
	}
	if (selected_wordlist_item == 0)
		return;
	if (selected_wordlist_item==nowvalue)
		selected_wordlist_item-=5;
	else
		selected_wordlist_item=nowvalue;
	if (selected_wordlist_item < 0)
		selected_wordlist_item =0;
	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		nowvalue = selected_wordlist_item;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}
		
	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
	set_wordview_label (selected_wordlist_item);	
}

void
CFirstRecite_skim::do_pagedown ()
{
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		nowvalue = selected_wordlist_item;
		if (nowvalue > g_pReciteWord->now_zu_wordcount - 6)
			nowvalue = g_pReciteWord->now_zu_wordcount - 6;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
		return;
	}
	if (selected_wordlist_item >= g_pReciteWord->now_zu_wordcount - 1)
		return;
	if (selected_wordlist_item==nowvalue+5)
		selected_wordlist_item+=5;
	else
		selected_wordlist_item=nowvalue+5;
	if (selected_wordlist_item > g_pReciteWord->now_zu_wordcount - 1)
		selected_wordlist_item = g_pReciteWord->now_zu_wordcount - 1;
	if (((selected_wordlist_item)>=nowvalue+6)||((selected_wordlist_item)<nowvalue))
	{
		nowvalue = selected_wordlist_item -5;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		set_wordlist_item (nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}
		
	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[((selected_wordlist_item) -
				       nowvalue)].set_selected (TRUE);
	set_wordview_label (selected_wordlist_item);
}

void
CFirstRecite_skim::do_home ()
{
	if (selected_wordlist_item == 0)
		return;
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	selected_wordlist_item = 0;
	if (nowvalue!=0)
	{
		((GtkAdjustment *) wordlist_adjust)->value = 0;
		set_wordlist_item (0);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}

	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[0].set_selected (TRUE);
	set_wordview_label (0);	
}

void
CFirstRecite_skim::do_end ()
{
	if (selected_wordlist_item == g_pReciteWord->now_zu_wordcount-1)
		return;
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) wordlist_adjust)->value);

	selected_wordlist_item = g_pReciteWord->now_zu_wordcount-1;
	if (nowvalue!=g_pReciteWord->now_zu_wordcount-6)
	{
		((GtkAdjustment *) wordlist_adjust)->value = g_pReciteWord->now_zu_wordcount-6;
		set_wordlist_item (g_pReciteWord->now_zu_wordcount-6);
		g_signal_emit_by_name (wordlist_adjust, "changed");
	}

	if (timeout)
	{
		g_source_remove(timeout);
		left_time = wait_time;
		timeout = g_timeout_add (250, firstrecite_skim_select_next_word, NULL);
	}
	wordlist_item[5].set_selected (TRUE);
	set_wordview_label (g_pReciteWord->now_zu_wordcount-1);	
}

void
CFirstRecite_skim::do_scroll(GdkEventScroll * event)
{
	//event->window is not always be the main window->window,so,event->x 's meaning is uncertainly.use event->x_root instead.
	int x, y;
	gtk_window_get_position (GTK_WINDOW(g_pReciteWord->window), &x, &y);
	if ((event->x_root>=x+Skin->skim.wordlist_area.x[0])&&(event->x_root<=x+Skin->skim.wordlist_area.x[0]+Skin->skim.wordlist_area.w)
		&&(event->y_root>=y+Skin->skim.wordlist_area.y[0])&&(event->y_root<=y+Skin->skim.wordlist_area.y[0]+Skin->skim.wordlist_area.h))
	{
		if (dialog.fixed)
		{
			int dialog_x=x+((Skin->reciteword.width - Skin->skim.dialog.w) / 2);
			int dialog_y=y+((Skin->reciteword.height - Skin->skim.dialog.h) / 2);
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->skim.dialog.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->skim.dialog.h))
				return;
		}
		GtkAdjustment *adj = (GtkAdjustment *)wordlist_adjust;
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
			g_signal_emit_by_name (wordlist_adjust, "changed");
			on_skim_wordlistadjust_valuechanged (adj,this);
		}
	}
}

static void
on_skim_dialog_ok_button_clicked (gpointer data)
{
	int i;
	if (g_pReciteWord->firstrecite_skim->dialog.test_robutton.selected)
		i = 1;
	else if (g_pReciteWord->firstrecite_skim->dialog.skim_robutton.
		 selected)
		i = 2;
	else
		i = 3;
	g_pReciteWord->firstrecite_skim->dialog.close ();
	if (i == 1)
	{
		g_pReciteWord->firstrecite_skim->close ();
		g_pReciteWord->ShowFirstRecite_test ();
	}
	else if (i == 3)
	{
		g_pReciteWord->firstrecite_skim->close ();
		g_pReciteWord->show ();
	}
	else
	{
		on_skim_start_clicked ();
	}
}

CSkimDialog::CSkimDialog ()
{
}

CSkimDialog::~CSkimDialog ()
{
}

void
CSkimDialog::show ()
{
	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_size_request (fixed, Skin->skim.dialog.w,
			      Skin->skim.dialog.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,
		       (Skin->reciteword.width - Skin->skim.dialog.w) / 2,
		       (Skin->reciteword.height -
			Skin->skim.dialog.h) / 2);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->skim.dialog.p[0]);

	test_robutton.create (NULL, fixed, Skin->skim.dialog_test_robutton.x[0],
			      Skin->skim.dialog_test_robutton.y[0],
			      Skin->skim.dialog_test_robutton.p[0],
			      Skin->skim.dialog_test_robutton.p[1],
			      Skin->skim.dialog_test_robutton.p[2],
			      Skin->skim.dialog_test_robutton.p[3], NULL,NULL);
	skim_robutton.create (test_robutton.group, fixed,
			      Skin->skim.dialog_skim_robutton.x[0],
			      Skin->skim.dialog_skim_robutton.y[0],
			      Skin->skim.dialog_skim_robutton.p[0],
			      Skin->skim.dialog_skim_robutton.p[1],
			      Skin->skim.dialog_skim_robutton.p[2],
			      Skin->skim.dialog_skim_robutton.p[3], NULL,NULL);
	return_robutton.create (skim_robutton.group, fixed,
				Skin->skim.dialog_return_robutton.x[0],
				Skin->skim.dialog_return_robutton.y[0],
				Skin->skim.dialog_return_robutton.p[0],
				Skin->skim.dialog_return_robutton.p[1],
				Skin->skim.dialog_return_robutton.p[2],
				Skin->skim.dialog_return_robutton.p[3], NULL,NULL);
   	gint tmp_i;
	rw_cfg_read_int(usercfgfile,
			     "firstrecite_skim", "dialog_index",
			     &tmp_i);
	if (tmp_i==1)
		skim_robutton.set_selected(true);
	else if (tmp_i==2)
		return_robutton.set_selected(true);

	ok_button.create (fixed, Skin->skim.dialog_ok_button.x[0],
			  Skin->skim.dialog_ok_button.y[0],
			  Skin->skim.dialog_ok_button.p[0],
			  Skin->skim.dialog_ok_button.p[1], NULL,
			  on_skim_dialog_ok_button_clicked, NULL);
}

void
CSkimDialog::close ()
{
	gint tmp_i,tmp_index;
	rw_cfg_read_int(usercfgfile,
			     "firstrecite_skim", "dialog_index",
			     &tmp_i);
	if (test_robutton.selected)
		tmp_index = 0;
	else if (skim_robutton.selected)
		tmp_index = 1;
	else
		tmp_index = 2;
	if (tmp_i!=tmp_index)
	{
		rw_cfg_write_int(usercfgfile,
					 "firstrecite_skim", "dialog_index",
					 tmp_index);
	}
	test_robutton.destroy();
	skim_robutton.destroy();
	return_robutton.destroy();
	ok_button.destroy();
	gtk_widget_destroy (fixed);
	fixed=NULL;
}

void
CSkimDialog::do_up ()
{
	if (!test_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (return_robutton.selected)
			skim_robutton.set_selected(true);
		else
			test_robutton.set_selected(true);
	}
}

void
CSkimDialog::do_down ()
{
	if (!return_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (test_robutton.selected)
			skim_robutton.set_selected(true);
		else
			return_robutton.set_selected(true);
	}
}

static gboolean
on_listitem_button_press (GtkWidget * widget, GdkEvent * event,
			  tListItem * parent)
{
	if (!parent->selected)
	{
		playsound (SND_BUTTONDOWN);
	}

	return TRUE;
}

static gboolean
on_listitem_button_release (GtkWidget * widget, GdkEvent * event,
			    tListItem * parent)
{
	if (!parent->selected)
	{
		playsound (SND_BUTTONUP);
		if (parent->mousein)
		{
			(*(parent->runfunc)) (parent->func_data);
			parent->set_selected (TRUE);
		}
	}
	return TRUE;
}

static gboolean
on_listitem_enter_notify (GtkWidget * widget, GdkEvent * event,
			  tListItem * parent)
{
	parent->mousein = TRUE;
	return true;
}

static gboolean
on_listitem_leave_notify (GtkWidget * widget, GdkEvent * event,
			  tListItem * parent)
{
	parent->mousein = FALSE;
	return true;
}

tListItem::tListItem ()
{
}

void
tListItem::init ()
{
	selected = FALSE;
	this->group = g_slist_prepend ((GSList *) NULL, this);
}

void
tListItem::create (GSList * group, GtkWidget * pfixed, gint x, gint y, gint w,
		   gint h, GdkPixmap * pix_1, GdkPixmap * pix_2, void (*func) (int), int data)
{
	init ();

	pix_normal = pix_1;
	pix_selected = pix_2;
	runfunc = func;
	func_data = data;

	fixed = gtk_fixed_new ();
	gtk_widget_set_events (fixed, GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
	gtk_widget_set_size_request (fixed, w, h);
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
//      gtk_widget_set_app_paintable(fixed,TRUE);
	gtk_fixed_put (GTK_FIXED (pfixed), fixed, x, y);
	label = gtk_label_new ("");
	SetFont(label,false);
	gtk_fixed_put (GTK_FIXED (fixed), label, 6, 0);
	gtk_widget_show (label);

	gtk_widget_show (fixed);
	g_signal_connect (G_OBJECT (fixed), "button_release_event",
			  G_CALLBACK(on_listitem_button_release), this);
	g_signal_connect (G_OBJECT (fixed), "button_press_event",
			  G_CALLBACK(on_listitem_button_press), this);
	g_signal_connect (G_OBJECT (fixed), "enter_notify_event",
			  G_CALLBACK(on_listitem_enter_notify), this);
	g_signal_connect (G_OBJECT (fixed), "leave_notify_event",
			  G_CALLBACK(on_listitem_leave_notify), this);

	SetBackPixmap (fixed, pix_normal);
	gdk_window_set_cursor (fixed->window, Skin->reciteword.hand_cursor.cursor);

	if (group)
		set_group (group);
}

void
tListItem::set_group (GSList * group)
{
	if (this->group)
	{
		GSList *slist;

		this->group = g_slist_remove (this->group, this);

		for (slist = this->group; slist; slist = slist->next)
		{
			tListItem *tmp_listitem;
			tmp_listitem = (tListItem *) slist->data;
			tmp_listitem->group = this->group;
		}
	}

	this->group = g_slist_prepend (group, this);

	if (group)
	{
		GSList *slist;

		for (slist = group; slist; slist = slist->next)
		{
			tListItem *tmp_listitem;
			tmp_listitem = (tListItem *) slist->data;

			tmp_listitem->group = this->group;
		}
	}

	set_selected (group == NULL);
}

void
tListItem::clean_selected ()
{
	GSList *tmp_list;
	tListItem *tmp_listitem;
	tmp_list = this->group;
	while (tmp_list)
	{
		tmp_listitem = (tListItem *) tmp_list->data;
		tmp_list = tmp_list->next;
		if (tmp_listitem->selected)
		{
			tmp_listitem->set_selected (FALSE);
			break;
		}
	}
}

void
tListItem::set_selected (int a)
{
	if (this->selected != a)
	{
		if (a)
		{
			clean_selected ();
			SetBackPixmap (fixed, pix_selected);
		}
		else
			SetBackPixmap (fixed, pix_normal);
		gtk_widget_queue_draw (fixed);
		this->selected = a;
	}
}

void
tListItem::set_label (gchar * str)
{
	gtk_label_set_text (GTK_LABEL (label), str);
}

void
tListItem::destroy ()
{
	GSList *tmp_list;
	tListItem *tmp_listitem;

	this->group = g_slist_remove (this->group, this);
	tmp_list = this->group;

	while (tmp_list)
	{
		tmp_listitem = (tListItem *) tmp_list->data;
		tmp_list = tmp_list->next;

		tmp_listitem->group = this->group;
	}

	gtk_widget_destroy (fixed);
}

tListItem::~tListItem ()
{
}
