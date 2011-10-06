#include "chooseword.h"
#include "reciteword.h"
#include "skin.h"
#include "rwconfig.h"
#include <cstdio>
#include <cstdlib>

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;


static gint
get_total_zu_count ()
{
	gchar tmpstr[256];
	gint i, total_zucount = 0, max_wordcount = 0, last_zu_wordcount =
		25, tmpint;

	for (i = 0; i <= g_pReciteWord->max_zu_index; i++)
	{
		sprintf (tmpstr, "zu%d_wordcount", i);
		if (rw_cfg_read_int
		    (usercfgfile, g_pReciteWord->now_book->bookfilename,
		     tmpstr, &tmpint))
			last_zu_wordcount = tmpint;
		max_wordcount += last_zu_wordcount;
	}
	max_wordcount -= last_zu_wordcount;

	total_zucount = g_pReciteWord->max_zu_index;

	total_zucount +=
		((g_pReciteWord->now_book->wordcount -
		  max_wordcount) / last_zu_wordcount);
	tmpint = (g_pReciteWord->now_book->wordcount -
		  max_wordcount) % last_zu_wordcount;

	if (tmpint>=6)
		total_zucount++;
	return total_zucount;
}

static gint
chooseword_hide_tip (gpointer widget)
{
	gtk_widget_hide (GTK_WIDGET (widget));
	g_pReciteWord->chooseword->tip_timer = 0;
	return FALSE;
}

static void
on_chooseword_ok_clicked (gpointer data)
{
	if (g_pReciteWord->chooseword->bygroup_robutton.selected)
	{
		gint zu_index;
		const gchar *a;
		a = gtk_entry_get_text (GTK_ENTRY
					(g_pReciteWord->chooseword->
					 group_entry));
		zu_index = atoi (a) - 1;
		if ((zu_index < 0)
		    || (zu_index > g_pReciteWord->chooseword->total_zucount-1))
		{
			if (g_pReciteWord->chooseword->tip_timer)
			{
				g_source_remove (g_pReciteWord->
						    chooseword->tip_timer);
			}
			gtk_widget_hide (g_pReciteWord->chooseword->
					 noclass_tip);
			gtk_widget_show (g_pReciteWord->chooseword->
					 scopeerr_tip);
			g_pReciteWord->chooseword->tip_timer =
				g_timeout_add (2000, chooseword_hide_tip,
						 g_pReciteWord->chooseword->
						 scopeerr_tip);

			return;
		}
		
		/*gint i, tmpint, last_zu_wordcount = 25, now_wordcount = 0;
		gchar tmpstr[256];
		for (i = 0; i <= zu_index; i++)
		{
			sprintf (tmpstr, "zu%d_wordcount", i);
			if (rw_cfg_read_int
			    (usercfgfile,
			     g_pReciteWord->now_book->bookfilename, tmpstr,
			     &tmpint))
				last_zu_wordcount = tmpint;
			now_wordcount += last_zu_wordcount;
		}
		now_wordcount -= last_zu_wordcount;
		if (zu_index==parent->total_zucount-1)
		{
			last_zu_wordcount=g_pReciteWord->now_book->wordcount-now_wordcount;
		}

		g_pReciteWord->now_zu_index = zu_index;
		g_pReciteWord->now_zu_wordcount = last_zu_wordcount;
		g_pReciteWord->now_zu_words.w =
			g_pReciteWord->now_book->words.w + now_wordcount;
		g_pReciteWord->now_zu_words.t =
			g_pReciteWord->now_book->words.t + now_wordcount;
		g_pReciteWord->now_zu_words.m =
			g_pReciteWord->now_book->words.m + now_wordcount;
		*/
		g_pReciteWord->chooseword->last_scope_start = g_pReciteWord->chooseword->scope_start;
		g_pReciteWord->chooseword->last_scope_count = g_pReciteWord->chooseword->scope_count;
		g_pReciteWord->set_now_zu_words(zu_index,g_pReciteWord->chooseword->scope_count,
						g_pReciteWord->now_book->words.w + g_pReciteWord->chooseword->scope_start,
						g_pReciteWord->now_book->words.t + g_pReciteWord->chooseword->scope_start,
						g_pReciteWord->now_book->words.m + g_pReciteWord->chooseword->scope_start,
						g_pReciteWord->chooseword->order_ckbutton.status);
	}
	else if (g_pReciteWord->chooseword->byscope_robutton.selected)
	{
		const gchar *a;
		a = gtk_entry_get_text (GTK_ENTRY
					(g_pReciteWord->chooseword->
					 scope_entry));
		gint scope_start = atoi (a) - 1;
		a = gtk_entry_get_text (GTK_ENTRY
					(g_pReciteWord->chooseword->
					 scopecount_entry));
		gint scope_count = atoi (a);

		if ((scope_start < 0) || (scope_count < 6)
		    || (scope_start + scope_count >
			g_pReciteWord->now_book->wordcount))
		{
			if (g_pReciteWord->chooseword->tip_timer)
			{
				g_source_remove (g_pReciteWord->
						    chooseword->tip_timer);
			}
			gtk_widget_hide (g_pReciteWord->chooseword->
					 noclass_tip);
			gtk_widget_show (g_pReciteWord->chooseword->
					 scopeerr_tip);
			g_pReciteWord->chooseword->tip_timer =
				g_timeout_add (2000, chooseword_hide_tip,
						 g_pReciteWord->chooseword->
						 scopeerr_tip);

			return;
		}
		g_pReciteWord->chooseword->last_scope_start = scope_start;
		g_pReciteWord->chooseword->last_scope_count = scope_count;
		g_pReciteWord->set_now_zu_words(-1,scope_count,
						g_pReciteWord->now_book->words.w + scope_start,
						g_pReciteWord->now_book->words.t + scope_start,
						g_pReciteWord->now_book->words.m + scope_start,
						g_pReciteWord->chooseword->order_ckbutton.status);
	}
	else if (g_pReciteWord->chooseword->byclass_robutton.selected)
	{
		if (g_pReciteWord->chooseword->tip_timer)
		{
			g_source_remove (g_pReciteWord->chooseword->
					    tip_timer);
		}
		gtk_widget_hide (g_pReciteWord->chooseword->scopeerr_tip);
		gtk_widget_show (g_pReciteWord->chooseword->noclass_tip);
		g_pReciteWord->chooseword->tip_timer =
			g_timeout_add (2000, chooseword_hide_tip,
					 g_pReciteWord->chooseword->
					 noclass_tip);

		return;
	}

	(*(g_pReciteWord->chooseword->func1)) ();
}

static void
on_chooseword_entry_activate (GtkWidget * widget, CChooseword * parent)
{
	on_chooseword_ok_clicked (NULL);
}

static void
on_chooseword_groupentry_changed (GtkWidget * widget, CChooseword * parent)
{
	gint zu_index;
	const gchar *a;

	a = gtk_entry_get_text (GTK_ENTRY (parent->group_entry));
	zu_index = atoi (a) - 1;
	if ((zu_index < 0) || (zu_index > parent->total_zucount-1))
	{
		gtk_label_set_text (GTK_LABEL (parent->scope_label), "");
		gtk_label_set_text (GTK_LABEL (parent->scopecount_label), "");
		return;
	}

	gint i, tmpint, last_zu_wordcount = 25, now_wordcount = 0;
	gchar tmpstr[256];
	for (i = 0; i <= zu_index; i++)
	{
		sprintf (tmpstr, "zu%d_wordcount", i);
		if (rw_cfg_read_int
		    (usercfgfile, g_pReciteWord->now_book->bookfilename,
		     tmpstr, &tmpint))
			last_zu_wordcount = tmpint;
		now_wordcount += last_zu_wordcount;
	}
	now_wordcount -= last_zu_wordcount;
	if (zu_index==parent->total_zucount-1)
	{
		last_zu_wordcount=g_pReciteWord->now_book->wordcount-now_wordcount;
	}
	parent->scope_start = now_wordcount;
	parent->scope_count = last_zu_wordcount;
	sprintf (tmpstr, "%d", now_wordcount + 1);
	gtk_label_set_text (GTK_LABEL (parent->scope_label), tmpstr);
	sprintf (tmpstr, "%d", last_zu_wordcount);
	gtk_label_set_text (GTK_LABEL (parent->scopecount_label), tmpstr);
}

static void
on_chooseword_bygroup_clicked (gpointer data)
{	
	gtk_widget_hide (g_pReciteWord->chooseword->scope_entry);
	gtk_widget_hide (g_pReciteWord->chooseword->scopecount_entry);
	gtk_widget_hide (g_pReciteWord->chooseword->scope_s_label);
	gtk_widget_show (g_pReciteWord->chooseword->group_entry);
	gtk_widget_show (g_pReciteWord->chooseword->group_s_label);
	gtk_widget_show (g_pReciteWord->chooseword->scope_label);
	gtk_widget_show (g_pReciteWord->chooseword->scopecount_label);
	gtk_widget_grab_focus (g_pReciteWord->chooseword->group_entry);
	if (g_pReciteWord->chooseword->scope_count == 0) //when chooseword initially by_scope,it will not change scope_count,then the user click by group,when should update it.
		on_chooseword_groupentry_changed(g_pReciteWord->chooseword->group_entry, g_pReciteWord->chooseword);
}

static void
on_chooseword_byscope_clicked (gpointer data)
{
	gchar tmpstr[8];
	gtk_widget_hide (g_pReciteWord->chooseword->group_entry);
	gtk_widget_hide (g_pReciteWord->chooseword->group_s_label);
	gtk_widget_hide (g_pReciteWord->chooseword->scope_label);
	gtk_widget_hide (g_pReciteWord->chooseword->scopecount_label);
	gtk_widget_show (g_pReciteWord->chooseword->scope_entry);
	gtk_widget_show (g_pReciteWord->chooseword->scopecount_entry);
	gtk_widget_show (g_pReciteWord->chooseword->scope_s_label);
	sprintf (tmpstr, "%d", g_pReciteWord->chooseword->scope_start + 1);
	gtk_entry_set_text (GTK_ENTRY
			    (g_pReciteWord->chooseword->scope_entry), tmpstr);
	sprintf (tmpstr, "%d", g_pReciteWord->chooseword->scope_count);
	gtk_entry_set_text (GTK_ENTRY
			    (g_pReciteWord->chooseword->scopecount_entry),
			    tmpstr);
	gtk_widget_grab_focus (g_pReciteWord->chooseword->scopecount_entry);
}

static void
on_chooseword_byclass_clicked (gpointer data)
{
}

static void
on_chooseword_cancel_clicked (gpointer data)
{
	(*(g_pReciteWord->chooseword->func2)) ();
}

CChooseword::CChooseword ()
{
	skin_load_chooseword (g_pReciteWord->window);
}

CChooseword::~CChooseword ()
{
}

void
CChooseword::init ()
{
	scope_start = 0;
	scope_count = 0;
	total_zucount = get_total_zu_count ();
}

void
CChooseword::show (void (*ok_func) (), void (*cancel_func) (), struct chooseword_info *info)
{
	init ();

	func1 = ok_func;
	func2 = cancel_func;

	gchar tmpstr[256];

	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_size_request (fixed, Skin->chooseword.chooseword.w,
			      Skin->chooseword.chooseword.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,
		       (Skin->reciteword.width - Skin->chooseword.chooseword.w) / 2,
		       (Skin->reciteword.height -
			Skin->chooseword.chooseword.h) / 2);
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->chooseword.chooseword.p[0]);

	group_entry = gtk_entry_new ();
	//gtk_entry_set_has_frame(GTK_ENTRY(group_entry),false);
	gtk_widget_set_size_request (group_entry, Skin->chooseword.group_entry.w,
			      Skin->chooseword.group_entry.h);
	gtk_fixed_put (GTK_FIXED (fixed), group_entry,
		       Skin->chooseword.group_entry.x[0],
		       Skin->chooseword.group_entry.y[0]);
	g_signal_connect (G_OBJECT (group_entry), "activate",
			  G_CALLBACK (on_chooseword_entry_activate), this);
	gtk_widget_show (group_entry);
	sprintf (tmpstr, "1-%d", total_zucount);
	group_s_label = gtk_label_new (tmpstr);
	gtk_fixed_put (GTK_FIXED (fixed), group_s_label,
		       Skin->chooseword.group_s_label.x[0],
		       Skin->chooseword.group_s_label.y[0]);
	gtk_widget_show (group_s_label);

	scope_entry = gtk_entry_new ();
	//gtk_entry_set_has_frame(GTK_ENTRY(scope_entry),false);
	gtk_widget_set_size_request (scope_entry, Skin->chooseword.scope_entry.w,
			      Skin->chooseword.scope_entry.h);
	gtk_fixed_put (GTK_FIXED (fixed), scope_entry,
		       Skin->chooseword.scope_entry.x[0],
		       Skin->chooseword.scope_entry.y[0]);
	g_signal_connect (G_OBJECT (scope_entry), "activate",
			  G_CALLBACK (on_chooseword_entry_activate), this);
	scope_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed), scope_label,
		       Skin->chooseword.scope_entry.x[0],
		       Skin->chooseword.scope_entry.y[0]);
	gtk_widget_show (scope_label);
	scopecount_entry = gtk_entry_new ();
	//gtk_entry_set_has_frame(GTK_ENTRY(scopecount_entry),false);
	gtk_widget_set_size_request (scopecount_entry,
			      Skin->chooseword.scopecount_entry.w,
			      Skin->chooseword.scopecount_entry.h);
	gtk_fixed_put (GTK_FIXED (fixed), scopecount_entry,
		       Skin->chooseword.scopecount_entry.x[0],
		       Skin->chooseword.scopecount_entry.y[0]);
	g_signal_connect (G_OBJECT (scopecount_entry), "activate",
			  G_CALLBACK (on_chooseword_entry_activate), this);
	scopecount_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed), scopecount_label,
		       Skin->chooseword.scopecount_entry.x[0],
		       Skin->chooseword.scopecount_entry.y[0]);
	gtk_widget_show (scopecount_label);
	sprintf (tmpstr, "1-%d", g_pReciteWord->now_book->wordcount);
	scope_s_label = gtk_label_new (tmpstr);
	gtk_fixed_put (GTK_FIXED (fixed), scope_s_label,
		       Skin->chooseword.scope_s_label.x[0],
		       Skin->chooseword.scope_s_label.y[0]);

	bygroup_robutton.create (NULL, fixed,
				 Skin->chooseword.bygroup_robutton.x[0],
				 Skin->chooseword.bygroup_robutton.y[0],
				 Skin->chooseword.bygroup_robutton.p[0],
				 Skin->chooseword.bygroup_robutton.p[1],
				 Skin->chooseword.bygroup_robutton.p[2],
				 Skin->chooseword.bygroup_robutton.p[3],
				 on_chooseword_bygroup_clicked,NULL);
	byscope_robutton.create (bygroup_robutton.group, fixed,
				 Skin->chooseword.byscope_robutton.x[0],
				 Skin->chooseword.byscope_robutton.y[0],
				 Skin->chooseword.byscope_robutton.p[0],
				 Skin->chooseword.byscope_robutton.p[1],
				 Skin->chooseword.byscope_robutton.p[2],
				 Skin->chooseword.byscope_robutton.p[3],
				 on_chooseword_byscope_clicked,NULL);
	byclass_robutton.create (byscope_robutton.group, fixed,
				 Skin->chooseword.byclass_robutton.x[0],
				 Skin->chooseword.byclass_robutton.y[0],
				 Skin->chooseword.byclass_robutton.p[0],
				 Skin->chooseword.byclass_robutton.p[1],
				 Skin->chooseword.byclass_robutton.p[2],
				 Skin->chooseword.byclass_robutton.p[3],
				 on_chooseword_byclass_clicked,NULL);

	order_ckbutton.create (fixed, Skin->chooseword.order_ckbutton.x[0],
			       Skin->chooseword.order_ckbutton.y[0],
			       Skin->chooseword.order_ckbutton.p[0],
			       Skin->chooseword.order_ckbutton.p[1],
			       Skin->chooseword.order_ckbutton.p[2],
			       Skin->chooseword.order_ckbutton.p[3], NULL);
   	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "chooseword", "disorder",
			     &order);
	order_ckbutton.set_status(order);

	ok_button.create (fixed, Skin->chooseword.ok_button.x[0],
			  Skin->chooseword.ok_button.y[0],
			  Skin->chooseword.ok_button.p[0],
			  Skin->chooseword.ok_button.p[1], NULL,
			  on_chooseword_ok_clicked, NULL);
	cancel_button.create (fixed, Skin->chooseword.cancel_button.x[0],
			      Skin->chooseword.cancel_button.y[0],
			      Skin->chooseword.cancel_button.p[0],
			      Skin->chooseword.cancel_button.p[1], NULL,
			      on_chooseword_cancel_clicked, NULL);
	noclass_tip = gtk_image_new_from_pixmap (Skin->chooseword.noclass_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (fixed), noclass_tip,
		       Skin->chooseword.noclass_tip.x[0],
		       Skin->chooseword.noclass_tip.y[0]);
	scopeerr_tip = gtk_image_new_from_pixmap (Skin->chooseword.scopeerr_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (fixed), scopeerr_tip,
		       Skin->chooseword.scopeerr_tip.x[0],
		       Skin->chooseword.scopeerr_tip.y[0]);
		
	sprintf (tmpstr, "%d", info->zu_index + 1);
	gtk_entry_set_text (GTK_ENTRY (group_entry), tmpstr);
	
	//connect after gtk_entry_set_text,so this function will not call when set_text.
	g_signal_connect (G_OBJECT (group_entry), "changed",
			  G_CALLBACK (on_chooseword_groupentry_changed),
			  this);

	if (info->by==1) //by scope
	{
		byscope_robutton.set_selected(true);
		on_chooseword_byscope_clicked(NULL);
		sprintf (tmpstr, "%d", info->scope_start + 1);
		gtk_entry_set_text (GTK_ENTRY (scope_entry), tmpstr);
		sprintf (tmpstr, "%d", info->scope_count);
		gtk_entry_set_text (GTK_ENTRY (scopecount_entry), tmpstr);
	}
	else
	{
		on_chooseword_groupentry_changed(group_entry, this);
		gtk_widget_grab_focus (group_entry);
	}
}

void
CChooseword::close ()
{
	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "chooseword", "disorder",
			     &order);
	if (order!=order_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "chooseword", "disorder",
			     order_ckbutton.status);
	}

	if (tip_timer)
		g_source_remove (tip_timer);
	tip_timer = 0;

	bygroup_robutton.destroy ();
	byscope_robutton.destroy ();
	byclass_robutton.destroy ();
	order_ckbutton.destroy ();

	ok_button.destroy ();
	cancel_button.destroy ();

	gtk_widget_destroy (fixed);
	fixed = NULL;
}

void
CChooseword::do_up()
{
	if (!bygroup_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (byscope_robutton.selected)
		{
			bygroup_robutton.set_selected(true);
			on_chooseword_bygroup_clicked(NULL);
		}
		else
		{
			byscope_robutton.set_selected(true);
			on_chooseword_byscope_clicked(NULL);
		}
	}
}

void
CChooseword::do_down()
{
	if (!byclass_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (byscope_robutton.selected)
		{
			byclass_robutton.set_selected(true);
			on_chooseword_byclass_clicked(NULL);
		}
		else
		{
			byscope_robutton.set_selected(true);
			on_chooseword_byscope_clicked(NULL);
		}
	}
}
