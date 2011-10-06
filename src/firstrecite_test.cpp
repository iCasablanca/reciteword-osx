#include "firstrecite_test.h"
#include "reciteword.h"
#include "skin.h"
#include "rwconfig.h"
#include "readword.h"
#include <gdk/gdkkeysyms.h>
#include <cstring>
#include <cstdio>
#include <ctime>

extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;

static void do_test_pause ()
{
	if (g_pReciteWord->firstrecite_test->pressanykey_status == 1)
	{
		g_pReciteWord->firstrecite_test->pressanykey_status =
			2;
		gtk_widget_hide (g_pReciteWord->firstrecite_test->
				 pressanykey_tip);
	}
	else
	{
		g_pReciteWord->firstrecite_test->wordtest_entry.
			set_showpen (FALSE);
	}
	g_pReciteWord->firstrecite_test->wordtest_entry.
		set_editable (FALSE);
}

static void
on_test_start_clicked ()
{
	if (g_pReciteWord->firstrecite_test->started)
	{
		do_test_pause ();		
		g_pReciteWord->firstrecite_test->started = FALSE;
		g_pReciteWord->firstrecite_test->pause_button.
			set_enable (FALSE);
		g_pReciteWord->firstrecite_test->start_button.reset (Skin->
								     test.
								     start_button.
								     p[0],
								     Skin->
								     test.
								     start_button.
								     p[1],
								     Skin->
								     test.
								     start_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
	}
	else
	{
		if (g_pReciteWord->firstrecite_test->status==FRTS_FIRSTTIMEOVER)
		{
			g_pReciteWord->firstrecite_test->save_setting();
		   	gboolean tmp_b;
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "showmean_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->meaning_ckbutton.set_status(tmp_b);
			if (tmp_b)
				gtk_widget_show(g_pReciteWord->firstrecite_test->wordtest_label_1);
			else
				gtk_widget_hide(g_pReciteWord->firstrecite_test->wordtest_label_1);
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "readword_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->readword_ckbutton.set_status(tmp_b);
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "showphonetic_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->phonetic_ckbutton.set_status(tmp_b);
			if (tmp_b)
				gtk_widget_show(g_pReciteWord->firstrecite_test->phonetic_label.hbox);
			else
				gtk_widget_hide(g_pReciteWord->firstrecite_test->phonetic_label.hbox);
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "showlength_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->length_ckbutton.set_status(tmp_b);
			g_pReciteWord->firstrecite_test->wordtest_entry.set_showlength(tmp_b);
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "showheadchar_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->headchar_ckbutton.set_status(tmp_b);
			if (tmp_b)
				gtk_widget_show(g_pReciteWord->firstrecite_test->headchar_label);
			else
				gtk_widget_hide(g_pReciteWord->firstrecite_test->headchar_label);
			rw_cfg_read_boolean(usercfgfile,
						 "firstrecite_test", "sndeffect_2",
						 &tmp_b);
			g_pReciteWord->firstrecite_test->sndeffect_ckbutton.set_status(tmp_b);
			g_pReciteWord->firstrecite_test->wordtest_entry.enablesnd = tmp_b;

			g_pReciteWord->firstrecite_test->status=FRTS_SECONDTIME;
		}
		if (g_pReciteWord->firstrecite_test->dialog.fixed)
		{
			g_pReciteWord->firstrecite_test->dialog.close();
		}
		gint i;
		for (i = 0; i < (g_pReciteWord->now_zu_wordcount); i++)
			g_pReciteWord->firstrecite_test->
				tested_wordlist_item[i] = 0;
		g_pReciteWord->firstrecite_test->recited_word_count = 0;
		gtk_label_set_text(GTK_LABEL(g_pReciteWord->firstrecite_test->recited_word_count_label)," 0");
		if (g_pReciteWord->firstrecite_test->pressanykey_status == 2)
		{
			g_pReciteWord->firstrecite_test->wordtest_entry.
				show_cursor = TRUE;
			gtk_widget_hide (g_pReciteWord->firstrecite_test->
					 wrongright_tip);
			gtk_widget_hide (g_pReciteWord->firstrecite_test->
					 right_label);
			g_pReciteWord->firstrecite_test->pressanykey_status =
				0;
		}
		else
		{
			g_pReciteWord->firstrecite_test->wordtest_entry.
				set_showpen (TRUE);
		}
		g_pReciteWord->firstrecite_test->wordtest_entry.
			set_editable (TRUE);
		g_pReciteWord->firstrecite_test->wordtest_entry.set_text ("");
		g_pReciteWord->firstrecite_test->wordtest_entry.
			set_righttext (g_pReciteWord->now_zu_words.w[0]);
		((GtkAdjustment *) g_pReciteWord->firstrecite_test->
		 wordlist_adjust)->value = 0;
		g_signal_emit_by_name (g_pReciteWord->firstrecite_test->
					 wordlist_adjust, "changed");
		g_pReciteWord->firstrecite_test->selected_wordlist_item = 0;
		g_pReciteWord->firstrecite_test->set_wordlist_item (0);
		g_pReciteWord->firstrecite_test->set_wordtest_label (0);
		g_pReciteWord->firstrecite_test->wordlist_item[0].
			set_selected (1);

		g_pReciteWord->firstrecite_test->pause_button.
			set_enable (TRUE);
		g_pReciteWord->firstrecite_test->start_button.reset (Skin->
								     test.
								     stop_button.
								     p[0],
								     Skin->
								     test.
								     stop_button.
								     p[1],
								     Skin->
								     test.
								     stop_button.
								     p[2],
								     Skin->
								     test.
								     stop_button.
								     p[3]);
		g_pReciteWord->firstrecite_test->started = TRUE;
	}
}

static void
on_test_pause_clicked ()
{
	if (g_pReciteWord->firstrecite_test->started)
	{
		do_test_pause ();
		g_pReciteWord->firstrecite_test->start_button.
			set_enable (FALSE);
		g_pReciteWord->firstrecite_test->pause_button.reset (Skin->
								     test.
								     continue_button.
								     p[0],
								     Skin->
								     test.
								     continue_button.
								     p[1],
								     Skin->
								     test.
								     continue_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->firstrecite_test->started = FALSE;
	}
	else
	{
		if (g_pReciteWord->firstrecite_test->pressanykey_status == 2)
		{
			g_pReciteWord->firstrecite_test->pressanykey_status =
				1;
			gtk_widget_show (g_pReciteWord->firstrecite_test->
					 pressanykey_tip);
		}
		else		//pressanykey_stauts==0
		{
			g_pReciteWord->firstrecite_test->wordtest_entry.
				set_showpen (TRUE);
		}
		g_pReciteWord->firstrecite_test->wordtest_entry.
			set_editable (TRUE);

		g_pReciteWord->firstrecite_test->start_button.
			set_enable (TRUE);
		g_pReciteWord->firstrecite_test->pause_button.reset (Skin->
								     test.
								     pause_button.
								     p[0],
								     Skin->
								     test.
								     pause_button.
								     p[1],
								     Skin->
								     test.
								     pause_button.
								     p[2],
								     Skin->
								     test.
								     pause_button.
								     p[3]);
		g_pReciteWord->firstrecite_test->started = TRUE;
	}
}

static void
on_test_group_clicked ()
{
	g_pReciteWord->firstrecite_test->close ();
	g_pReciteWord->ShowFirstRecite_group ();
}

static void
on_test_meaning_ckbutton_clicked ()
{
	if (g_pReciteWord->firstrecite_test->meaning_ckbutton.status)
		gtk_widget_show(g_pReciteWord->firstrecite_test->wordtest_label_1);
	else
		gtk_widget_hide(g_pReciteWord->firstrecite_test->wordtest_label_1);
}

static void
on_test_readword_ckbutton_clicked ()
{
}

static void
on_test_phonetic_ckbutton_clicked ()
{
	if (g_pReciteWord->firstrecite_test->phonetic_ckbutton.status)
		gtk_widget_show(g_pReciteWord->firstrecite_test->phonetic_label.hbox);
	else
		gtk_widget_hide(g_pReciteWord->firstrecite_test->phonetic_label.hbox);
}

static void
on_test_length_ckbutton_clicked ()
{
	g_pReciteWord->firstrecite_test->wordtest_entry.set_showlength(g_pReciteWord->firstrecite_test->length_ckbutton.status);
}

static void
on_test_headchar_ckbutton_clicked ()
{
	if (g_pReciteWord->firstrecite_test->headchar_ckbutton.status)
		gtk_widget_show(g_pReciteWord->firstrecite_test->headchar_label);
	else
		gtk_widget_hide(g_pReciteWord->firstrecite_test->headchar_label);
}

static void
on_test_sndeffect_ckbutton_clicked ()
{
	g_pReciteWord->firstrecite_test->wordtest_entry.enablesnd = g_pReciteWord->firstrecite_test->sndeffect_ckbutton.status;
}

static void
on_test_readword_button_clicked (gpointer data)
{
	CFirstRecite_test *parent = (CFirstRecite_test *)data;
	if (parent->selected_wordlist_item>=0 && parent->selected_wordlist_item<g_pReciteWord->now_zu_wordcount)
		readword(g_pReciteWord->now_zu_words.w[parent->selected_wordlist_item]);
}

static void
test_select_next_word ()
{
	int nowvalue =
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->firstrecite_test->
		  wordlist_adjust)->value);

	do
	{
		g_pReciteWord->firstrecite_test->selected_wordlist_item++;
		if (g_pReciteWord->firstrecite_test->selected_wordlist_item >=
		    g_pReciteWord->now_zu_wordcount)
		{
			int i, b;
			b = 1;
			for (i = 0; i < g_pReciteWord->now_zu_wordcount; i++)
			{
				if (g_pReciteWord->firstrecite_test->
				    tested_wordlist_item[i] != -1)
				{
					b = 0;
					break;
				}
			}
			if (b)
			{
				on_test_start_clicked ();	//stop
				if (g_pReciteWord->firstrecite_test->status ==
				    FRTS_FIRSTTIME)
				{
					g_pReciteWord->next_zu_index = g_pReciteWord->now_zu_index + 1;
					rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename, "next_zu_index", g_pReciteWord->next_zu_index);
					g_pReciteWord->shooting_chooseword_info.by = g_pReciteWord->typing_chooseword_info.by = 0; // by group
					g_pReciteWord->shooting_chooseword_info.zu_index = g_pReciteWord->typing_chooseword_info.zu_index = g_pReciteWord->now_zu_index;
					rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename, "shooting_chooseword_by", g_pReciteWord->shooting_chooseword_info.by);
					rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename, "shooting_chooseword_zu_index", g_pReciteWord->shooting_chooseword_info.zu_index);
					rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename, "typing_chooseword_by", g_pReciteWord->typing_chooseword_info.by);
					rw_cfg_write_int (usercfgfile, g_pReciteWord->now_book->bookfilename, "typing_chooseword_zu_index", g_pReciteWord->typing_chooseword_info.zu_index);
					gchar tmpstr[256];
					sprintf (tmpstr, "zu%d_time0", g_pReciteWord->now_zu_index);
					std::time_t now = std::time (0);
					rw_cfg_write_time (usercfgfile,
							   g_pReciteWord->
							   now_book->
							   bookfilename,
							   tmpstr, now);
					glong totalwordcount;
					rw_cfg_read_long(usercfgfile,"firstrecite_test","totalwordcount",&totalwordcount);
					totalwordcount+= g_pReciteWord->now_zu_wordcount;
					rw_cfg_write_long(usercfgfile,"firstrecite_test","totalwordcount",totalwordcount);
					
					conf_save_usr();  //save to disk.
					g_pReciteWord->firstrecite_test->
						status = FRTS_FIRSTTIMEOVER;
				}
				else
				{
				}
				g_pReciteWord->firstrecite_test->dialog.show ();
				return;
			}
			else
			{
				g_pReciteWord->firstrecite_test->
					selected_wordlist_item = 0;
				for (i = 0;
				     i < (g_pReciteWord->now_zu_wordcount);
				     i++)
					if (g_pReciteWord->firstrecite_test->
					    tested_wordlist_item[i] == 1)
						g_pReciteWord->
							firstrecite_test->
							tested_wordlist_item
							[i] = 0;
				g_pReciteWord->firstrecite_test->
					set_wordlist_item (nowvalue);
			}
		}
	}
	while (g_pReciteWord->firstrecite_test->
	       tested_wordlist_item[g_pReciteWord->firstrecite_test->
				    selected_wordlist_item] == -1);

	if ((g_pReciteWord->firstrecite_test->selected_wordlist_item) >=
	    (nowvalue + 5)
	    || ((g_pReciteWord->firstrecite_test->selected_wordlist_item) <
		nowvalue))
	{
		nowvalue =
			g_pReciteWord->firstrecite_test->
			selected_wordlist_item;
		if (nowvalue > (g_pReciteWord->now_zu_wordcount - 5))
			nowvalue = (g_pReciteWord->now_zu_wordcount - 5);
		((GtkAdjustment *) g_pReciteWord->firstrecite_test->
		 wordlist_adjust)->value = nowvalue;
		g_pReciteWord->firstrecite_test->set_wordlist_item (nowvalue);
		g_signal_emit_by_name (g_pReciteWord->firstrecite_test->
					 wordlist_adjust, "changed");
	}

	if (((g_pReciteWord->firstrecite_test->selected_wordlist_item) >=
	     nowvalue)
	    && ((g_pReciteWord->firstrecite_test->selected_wordlist_item) <
		(nowvalue + 5)))
		g_pReciteWord->firstrecite_test->
			wordlist_item[((g_pReciteWord->firstrecite_test->
					selected_wordlist_item) -
				       nowvalue)].set_selected (1);
	g_pReciteWord->firstrecite_test->set_wordtest_label (g_pReciteWord->
							     firstrecite_test->
							     selected_wordlist_item);
	g_pReciteWord->firstrecite_test->wordtest_entry.set_text ("");
	g_pReciteWord->firstrecite_test->wordtest_entry.
		set_righttext (g_pReciteWord->now_zu_words.
			       w[g_pReciteWord->firstrecite_test->
				 selected_wordlist_item]);
}

static void
on_test_skim_clicked ()
{
	g_pReciteWord->firstrecite_test->close ();
	g_pReciteWord->ShowFirstRecite_skim ();
}

static void
on_test_return_clicked ()
{
	g_pReciteWord->firstrecite_test->close ();
	g_pReciteWord->show ();
}

static gint
on_test_wordtestentry_keypress (gint calltime, gint keyval)
{
	if (calltime != 0)
		return FALSE;
	if (!g_pReciteWord->firstrecite_test->started)
		return true;

	if (g_pReciteWord->firstrecite_test->pressanykey_status == 1)
	{
		if (((keyval >= 0x20) && (keyval <= 0xFF))
		    || (keyval == GDK_Return))
		{
			g_pReciteWord->firstrecite_test->wordtest_entry.
				show_cursor = TRUE;
			gtk_widget_hide (g_pReciteWord->firstrecite_test->
					 pressanykey_tip);
			gtk_widget_hide (g_pReciteWord->firstrecite_test->
					 wrongright_tip);
			gtk_widget_hide (g_pReciteWord->firstrecite_test->
					 right_label);
			g_pReciteWord->firstrecite_test->pressanykey_status =
				0;
			test_select_next_word ();
		}
		return TRUE;
	}

	if (keyval != GDK_Return)
		return FALSE;

	g_pReciteWord->firstrecite_test->pressanykey_status = 1;

	gchar *entry_text;
	entry_text =
		g_strstrip (g_pReciteWord->firstrecite_test->wordtest_entry.
			    get_text ());

	if (strcmp
	    (entry_text,
	     g_pReciteWord->now_zu_words.w[g_pReciteWord->firstrecite_test->
					   selected_wordlist_item]) == 0)
	{
		g_pReciteWord->firstrecite_test->
			tested_wordlist_item[g_pReciteWord->firstrecite_test->
					     selected_wordlist_item] = -1;
		g_pReciteWord->firstrecite_test->recited_word_count ++;
		gchar aa[5];
		sprintf(aa,"%2d",g_pReciteWord->firstrecite_test->recited_word_count);
		gtk_label_set_text(GTK_LABEL(g_pReciteWord->firstrecite_test->recited_word_count_label),aa);
	}
	else
	{
		g_pReciteWord->firstrecite_test->
			tested_wordlist_item[g_pReciteWord->firstrecite_test->
					     selected_wordlist_item] = 1;
		gtk_widget_show (g_pReciteWord->firstrecite_test->
				 wrongright_tip);
		gtk_label_set_text (GTK_LABEL
				    (g_pReciteWord->firstrecite_test->
				     right_label),
				    g_pReciteWord->now_zu_words.
				    w[g_pReciteWord->firstrecite_test->
				      selected_wordlist_item]);
		gtk_widget_show (g_pReciteWord->firstrecite_test->
				 right_label);
	}

	int nowvalue =
		((gint)
		 ((GtkAdjustment *) g_pReciteWord->firstrecite_test->
		  wordlist_adjust)->value);
	if (((g_pReciteWord->firstrecite_test->selected_wordlist_item) >=
	     nowvalue)
	    && ((g_pReciteWord->firstrecite_test->selected_wordlist_item) <
		(nowvalue + 5)))
	{
		if (g_pReciteWord->firstrecite_test->
		    tested_wordlist_item[g_pReciteWord->firstrecite_test->
					 selected_wordlist_item] == -1)
			g_pReciteWord->firstrecite_test->
				wordlist_item[g_pReciteWord->
					      firstrecite_test->
					      selected_wordlist_item -
					      nowvalue].set_selected (-1);
		g_pReciteWord->firstrecite_test->wordlist_item[g_pReciteWord->
							       firstrecite_test->
							       selected_wordlist_item
							       -
							       nowvalue].
			set_label (g_pReciteWord->now_zu_words.
				   w[g_pReciteWord->firstrecite_test->
				     selected_wordlist_item]);
	}

	gtk_widget_show (g_pReciteWord->firstrecite_test->pressanykey_tip);
	g_pReciteWord->firstrecite_test->wordtest_entry.
		check_text (g_pReciteWord->now_zu_words.
			    w[g_pReciteWord->firstrecite_test->
			      selected_wordlist_item],SND_ANSWEROK,SND_ANSWERNO);
	return TRUE;
}

static void
on_test_wordlistadjust_valuechanged (GtkAdjustment * adj,
				     CFirstRecite_test * parent)
{

	int newvalue = ((gint) adj->value);

	g_pReciteWord->firstrecite_test->set_wordlist_item (newvalue);

	if (((parent->selected_wordlist_item) >= newvalue)
	    && ((parent->selected_wordlist_item) < (newvalue + 5)))
		parent->wordlist_item[((parent->selected_wordlist_item) -
				       newvalue)].set_selected (1);
}

CFirstRecite_test::CFirstRecite_test ()
{
	skin_load_test (g_pReciteWord->window);
	skin_load_text (g_pReciteWord->window);

	rw_cfg_read_int (usercfgfile, "firstrecite_test", "headchar_len", &headchar_len);
	if (headchar_len <= 0)
		headchar_len = 2;
}

CFirstRecite_test::~CFirstRecite_test ()
{
}

void
CFirstRecite_test::do_scroll(GdkEventScroll * event)
{
	int x, y;
	gtk_window_get_position (GTK_WINDOW(g_pReciteWord->window), &x, &y);
	if ((event->x_root>=x+Skin->test.wordlist_area.x[0])&&(event->x_root<=x+Skin->test.wordlist_area.x[0]+Skin->test.wordlist_area.w)
		&&(event->y_root>=y+Skin->test.wordlist_area.y[0])&&(event->y_root<=y+Skin->test.wordlist_area.y[0]+Skin->test.wordlist_area.h))
	{
		if (dialog.fixed)
		{
			int dialog_x=x+((Skin->reciteword.width - Skin->test.dialog1.w) / 2);
			int dialog_y=y+((Skin->reciteword.height - Skin->test.dialog1.h) / 2);
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->test.dialog1.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->test.dialog1.h))
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
			on_test_wordlistadjust_valuechanged (adj,this);
		}
	}
}

void
CFirstRecite_test::do_up ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue>0)
	{
		nowvalue--;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_test_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_test::do_down ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue < ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
	{
		nowvalue++;
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_test_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_test::do_pageup ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue> 0)
	{
		nowvalue-= (gint) ((GtkAdjustment *)wordlist_adjust)->page_increment;
		if (nowvalue < 0)
			nowvalue = 0;
		((GtkAdjustment *) wordlist_adjust)->value = nowvalue;
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_test_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_test::do_pagedown ()
{
	int nowvalue = ((gint) ((GtkAdjustment *)wordlist_adjust)->value);
	if (nowvalue < ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
	{
		nowvalue+=(gint) ((GtkAdjustment *)wordlist_adjust)->page_increment;
		if (nowvalue > ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size)))
			nowvalue = ((gint) (((GtkAdjustment *)wordlist_adjust)->upper - ((GtkAdjustment *)wordlist_adjust)->page_size));
		(((GtkAdjustment *) wordlist_adjust)->value = nowvalue);
		g_signal_emit_by_name (wordlist_adjust, "changed");
		on_test_wordlistadjust_valuechanged((GtkAdjustment *)wordlist_adjust,this);
	}
}

void
CFirstRecite_test::init ()
{
	g_pReciteWord->status = STATUS_FIRSTRECITE_TEST;
	started = FALSE;
	dialog.fixed = NULL;
	status = FRTS_FIRSTTIME;
	pressanykey_status = 0;
	headchar_label = NULL;
	tested_wordlist_item =
		(int *) g_malloc (g_pReciteWord->now_zu_wordcount *
				  (sizeof (int)));
}

void
CFirstRecite_test::set_wordtest_label (gint i)
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

	/*
	gint len = strlen (g_pReciteWord->now_zu_words.m[i]);
	GdkWChar *str_wc = g_new (GdkWChar, len + 1);
	gint wlen =
		gdk_mbstowcs (str_wc, g_pReciteWord->now_zu_words.m[i],
			      len + 1);
	*/

	if (phonetic_ckbutton.status)
		gtk_widget_hide(phonetic_label.hbox);
	phonetic_label.set_text (g_pReciteWord->now_zu_words.t[i]);	
	gint w;
	pango_layout_get_pixel_size(gtk_label_get_layout(GTK_LABEL(wordtest_label_1)),&w,NULL);
	gtk_fixed_move (GTK_FIXED (g_pReciteWord->fixed),
			phonetic_label.hbox,
			Skin->test.phonetic_label.x[0] +
			Skin->test.wordtest_label_1.x[0] +
			w
	/*		gdk_text_width_wc (gtk_style_get_font
					   (wordtest_label_1->style), str_wc,
					   wlen)*/, Skin->test.phonetic_label.y[0]);
	//g_free (str_wc);
	if (phonetic_ckbutton.status)
		gtk_widget_show(phonetic_label.hbox);
	if (readword_ckbutton.status)
		readword(g_pReciteWord->now_zu_words.w[i]);
}

void
CFirstRecite_test::set_wordlist_item (int index)
{
	gchar aa[5];
	sprintf(aa,"%2d",index+1);
	gtk_label_set_text (GTK_LABEL (wordlist_scrollvalue_label),
			    aa);

	int i;

	for (i = 0; i < 5; i++)
	{
		if (tested_wordlist_item[i + index] == 1)
		{
			wordlist_item[i].set_label (g_pReciteWord->
						    now_zu_words.w[i +
								   index]);
			wordlist_item[i].set_selected (0);
		}
		else if (tested_wordlist_item[i + index] == -1)
		{
			wordlist_item[i].set_label (g_pReciteWord->
						    now_zu_words.w[i +
								   index]);
			wordlist_item[i].set_selected (-1);
		}
		else
		{
			wordlist_item[i].set_label ("");
			wordlist_item[i].set_selected (0);
		}
	}
}

void
CFirstRecite_test::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->test.test.p[0]);

	wordlist_item[0].create (NULL, g_pReciteWord->fixed,
				 Skin->test.wordlist_item_1.x[0],
				 Skin->test.wordlist_item_1.y[0],
				 Skin->test.wordlist_item_1.w,
				 Skin->test.wordlist_item_1.h,
				 Skin->test.wordlist_item_1.p[0],
				 Skin->test.wordlist_item_1.p[1],
				 Skin->test.wordlist_item_1.p[2]);
	wordlist_item[1].create (wordlist_item[0].group, g_pReciteWord->fixed,
				 Skin->test.wordlist_item_2.x[0],
				 Skin->test.wordlist_item_2.y[0],
				 Skin->test.wordlist_item_2.w,
				 Skin->test.wordlist_item_2.h,
				 Skin->test.wordlist_item_2.p[0],
				 Skin->test.wordlist_item_2.p[1],
				 Skin->test.wordlist_item_2.p[2]);
	wordlist_item[2].create (wordlist_item[1].group, g_pReciteWord->fixed,
				 Skin->test.wordlist_item_3.x[0],
				 Skin->test.wordlist_item_3.y[0],
				 Skin->test.wordlist_item_3.w,
				 Skin->test.wordlist_item_3.h,
				 Skin->test.wordlist_item_3.p[0],
				 Skin->test.wordlist_item_3.p[1],
				 Skin->test.wordlist_item_3.p[2]);
	wordlist_item[3].create (wordlist_item[2].group, g_pReciteWord->fixed,
				 Skin->test.wordlist_item_4.x[0],
				 Skin->test.wordlist_item_4.y[0],
				 Skin->test.wordlist_item_4.w,
				 Skin->test.wordlist_item_4.h,
				 Skin->test.wordlist_item_4.p[0],
				 Skin->test.wordlist_item_4.p[1],
				 Skin->test.wordlist_item_4.p[2]);
	wordlist_item[4].create (wordlist_item[3].group, g_pReciteWord->fixed,
				 Skin->test.wordlist_item_5.x[0],
				 Skin->test.wordlist_item_5.y[0],
				 Skin->test.wordlist_item_5.w,
				 Skin->test.wordlist_item_5.h,
				 Skin->test.wordlist_item_5.p[0],
				 Skin->test.wordlist_item_5.p[1],
				 Skin->test.wordlist_item_5.p[2]);

	start_button.create (g_pReciteWord->fixed, Skin->test.stop_button.x[0],
			     Skin->test.stop_button.y[0],
			     Skin->test.stop_button.p[0],
			     Skin->test.stop_button.p[1],
			     Skin->test.stop_button.p[2],
			     Skin->test.stop_button.p[3], on_test_start_clicked);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed, Skin->test.pause_button.x[0],
			     Skin->test.pause_button.y[0],
			     Skin->test.pause_button.p[0],
			     Skin->test.pause_button.p[1],
			     Skin->test.pause_button.p[2],
			     Skin->test.pause_button.p[3],
			     on_test_pause_clicked);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, pause_button.event_box,"Ctrl-P","");
	group_button.create (g_pReciteWord->fixed, Skin->test.group_button.x[0],
			     Skin->test.group_button.y[0],
			     Skin->test.group_button.p[0],
			     Skin->test.group_button.p[1],
			     Skin->test.group_button.p[2], (GdkPixmap *) NULL,
			     on_test_group_clicked);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, group_button.event_box,"Ctrl-G","");
	skim_button.create (g_pReciteWord->fixed, Skin->test.skim_button.x[0],
			    Skin->test.skim_button.y[0], Skin->test.skim_button.p[0],
			    Skin->test.skim_button.p[1],
			    Skin->test.skim_button.p[2], (GdkPixmap *) NULL,
			    on_test_skim_clicked);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, skim_button.event_box,"Ctrl-B","");
	return_button.create (g_pReciteWord->fixed, Skin->test.return_button.x[0],
			      Skin->test.return_button.y[0],
			      Skin->test.return_button.p[0],
			      Skin->test.return_button.p[1],
			      Skin->test.return_button.p[2], (GdkPixmap *) NULL,
			      on_test_return_clicked);
	meaning_ckbutton.create (g_pReciteWord->fixed, Skin->test.meaning_ckbutton.x[0],
			      Skin->test.meaning_ckbutton.y[0],
			      Skin->test.meaning_ckbutton.p[0],
			      Skin->test.meaning_ckbutton.p[1],
			      Skin->test.meaning_ckbutton.p[2],
				  Skin->test.meaning_ckbutton.p[3],
			      on_test_meaning_ckbutton_clicked);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showmean_1",
			     &tmp_b);
	meaning_ckbutton.set_status(tmp_b);
	readword_ckbutton.create (g_pReciteWord->fixed, Skin->test.readword_ckbutton.x[0],
			      Skin->test.readword_ckbutton.y[0],
			      Skin->test.readword_ckbutton.p[0],
			      Skin->test.readword_ckbutton.p[1],
			      Skin->test.readword_ckbutton.p[2],
				  Skin->test.readword_ckbutton.p[3],
			      on_test_readword_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "readword_1",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	phonetic_ckbutton.create (g_pReciteWord->fixed, Skin->test.phonetic_ckbutton.x[0],
			      Skin->test.phonetic_ckbutton.y[0],
			      Skin->test.phonetic_ckbutton.p[0],
			      Skin->test.phonetic_ckbutton.p[1],
			      Skin->test.phonetic_ckbutton.p[2],
				  Skin->test.phonetic_ckbutton.p[3],
			      on_test_phonetic_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showphonetic_1",
			     &tmp_b);
	phonetic_ckbutton.set_status(tmp_b);
	length_ckbutton.create (g_pReciteWord->fixed, Skin->test.length_ckbutton.x[0],
			      Skin->test.length_ckbutton.y[0],
			      Skin->test.length_ckbutton.p[0],
			      Skin->test.length_ckbutton.p[1],
			      Skin->test.length_ckbutton.p[2],
				  Skin->test.length_ckbutton.p[3],
			      on_test_length_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showlength_1",
			     &tmp_b);
	length_ckbutton.set_status(tmp_b);
	headchar_ckbutton.create (g_pReciteWord->fixed, Skin->test.headchar_ckbutton.x[0],
			      Skin->test.headchar_ckbutton.y[0],
			      Skin->test.headchar_ckbutton.p[0],
			      Skin->test.headchar_ckbutton.p[1],
			      Skin->test.headchar_ckbutton.p[2],
				  Skin->test.headchar_ckbutton.p[3],
			      on_test_headchar_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showheadchar_1",
			     &tmp_b);
	headchar_ckbutton.set_status(tmp_b);
	sndeffect_ckbutton.create (g_pReciteWord->fixed, Skin->test.sndeffect_ckbutton.x[0],
			      Skin->test.sndeffect_ckbutton.y[0],
			      Skin->test.sndeffect_ckbutton.p[0],
			      Skin->test.sndeffect_ckbutton.p[1],
			      Skin->test.sndeffect_ckbutton.p[2],
				  Skin->test.sndeffect_ckbutton.p[3],
			      on_test_sndeffect_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "sndeffect_1",
			     &tmp_b);
	sndeffect_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->test.readword_button.x[0],
			      Skin->test.readword_button.y[0],
			      Skin->test.readword_button.p[0],
			      Skin->test.readword_button.p[1],
			      Skin->test.readword_button.p[2],
			      on_test_readword_button_clicked,this);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, readword_button.event_box,"Tab","");
	wordtest_label_1 = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordtest_label_1,
		       Skin->test.wordtest_label_1.x[0],
		       Skin->test.wordtest_label_1.y[0]);
	if (meaning_ckbutton.status)
		gtk_widget_show (wordtest_label_1);
	wordlist_scrollvalue_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_scrollvalue_label,
		       Skin->test.wordlist_scrollvalue_label.x[0],
		       Skin->test.wordlist_scrollvalue_label.y[0]);
	gtk_widget_show (wordlist_scrollvalue_label);
	word_index_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_index_label,
		       Skin->test.word_index_label.x[0],
		       Skin->test.word_index_label.y[0]);
	gtk_widget_show (word_index_label);
	recited_word_count_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), recited_word_count_label,
		       Skin->test.recited_word_count_label.x[0],
		       Skin->test.recited_word_count_label.y[0]);
	gtk_widget_show (recited_word_count_label);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (aa);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->test.word_count_label.x[0],
		       Skin->test.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	phonetic_label.create (g_pReciteWord->fixed, 0,
			       Skin->test.phonetic_label.y[0],
			       &(Skin->text.b_phonetic));
	if (!phonetic_ckbutton.status)
		gtk_widget_hide(phonetic_label.hbox);
	wordtest_entry.create (g_pReciteWord->fixed,
			       Skin->test.wordtest_entry.x[0],
			       Skin->test.wordtest_entry.y[0],
			       Skin->test.wordtest_entry.w,
			       Skin->test.wordtest_entry.h, "", "", 1,
			       on_test_wordtestentry_keypress);
	wordtest_entry.set_showlength(length_ckbutton.status);
	wordtest_entry.enablesnd = sndeffect_ckbutton.status;
	gtk_widget_grab_focus (wordtest_entry.text_area);

	wordlist_adjust =
		gtk_adjustment_new (0, 0, g_pReciteWord->now_zu_wordcount, 1,
				    5, 5);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK (on_test_wordlistadjust_valuechanged),
			  this);

	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll, Skin->test.wordlist_vscroll.w,
			      Skin->test.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_vscroll,
		       Skin->test.wordlist_vscroll.x[0],
		       Skin->test.wordlist_vscroll.y[0]);

	wrongright_tip = gtk_image_new_from_pixmap (Skin->test.wrongright_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wrongright_tip,
		       Skin->test.wrongright_tip.x[0],
		       Skin->test.wrongright_tip.y[0]);
	headchar_label = gtk_label_new ("");
	gtk_widget_set_name (headchar_label, "firstrecite_test-headchar_label");
	SetFont(headchar_label,true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), headchar_label,
		       Skin->test.headchar_label.x[0], Skin->test.headchar_label.y[0]);
	if (headchar_ckbutton.status)
		gtk_widget_show(headchar_label);
	right_label = gtk_label_new ("");
	SetFont(right_label,true);
/*	gtk_misc_set_alignment (GTK_MISC (right_label), 0, .5);
	gtk_widget_set_size_request (right_label, Skin->test.right_label.w,
			      Skin->test.right_label.h);*/
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_label,
		       Skin->test.right_label.x[0], Skin->test.right_label.y[0]);
	pressanykey_tip = gtk_image_new_from_pixmap (Skin->test.pressanykey_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), pressanykey_tip,
		       Skin->test.pressanykey_tip.x[0],
		       Skin->test.pressanykey_tip.y[0]);

	on_test_start_clicked ();
}

void
CFirstRecite_test::save_setting ()
{
	gboolean tmp_b;
	if (status==FRTS_FIRSTTIME || status==FRTS_FIRSTTIMEOVER)
	{
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showmean_1",
			     &tmp_b);
		if (tmp_b!=meaning_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showmean_1",
					 meaning_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "readword_1",
			     &tmp_b);
		if (tmp_b!=readword_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "readword_1",
					 readword_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showphonetic_1",
			     &tmp_b);
		if (tmp_b!=phonetic_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showphonetic_1",
					 phonetic_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showlength_1",
			     &tmp_b);
		if (tmp_b!=length_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showlength_1",
					 length_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showheadchar_1",
			     &tmp_b);
		if (tmp_b!=headchar_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showheadchar_1",
					 headchar_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "sndeffect_1",
			     &tmp_b);
		if (tmp_b!=sndeffect_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "sndeffect_1",
					 sndeffect_ckbutton.status);
		}
	}
	else if (status==FRTS_SECONDTIME)
	{
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showmean_2",
			     &tmp_b);
		if (tmp_b!=meaning_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showmean_2",
					 meaning_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "readword_2",
			     &tmp_b);
		if (tmp_b!=readword_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "readword_2",
					 readword_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showphonetic_2",
			     &tmp_b);
		if (tmp_b!=phonetic_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showphonetic_2",
					 phonetic_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showlength_2",
			     &tmp_b);
		if (tmp_b!=length_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showlength_2",
					 length_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "showheadchar_2",
			     &tmp_b);
		if (tmp_b!=headchar_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "showheadchar_2",
					 headchar_ckbutton.status);
		}
		rw_cfg_read_boolean(usercfgfile,
			     "firstrecite_test", "sndeffect_2",
			     &tmp_b);
		if (tmp_b!=sndeffect_ckbutton.status)
		{
			rw_cfg_write_boolean(usercfgfile,
					 "firstrecite_test", "sndeffect_2",
					 sndeffect_ckbutton.status);
		}
	}
}

void
CFirstRecite_test::close ()
{
	save_setting();
	
	if (dialog.fixed)
	{
		dialog.close();
	}
	
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
	
	gtk_widget_destroy (wordtest_label_1);
	gtk_widget_destroy (wordlist_scrollvalue_label);
	gtk_widget_destroy (word_index_label);
	gtk_widget_destroy (recited_word_count_label);
	gtk_widget_destroy (word_count_label);
	phonetic_label.destroy ();

	gtk_widget_destroy (wrongright_tip);
	gtk_widget_destroy (right_label);
	gtk_widget_destroy (headchar_label);
	headchar_label = NULL;
	gtk_widget_destroy (pressanykey_tip);

	wordtest_entry.destroy ();

	wordlist_item[0].destroy ();
	wordlist_item[1].destroy ();
	wordlist_item[2].destroy ();
	wordlist_item[3].destroy ();
	wordlist_item[4].destroy ();
	gtk_widget_destroy (wordlist_vscroll);

	g_free (tested_wordlist_item);
}


static void
on_test_dialog_ok_button_clicked (gpointer data)
{
	int i;
	if (g_pReciteWord->firstrecite_test->dialog.testagain_group_robutton.
	    selected)
		i = 0;
	else
		i = 1;
	g_pReciteWord->firstrecite_test->dialog.close ();
	if (g_pReciteWord->firstrecite_test->status == FRTS_FIRSTTIMEOVER)
	{
		if (i == 1)
		{
			/*g_pReciteWord->firstrecite_test->close ();
			g_pReciteWord->show ();*/
		}
		else
		{
			on_test_start_clicked ();
		}
	}
	else
	{
		if (i==1)
		{
		}
		else
		{
			g_pReciteWord->firstrecite_test->close ();
			g_pReciteWord->ShowFirstRecite_group ();
		}
	}
}

CTestDialog::CTestDialog ()
{
}

CTestDialog::~CTestDialog ()
{
}

void
CTestDialog::show ()
{
	fixed = gtk_fixed_new ();
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_widget_set_size_request (fixed, Skin->test.dialog1.w,
			      Skin->test.dialog1.h);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), fixed,
		       (Skin->reciteword.width - Skin->test.dialog1.w) / 2,
		       (Skin->reciteword.height -
			Skin->test.dialog1.h) / 2);
	gtk_widget_show (fixed);
	if (g_pReciteWord->firstrecite_test->status == FRTS_FIRSTTIMEOVER)
		SetBackPixmap (fixed, Skin->test.dialog1.p[0]);
	else
		SetBackPixmap (fixed, Skin->test.dialog2.p[0]);

	testagain_group_robutton.create (NULL, fixed,
				   Skin->test.dialog_testagain_group_robutton.x[0],
				   Skin->test.dialog_testagain_group_robutton.y[0],
				   Skin->test.dialog_testagain_group_robutton.p[0],
				   Skin->test.dialog_testagain_group_robutton.p[1],
				   Skin->test.dialog_testagain_group_robutton.p[2],
				   Skin->test.dialog_testagain_group_robutton.p[3],
				   NULL,NULL);
	testover_robutton.create (testagain_group_robutton.group, fixed,
				  Skin->test.dialog_testover_robutton.x[0],
				  Skin->test.dialog_testover_robutton.y[0],
				  Skin->test.dialog_testover_robutton.p[0],
				  Skin->test.dialog_testover_robutton.p[1],
				  Skin->test.dialog_testover_robutton.p[2],
				  Skin->test.dialog_testover_robutton.p[3],
				  NULL,NULL);
   	gint tmp_i;
	if (g_pReciteWord->firstrecite_test->status == FRTS_FIRSTTIMEOVER)
		rw_cfg_read_int(usercfgfile, "firstrecite_test", "dialog1_index", &tmp_i);
	else
		rw_cfg_read_int(usercfgfile, "firstrecite_test", "dialog2_index", &tmp_i);
	if (tmp_i==1)
		testover_robutton.set_selected(true);

	ok_button.create (fixed, Skin->test.dialog_ok_button.x[0],
			  Skin->test.dialog_ok_button.y[0],
			  Skin->test.dialog_ok_button.p[0],
			  Skin->test.dialog_ok_button.p[1], NULL,
			  on_test_dialog_ok_button_clicked, NULL);
}

void
CTestDialog::close ()
{
	gint tmp_i,tmp_index;
	if (g_pReciteWord->firstrecite_test->status == FRTS_FIRSTTIMEOVER)
		rw_cfg_read_int(usercfgfile, "firstrecite_test", "dialog1_index", &tmp_i);
	else
		rw_cfg_read_int(usercfgfile, "firstrecite_test", "dialog2_index", &tmp_i);
	if (testagain_group_robutton.selected)
		tmp_index = 0;
	else
		tmp_index = 1;
	if (tmp_i!=tmp_index)
	{
		if (g_pReciteWord->firstrecite_test->status == FRTS_FIRSTTIMEOVER)
			rw_cfg_write_int(usercfgfile, "firstrecite_test", "dialog1_index", tmp_index);
		else
			rw_cfg_write_int(usercfgfile, "firstrecite_test", "dialog2_index", tmp_index);
	}
	testagain_group_robutton.destroy();
	testover_robutton.destroy();
	ok_button.destroy();
	gtk_widget_destroy (fixed);
	fixed = NULL;
}

void
CTestDialog::do_up ()
{
	if (!testagain_group_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		testagain_group_robutton.set_selected(true);
	}
}

void
CTestDialog::do_down ()
{
	if (!testover_robutton.selected)
	{
		playsound (SND_BUTTONUP);
		testover_robutton.set_selected(true);
	}
}


tListItem2::tListItem2 ()
{
}

void
tListItem2::init ()
{
	selected = FALSE;
	this->group = g_slist_prepend ((GSList *) NULL, this);
}

void
tListItem2::create (GSList * group, GtkWidget * pfixed, gint x, gint y,
		    gint w, gint h, GdkPixmap * pix_1, GdkPixmap * pix_2,
		    GdkPixmap * pix_3)
{
	init ();

	pix_normal = pix_1;
	pix_selected = pix_2;
	pix_finished = pix_3;

	fixed = gtk_fixed_new ();
	gtk_widget_set_size_request (fixed, w, h);
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	gtk_fixed_put (GTK_FIXED (pfixed), fixed, x, y);
	label = gtk_label_new ("");
	SetFont(label,false);
	gtk_fixed_put (GTK_FIXED (fixed), label, 6, 0);
	gtk_widget_show (label);
	gtk_widget_show (fixed);

	SetBackPixmap (fixed, pix_normal);

	if (group)
		set_group (group);
	else
		set_selected (TRUE);
}

void
tListItem2::set_group (GSList * group)
{
	if (this->group)
	{
		GSList *slist;

		this->group = g_slist_remove (this->group, this);

		for (slist = this->group; slist; slist = slist->next)
		{
			tListItem2 *tmp_listitem;
			tmp_listitem = (tListItem2 *) slist->data;
			tmp_listitem->group = this->group;
		}
	}

	this->group = g_slist_prepend (group, this);

	if (group)
	{
		GSList *slist;

		for (slist = group; slist; slist = slist->next)
		{
			tListItem2 *tmp_listitem;
			tmp_listitem = (tListItem2 *) slist->data;

			tmp_listitem->group = this->group;
		}
	}

	set_selected (group == NULL);
}

void
tListItem2::clean_selected ()
{
	GSList *tmp_list;
	tListItem2 *tmp_listitem;
	tmp_list = this->group;
	while (tmp_list)
	{
		tmp_listitem = (tListItem2 *) tmp_list->data;
		tmp_list = tmp_list->next;
		if (tmp_listitem->selected == 1)
		{
			tmp_listitem->set_selected (FALSE);
			break;
		}
	}
}

void
tListItem2::set_selected (int a)
{
	if (this->selected != a)
	{
		if (a == 1)
		{
			clean_selected ();
			SetBackPixmap (fixed, pix_selected);
		}
		else if (a == -1)
		{
			SetBackPixmap (fixed, pix_finished);
		}
		else
			SetBackPixmap (fixed, pix_normal);
		gtk_widget_queue_draw (fixed);
		this->selected = a;
	}
}

void
tListItem2::set_label (gchar * str)
{
	gtk_label_set_text (GTK_LABEL (label), str);
}

void
tListItem2::destroy ()
{
	GSList *tmp_list;
	tListItem2 *tmp_listitem;

	this->group = g_slist_remove (this->group, this);
	tmp_list = this->group;

	while (tmp_list)
	{
		tmp_listitem = (tListItem2 *) tmp_list->data;
		tmp_list = tmp_list->next;

		tmp_listitem->group = this->group;
	}

	gtk_widget_destroy (fixed);
}

tListItem2::~tListItem2 ()
{
}
