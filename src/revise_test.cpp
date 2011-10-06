#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "revise.h"
#include "reciteword.h"
#include "skin.h"
#include <glib/gi18n.h>
#include "rwconfig.h"
#include "sound.h"
#include "readword.h"
#include <gdk/gdkkeysyms.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <string>
#include <fstream>

using namespace std;

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;

static gboolean
on_listitem3_button_press (GtkWidget * widget, GdkEvent * event,
			   tListItem3 * parent)
{
	if ((!parent->selected)&&(parent->can_select))
	{
		playsound (SND_BUTTONDOWN);
	}
	
	return TRUE;
}

static gboolean
on_listitem3_button_release (GtkWidget * widget, GdkEvent * event,
			     tListItem3 * parent)
{
	if ((!parent->selected)&&(parent->can_select))
	{
		playsound (SND_BUTTONUP);
	}
	if (parent->mousein)
	{
		if (!parent->selected)
		{
			if (!parent->can_select)
				return TRUE;
			(*(parent->runfunc)) (parent->func_data);
			parent->set_selected (TRUE);
		}
	}
	return TRUE;
}

static gboolean
on_listitem3_enter_notify (GtkWidget * widget, GdkEvent * event,
			   tListItem3 * parent)
{
	parent->mousein = TRUE;
	return true;
}

static gboolean
on_listitem3_leave_notify (GtkWidget * widget, GdkEvent * event,
			   tListItem3 * parent)
{
	parent->mousein = FALSE;
	return true;
}

tListItem3::tListItem3 ()
{
}

void
tListItem3::init ()
{
	selected = FALSE;
	can_select = TRUE;
	this->group = g_slist_prepend ((GSList *) NULL, this);
}

void
tListItem3::create (GSList * group, GtkWidget * pfixed, gint x, gint y,
		    gint w, gint h, GdkPixmap * pix_1, GdkPixmap * pix_2,
		    gint label_x1, gint label_y1, gint label_x2,
		    gint label_y2, void (*func) (int), int data)
{
	init ();

	pix_normal = pix_1;
	pix_selected = pix_2;
	runfunc = func;
	func_data = data;

	fixed = gtk_fixed_new ();
	gtk_widget_set_size_request (fixed, w, h);
	gtk_widget_set_events (fixed, GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
//      gtk_widget_set_app_paintable(fixed,TRUE);
	gtk_fixed_put (GTK_FIXED (pfixed), fixed, x, y);
	label1 = gtk_label_new ("");
	gtk_widget_set_name (label1, "revise_test-wordlist_item_label_1");
	gtk_fixed_put (GTK_FIXED (fixed), label1, label_x1, label_y1);
	GtkWidget *hbox =
		gtk_hbox_new (FALSE, Skin->revise_test.wordlist_item_normal.x[0]);
	label2 = gtk_label_new ("");
	gtk_widget_set_name (label2, "revise_test-wordlist_item_label_2");
	SetFont(label2,false);
	gtk_box_pack_start (GTK_BOX (hbox), label2, FALSE, FALSE, FALSE);
	pixmap = gtk_image_new_from_pixbuf (Skin->revise_test.wordlist_item_normal.p[0]);
	gtk_box_pack_start (GTK_BOX (hbox), pixmap, FALSE, FALSE, FALSE);
	gtk_fixed_put (GTK_FIXED (fixed), hbox, label_x2, label_y2);

	gtk_widget_show (label1);
	gtk_widget_show (label2);
	gtk_widget_show (pixmap);
	gtk_widget_show (hbox);
	gtk_widget_show (fixed);
	g_signal_connect (G_OBJECT (fixed), "button_release_event",
			  G_CALLBACK(on_listitem3_button_release), this);
	g_signal_connect (G_OBJECT (fixed), "button_press_event",
			  G_CALLBACK(on_listitem3_button_press), this);
	g_signal_connect (G_OBJECT (fixed), "enter_notify_event",
			  G_CALLBACK(on_listitem3_enter_notify), this);
	g_signal_connect (G_OBJECT (fixed), "leave_notify_event",
			  G_CALLBACK(on_listitem3_leave_notify), this);

	SetBackPixmap (fixed, pix_normal);
	gdk_window_set_cursor (fixed->window, Skin->reciteword.hand_cursor.cursor);

	if (group)
		set_group (group);
}

void
tListItem3::set_group (GSList * group)
{
	if (this->group)
	{
		GSList *slist;

		this->group = g_slist_remove (this->group, this);

		for (slist = this->group; slist; slist = slist->next)
		{
			tListItem3 *tmp_listitem;
			tmp_listitem = (tListItem3 *) slist->data;
			tmp_listitem->group = this->group;
		}
	}

	this->group = g_slist_prepend (group, this);

	if (group)
	{
		GSList *slist;

		for (slist = group; slist; slist = slist->next)
		{
			tListItem3 *tmp_listitem;
			tmp_listitem = (tListItem3 *) slist->data;

			tmp_listitem->group = this->group;
		}
	}

	set_selected (group == NULL);
}

void
tListItem3::set_can_select (int a)
{
	if (can_select != a)
	{
		if (a)
		{
			gdk_window_set_cursor (fixed->window,Skin->reciteword.hand_cursor.cursor);
		}
		else
		{
			gdk_window_set_cursor (fixed->window,Skin->reciteword.disable_cursor.cursor);
		}
		can_select = a;
	}
}

void
tListItem3::clean_selected ()
{
	GSList *tmp_list;
	tListItem3 *tmp_listitem;
	tmp_list = this->group;
	while (tmp_list)
	{
		tmp_listitem = (tListItem3 *) tmp_list->data;
		tmp_list = tmp_list->next;
		if (tmp_listitem->selected)
		{
			tmp_listitem->set_selected (FALSE);
			break;
		}
	}
}

void
tListItem3::set_selected (int a)
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
tListItem3::set_label (gint a, gchar * str)
{
	if (a == 1)
		gtk_label_set_text (GTK_LABEL (label1), str);
	else if (a == 2)
		gtk_label_set_text (GTK_LABEL (label2), str);
}

void
tListItem3::set_pixmap (TestStatus status)
{
	switch (status)
	{
	case S_not_tested:
		gtk_image_set_from_pixbuf (GTK_IMAGE (pixmap),
				Skin->revise_test.wordlist_item_normal.p[0]);
		break;
	case S_right:
	case S_right_2:
		gtk_image_set_from_pixbuf (GTK_IMAGE (pixmap),
				Skin->revise_test.wordlist_item_right.p[0]);
		break;
	case S_wrong:
		gtk_image_set_from_pixbuf (GTK_IMAGE (pixmap),
				Skin->revise_test.wordlist_item_wrong.p[0]);
		break;
	case S_right_1:
		gtk_image_set_from_pixbuf (GTK_IMAGE (pixmap),
				Skin->revise_test.wordlist_item_ok.p[0]);
		break;
	}
}

void
tListItem3::destroy ()
{
	GSList *tmp_list;
	tListItem3 *tmp_listitem;

	this->group = g_slist_remove (this->group, this);
	tmp_list = this->group;

	while (tmp_list)
	{
		tmp_listitem = (tListItem3 *) tmp_list->data;
		tmp_list = tmp_list->next;

		tmp_listitem->group = this->group;
	}

	gtk_widget_destroy (fixed);
}

tListItem3::~tListItem3 ()
{
}

static void
on_revise_test_dialog_ok_button_clicked (gpointer data)
{
	int i;
	if (g_pReciteWord->revise_test->dialog.skim_robutton.selected)
		i = 0;
	else if (g_pReciteWord->revise_test->dialog.group_robutton.selected)
		i = 1;
	else
		i = 2;
	g_pReciteWord->revise_test->dialog.close ();
	if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
		g_pReciteWord->mark->close ();
	if (i == 1)
	{
		g_pReciteWord->revise_test->close ();
		g_pReciteWord->ShowRevise_group ();
	}
	else if (i == 2)
	{
		/*g_pReciteWord->revise_test->close ();
		g_pReciteWord->show ();*/
	}
	else
	{
		g_pReciteWord->revise_test->close ();
		g_pReciteWord->ShowRevise_skim ();
	}
}

CRevise_test_Dialog::CRevise_test_Dialog ()
{
}

CRevise_test_Dialog::~CRevise_test_Dialog ()
{
}

void
CRevise_test_Dialog::show ()
{
	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_size_request (fixed, Skin->revise_test.dialog.w,
			      Skin->revise_test.dialog.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,Skin->revise_test.dialog.x[0], Skin->revise_test.dialog.y[0]);
	/*gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,
				   (Skin->reciteword.width - Skin->revise_test.dialog.w) / 2,
				   (Skin->reciteword.height -
				Skin->revise_test.dialog.h) / 2);*/
	gtk_widget_show (fixed);
	SetBackPixmap (fixed, Skin->revise_test.dialog.p[0]);

	skim_robutton.create (NULL, fixed,
			      Skin->revise_test.dialog_skim_robutton.x[0],
			      Skin->revise_test.dialog_skim_robutton.y[0],
			      Skin->revise_test.dialog_skim_robutton.p[0],
			      Skin->revise_test.dialog_skim_robutton.p[1],
			      Skin->revise_test.dialog_skim_robutton.p[2],
			      Skin->revise_test.dialog_skim_robutton.p[3], NULL,NULL);
	group_robutton.create (skim_robutton.group, fixed,
			      Skin->revise_test.dialog_group_robutton.x[0],
			      Skin->revise_test.dialog_group_robutton.y[0],
			      Skin->revise_test.dialog_group_robutton.p[0],
			      Skin->revise_test.dialog_group_robutton.p[1],
			      Skin->revise_test.dialog_group_robutton.p[2],
			      Skin->revise_test.dialog_group_robutton.p[3], NULL,NULL);
	testover_robutton.create (group_robutton.group, fixed,
				  Skin->revise_test.dialog_testover_robutton.x[0],
				  Skin->revise_test.dialog_testover_robutton.y[0],
				  Skin->revise_test.dialog_testover_robutton.
				  p[0],
				  Skin->revise_test.dialog_testover_robutton.
				  p[1],
				  Skin->revise_test.dialog_testover_robutton.
				  p[2],
				  Skin->revise_test.dialog_testover_robutton.
				  p[3], NULL,NULL);
   	gint tmp_i;
	rw_cfg_read_int(usercfgfile,
			     "revise_test", "dialog_index",
			     &tmp_i);
	if (tmp_i==1)
		group_robutton.set_selected(true);
	else if (tmp_i==2)
		testover_robutton.set_selected(true);

	ok_button.create (fixed, Skin->revise_test.dialog_ok_button.x[0],
			  Skin->revise_test.dialog_ok_button.y[0],
			  Skin->revise_test.dialog_ok_button.p[0],
			  Skin->revise_test.dialog_ok_button.p[1], NULL,
			  on_revise_test_dialog_ok_button_clicked, NULL);
}

void
CRevise_test_Dialog::close ()
{
	gint tmp_i,tmp_index;
	rw_cfg_read_int(usercfgfile,
			     "revise_test", "dialog_index",
			     &tmp_i);
	if (skim_robutton.selected)
		tmp_index = 0;
	else if (group_robutton.selected)
		tmp_index = 1;
	else
		tmp_index = 2;
	if (tmp_i!=tmp_index)
	{
		rw_cfg_write_int(usercfgfile,
					 "revise_test", "dialog_index",
					 tmp_index);
	}
	skim_robutton.destroy();
	group_robutton.destroy();
	testover_robutton.destroy();
	ok_button.destroy();
	gtk_widget_destroy (fixed);
	fixed = NULL;
}

void
CRevise_test_Dialog::do_up ()
{
	if (!skim_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (testover_robutton.selected)
			group_robutton.set_selected(true);
		else
			skim_robutton.set_selected(true);
	}
}

void
CRevise_test_Dialog::do_down ()
{
	if (!testover_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		if (skim_robutton.selected)
			group_robutton.set_selected(true);
		else
			testover_robutton.set_selected(true);
	}
}


static gint
revise_test_timecount_func (gpointer data)
{
	g_pReciteWord->revise_test->timecount++;
	return true;
}

static gint
revise_test_show_tip (gpointer widget)
{
	gtk_widget_hide (GTK_WIDGET (widget));
	g_pReciteWord->revise_test->tip_timer = 0;
	return FALSE;
}

static void do_revise_test_pause ()
{
		if (g_pReciteWord->revise_test->status==RTS_ANSWERING)
		{
			g_pReciteWord->revise_test->wordtest_entry.
				set_showpen (FALSE);
			g_pReciteWord->revise_test->wordtest_entry.
			set_editable (FALSE);
			g_pReciteWord->revise_test->handinpaper_button.set_enable (FALSE);
		}
		else if (g_pReciteWord->revise_test->status==RTS_CHECKING)
		{
			if (g_pReciteWord->revise_test->pressanykey_status == 1)
			{
				g_pReciteWord->revise_test->pressanykey_status =
					2;
				gtk_widget_hide (g_pReciteWord->revise_test->pressanykey_tip);
			}
			else		//pressanykey_stauts==0
			{
				if (g_pReciteWord->revise_test->checknext_timer)
				{
					g_source_remove (g_pReciteWord->
							    revise_test->
							    checknext_timer);
					g_pReciteWord->revise_test->checknext_timer = 0;
				}
			}
		}
		else if (g_pReciteWord->revise_test->status==RTS_TESTING)
		{
			if (g_pReciteWord->revise_test->pressanykey_status == 1)
			{
				g_pReciteWord->revise_test->pressanykey_status =
					2;
				gtk_widget_hide (g_pReciteWord->revise_test->pressanykey_tip);
			}
			else		//pressanykey_stauts==0
			{
				if (g_pReciteWord->revise_test->checknext_timer)
				{
					g_pReciteWord->revise_test->pressanykey_status = 3;
					g_source_remove (g_pReciteWord->
							    revise_test->
							    checknext_timer);
					g_pReciteWord->revise_test->checknext_timer = 0;
				}
				else
				{
					g_pReciteWord->revise_test->wordtest_entry.
						set_showpen (FALSE);
					g_pReciteWord->revise_test->wordtest_entry.
						set_editable (FALSE);
				}
			}
		}
		if (g_pReciteWord->revise_test->timecount_timer)
		{
			g_source_remove (g_pReciteWord->revise_test->timecount_timer);
			g_pReciteWord->revise_test->timecount_timer = 0;
		}
}

static void
on_revise_test_start_clicked ()
{
	if (g_pReciteWord->revise_test->started)
	{
		do_revise_test_pause ();
		g_pReciteWord->revise_test->started = FALSE;
		g_pReciteWord->revise_test->pause_button.set_enable (FALSE);
		g_pReciteWord->revise_test->start_button.reset (Skin->
								revise_test.
								start_button.
								p[0],
								Skin->
								revise_test.
								start_button.
								p[1],
								Skin->
								revise_test.
								start_button.
								p[2],
								(GdkPixmap *)
								NULL);
	}
	else
	{
		g_pReciteWord->revise_test->refresh_saverecord();
		g_pReciteWord->revise_test->save_setting();
		gboolean tmp_b;
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showmean_1",
			     &tmp_b);
		g_pReciteWord->revise_test->meaning_ckbutton.set_status(tmp_b);
		if (g_pReciteWord->revise_test->meaning_ckbutton.status)
			gtk_widget_show(g_pReciteWord->revise_test->wordtest_label_1);
		else
			gtk_widget_hide(g_pReciteWord->revise_test->wordtest_label_1);
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "readword_1",
			     &tmp_b);
		g_pReciteWord->revise_test->readword_ckbutton.set_status(tmp_b);
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showphonetic_1",
			     &tmp_b);
		g_pReciteWord->revise_test->phonetic_ckbutton.set_status(tmp_b);
		if (g_pReciteWord->revise_test->phonetic_ckbutton.status)
			gtk_widget_show(g_pReciteWord->revise_test->phonetic_label.hbox);
		else
			gtk_widget_hide(g_pReciteWord->revise_test->phonetic_label.hbox);
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showlength_1",
			     &tmp_b);
		g_pReciteWord->revise_test->length_ckbutton.set_status(tmp_b);
		g_pReciteWord->revise_test->wordtest_entry.set_showlength(tmp_b);
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showheadchar_1",
			     &tmp_b);
		g_pReciteWord->revise_test->headchar_ckbutton.set_status(tmp_b);
		if (g_pReciteWord->revise_test->headchar_ckbutton.status)
			gtk_widget_show(g_pReciteWord->revise_test->headchar_label);
		else
			gtk_widget_hide(g_pReciteWord->revise_test->headchar_label);
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "sndeffect_1",
			     &tmp_b);
		g_pReciteWord->revise_test->sndeffect_ckbutton.set_status(tmp_b);
		g_pReciteWord->revise_test->wordtest_entry.enablesnd = tmp_b;
		
		if (g_pReciteWord->revise_test->dialog.fixed)
		{
			g_pReciteWord->revise_test->dialog.close();
		}
		if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
			g_pReciteWord->mark->close ();
		g_pReciteWord->revise_test->status = RTS_ANSWERING;
		g_pReciteWord->revise_test->pressanykey_status = 0;
		g_pReciteWord->revise_test->test_status = 0;
		for (gint i = 0; i < (g_pReciteWord->now_zu_wordcount); i++)
		{
			if (g_pReciteWord->revise_test->tested_wordlist_item_str[i])
			{
				g_free (g_pReciteWord->revise_test->tested_wordlist_item_str[i]);
				g_pReciteWord->revise_test->tested_wordlist_item_str[i] = (gchar *) NULL;
			}
			g_pReciteWord->revise_test->tested_wordlist_item_status[i] = S_not_tested;
		}
		g_pReciteWord->revise_test->recited_word_count = 0;
		gtk_label_set_text(GTK_LABEL(g_pReciteWord->revise_test->recited_word_count_label)," 0");

		g_pReciteWord->revise_test->wordtest_entry.set_text ("");
		g_pReciteWord->revise_test->wordtest_entry.set_editable (TRUE);
		g_pReciteWord->revise_test->wordtest_entry.set_showpen (TRUE);
		g_pReciteWord->revise_test->wordtest_entry.show_cursor = TRUE;
		g_pReciteWord->revise_test->wordtest_entry.set_righttext (g_pReciteWord->now_zu_words.w[0]);

		((GtkAdjustment *) g_pReciteWord->revise_test->wordlist_adjust)->value = 0;
		((GtkAdjustment *) g_pReciteWord->revise_test->wordlist_adjust)->upper = g_pReciteWord->now_zu_wordcount;  //needed when chooseword again.
		g_signal_emit_by_name (g_pReciteWord->revise_test->
					 wordlist_adjust, "changed");
		
		g_pReciteWord->revise_test->wordlist_item[0].set_can_select (1);
		g_pReciteWord->revise_test->wordlist_item[1].set_can_select (1);
		g_pReciteWord->revise_test->wordlist_item[2].set_can_select (1);
		g_pReciteWord->revise_test->wordlist_item[0].set_pixmap (S_not_tested);
		g_pReciteWord->revise_test->wordlist_item[1].set_pixmap (S_not_tested);
		g_pReciteWord->revise_test->wordlist_item[2].set_pixmap (S_not_tested);
		g_pReciteWord->revise_test->selected_wordlist_item = 0;
		g_pReciteWord->revise_test->set_wordlist_item (0);
		g_pReciteWord->revise_test->set_wordtest_label (0);
		gtk_label_set_text (GTK_LABEL(g_pReciteWord->revise_test->right_label),"");
		g_pReciteWord->revise_test->wordlist_item[0].set_selected (1);
		g_pReciteWord->revise_test->handinpaper_button.set_enable (TRUE);

		g_pReciteWord->revise_test->pause_button.set_enable (TRUE);
		g_pReciteWord->revise_test->start_button.reset (Skin->
								revise_test.
								stop_button.
								p[0],
								Skin->
								revise_test.
								stop_button.
								p[1],
								Skin->
								revise_test.
								stop_button.
								p[2],
								Skin->
								revise_test.
								stop_button.
								p[3]);
		g_pReciteWord->revise_test->started = TRUE;
		gtk_widget_hide (g_pReciteWord->revise_test->right_tip);
		gtk_widget_show (g_pReciteWord->revise_test->wordtest_tip);
		g_pReciteWord->revise_test->tip_timer =
			g_timeout_add (2000, revise_test_show_tip,
					 g_pReciteWord->revise_test->
					 wordtest_tip);
		g_pReciteWord->revise_test->timecount = 0;
		g_pReciteWord->revise_test->timecount_timer = g_timeout_add (1000, revise_test_timecount_func, NULL);
	}
}

static gint revise_test_check_next_word (gpointer data);

static void
on_revise_test_pause_clicked ()
{
	if (g_pReciteWord->revise_test->started)
	{
		do_revise_test_pause ();
		g_pReciteWord->revise_test->start_button.
			set_enable (FALSE);
		g_pReciteWord->revise_test->pause_button.reset (Skin->
								     revise_test.
								     continue_button.
								     p[0],
								     Skin->
								     revise_test.
								     continue_button.
								     p[1],
								     Skin->
								     revise_test.
								     continue_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->revise_test->started = FALSE;
	}
	else
	{
		if (g_pReciteWord->revise_test->status==RTS_ANSWERING)
		{
			g_pReciteWord->revise_test->wordtest_entry.
				set_showpen (TRUE);
			g_pReciteWord->revise_test->wordtest_entry.
			set_editable (TRUE);
			g_pReciteWord->revise_test->handinpaper_button.set_enable (TRUE);
		}
		else if (g_pReciteWord->revise_test->status==RTS_CHECKING)
		{
			if (g_pReciteWord->revise_test->pressanykey_status == 2)
			{
				g_pReciteWord->revise_test->pressanykey_status =
					1;
				gtk_widget_show (g_pReciteWord->revise_test->pressanykey_tip);
			}
			else		//pressanykey_stauts==0
			{
				if (!g_pReciteWord->revise_test->checknext_timer)
				{
					g_pReciteWord->revise_test->checknext_timer =
						g_timeout_add (1000,
							 revise_test_check_next_word,
							 NULL);
				}
			}
		}
		else if (g_pReciteWord->revise_test->status==RTS_TESTING)
		{
			if (g_pReciteWord->revise_test->pressanykey_status == 2)
			{
				g_pReciteWord->revise_test->pressanykey_status =
					1;
				gtk_widget_show (g_pReciteWord->revise_test->pressanykey_tip);
			}
			else if (g_pReciteWord->revise_test->pressanykey_status == 3) //will select next 1 second after,but paused.
			{
				g_pReciteWord->revise_test->pressanykey_status = 0;
				g_pReciteWord->revise_test->checknext_timer =
						g_timeout_add (1000,
							 revise_test_check_next_word,
							 NULL);
			}
			else		//pressanykey_stauts==0
			{
				g_pReciteWord->revise_test->wordtest_entry.
					set_showpen (TRUE);
				g_pReciteWord->revise_test->wordtest_entry.
					set_editable (TRUE);
			}
		}

		g_pReciteWord->revise_test->start_button.
			set_enable (TRUE);
		g_pReciteWord->revise_test->pause_button.reset (Skin->
								     revise_test.
								     pause_button.
								     p[0],
								     Skin->
								     revise_test.
								     pause_button.
								     p[1],
								     Skin->
								     revise_test.
								     pause_button.
								     p[2],
								     Skin->
								     revise_test.
								     pause_button.
								     p[3]);
		g_pReciteWord->revise_test->started = TRUE;
		if (!g_pReciteWord->revise_test->timecount_timer)
			g_pReciteWord->revise_test->timecount_timer = g_timeout_add (1000, revise_test_timecount_func, NULL);
	}
}

static void
on_revise_test_group_clicked ()
{
	g_pReciteWord->revise_test->close ();
	g_pReciteWord->ShowRevise_group ();
}

static void
on_revise_test_skim_clicked ()
{
	g_pReciteWord->revise_test->close ();
	g_pReciteWord->ShowRevise_skim ();
}

static void
on_revise_test_return_clicked ()
{
	g_pReciteWord->revise_test->close ();
	g_pReciteWord->show ();
}

static void revise_test_select_next_word ();

static gint
revise_test_check_next_word (gpointer data)
{
	g_pReciteWord->revise_test->checknext_timer = 0;
	revise_test_select_next_word ();
	return FALSE;
}

static void
revise_test_selected_next_refresh ()
{
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->revise_test->
		  wordlist_adjust)->value);
	if ((g_pReciteWord->revise_test->selected_wordlist_item) >=
	    (nowvalue + 3)
	    || ((g_pReciteWord->revise_test->selected_wordlist_item) <
		nowvalue))
	{
		if (g_pReciteWord->revise_test->selected_wordlist_item<=2)
			nowvalue = 0;
		else
			nowvalue = g_pReciteWord->revise_test->selected_wordlist_item-2;
		((GtkAdjustment *) g_pReciteWord->revise_test->
		 wordlist_adjust)->value = nowvalue;
		g_pReciteWord->revise_test->set_wordlist_item (nowvalue);
		g_signal_emit_by_name (g_pReciteWord->revise_test->
					 wordlist_adjust, "changed");
	}

	g_pReciteWord->revise_test->
			wordlist_item[((g_pReciteWord->revise_test->
					selected_wordlist_item) -
				       nowvalue)].set_selected (1);
	g_pReciteWord->revise_test->set_wordtest_label (g_pReciteWord->
							revise_test->
							selected_wordlist_item);
}

static void
revise_test_select_next_word ()
{
	int nowvalue;
	switch (g_pReciteWord->revise_test->status)
	{
	case RTS_ANSWERING:		// begin test
		if (g_pReciteWord->revise_test->selected_wordlist_item >=
		    g_pReciteWord->now_zu_wordcount - 1)
		{
			return;
		}
		g_pReciteWord->revise_test->selected_wordlist_item++;
		revise_test_selected_next_refresh ();
		if (g_pReciteWord->revise_test->
		    tested_wordlist_item_str[g_pReciteWord->revise_test->
					     selected_wordlist_item])
		{
			g_pReciteWord->revise_test->wordtest_entry.
				set_text (g_pReciteWord->revise_test->
					  tested_wordlist_item_str
					  [g_pReciteWord->revise_test->
					   selected_wordlist_item]);
			g_pReciteWord->revise_test->wordtest_entry.position =
				strlen (g_pReciteWord->revise_test->
					tested_wordlist_item_str
					[g_pReciteWord->revise_test->
					 selected_wordlist_item]);
		}
		else
		{
			g_pReciteWord->revise_test->wordtest_entry.
				set_text ("");
		}
		g_pReciteWord->revise_test->wordtest_entry.
			set_righttext (g_pReciteWord->now_zu_words.
				       w[g_pReciteWord->revise_test->
					 selected_wordlist_item]);
		break;
	case RTS_CHECKING:		//after handin paper,checking
		if (g_pReciteWord->revise_test->selected_wordlist_item >=
		    g_pReciteWord->now_zu_wordcount - 1)
		{
			g_pReciteWord->revise_test->save_setting();
			gboolean tmp_b;
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showmean_2",
			     &tmp_b);
			g_pReciteWord->revise_test->meaning_ckbutton.set_status(tmp_b);
			if (g_pReciteWord->revise_test->meaning_ckbutton.status)
				gtk_widget_show(g_pReciteWord->revise_test->wordtest_label_1);
			else
				gtk_widget_hide(g_pReciteWord->revise_test->wordtest_label_1);
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "readword_2",
			     &tmp_b);
			g_pReciteWord->revise_test->readword_ckbutton.set_status(tmp_b);
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showphonetic_2",
			     &tmp_b);
			g_pReciteWord->revise_test->phonetic_ckbutton.set_status(tmp_b);
			if (g_pReciteWord->revise_test->phonetic_ckbutton.status)
				gtk_widget_show(g_pReciteWord->revise_test->phonetic_label.hbox);
			else
				gtk_widget_hide(g_pReciteWord->revise_test->phonetic_label.hbox);
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showlength_2",
			     &tmp_b);
			g_pReciteWord->revise_test->length_ckbutton.set_status(tmp_b);
			g_pReciteWord->revise_test->wordtest_entry.set_showlength(tmp_b);
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showheadchar_2",
			     &tmp_b);
			g_pReciteWord->revise_test->headchar_ckbutton.set_status(tmp_b);
			if (g_pReciteWord->revise_test->headchar_ckbutton.status)
				gtk_widget_show(g_pReciteWord->revise_test->headchar_label);
			else
				gtk_widget_hide(g_pReciteWord->revise_test->headchar_label);
			rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "sndeffect_2",
			     &tmp_b);
			g_pReciteWord->revise_test->sndeffect_ckbutton.set_status(tmp_b);
			g_pReciteWord->revise_test->wordtest_entry.enablesnd = tmp_b;

			g_pReciteWord->revise_test->status = RTS_TESTING;			
			revise_test_select_next_word ();
			return;
		}
		g_pReciteWord->revise_test->selected_wordlist_item++;
		revise_test_selected_next_refresh ();
		if (g_pReciteWord->revise_test->
		    tested_wordlist_item_str[g_pReciteWord->revise_test->
					     selected_wordlist_item])
			g_pReciteWord->revise_test->wordtest_entry.
				set_text (g_pReciteWord->revise_test->
					  tested_wordlist_item_str
					  [g_pReciteWord->revise_test->
					   selected_wordlist_item]);
		else
			g_pReciteWord->revise_test->wordtest_entry.
				set_text ("");
		g_pReciteWord->revise_test->wordtest_entry.
			check_text (g_pReciteWord->now_zu_words.
				    w[g_pReciteWord->revise_test->
				      selected_wordlist_item],SND_NULL,SND_ANSWERNO2);
		gtk_label_set_text (GTK_LABEL
				    (g_pReciteWord->revise_test->right_label),
				    g_pReciteWord->now_zu_words.
				    w[g_pReciteWord->revise_test->
				      selected_wordlist_item]);
		nowvalue =
			((gint)
			 ((GtkAdjustment *) g_pReciteWord->revise_test->
			  wordlist_adjust)->value);
		if ((g_pReciteWord->revise_test->
		     tested_wordlist_item_str[g_pReciteWord->revise_test->
					      selected_wordlist_item])
		    &&
		    (strcmp
		     (g_strstrip
		      (g_pReciteWord->revise_test->
		       tested_wordlist_item_str[g_pReciteWord->revise_test->
						selected_wordlist_item]),
		      g_pReciteWord->now_zu_words.w[g_pReciteWord->
						    revise_test->
						    selected_wordlist_item])
		     == 0))
		{
			g_pReciteWord->revise_test->tested_wordlist_item_status[g_pReciteWord->revise_test->selected_wordlist_item] = S_right;	//right
			g_pReciteWord->revise_test->recited_word_count ++;
			gchar aa[5];
			sprintf(aa,"%2d",g_pReciteWord->revise_test->recited_word_count);
			gtk_label_set_text(GTK_LABEL(g_pReciteWord->revise_test->recited_word_count_label),aa);
			g_pReciteWord->revise_test->
				wordlist_item[((g_pReciteWord->revise_test->
						selected_wordlist_item) -
					       nowvalue)].set_pixmap (S_right);
			g_pReciteWord->revise_test->checknext_timer =
				g_timeout_add (1000,
						 revise_test_check_next_word,
						 NULL);
		}
		else
		{
			g_pReciteWord->revise_test->tested_wordlist_item_status[g_pReciteWord->revise_test->selected_wordlist_item] = S_wrong;	//wrong
			g_pReciteWord->revise_test->
				wordlist_item[((g_pReciteWord->revise_test->
						selected_wordlist_item) -
					       nowvalue)].set_pixmap (S_wrong);
			gtk_widget_show (g_pReciteWord->revise_test->pressanykey_tip);
			gtk_widget_show (g_pReciteWord->revise_test->right_tip);
			g_pReciteWord->revise_test->pressanykey_status = 1;
		}
		break;
	case RTS_TESTING:		//after handinpaper and check over,begain test
		do
		{
			g_pReciteWord->revise_test->selected_wordlist_item++;
			if (g_pReciteWord->revise_test->
			    selected_wordlist_item >=
			    g_pReciteWord->now_zu_wordcount)
			{
				int i, b;
				b = 1;
				for (i = 0;
				     i < g_pReciteWord->now_zu_wordcount; i++)
				{
					if (!((g_pReciteWord->revise_test->
					    tested_wordlist_item_status[i] ==
					    S_right) || (g_pReciteWord->revise_test->
					    tested_wordlist_item_status[i] ==
					    S_right_2)))
					{
						b = 0;
						break;
					}
				}
				if (b)
				{
					on_revise_test_start_clicked ();	//stop

					if (g_pReciteWord->now_zu_index >= 0) // when chooseword by scope,now_zu_index==-1,so should not set to by group & zu_index.
					{
						g_pReciteWord->revise_test_chooseword_info.by = g_pReciteWord->revise_skim_chooseword_info.by = g_pReciteWord->shooting_chooseword_info.by = g_pReciteWord->typing_chooseword_info.by = 0; // by group
						g_pReciteWord->shooting_chooseword_info.zu_index = g_pReciteWord->typing_chooseword_info.zu_index = g_pReciteWord->now_zu_index;
						g_pReciteWord->revise_test_chooseword_info.zu_index = g_pReciteWord->revise_skim_chooseword_info.zu_index = g_pReciteWord->now_zu_index+1;
					}
					else // now_zu_index == -1,it must be set by chooseword dialog's by_scope.
					{
						g_pReciteWord->revise_test_chooseword_info.by = g_pReciteWord->revise_skim_chooseword_info.by = 1; // by scope
						g_pReciteWord->revise_test_chooseword_info.scope_start = g_pReciteWord->revise_skim_chooseword_info.scope_start = g_pReciteWord->chooseword->last_scope_start + g_pReciteWord->chooseword->last_scope_count;
						g_pReciteWord->revise_test_chooseword_info.scope_count = g_pReciteWord->revise_skim_chooseword_info.scope_count = g_pReciteWord->chooseword->last_scope_count;
					}
					if (g_pReciteWord->revise_test->save_record)
					{
						gchar tmpstr[256];
						sprintf (tmpstr, "zu%d_revisetimes",
							 g_pReciteWord->now_zu_index);
						gint revise_times;
						rw_cfg_read_int (usercfgfile,
								 g_pReciteWord->
								 now_book->
								 bookfilename, tmpstr,
								 &revise_times);
						revise_times++;
						rw_cfg_write_int (usercfgfile,
								  g_pReciteWord->
								  now_book->
								  bookfilename,
								  tmpstr,
								  revise_times);
						sprintf (tmpstr, "zu%d_time%d",
							 g_pReciteWord->now_zu_index,
							 revise_times);
						std::time_t now = std::time (0);
						rw_cfg_write_time (usercfgfile,
								   g_pReciteWord->
								   now_book->
								   bookfilename,
								   tmpstr, now);
						gint wrongcount=0;
						for (int i = 0;i < g_pReciteWord->now_zu_wordcount; i++)
						{
							if (g_pReciteWord->revise_test->tested_wordlist_item_status[i] == S_right_2)
							{
								wrongcount++;
							}
						}
						sprintf (tmpstr, "zu%d_time%d_wrongcount",
							 g_pReciteWord->now_zu_index,
							 revise_times);
						rw_cfg_write_int (usercfgfile,
								  g_pReciteWord->
								  now_book->
								  bookfilename,
								  tmpstr,
								  wrongcount);
						glong totalwordcount;
						rw_cfg_read_long(usercfgfile,"revise_test","totalwordcount",&totalwordcount);
						totalwordcount+= g_pReciteWord->now_zu_wordcount;
						rw_cfg_write_long(usercfgfile,"revise_test","totalwordcount",totalwordcount);
								   
						if (revise_times==g_pReciteWord->revise_group->max_revise_times)  // only save hard words  at the last revise time.
						{
							g_pReciteWord->revise_test->save_hard_words();
						}
						conf_save_usr();  //save to disk.
					}
					g_pReciteWord->PopupMark();
					g_pReciteWord->revise_test->dialog.show ();
					return;
				}
				b = true;
				for (i = 0;
				     i < g_pReciteWord->now_zu_wordcount; i++)
				{
					if ((g_pReciteWord->revise_test->
					     tested_wordlist_item_status[i] ==
					     S_not_tested)
					    || (g_pReciteWord->revise_test->
						tested_wordlist_item_status[i]
						== S_wrong))
					{
						b = false;
						break;
					}
				}
				if (b)
				{
					if (g_pReciteWord->revise_test->
					    test_status == 1)
					{
						gtk_widget_show
							(g_pReciteWord->
							 revise_test->
							 secondtest_tip);
						g_pReciteWord->revise_test->
							tip_timer =
							g_timeout_add (2000,
									 revise_test_show_tip,
									 g_pReciteWord->
									 revise_test->
									 secondtest_tip);
						g_pReciteWord->revise_test->
							test_status = 2;
					}
				}
				else
				{
					if (g_pReciteWord->revise_test->
					    test_status == 0)
					{
						gtk_label_set_text (GTK_LABEL
								    (g_pReciteWord->
								     revise_test->
								     right_label),
								    "");
						g_pReciteWord->revise_test->
							wordtest_entry.
							show_cursor = TRUE;
						gtk_widget_show
							(g_pReciteWord->
							 revise_test->
							 firsttest_tip);
						g_pReciteWord->revise_test->
							tip_timer =
							g_timeout_add (2000,
									 revise_test_show_tip,
									 g_pReciteWord->
									 revise_test->
									 firsttest_tip);
						g_pReciteWord->revise_test->
							test_status = 1;
					}
				}
				g_pReciteWord->revise_test->
					selected_wordlist_item = 0;
			}
		}
		while ((g_pReciteWord->revise_test->
			tested_wordlist_item_status[g_pReciteWord->
						    revise_test->
						    selected_wordlist_item] ==
			S_right) || (g_pReciteWord->revise_test->
			tested_wordlist_item_status[g_pReciteWord->
						    revise_test->
						    selected_wordlist_item] ==
			S_right_2) || ((g_pReciteWord->revise_test->test_status == 1)
			       && (g_pReciteWord->revise_test->
				   tested_wordlist_item_status[g_pReciteWord->
							       revise_test->
							       selected_wordlist_item]
				   == S_right_1)));
		revise_test_selected_next_refresh ();
		if (g_pReciteWord->revise_test->headchar_ckbutton.status)  //it will be hide if precious answer is right
			gtk_widget_show(g_pReciteWord->revise_test->headchar_label);
		g_pReciteWord->revise_test->wordtest_entry.show_cursor = TRUE;
		g_pReciteWord->revise_test->wordtest_entry.set_text ("");
		g_pReciteWord->revise_test->wordtest_entry.
			set_righttext (g_pReciteWord->now_zu_words.
				       w[g_pReciteWord->revise_test->
					 selected_wordlist_item]);
		break;
	}
}

static gint
on_test_wordtestentry_keypress (gint calltime, gint keyval)
{
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->revise_test->
		  wordlist_adjust)->value);

	gchar *entry_text;
	entry_text = g_pReciteWord->revise_test->wordtest_entry.get_text ();

	switch (g_pReciteWord->revise_test->status)
	{
	case RTS_ANSWERING:		// begain test
		if ((calltime == 0) && (keyval == GDK_Return))
		{
			if (g_pReciteWord->revise_test->
			    selected_wordlist_item >=
			    g_pReciteWord->now_zu_wordcount - 1)
			{				
				playsound(SND_DING);
				if (g_pReciteWord->revise_test->tip_timer)
				{
					g_source_remove (g_pReciteWord->
							    revise_test->
							    tip_timer);
				}
				gtk_widget_show (g_pReciteWord->revise_test->
						 handinpaper_tip);
				g_pReciteWord->revise_test->tip_timer =
					g_timeout_add (2000,
							 revise_test_show_tip,
							 g_pReciteWord->
							 revise_test->
							 handinpaper_tip);
			}
			else
				revise_test_select_next_word ();
			return TRUE;
		}

		if ((calltime == 0) && (keyval == GDK_Down))
		{
			revise_test_select_next_word ();
			return TRUE;
		}

		if ((calltime == 0) && (keyval == GDK_Up))
		{
			if (g_pReciteWord->revise_test->
			    selected_wordlist_item != 0)
			{
				g_pReciteWord->revise_test->
					selected_wordlist_item--;
	
				if ((g_pReciteWord->revise_test->selected_wordlist_item) >=	(nowvalue + 3)
					|| ((g_pReciteWord->revise_test->selected_wordlist_item) <nowvalue) )
				{
					nowvalue = g_pReciteWord->revise_test->selected_wordlist_item;
					if (nowvalue > g_pReciteWord->now_zu_wordcount - 3)
						nowvalue = g_pReciteWord->now_zu_wordcount - 3;
					((GtkAdjustment *) g_pReciteWord->revise_test->
					 wordlist_adjust)->value = nowvalue;
					g_pReciteWord->revise_test->set_wordlist_item (nowvalue);
					g_signal_emit_by_name (g_pReciteWord->revise_test->
							 wordlist_adjust, "changed");
				}
				g_pReciteWord->revise_test->
					wordlist_item[((g_pReciteWord->revise_test->
					selected_wordlist_item) -
				       nowvalue)].set_selected (1);
				g_pReciteWord->revise_test->set_wordtest_label (g_pReciteWord->
							revise_test->
							selected_wordlist_item);	
				if (g_pReciteWord->revise_test->
				    tested_wordlist_item_str[g_pReciteWord->
							     revise_test->
							     selected_wordlist_item])
				{
					g_pReciteWord->revise_test->
						wordtest_entry.
						set_text (g_pReciteWord->
							  revise_test->
							  tested_wordlist_item_str
							  [g_pReciteWord->
							   revise_test->
							   selected_wordlist_item]);
					g_pReciteWord->revise_test->
						wordtest_entry.position =
						strlen (g_pReciteWord->
							revise_test->
							tested_wordlist_item_str
							[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]);
				}
				else
				{
					g_pReciteWord->revise_test->
						wordtest_entry.set_text ("");
				}
				g_pReciteWord->revise_test->wordtest_entry.
					set_righttext (g_pReciteWord->
						       now_zu_words.
						       w[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]);
			}
			return TRUE;
		}

		if ((calltime == 0) && ((keyval == GDK_Page_Up)||(keyval == GDK_Page_Down)))
		{
			if ((g_pReciteWord->revise_test->selected_wordlist_item) >=	(nowvalue + 3)
					|| ((g_pReciteWord->revise_test->selected_wordlist_item) <nowvalue) )
			{
					nowvalue = g_pReciteWord->revise_test->selected_wordlist_item;
					if (nowvalue > g_pReciteWord->now_zu_wordcount - 3)
						nowvalue = g_pReciteWord->now_zu_wordcount - 3;
					((GtkAdjustment *) g_pReciteWord->revise_test->
					 wordlist_adjust)->value = nowvalue;
					g_pReciteWord->revise_test->set_wordlist_item (nowvalue);
					g_signal_emit_by_name (g_pReciteWord->revise_test->
							 wordlist_adjust, "changed");
					g_pReciteWord->revise_test->
						wordlist_item[((g_pReciteWord->revise_test->
							selected_wordlist_item) -
							   nowvalue)].set_selected (1);
					return true;
			}
			if (keyval == GDK_Page_Up)
			{
				if (g_pReciteWord->revise_test->selected_wordlist_item == 0)
					return true;
				if (g_pReciteWord->revise_test->selected_wordlist_item==nowvalue)
					g_pReciteWord->revise_test->selected_wordlist_item-=2;
				else
					g_pReciteWord->revise_test->selected_wordlist_item=nowvalue;
				if (g_pReciteWord->revise_test->selected_wordlist_item < 0)
					g_pReciteWord->revise_test->selected_wordlist_item =0;
			}
			else  //GDK_Pagedown
			{
				if (g_pReciteWord->revise_test->selected_wordlist_item >= g_pReciteWord->now_zu_wordcount - 1)
					return true;
				if (g_pReciteWord->revise_test->selected_wordlist_item==nowvalue+2)
					g_pReciteWord->revise_test->selected_wordlist_item+=2;
				else
					g_pReciteWord->revise_test->selected_wordlist_item=nowvalue+2;
				if (g_pReciteWord->revise_test->selected_wordlist_item > g_pReciteWord->now_zu_wordcount - 1)
					g_pReciteWord->revise_test->selected_wordlist_item = g_pReciteWord->now_zu_wordcount - 1;
			}
	
			if ((g_pReciteWord->revise_test->selected_wordlist_item) >=	(nowvalue + 3)
					|| ((g_pReciteWord->revise_test->selected_wordlist_item) <nowvalue) )
			{
				if (keyval == GDK_Page_Up)
				{
					nowvalue = g_pReciteWord->revise_test->selected_wordlist_item;
				}
				else
				{
					nowvalue = g_pReciteWord->revise_test->selected_wordlist_item -2;
				}					
				((GtkAdjustment *) g_pReciteWord->revise_test->
					 wordlist_adjust)->value = nowvalue;
				g_pReciteWord->revise_test->set_wordlist_item (nowvalue);
				g_signal_emit_by_name (g_pReciteWord->revise_test->
							 wordlist_adjust, "changed");
			}
			g_pReciteWord->revise_test->
					wordlist_item[((g_pReciteWord->revise_test->
					selected_wordlist_item) -
				       nowvalue)].set_selected (1);
			g_pReciteWord->revise_test->set_wordtest_label (g_pReciteWord->
							revise_test->
							selected_wordlist_item);	
			if (g_pReciteWord->revise_test->
				    tested_wordlist_item_str[g_pReciteWord->
							     revise_test->
							     selected_wordlist_item])
			{
				g_pReciteWord->revise_test->
						wordtest_entry.
						set_text (g_pReciteWord->
							  revise_test->
							  tested_wordlist_item_str
							  [g_pReciteWord->
							   revise_test->
							   selected_wordlist_item]);
				g_pReciteWord->revise_test->
						wordtest_entry.position =
						strlen (g_pReciteWord->
							revise_test->
							tested_wordlist_item_str
							[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]);
			}
			else
			{
				g_pReciteWord->revise_test->
						wordtest_entry.set_text ("");
			}
			g_pReciteWord->revise_test->wordtest_entry.
					set_righttext (g_pReciteWord->
						       now_zu_words.
						       w[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]);			
			return TRUE;
		}
		
		if (calltime != 1)
			return FALSE;

		if (!g_pReciteWord->revise_test->
		    tested_wordlist_item_str[g_pReciteWord->revise_test->
					     selected_wordlist_item])
		{
			g_pReciteWord->revise_test->
				tested_wordlist_item_str[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]
				= (gchar *) g_malloc (256);
		}
		strcpy (g_pReciteWord->revise_test->
			tested_wordlist_item_str[g_pReciteWord->revise_test->
						 selected_wordlist_item],
			entry_text);

		if (((g_pReciteWord->revise_test->selected_wordlist_item) >=
		     nowvalue)
		    && ((g_pReciteWord->revise_test->selected_wordlist_item) <
			(nowvalue + 3)))
			g_pReciteWord->revise_test->
				wordlist_item[((g_pReciteWord->revise_test->
						selected_wordlist_item) -
					       nowvalue)].set_label (2,
								     entry_text);

		return FALSE;
		break;
	case RTS_CHECKING:		//after handinpaper
		if (calltime != 0)
			return TRUE;
		if (g_pReciteWord->revise_test->pressanykey_status==1)  // if =2,then it is paused
		{
			if (((keyval >= 0x20) && (keyval <= 0xFF))
			    || (keyval == GDK_Return))
			{
				gtk_widget_hide (g_pReciteWord->revise_test->pressanykey_tip);
				gtk_widget_hide (g_pReciteWord->revise_test->right_tip);
				g_pReciteWord->revise_test->
					pressanykey_status = 0;
				revise_test_select_next_word ();
			}
			return TRUE;
		}
		else if (g_pReciteWord->revise_test->pressanykey_status==0)
		{
			if (((keyval >= 0x20) && (keyval <= 0xFF))
			    || (keyval == GDK_Return))
			{
				if (g_pReciteWord->revise_test->
				    checknext_timer)
				{
					g_source_remove (g_pReciteWord->
							    revise_test->
							    checknext_timer);
					g_pReciteWord->revise_test->
						checknext_timer = 0;
					revise_test_select_next_word ();
				}
			}
		}
		return TRUE;
		break;
	case RTS_TESTING:		//handinpaper's check is over,and begain test
		if (calltime != 0)
			return FALSE;
		if (!g_pReciteWord->revise_test->started)
			return true;
		if (g_pReciteWord->revise_test->pressanykey_status==1)
		{
			if (((keyval >= 0x20) && (keyval <= 0xFF))
			    || (keyval == GDK_Return))
			{
				gtk_widget_hide (g_pReciteWord->revise_test->pressanykey_tip);
				gtk_widget_hide (g_pReciteWord->revise_test->right_tip);
				g_pReciteWord->revise_test->
					pressanykey_status = 0;
				gtk_label_set_text (GTK_LABEL
						    (g_pReciteWord->
						     revise_test->
						     right_label), "");
				revise_test_select_next_word ();
			}
			return TRUE;
		}
		if (g_pReciteWord->revise_test->pressanykey_status==2) // paused
		{
			return TRUE;
		}
		if (g_pReciteWord->revise_test->pressanykey_status==3) // paused when will select next item 1 second latter.
		{
			return TRUE;
		}
		if ((g_pReciteWord->revise_test->pressanykey_status==0) // precious answer is right,will select next after 1 second.if get key,no wait.
			&& (g_pReciteWord->revise_test->checknext_timer))
		{
			if (((keyval >= 0x20) && (keyval <= 0xFF))
			    || (keyval == GDK_Return))
			{
				g_source_remove (g_pReciteWord->
							    revise_test->
							    checknext_timer);
				g_pReciteWord->revise_test->
						checknext_timer = 0;
				revise_test_select_next_word ();
			}
			return TRUE;
		}
		if (keyval == GDK_Return)
		{
			g_pReciteWord->revise_test->wordtest_entry.
				check_text (g_pReciteWord->now_zu_words.
					    w[g_pReciteWord->revise_test->
					      selected_wordlist_item],SND_ANSWEROK,SND_ANSWERNO);
			if (strcmp
			    (entry_text,
			     g_pReciteWord->now_zu_words.w[g_pReciteWord->
							   revise_test->
							   selected_wordlist_item])
			    == 0)
			{
				if (g_pReciteWord->revise_test->
				    tested_wordlist_item_status
				    [g_pReciteWord->revise_test->
				     selected_wordlist_item] == S_right_1)
				{
					if (!g_pReciteWord->revise_test->
					    tested_wordlist_item_str
					    [g_pReciteWord->revise_test->
					     selected_wordlist_item])
					{
						g_pReciteWord->revise_test->
							tested_wordlist_item_str
							[g_pReciteWord->
							 revise_test->
							 selected_wordlist_item]
							=
							(gchar *)
							g_malloc (256);
					}
					strcpy (g_pReciteWord->revise_test->
						tested_wordlist_item_str
						[g_pReciteWord->revise_test->
						 selected_wordlist_item],
						entry_text);
					if (((g_pReciteWord->revise_test->selected_wordlist_item) >= nowvalue)
						&& ((g_pReciteWord->revise_test->selected_wordlist_item) < (nowvalue + 3)))
					{
						g_pReciteWord->revise_test->
							wordlist_item[((g_pReciteWord->revise_test->selected_wordlist_item) - nowvalue)].set_label (2, entry_text);
					}
					g_pReciteWord->revise_test->
						tested_wordlist_item_status
						[g_pReciteWord->revise_test->
						 selected_wordlist_item] = S_right_2;
					g_pReciteWord->revise_test->recited_word_count ++;
					gchar aa[5];
					sprintf(aa,"%2d",g_pReciteWord->revise_test->recited_word_count);
					gtk_label_set_text(GTK_LABEL(g_pReciteWord->revise_test->recited_word_count_label),aa);
				}
				else
				{
					g_pReciteWord->revise_test->tested_wordlist_item_status[g_pReciteWord->revise_test->selected_wordlist_item] = S_right_1;	//right,but need to test again
				}
				if (((g_pReciteWord->revise_test->selected_wordlist_item) >= nowvalue)
						&& ((g_pReciteWord->revise_test->selected_wordlist_item) < (nowvalue + 3)))
				{
					g_pReciteWord->revise_test->
						wordlist_item[((g_pReciteWord->
								revise_test->
								selected_wordlist_item)
								   -
								   nowvalue)].
						set_pixmap (g_pReciteWord->
								revise_test->
								tested_wordlist_item_status
								[g_pReciteWord->
								 revise_test->
								 selected_wordlist_item]);
				}
				if (g_pReciteWord->revise_test->headchar_ckbutton.status)
					gtk_widget_hide(g_pReciteWord->revise_test->headchar_label);
				g_pReciteWord->revise_test->checknext_timer =
					g_timeout_add (1000,
						 revise_test_check_next_word,
						 NULL);				
				//when answer is right readword too should be better,but before the ability of stop reading is done,it is not very good,
				//so now just give user a choose.
				if ((g_pReciteWord->revise_test->readword_when_answerright)&&(g_pReciteWord->revise_test->readword_ckbutton.status))
					readword(g_pReciteWord->now_zu_words.w[g_pReciteWord->revise_test->selected_wordlist_item]);
			}
			else
			{
				gtk_label_set_text (GTK_LABEL
					    (g_pReciteWord->revise_test->
					     right_label),
					    g_pReciteWord->now_zu_words.
					    w[g_pReciteWord->revise_test->
					      selected_wordlist_item]);
				gtk_widget_show (g_pReciteWord->revise_test->pressanykey_tip);
				gtk_widget_show (g_pReciteWord->revise_test->right_tip);
				g_pReciteWord->revise_test->pressanykey_status = 1;
				if (g_pReciteWord->revise_test->readword_ckbutton.status)
					readword(g_pReciteWord->now_zu_words.w[g_pReciteWord->revise_test->selected_wordlist_item]);
			}
			return TRUE;
		}
		break; // goto the next line,make it return false.
	}
	return FALSE;
}

static void
on_revise_test_wordlistitem_clicked (int index)
{
	int i = index +
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->revise_test->
		  wordlist_adjust)->value);
	g_pReciteWord->revise_test->selected_wordlist_item = i;
	g_pReciteWord->revise_test->set_wordtest_label (i);
	if (g_pReciteWord->revise_test->status == RTS_ANSWERING)
	{
		if (g_pReciteWord->revise_test->
		    tested_wordlist_item_str[g_pReciteWord->revise_test->
					     selected_wordlist_item])
		{
			g_pReciteWord->revise_test->wordtest_entry.
				set_text (g_pReciteWord->revise_test->
					  tested_wordlist_item_str
					  [g_pReciteWord->revise_test->
					   selected_wordlist_item]);
			g_pReciteWord->revise_test->wordtest_entry.position =
				strlen (g_pReciteWord->revise_test->
					tested_wordlist_item_str
					[g_pReciteWord->revise_test->
					 selected_wordlist_item]);
		}
		else
			g_pReciteWord->revise_test->wordtest_entry.
				set_text ("");
		g_pReciteWord->revise_test->wordtest_entry.
			set_righttext (g_pReciteWord->now_zu_words.
				       w[g_pReciteWord->revise_test->
					 selected_wordlist_item]);
	}
}

static void
on_revise_test_wordlistadjust_valuechanged (GtkAdjustment * adj,
					    CRevise_test * parent)
{
	int newvalue = ((gint) adj->value);

	if (((parent->selected_wordlist_item) >= newvalue)
	    && ((parent->selected_wordlist_item) < (newvalue + 3)))
		parent->wordlist_item[((parent->selected_wordlist_item) -
				       newvalue)].set_selected (TRUE);
	else
		parent->wordlist_item[0].clean_selected ();

	g_pReciteWord->revise_test->set_wordlist_item (newvalue);
}

static void
on_revise_test_meaning_ckbutton_clicked ()
{
	if (g_pReciteWord->revise_test->meaning_ckbutton.status)
		gtk_widget_show(g_pReciteWord->revise_test->wordtest_label_1);
	else
		gtk_widget_hide(g_pReciteWord->revise_test->wordtest_label_1);
}

static void
on_revise_test_readword_ckbutton_clicked ()
{
}

static void
on_revise_test_phonetic_ckbutton_clicked ()
{
	if (g_pReciteWord->revise_test->phonetic_ckbutton.status)
		gtk_widget_show(g_pReciteWord->revise_test->phonetic_label.hbox);
	else
		gtk_widget_hide(g_pReciteWord->revise_test->phonetic_label.hbox);
}

static void
on_revise_test_length_ckbutton_clicked ()
{
	g_pReciteWord->revise_test->wordtest_entry.set_showlength(g_pReciteWord->revise_test->length_ckbutton.status);
}

static void
on_revise_test_headchar_ckbutton_clicked ()
{
	if (g_pReciteWord->revise_test->headchar_ckbutton.status)
		gtk_widget_show(g_pReciteWord->revise_test->headchar_label);
	else
		gtk_widget_hide(g_pReciteWord->revise_test->headchar_label);
}

static void
on_revise_test_sndeffect_ckbutton_clicked ()
{
	g_pReciteWord->revise_test->wordtest_entry.enablesnd = g_pReciteWord->revise_test->sndeffect_ckbutton.status;
}

static void
on_revise_test_readword_button_clicked (gpointer data)
{
	CRevise_test *parent = (CRevise_test *)data;
	if (parent->selected_wordlist_item>=0 && parent->selected_wordlist_item<g_pReciteWord->now_zu_wordcount)
		readword(g_pReciteWord->now_zu_words.w[parent->selected_wordlist_item]);
}

static void 
on_revise_test_chooseword_ok_clicked ()
{
	g_free(g_pReciteWord->revise_test->tested_wordlist_item_str);
	g_pReciteWord->revise_test->tested_wordlist_item_str =
		(char **) g_malloc0 (g_pReciteWord->now_zu_wordcount *
				    (sizeof (char *)));
	g_free(g_pReciteWord->revise_test->tested_wordlist_item_status);
	g_pReciteWord->revise_test->tested_wordlist_item_status =
		(TestStatus *) g_malloc (g_pReciteWord->now_zu_wordcount *
				   (sizeof (TestStatus)));
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	gtk_label_set_text(GTK_LABEL(g_pReciteWord->revise_test->word_count_label),aa);
	//after chooseword again,should change adj->upper too,this is done in on_revise_test_start_clicked().
	if (g_pReciteWord->revise_test->start_pause_button_status==0)
	{
		g_pReciteWord->revise_test->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_test->start_pause_button_status==1)
	{
		g_pReciteWord->revise_test->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_test->start_pause_button_status==2)
	{
		g_pReciteWord->revise_test->start_button.set_enable(true);
	}

	if (!g_pReciteWord->revise_test->start_button.enable)
		on_revise_test_pause_clicked();  //continue
	if (g_pReciteWord->revise_test->started)
		on_revise_test_start_clicked();  //stop
	on_revise_test_start_clicked();  //start
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->revise_test->chooseword_button.set_enable(true);
	gtk_widget_grab_focus (g_pReciteWord->revise_test->wordtest_entry.text_area);
}

static void 
on_revise_test_chooseword_cancel_clicked ()
{
	g_pReciteWord->chooseword->close ();
	if (g_pReciteWord->revise_test->start_pause_button_status==0)
	{
		g_pReciteWord->revise_test->pause_button.set_enable(true);
		on_revise_test_pause_clicked();  //continue
	}
	else if (g_pReciteWord->revise_test->start_pause_button_status==1)
	{
		g_pReciteWord->revise_test->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_test->start_pause_button_status==2)
	{
		g_pReciteWord->revise_test->start_button.set_enable(true);
	}
	g_pReciteWord->revise_test->chooseword_button.set_enable(true);
	gtk_widget_grab_focus (g_pReciteWord->revise_test->wordtest_entry.text_area);
}

static void
on_revise_test_chooseword_button_clicked (gpointer data)
{
	if (!g_pReciteWord->revise_test->start_button.enable)
	{
		g_pReciteWord->revise_test->start_pause_button_status=1;
		g_pReciteWord->revise_test->pause_button.set_enable(false);
	}
	else if (!g_pReciteWord->revise_test->pause_button.enable)
	{
		g_pReciteWord->revise_test->start_pause_button_status=2;
		g_pReciteWord->revise_test->start_button.set_enable(false);
	}
	else
	{
		g_pReciteWord->revise_test->start_pause_button_status=0;
		on_revise_test_pause_clicked();  //pause
		g_pReciteWord->revise_test->pause_button.set_enable(false);
	}
	
	g_pReciteWord->PopupChooseword (on_revise_test_chooseword_ok_clicked,
					on_revise_test_chooseword_cancel_clicked,&(g_pReciteWord->revise_test_chooseword_info));
	g_pReciteWord->revise_test->chooseword_button.set_enable(false);
}

static void
on_revise_test_handinpaper_clicked (gpointer data)
{
	g_pReciteWord->revise_test->wordlist_item[0].set_can_select (0);
	g_pReciteWord->revise_test->wordlist_item[1].set_can_select (0);
	g_pReciteWord->revise_test->wordlist_item[2].set_can_select (0);
	g_pReciteWord->revise_test->status = RTS_CHECKING;
	g_pReciteWord->revise_test->selected_wordlist_item = -1;
	revise_test_select_next_word ();
	g_pReciteWord->revise_test->handinpaper_button.set_enable (FALSE);
}

CRevise_test::CRevise_test ()
{
	skin_load_revise_test (g_pReciteWord->window);
	skin_load_text (g_pReciteWord->window);
	rw_cfg_read_boolean (usercfgfile, "revise_test", "readword_when_answerright", &readword_when_answerright);
	rw_cfg_read_int (usercfgfile, "revise_test", "headchar_len", &headchar_len);
	if (headchar_len <= 0)
		headchar_len = 2;
}

CRevise_test::~CRevise_test ()
{
}

void
CRevise_test::do_scroll(GdkEventScroll * event)
{
	int x, y;
	gtk_window_get_position (GTK_WINDOW(g_pReciteWord->window), &x, &y);
	if ((event->x_root>=x+Skin->revise_test.wordlist_area.x[0])&&(event->x_root<=x+Skin->revise_test.wordlist_area.x[0]+Skin->revise_test.wordlist_area.w)
		&&(event->y_root>=y+Skin->revise_test.wordlist_area.y[0])&&(event->y_root<=y+Skin->revise_test.wordlist_area.y[0]+Skin->revise_test.wordlist_area.h))
	{
		if (dialog.fixed)  //check if mouse is in dialog
		{
			int dialog_x=x+Skin->revise_test.dialog.x[0];
			int dialog_y=y+Skin->revise_test.dialog.y[0];
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->revise_test.dialog.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->revise_test.dialog.h))
				return;
		}
		if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
		{
			int dialog_x=x+Skin->mark.mark.x[0];
			int dialog_y=y+Skin->mark.mark.y[0];
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->mark.mark.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->mark.mark.h))
				return;
		}		
		if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		{
			int dialog_x=x+((Skin->reciteword.width - Skin->chooseword.chooseword.w) / 2);
			int dialog_y=y+((Skin->reciteword.height - Skin->chooseword.chooseword.h) / 2);
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->chooseword.chooseword.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->chooseword.chooseword.h))
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
			on_revise_test_wordlistadjust_valuechanged (adj,this);
		}
	}
}

void
CRevise_test::init ()
{
	g_pReciteWord->status = STATUS_REVISE_TEST;
	dialog.fixed = NULL;
	group_id_label = NULL;
	group_info_label = NULL;
	revise_times_pix = NULL;
	record_label[0] = NULL;
	record_label[1] = NULL;
	record_label[2] = NULL;
	record_label[3] = NULL;
	record_label[4] = NULL;
	headchar_label = NULL;
	
	status = RTS_ANSWERING;
	started = FALSE;
	tip_timer = 0;
	checknext_timer = 0;
	tested_wordlist_item_str =
		(char **) g_malloc0 (g_pReciteWord->now_zu_wordcount *
				    (sizeof (char *)));
	tested_wordlist_item_status =
		(TestStatus *) g_malloc (g_pReciteWord->now_zu_wordcount *
				   (sizeof (TestStatus)));
	if ((g_pReciteWord->collectword) && (g_pReciteWord->collectword->window))
		g_pReciteWord->collectword->close ();
}

void
CRevise_test::refresh_saverecord ()
{
	if (revise_times_pix)
	{
		gtk_widget_destroy (revise_times_pix);
		revise_times_pix = NULL;
	}
	if (group_id_label)
	{
		gtk_widget_destroy (group_id_label);
		group_id_label = NULL;
	}
	if (group_info_label)
	{
		gtk_widget_destroy (group_info_label);
		group_info_label = NULL;
	}
	gint i;
	for (i=0;i<5;i++)
	{
		if (record_label[i])
		{
			gtk_widget_destroy (record_label[i]);
			record_label[i] = NULL;
		}
	}

	gchar tmpstr[256];
	if (g_pReciteWord->now_zu_index < 0)
	{
		save_record = false;
	}
	else
	{
		std::time_t last_time;
		// these code should equal revise_group::show 's code.
		sprintf (tmpstr, "zu%d_time0", g_pReciteWord->now_zu_index);
		if (rw_cfg_read_time (usercfgfile, g_pReciteWord->now_book->bookfilename, tmpstr, &last_time))	// have firstrecited 
		{
			double between_time;
			gint revise_times;

			sprintf (tmpstr, "zu%d_revisetimes", g_pReciteWord->now_zu_index);
			rw_cfg_read_int (usercfgfile,
					 g_pReciteWord->now_book->
					 bookfilename, tmpstr, &revise_times);
			if (revise_times < g_pReciteWord->revise_group->max_revise_times)
			{
				sprintf (tmpstr, "zu%d_time%d", g_pReciteWord->now_zu_index,
					 revise_times);
				rw_cfg_read_time (usercfgfile,
						  g_pReciteWord->now_book->
						  bookfilename, tmpstr,
						  &last_time);
				between_time = (revise_times * (24 * 60 * 60)) - (12 * 60 * 60);
				std::time_t now = std::time (0);
				if (difftime (now, last_time) > between_time)
				{
					save_record = true;
				}
				else
				{
					save_record = false;
				}
			}
			else
			{
				save_record = false;
			}
		}
		else
		{
			save_record = false;
		}
	}

	if (g_pReciteWord->now_zu_index < 0)
		tmpstr[0]='\0';
	else
		sprintf(tmpstr,"%d",g_pReciteWord->now_zu_index+1);
	group_id_label = gtk_label_new (tmpstr);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), group_id_label,
		       Skin->revise_test.group_id_label.x[0],
		       Skin->revise_test.group_id_label.y[0]);
	gtk_widget_show (group_id_label);
	group_info_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), group_info_label,
		       Skin->revise_test.group_info_label.x[0],
		       Skin->revise_test.group_info_label.y[0]);
	gtk_widget_show (group_info_label);
	if (g_pReciteWord->now_zu_index >= 0)
	{
		sprintf (tmpstr, "zu%d_revisetimes", g_pReciteWord->now_zu_index);
		gint revise_times;
		rw_cfg_read_int (usercfgfile, g_pReciteWord->now_book->bookfilename, tmpstr,&revise_times);

		gint i,wrongcount,wordcount,score;
		sprintf (tmpstr, "zu%d_wordcount", g_pReciteWord->now_zu_index);
		rw_cfg_read_int (usercfgfile, g_pReciteWord->now_book->bookfilename, tmpstr,&wordcount);
		for (i=1;i<=revise_times;i++)
		{
			if (i>5)
				break;
			sprintf (tmpstr, "zu%d_time%d_wrongcount", g_pReciteWord->now_zu_index,i);
			rw_cfg_read_int (usercfgfile, g_pReciteWord->now_book->bookfilename, tmpstr,&wrongcount);
			score=(((wordcount-wrongcount)*100)/wordcount);
			sprintf(tmpstr,"%3d",score);
			record_label[i-1] = gtk_label_new(tmpstr);
			gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), record_label[i-1],
				   Skin->revise_test.record_label[i-1].x[0],
				   Skin->revise_test.record_label[i-1].y[0]);
			gtk_widget_show (record_label[i-1]);
		}
		if (save_record)
		{
			sprintf(tmpstr,_("%dth test"),revise_times+1);
			gtk_label_set_text(GTK_LABEL(group_info_label),tmpstr);
			if (revise_times<5)
			{
				revise_times_pix=gtk_image_new_from_pixmap(Skin->revise_test.revise_times_pix[revise_times].p[0],NULL);
				gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), revise_times_pix,
					   Skin->revise_test.revise_times_pix[revise_times].x[0],
					   Skin->revise_test.revise_times_pix[revise_times].y[0]);
				gtk_widget_show (revise_times_pix);
			}
		}	
	}
	if (!save_record)
	{
		gtk_label_set_text(GTK_LABEL(group_info_label),_("Temporary test."));
	}
}

void
CRevise_test::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->revise_test.revise_test.p[0]);

	wordlist_item[0].create (NULL, g_pReciteWord->fixed,
				 Skin->revise_test.wordlist_item_1.x[0],
				 Skin->revise_test.wordlist_item_1.y[0],
				 Skin->revise_test.wordlist_item_1.w,
				 Skin->revise_test.wordlist_item_1.h,
				 Skin->revise_test.wordlist_item_1.p[0],
				 Skin->revise_test.wordlist_item_1.p[1],
				 Skin->revise_test.wordlist_item_label_1.x[0],
				 Skin->revise_test.wordlist_item_label_1.y[0],
				 Skin->revise_test.wordlist_item_label_2.x[0],
				 Skin->revise_test.wordlist_item_label_2.y[0],
				 on_revise_test_wordlistitem_clicked, 0);
	wordlist_item[1].create (wordlist_item[0].group, g_pReciteWord->fixed,
				 Skin->revise_test.wordlist_item_2.x[0],
				 Skin->revise_test.wordlist_item_2.y[0],
				 Skin->revise_test.wordlist_item_2.w,
				 Skin->revise_test.wordlist_item_2.h,
				 Skin->revise_test.wordlist_item_2.p[0],
				 Skin->revise_test.wordlist_item_2.p[1],
				 Skin->revise_test.wordlist_item_label_1.x[0],
				 Skin->revise_test.wordlist_item_label_1.y[0],
				 Skin->revise_test.wordlist_item_label_2.x[0],
				 Skin->revise_test.wordlist_item_label_2.y[0],
				 on_revise_test_wordlistitem_clicked, 1);
	wordlist_item[2].create (wordlist_item[1].group, g_pReciteWord->fixed,
				 Skin->revise_test.wordlist_item_3.x[0],
				 Skin->revise_test.wordlist_item_3.y[0],
				 Skin->revise_test.wordlist_item_3.w,
				 Skin->revise_test.wordlist_item_3.h,
				 Skin->revise_test.wordlist_item_3.p[0],
				 Skin->revise_test.wordlist_item_3.p[1],
				 Skin->revise_test.wordlist_item_label_1.x[0],
				 Skin->revise_test.wordlist_item_label_1.y[0],
				 Skin->revise_test.wordlist_item_label_2.x[0],
				 Skin->revise_test.wordlist_item_label_2.y[0],
				 on_revise_test_wordlistitem_clicked, 2);

	wordlist_adjust =
		gtk_adjustment_new (0, 0, g_pReciteWord->now_zu_wordcount, 1,
				    3, 3);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK
			  (on_revise_test_wordlistadjust_valuechanged), this);

	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll,
			      Skin->revise_test.wordlist_vscroll.w,
			      Skin->revise_test.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_vscroll,
		       Skin->revise_test.wordlist_vscroll.x[0],
		       Skin->revise_test.wordlist_vscroll.y[0]);

	meaning_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.meaning_ckbutton.x[0],
			      Skin->revise_test.meaning_ckbutton.y[0],
			      Skin->revise_test.meaning_ckbutton.p[0],
			      Skin->revise_test.meaning_ckbutton.p[1],
			      Skin->revise_test.meaning_ckbutton.p[2],
				  Skin->revise_test.meaning_ckbutton.p[3],
			      on_revise_test_meaning_ckbutton_clicked);
	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showmean_1",
			     &tmp_b);
	meaning_ckbutton.set_status(tmp_b);
	readword_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.readword_ckbutton.x[0],
			      Skin->revise_test.readword_ckbutton.y[0],
			      Skin->revise_test.readword_ckbutton.p[0],
			      Skin->revise_test.readword_ckbutton.p[1],
			      Skin->revise_test.readword_ckbutton.p[2],
				  Skin->revise_test.readword_ckbutton.p[3],
			      on_revise_test_readword_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "readword_1",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	phonetic_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.phonetic_ckbutton.x[0],
			      Skin->revise_test.phonetic_ckbutton.y[0],
			      Skin->revise_test.phonetic_ckbutton.p[0],
			      Skin->revise_test.phonetic_ckbutton.p[1],
			      Skin->revise_test.phonetic_ckbutton.p[2],
				  Skin->revise_test.phonetic_ckbutton.p[3],
			      on_revise_test_phonetic_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showphonetic_1",
			     &tmp_b);
	phonetic_ckbutton.set_status(tmp_b);
	length_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.length_ckbutton.x[0],
			      Skin->revise_test.length_ckbutton.y[0],
			      Skin->revise_test.length_ckbutton.p[0],
			      Skin->revise_test.length_ckbutton.p[1],
			      Skin->revise_test.length_ckbutton.p[2],
				  Skin->revise_test.length_ckbutton.p[3],
			      on_revise_test_length_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showlength_1",
			     &tmp_b);
	length_ckbutton.set_status(tmp_b);
	headchar_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.headchar_ckbutton.x[0],
			      Skin->revise_test.headchar_ckbutton.y[0],
			      Skin->revise_test.headchar_ckbutton.p[0],
			      Skin->revise_test.headchar_ckbutton.p[1],
			      Skin->revise_test.headchar_ckbutton.p[2],
				  Skin->revise_test.headchar_ckbutton.p[3],
			      on_revise_test_headchar_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showheadchar_1",
			     &tmp_b);
	headchar_ckbutton.set_status(tmp_b);
	sndeffect_ckbutton.create (g_pReciteWord->fixed, Skin->revise_test.sndeffect_ckbutton.x[0],
			      Skin->revise_test.sndeffect_ckbutton.y[0],
			      Skin->revise_test.sndeffect_ckbutton.p[0],
			      Skin->revise_test.sndeffect_ckbutton.p[1],
			      Skin->revise_test.sndeffect_ckbutton.p[2],
				  Skin->revise_test.sndeffect_ckbutton.p[3],
			      on_revise_test_sndeffect_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "sndeffect_1",
			     &tmp_b);
	sndeffect_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->revise_test.readword_button.x[0],
			      Skin->revise_test.readword_button.y[0],
			      Skin->revise_test.readword_button.p[0],
			      Skin->revise_test.readword_button.p[1],
			      Skin->revise_test.readword_button.p[2],
			      on_revise_test_readword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), readword_button.event_box,"Tab","");
	chooseword_button.create (g_pReciteWord->fixed, Skin->revise_test.chooseword_button.x[0],
			      Skin->revise_test.chooseword_button.y[0],
			      Skin->revise_test.chooseword_button.p[0],
			      Skin->revise_test.chooseword_button.p[1],
			      Skin->revise_test.chooseword_button.p[2],
			      on_revise_test_chooseword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), chooseword_button.event_box,"Ctrl-W","");

	wordtest_entry.create (g_pReciteWord->fixed,
			       Skin->revise_test.wordtest_entry.x[0],
			       Skin->revise_test.wordtest_entry.y[0],
			       Skin->revise_test.wordtest_entry.w,
			       Skin->revise_test.wordtest_entry.h, "", "", 1,
			       on_test_wordtestentry_keypress);	
	wordtest_entry.set_showlength(length_ckbutton.status);
	wordtest_entry.enablesnd = sndeffect_ckbutton.status;
	gtk_widget_grab_focus (wordtest_entry.text_area);

	wordtest_label_1 = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordtest_label_1,
		       Skin->revise_test.wordtest_label_1.x[0],
		       Skin->revise_test.wordtest_label_1.y[0]);
	gtk_widget_show (wordtest_label_1);
	wordlist_scrollvalue_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_scrollvalue_label,
		       Skin->revise_test.wordlist_scrollvalue_label.x[0],
		       Skin->revise_test.wordlist_scrollvalue_label.y[0]);
	gtk_widget_show (wordlist_scrollvalue_label);
	word_index_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_index_label,
		       Skin->revise_test.word_index_label.x[0],
		       Skin->revise_test.word_index_label.y[0]);
	gtk_widget_show (word_index_label);
	recited_word_count_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), recited_word_count_label,
		       Skin->revise_test.recited_word_count_label.x[0],
		       Skin->revise_test.recited_word_count_label.y[0]);
	gtk_widget_show (recited_word_count_label);
	gchar tmpstr[256];
	sprintf(tmpstr,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (tmpstr);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->revise_test.word_count_label.x[0],
		       Skin->revise_test.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	
	phonetic_label.create (g_pReciteWord->fixed, 0,
			       Skin->revise_test.phonetic_label.y[0],
			       &(Skin->text.b_phonetic));

	headchar_label = gtk_label_new ("");
	gtk_widget_set_name (headchar_label, "revise_test-headchar_label");
	SetFont(headchar_label,true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), headchar_label,
		       Skin->revise_test.headchar_label.x[0],
		       Skin->revise_test.headchar_label.y[0]);
	if (headchar_ckbutton.status)
		gtk_widget_show (headchar_label);
	right_label = gtk_label_new ("");
	SetFont(right_label,true);
	gtk_misc_set_alignment (GTK_MISC (right_label), 0, .5);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_label,
		       Skin->revise_test.right_label.x[0],
		       Skin->revise_test.right_label.y[0]);
	gtk_widget_show (right_label);

	start_button.create (g_pReciteWord->fixed,
			     Skin->revise_test.stop_button.x[0],
			     Skin->revise_test.stop_button.y[0],
			     Skin->revise_test.stop_button.p[0],
			     Skin->revise_test.stop_button.p[1],
			     Skin->revise_test.stop_button.p[2],
			     (GdkPixmap *) NULL,
			     on_revise_test_start_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed,
			     Skin->revise_test.pause_button.x[0],
			     Skin->revise_test.pause_button.y[0],
			     Skin->revise_test.pause_button.p[0],
			     Skin->revise_test.pause_button.p[1],
			     Skin->revise_test.pause_button.p[2],
			     Skin->revise_test.pause_button.p[3],
			     on_revise_test_pause_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), pause_button.event_box,"Ctrl-P","");
	group_button.create (g_pReciteWord->fixed,
			     Skin->revise_test.group_button.x[0],
			     Skin->revise_test.group_button.y[0],
			     Skin->revise_test.group_button.p[0],
			     Skin->revise_test.group_button.p[1],
			     Skin->revise_test.group_button.p[2],
			     (GdkPixmap *) NULL,
			     on_revise_test_group_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), group_button.event_box,"Ctrl-C","");
	skim_button.create (g_pReciteWord->fixed,
			    Skin->revise_test.skim_button.x[0],
			    Skin->revise_test.skim_button.y[0],
			    Skin->revise_test.skim_button.p[0],
			    Skin->revise_test.skim_button.p[1],
			    Skin->revise_test.skim_button.p[2],
			    (GdkPixmap *) NULL, on_revise_test_skim_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), skim_button.event_box,"Ctrl-R","");
	return_button.create (g_pReciteWord->fixed,
			      Skin->revise_test.return_button.x[0],
			      Skin->revise_test.return_button.y[0],
			      Skin->revise_test.return_button.p[0],
			      Skin->revise_test.return_button.p[1],
			      Skin->revise_test.return_button.p[2],
			      (GdkPixmap *) NULL,
			      on_revise_test_return_clicked);

	handinpaper_button.create (g_pReciteWord->fixed,
				   Skin->revise_test.handinpaper_button.x[0],
				   Skin->revise_test.handinpaper_button.y[0],
				   Skin->revise_test.handinpaper_button.p[0],
				   Skin->revise_test.handinpaper_button.p[1],
				   Skin->revise_test.handinpaper_button.p[2],
				   on_revise_test_handinpaper_clicked, NULL);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), handinpaper_button.event_box,"F12","");

	wordtest_tip = gtk_image_new_from_pixmap (Skin->revise_test.wordtest_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordtest_tip,
		       Skin->revise_test.wordtest_tip.x[0],
		       Skin->revise_test.wordtest_tip.y[0]);
	handinpaper_tip =
		gtk_image_new_from_pixmap (Skin->revise_test.handinpaper_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), handinpaper_tip,
		       Skin->revise_test.handinpaper_tip.x[0],
		       Skin->revise_test.handinpaper_tip.y[0]);
	firsttest_tip =
		gtk_image_new_from_pixmap (Skin->revise_test.firsttest_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), firsttest_tip,
		       Skin->revise_test.firsttest_tip.x[0],
		       Skin->revise_test.firsttest_tip.y[0]);
	secondtest_tip =
		gtk_image_new_from_pixmap (Skin->revise_test.secondtest_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), secondtest_tip,
		       Skin->revise_test.secondtest_tip.x[0],
		       Skin->revise_test.secondtest_tip.y[0]);
	pressanykey_tip =
		gtk_image_new_from_pixmap (Skin->revise_test.pressanykey_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), pressanykey_tip,
		       Skin->revise_test.pressanykey_tip.x[0],
		       Skin->revise_test.pressanykey_tip.y[0]);
	right_tip =
		gtk_image_new_from_pixmap (Skin->revise_test.right_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_tip,
		       Skin->revise_test.right_tip.x[0],
		       Skin->revise_test.right_tip.y[0]);
	on_revise_test_start_clicked ();
}

void
CRevise_test::set_wordtest_label (gint i)
{
	gtk_label_set_text (GTK_LABEL (wordtest_label_1),
			    g_pReciteWord->now_zu_words.m[i]);
	gchar aa[5];
	sprintf(aa,"%2d",i+1);
	gtk_label_set_text (GTK_LABEL (word_index_label),
			    aa);
	gchar headchar[6];
	strncpy(headchar,g_pReciteWord->now_zu_words.w[i],headchar_len);
	headchar[headchar_len]='\0';
	gtk_label_set_text (GTK_LABEL (headchar_label),headchar);

/*	gint len = strlen (g_pReciteWord->now_zu_words.m[i]);
	GdkWChar *str_wc = g_new (GdkWChar, len + 1);
	gint wlen =
		gdk_mbstowcs (str_wc, g_pReciteWord->now_zu_words.m[i],
			      len + 1);*/
	if (phonetic_ckbutton.status)
		gtk_widget_hide(phonetic_label.hbox);
	phonetic_label.set_text (g_pReciteWord->now_zu_words.t[i]);	
	gint w;
	pango_layout_get_pixel_size(gtk_label_get_layout(GTK_LABEL(wordtest_label_1)),&w,NULL);
	gtk_fixed_move (GTK_FIXED (g_pReciteWord->fixed),
			phonetic_label.hbox,
			Skin->revise_test.phonetic_label.x[0] +
			Skin->revise_test.wordtest_label_1.x[0] +
			w
		/*	gdk_text_width_wc (gtk_style_get_font
					   (wordtest_label_1->style), str_wc,
					   wlen)*/,
			Skin->revise_test.phonetic_label.y[0]);
	//g_free (str_wc);
	if (phonetic_ckbutton.status)
		gtk_widget_show(phonetic_label.hbox);
	if (readword_ckbutton.status)
		readword(g_pReciteWord->now_zu_words.w[i]);
}

void
CRevise_test::set_wordlist_item (gint index)
{
	gchar aa[5];
	sprintf(aa,"%2d",index+1);
	gtk_label_set_text (GTK_LABEL (wordlist_scrollvalue_label),
			    aa);

	int i;

	for (i = 0; i < 3; i++)
	{
		wordlist_item[i].set_label (1,
					    g_pReciteWord->now_zu_words.m[i +
									  index]);
		wordlist_item[i].set_label (2,
					    tested_wordlist_item_str[i +
								     index]);
		if ((status == RTS_CHECKING) || (status == RTS_TESTING))
		{
			g_pReciteWord->revise_test->wordlist_item[i].
				set_pixmap (g_pReciteWord->revise_test->
					    tested_wordlist_item_status[i +
									index]);
		}
	}
}

void
CRevise_test::save_setting ()
{
	gboolean tmp_b;
	if (status==RTS_ANSWERING || status==RTS_CHECKING)
	{
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showmean_1",
			     &tmp_b);
		if (tmp_b!=meaning_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showmean_1",
					 meaning_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "readword_1",
			     &tmp_b);
		if (tmp_b!=readword_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "readword_1",
					 readword_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showphonetic_1",
			     &tmp_b);
		if (tmp_b!=phonetic_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showphonetic_1",
					 phonetic_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showlength_1",
			     &tmp_b);
		if (tmp_b!=length_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showlength_1",
					 length_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showheadchar_1",
			     &tmp_b);
		if (tmp_b!=headchar_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showheadchar_1",
					 headchar_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "sndeffect_1",
			     &tmp_b);
		if (tmp_b!=sndeffect_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "sndeffect_1",
					 sndeffect_ckbutton.status);
		}
	}
	else if (status==RTS_TESTING)
	{
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showmean_2",
			     &tmp_b);
		if (tmp_b!=meaning_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showmean_2",
					 meaning_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "readword_2",
			     &tmp_b);
		if (tmp_b!=readword_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "readword_2",
					 readword_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showphonetic_2",
			     &tmp_b);
		if (tmp_b!=phonetic_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showphonetic_2",
					 phonetic_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showlength_2",
			     &tmp_b);
		if (tmp_b!=length_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showlength_2",
					 length_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "showheadchar_2",
			     &tmp_b);
		if (tmp_b!=headchar_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "showheadchar_2",
					 headchar_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "revise_test", "sndeffect_2",
			     &tmp_b);
		if (tmp_b!=sndeffect_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "revise_test", "sndeffect_2",
					 sndeffect_ckbutton.status);
		}
	}
}

void
CRevise_test::save_hard_words ()
{
	gchar filename[256];
	sprintf(filename,"%s"G_DIR_SEPARATOR_S"hardbooks"G_DIR_SEPARATOR_S"my-super-hard.bok",g_pReciteWord->userpath);
	if (!strcmp(g_pReciteWord->now_book->bookfilename,filename))
		return;  // don't save my-super-hard.bok 's hard word again.
	int i;
	bool b=true;
	for (i = 0;i < g_pReciteWord->now_zu_wordcount; i++)
	{
		if (tested_wordlist_item_status[i] == S_right_2)
		{
			b=false;
			break;
		}
	}
	if (b)  // no hard word found.
		return;

	sprintf(filename,"%s"G_DIR_SEPARATOR_S"hardbooks",g_pReciteWord->userpath);	
	if (!g_file_test (filename, G_FILE_TEST_EXISTS))
#ifdef G_OS_WIN32
		mkdir (filename);
#else
		mkdir (filename,00755);
#endif	
	
	std::ofstream bookfile;
	sprintf(filename,"%s"G_DIR_SEPARATOR_S"hardbooks"G_DIR_SEPARATOR_S"my-hard.bok",g_pReciteWord->userpath);
	if (!strcmp(g_pReciteWord->now_book->bookfilename,filename))
		sprintf(filename,"%s"G_DIR_SEPARATOR_S"hardbooks"G_DIR_SEPARATOR_S"my-super-hard.bok",g_pReciteWord->userpath);
	if (!g_file_test(filename,G_FILE_TEST_EXISTS))
	{
		bookfile.open(filename,std::ios::app);
		bookfile << "[H]recitewordbookfile[N]" << _("My Hard Words") << "[R]reciteword[P]http://reciteword.cosoft.org.cn[E]huzheng_001@163.com[A]" << _("this is the user's hard words book");
	}
	else
	{
		bookfile.open(filename,std::ios::app);
	}
	string wordline;
	for (i = 0;i < g_pReciteWord->now_zu_wordcount; i++)
	{
		if (tested_wordlist_item_status[i] == S_right_2)
		{
			wordline = "[W]";
			wordline += g_pReciteWord->now_zu_words.w[i];
			if (g_pReciteWord->now_zu_words.t[i])
			{
				wordline += "[T]";
				wordline += g_pReciteWord->now_zu_words.t[i];
			}
			if (g_pReciteWord->now_zu_words.m[i])
			{
				wordline += "[M]";
				wordline += g_pReciteWord->now_zu_words.m[i];
			}
			bookfile << "\n" << wordline;
		}
	}
	bookfile.close();
	g_pReciteWord->PopupCollectword();
}

void
CRevise_test::close ()
{
	if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		g_pReciteWord->chooseword->close ();
	
	save_setting ();

	if (timecount_timer)
	{
		g_source_remove (timecount_timer);
		timecount_timer = 0;
	}
	if (tip_timer)
	{
		g_source_remove (tip_timer);
		tip_timer = 0;
	}
	if (checknext_timer)
	{
		g_source_remove (checknext_timer);
		checknext_timer = 0;
	}

	if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
		g_pReciteWord->mark->close ();
	if (dialog.fixed)
	{
		dialog.close();
	}
	
	int i;
	for (i = 0; i < (g_pReciteWord->now_zu_wordcount); i++)
	{
		if (tested_wordlist_item_str[i])
			g_free (tested_wordlist_item_str[i]);
	}
	g_free (tested_wordlist_item_str);
	g_free (tested_wordlist_item_status);

	wordlist_item[0].destroy ();
	wordlist_item[1].destroy ();
	wordlist_item[2].destroy ();
	gtk_widget_destroy (wordlist_vscroll);


	start_button.destroy ();
	pause_button.destroy ();
	group_button.destroy ();
	skim_button.destroy ();
	return_button.destroy ();

	meaning_ckbutton.destroy ();
	readword_ckbutton.destroy ();
	phonetic_ckbutton.destroy ();
	length_ckbutton.destroy ();
	headchar_ckbutton.destroy ();
	sndeffect_ckbutton.destroy ();
	readword_button.destroy ();
	chooseword_button.destroy ();

	gtk_widget_destroy (wordtest_label_1);
	gtk_widget_destroy (wordlist_scrollvalue_label);
	gtk_widget_destroy (word_index_label);
	gtk_widget_destroy (recited_word_count_label);
	gtk_widget_destroy (word_count_label);
	phonetic_label.destroy ();
	gtk_widget_destroy (right_label);
	gtk_widget_destroy (headchar_label);
	headchar_label = NULL;
	wordtest_entry.destroy ();

	if (revise_times_pix)
		gtk_widget_destroy (revise_times_pix);
	if (group_id_label)
		gtk_widget_destroy (group_id_label);
	if (group_info_label)
		gtk_widget_destroy (group_info_label);
	for (i=0;i<5;i++)
	{
		if (record_label[i])
			gtk_widget_destroy (record_label[i]);
	}
		

	gtk_widget_destroy (wordtest_tip);
	gtk_widget_destroy (handinpaper_tip);
	gtk_widget_destroy (firsttest_tip);
	gtk_widget_destroy (secondtest_tip);
	gtk_widget_destroy (pressanykey_tip);
	gtk_widget_destroy (right_tip);

	handinpaper_button.destroy ();

}
