#include "typing.h"
#include "reciteword.h"
#include "skin.h"
#include "sound.h"
#include "readword.h"
#include "rwconfig.h"
#include <cstring>
#include <cstdio>
#include <gdk/gdkkeysyms.h>


extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;


static gint
on_label_expose (GtkWidget * widget, GdkEventExpose * event, tLabel * parent)
{
	gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 parent->draw_pixmap,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	return FALSE;
}

tLabel::tLabel ()
{
}

tLabel::~tLabel ()
{
}

void
tLabel::init ()
{
	show_line = FALSE;
	text_w[0] = '\0';
	text_m[0] = '\0';
	layout_w=NULL;
	layout_m=NULL;

	gc = gdk_gc_new (g_pReciteWord->window->window);
	GdkColormap *cmap = gdk_rgb_get_colormap ();
	GdkColor color = { 0, 0xFFFF, 35584, 0x0000 };
	gdk_colormap_alloc_color (cmap, &(color), FALSE, TRUE);
	gdk_gc_set_foreground (gc, &color);
	gdk_colormap_free_colors (cmap, &color, 1);
}

void
tLabel::create (GtkWidget * pfixed, gint x, gint y, gint w, gint h)
{
	init ();

	width = w;
	height = h;


	draw_area = gtk_drawing_area_new ();
	gtk_widget_set_size_request (draw_area, width, height);


	g_signal_connect (G_OBJECT (draw_area), "expose_event",
			  G_CALLBACK(on_label_expose), this);

	gtk_fixed_put (GTK_FIXED (pfixed), draw_area, x, y);
	gtk_widget_show (draw_area);
	draw_pixmap =
		gdk_pixmap_new (draw_area->window, width, height,
				gdk_rgb_get_visual ()->depth);
//	draw ();

}

void
tLabel::draw ()
{
	gdk_draw_rectangle (draw_pixmap, draw_area->style->white_gc,
			    TRUE, 0, 0, width, height);
	gint w=0,h,y,w1=0,h1,y1;
	if (layout_w)
	{
		pango_layout_get_pixel_size(layout_w,&w,&h);
		y = (height - h) / 2 ;
		gdk_draw_layout(draw_pixmap,draw_area->style->black_gc,5,y,layout_w);
		if (layout_m)
		{
			pango_layout_get_pixel_size(layout_m,&w1,&h1);
			y1 = (height - h1) / 2;
			gdk_draw_layout(draw_pixmap,draw_area->style->black_gc,w+20,y1,layout_m);		
		}
	}
	
	if (show_line)
		gdk_draw_rectangle (draw_pixmap, gc,
				    TRUE, 5, height-1,
				    w1 + w + 15, 1);
	gtk_widget_queue_draw (draw_area);
}

void
tLabel::set_label (gchar * str_w, gchar * str_m)
{
	strcpy (text_w, str_w);
	if (str_m)
		strcpy (text_m, str_m);
	else
		text_m[0]='\0';
		
	if (layout_w)
		pango_layout_set_text(layout_w,text_w,-1);
	else
	{
		layout_w=gtk_widget_create_pango_layout (draw_area, text_w);
		pango_layout_set_font_description(layout_w,Skin->reciteword.n_font);
	}
	if (layout_m)
		pango_layout_set_text(layout_m,text_m,-1);
	else
		layout_m=gtk_widget_create_pango_layout (draw_area, text_m);

	draw ();
}

void
tLabel::set_showline (gint a)
{
	if (show_line != a)
	{
		show_line = a;
		draw ();
	}
}

void
tLabel::destroy ()
{
	if (layout_w)
		g_object_unref (G_OBJECT (layout_w));
	if (layout_m)
		g_object_unref (G_OBJECT (layout_m));
	gtk_widget_destroy (draw_area);
	g_object_unref (G_OBJECT (gc));
	//gdk_gc_unref (gc);
}

static gboolean
on_typing_speedinfobar_button_press(GtkWidget * widget, GdkEvent * event,
			    CTyping * parent)
{
	gboolean visible = GTK_WIDGET_VISIBLE(parent->speed_info_pixmap);
	if (visible)
		gtk_widget_hide(parent->speed_info_pixmap);
	else
		gtk_widget_show(parent->speed_info_pixmap);
	return true;
}

static void do_typing_pause ()
{
	if (g_pReciteWord->typing->pressanykey_status == 1)
	{
		g_pReciteWord->typing->pressanykey_status =
			2;
		gtk_widget_hide (g_pReciteWord->typing->
				 pressanykey_tip);
	}
	else
	{
		if (g_pReciteWord->typing->speed_timer)
		{
			g_source_remove (g_pReciteWord->typing->
					    speed_timer);
			g_pReciteWord->typing->speed_timer = 0;
		}
	}
	g_pReciteWord->typing->wordtest_entry.set_editable (FALSE);
}

static void
on_typing_start_clicked ()
{
	if (g_pReciteWord->typing->started)
	{
		do_typing_pause();
		g_pReciteWord->typing->started = FALSE;
		g_pReciteWord->typing->pause_button.set_enable (FALSE);
		g_pReciteWord->typing->start_button.reset (Skin->typing.
							   start_button.p[0],
							   Skin->typing.
							   start_button.p[1],
							   Skin->typing.
							   start_button.p[2],
							   (GdkPixmap *)
							   NULL);
	}
	else
	{
		if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
			g_pReciteWord->mark->close ();
		g_pReciteWord->typing->started = TRUE;
		g_pReciteWord->typing->pressanykey_status = 1;
		gtk_widget_show (g_pReciteWord->typing->pressanykey_tip);
		g_pReciteWord->typing->wordtest_entry.set_text ("");
		g_pReciteWord->typing->wordtest_entry.set_editable (true);
		g_pReciteWord->typing->set_key_pixmap ('\0');
		((GtkAdjustment *) g_pReciteWord->typing->wordlist_adjust)->
			value = 0;
		((GtkAdjustment *) g_pReciteWord->typing->wordlist_adjust)->upper = g_pReciteWord->now_zu_wordcount;  //needed when chooseword again.
		g_signal_emit_by_name (g_pReciteWord->typing->
					 wordlist_adjust, "changed");
		g_pReciteWord->typing->selected_wordlist_item = 0;
		g_pReciteWord->typing->wordlist_item[0].set_showline (TRUE);
		int i;
		for (i = 1; i < 6; i++)
		{
			g_pReciteWord->typing->wordlist_item[i].
				set_showline (FALSE);
		}
		g_pReciteWord->typing->set_wordlist_item (0);
		g_pReciteWord->typing->set_wordtest_label (0);
		g_pReciteWord->typing->set_speed_pixmap (0);
		g_pReciteWord->typing->set_speed_number (0);	
		g_pReciteWord->typing->pause_button.set_enable (TRUE);
		g_pReciteWord->typing->start_button.reset (Skin->typing.
							   stop_button.p[0],
							   Skin->typing.
							   stop_button.p[1],
							   Skin->typing.
							   stop_button.p[2],
							   Skin->typing.
							   stop_button.p[3]);
	}
}

static gint typing_speed_log (gpointer data);

static void
on_typing_pause_clicked ()
{
	if (g_pReciteWord->typing->started)
	{
		do_typing_pause ();
		g_pReciteWord->typing->start_button.
			set_enable (FALSE);
		g_pReciteWord->typing->pause_button.reset (Skin->
								     typing.
								     continue_button.
								     p[0],
								     Skin->
								     typing.
								     continue_button.
								     p[1],
								     Skin->
								     typing.
								     continue_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->typing->started = FALSE;
	}
	else
	{
		if (g_pReciteWord->typing->pressanykey_status == 2)
		{
			g_pReciteWord->typing->pressanykey_status =
				1;
			gtk_widget_show (g_pReciteWord->typing->
					 pressanykey_tip);
		}
		else		//pressanykey_stauts==0
		{
			g_pReciteWord->typing->speed_timer =
				g_timeout_add (200, typing_speed_log, NULL);	
		}
		g_pReciteWord->typing->wordtest_entry.set_editable (true);
		g_pReciteWord->typing->start_button.
			set_enable (TRUE);
		g_pReciteWord->typing->pause_button.reset (Skin->
								     typing.
								     pause_button.
								     p[0],
								     Skin->
								     typing.
								     pause_button.
								     p[1],
								     Skin->
								     typing.
								     pause_button.
								     p[2],
								     Skin->
								     typing.
								     pause_button.
								     p[3]);
		g_pReciteWord->typing->started = TRUE;
	}
}

static void
on_typing_return_clicked ()
{
	g_pReciteWord->typing->close ();
	g_pReciteWord->show ();
}

static void
on_typing_sndeffect_ckbutton_clicked ()
{
	g_pReciteWord->typing->wordtest_entry.enablesnd = g_pReciteWord->typing->sndeffect_ckbutton.status;
}

static void
on_typing_readword_button_clicked (gpointer data)
{
	CTyping *parent = (CTyping *)data;
	if (parent->selected_wordlist_item>=0 && parent->selected_wordlist_item<g_pReciteWord->now_zu_wordcount)
		readword(g_pReciteWord->now_zu_words.w[parent->selected_wordlist_item]);
}

static void 
on_typing_chooseword_ok_clicked ()
{
	if (g_pReciteWord->typing->start_pause_button_status==0)
	{
		g_pReciteWord->typing->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->typing->start_pause_button_status==1)
	{
		g_pReciteWord->typing->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->typing->start_pause_button_status==2)
	{
		g_pReciteWord->typing->start_button.set_enable(true);
	}
	if (!g_pReciteWord->typing->start_button.enable)
		on_typing_pause_clicked();  //continue
	if (g_pReciteWord->typing->started)
		on_typing_start_clicked();  //stop
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	gtk_label_set_text(GTK_LABEL(g_pReciteWord->typing->word_count_label),aa);
	on_typing_start_clicked();  //start
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->typing->chooseword_button.set_enable(true);
	gtk_widget_grab_focus (g_pReciteWord->typing->wordtest_entry.text_area);
}

static void 
on_typing_chooseword_cancel_clicked ()
{
	g_pReciteWord->chooseword->close ();
	if (g_pReciteWord->typing->start_pause_button_status==0)
	{
		g_pReciteWord->typing->pause_button.set_enable(true);
		on_typing_pause_clicked();  //continue
	}
	else if (g_pReciteWord->typing->start_pause_button_status==1)
	{
		g_pReciteWord->typing->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->typing->start_pause_button_status==2)
	{
		g_pReciteWord->typing->start_button.set_enable(true);
	}
	g_pReciteWord->typing->chooseword_button.set_enable(true);
	gtk_widget_grab_focus (g_pReciteWord->typing->wordtest_entry.text_area);
}

static void
on_typing_chooseword_button_clicked (gpointer data)
{	
	if (!g_pReciteWord->typing->start_button.enable)
	{
		g_pReciteWord->typing->start_pause_button_status=1;
		g_pReciteWord->typing->pause_button.set_enable(false);
	}
	else if (!g_pReciteWord->typing->pause_button.enable)
	{
		g_pReciteWord->typing->start_pause_button_status=2;
		g_pReciteWord->typing->start_button.set_enable(false);
	}
	else
	{
		g_pReciteWord->typing->start_pause_button_status=0;
		on_typing_pause_clicked();  //pause
		g_pReciteWord->typing->pause_button.set_enable(false);
	}
	
	g_pReciteWord->PopupChooseword (on_typing_chooseword_ok_clicked,
					on_typing_chooseword_cancel_clicked, &(g_pReciteWord->typing_chooseword_info));
	g_pReciteWord->typing->chooseword_button.set_enable(false);
}

static void
on_typing_wordlistadjust_valuechanged (GtkAdjustment * adj, CTyping * parent)
{
	int newvalue = ((gint) adj->value);
	int index = -1;

	if (((parent->selected_wordlist_item) >= newvalue)
	    && ((parent->selected_wordlist_item) < (newvalue + 6)))
	{
		index = parent->selected_wordlist_item - newvalue;
		parent->wordlist_item[index].set_showline (1);
	}
	int i;
	for (i = 0; i < 6; i++)
	{
		if (i != index)
			parent->wordlist_item[i].set_showline (0);
	}

	parent->set_wordlist_item (newvalue);
}

static gint
typing_speed_log (gpointer data)
{
	g_pReciteWord->typing->speed_time++;

	gint speed_value, speed_key;
	speed_key = g_pReciteWord->typing->speed_key;
	speed_value = (speed_key * 300) / g_pReciteWord->typing->speed_time;
	if (speed_value != g_pReciteWord->typing->speed_value)
	{
		g_pReciteWord->typing->speed_value = speed_value;
		g_pReciteWord->typing->set_speed_number (speed_value);
	}

	gint speed_tmpvalue;
	g_pReciteWord->typing->speed_oldkey[g_pReciteWord->typing->
					    speed_turn] = speed_key;
	g_pReciteWord->typing->speed_turn++;
	if (g_pReciteWord->typing->speed_turn == 30)
		g_pReciteWord->typing->speed_turn = 0;
	speed_tmpvalue =
		(speed_key -
		 g_pReciteWord->typing->speed_oldkey[g_pReciteWord->typing->
						     speed_turn]);
	if (speed_tmpvalue != g_pReciteWord->typing->speed_tmpvalue)
	{
		g_pReciteWord->typing->speed_tmpvalue = speed_tmpvalue;
		g_pReciteWord->typing->set_speed_pixmap (speed_tmpvalue);
	}

	return TRUE;
}

static gint
on_typing_wordtestentry_keypress (gint calltime, gint keyval)
{
	if (calltime == 1)
		return TRUE;
	if (!g_pReciteWord->typing->started)
		return true;
	if (g_pReciteWord->typing->pressanykey_status==1)
	{
		if (keyval == GDK_space)
		{
			g_pReciteWord->typing->pressanykey_status = 0;
			gtk_widget_hide (g_pReciteWord->typing->
					 pressanykey_tip);
			g_pReciteWord->typing->wordtest_entry.
				set_editable (TRUE);
			g_pReciteWord->typing->set_key_pixmap (g_pReciteWord->
							       now_zu_words.
							       w
							       [g_pReciteWord->
								typing->
								selected_wordlist_item]
							       [0]);
			g_pReciteWord->typing->speed_key = 0;
			g_pReciteWord->typing->speed_time = 0;
			g_pReciteWord->typing->speed_value = 0;
			g_pReciteWord->typing->speed_tmpvalue = 0;
			gint i;
			for (i = 0; i < 30; i++)
				g_pReciteWord->typing->speed_oldkey[i] = 0;
			g_pReciteWord->typing->speed_turn = 0;
			g_pReciteWord->typing->speed_timer =
				g_timeout_add (200, typing_speed_log, NULL);
			if (g_pReciteWord->typing->readword_ckbutton.status)
				readword(g_pReciteWord->now_zu_words.w[g_pReciteWord->typing->selected_wordlist_item]);
		}
		else
		{
			if ((keyval >= 0x20) && (keyval <= 0xFF))
				playsound(SND_DING);
		}
		return TRUE;
	}

	tEntry *parent = &(g_pReciteWord->typing->wordtest_entry);

	if (parent->editable == FALSE)
		return TRUE;

	gchar text0[256], text[256];
	gint length;
	guint position;
	gboolean entry_re_enablesnd=FALSE;

	position = parent->position;
	strcpy (text, parent->text);
	strcpy (text0, parent->text);
	length = strlen (text0);

	switch (keyval)
	{
	case GDK_Home:
	case GDK_End:
	case GDK_Left:
	case GDK_Right:
	case GDK_Tab:
	case GDK_Escape:
		break;
	case GDK_BackSpace:
	case GDK_Delete:
	case GDK_Return:
		text[position] = '\0';
		break;
	default:
		if ((length < parent->max_length) && (keyval >= 0x20)
		    && (keyval <= 0xFF))
		{
			if (position ==
			    strlen (g_pReciteWord->now_zu_words.
				    w[g_pReciteWord->typing->
				      selected_wordlist_item]))
			{
				if (keyval == GDK_space)
				{
					if (g_pReciteWord->typing->
					    selected_wordlist_item >=
					    g_pReciteWord->now_zu_wordcount -
					    1)
					{
						on_typing_start_clicked ();  //stop
						g_pReciteWord->typing->wordtest_entry.set_text ("");
						gchar aa[5];
						sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
						gtk_label_set_text(GTK_LABEL(g_pReciteWord->typing->recited_word_count_label),aa);
						g_pReciteWord->PopupMark();
						if (g_pReciteWord->now_zu_index >= 0) // come in typing by chooseword's by_group
						{
							g_pReciteWord->typing_chooseword_info.by = 0; // by group next time.
							g_pReciteWord->typing_chooseword_info.zu_index = g_pReciteWord->now_zu_index+1;
						}
						else // now_zu_index == -1,it must be set by chooseword dialog's by_scope.
						{
							g_pReciteWord->typing_chooseword_info.by = 1; // by scope
							g_pReciteWord->typing_chooseword_info.scope_start = g_pReciteWord->chooseword->last_scope_start + g_pReciteWord->chooseword->last_scope_count;
							g_pReciteWord->typing_chooseword_info.scope_count = g_pReciteWord->chooseword->last_scope_count;
						}
						return TRUE;
					}
					parent->set_text ("");
					g_pReciteWord->typing->
						selected_wordlist_item++;
					g_pReciteWord->typing->
						set_key_pixmap
						(g_pReciteWord->now_zu_words.
						 w[g_pReciteWord->typing->
						   selected_wordlist_item]
						 [0]);
					g_pReciteWord->typing->speed_key++;
					int nowvalue =
						((gint)
						 ((GtkAdjustment *)
						  g_pReciteWord->typing->
						  wordlist_adjust)->value);
					if ((g_pReciteWord->typing->
					     selected_wordlist_item) >=
					    (nowvalue + 6)
					    ||
					    ((g_pReciteWord->typing->
					      selected_wordlist_item) <
					     nowvalue))
					{
						nowvalue =
							g_pReciteWord->
							typing->
							selected_wordlist_item;
						if (nowvalue >
						    (g_pReciteWord->
						     now_zu_wordcount - 6))
							nowvalue =
								(g_pReciteWord->
								 now_zu_wordcount
								 - 6);
						((GtkAdjustment *)
						 g_pReciteWord->typing->
						 wordlist_adjust)->value =
				      nowvalue;
						g_signal_emit_by_name
							(g_pReciteWord->
							 typing->
							 wordlist_adjust,
							 "changed");
					}
					on_typing_wordlistadjust_valuechanged
						((GtkAdjustment *)
						 g_pReciteWord->typing->
						 wordlist_adjust,
						 g_pReciteWord->typing);
					g_pReciteWord->typing->
						set_wordtest_label
						(g_pReciteWord->typing->
						 selected_wordlist_item);
					if (g_pReciteWord->typing->readword_ckbutton.status)
						readword(g_pReciteWord->now_zu_words.w[g_pReciteWord->typing->selected_wordlist_item]);
					return TRUE;
				}
			}

			text0[position] = '\0';
			gchar key = keyval;
			sprintf (text, "%s%c", text0, key);
			
			if (key ==
			    g_pReciteWord->now_zu_words.w[g_pReciteWord->
							  typing->
							  selected_wordlist_item]
			    [position])
			{
				position++;
				g_pReciteWord->typing->
					set_key_pixmap (g_pReciteWord->
							now_zu_words.
							w[g_pReciteWord->
							  typing->
							  selected_wordlist_item]
							[position]);
				g_pReciteWord->typing->speed_key++;
			}
			else
			{
				if (parent->enablesnd)
				{
					entry_re_enablesnd = TRUE;
					parent->enablesnd = FALSE;  // pause entry's sound					
					playsound(SND_DING);
				}
			}
		}
		break;
	}
	parent->changed (text, position, keyval);
	if (entry_re_enablesnd)
	{
		parent->enablesnd = TRUE;
	}

	return TRUE;		//always return TRUE,so the origin function will not run.
}


CTyping::CTyping ()
{
	skin_load_text (g_pReciteWord->window);
	skin_load_typing (g_pReciteWord->window);
}

CTyping::~CTyping ()
{
}

void
CTyping::do_scroll(GdkEventScroll * event)
{
	if (!wordlist_vscroll)
		return;
	int x, y;
	gtk_window_get_position (GTK_WINDOW(g_pReciteWord->window), &x, &y);
	if ((event->x_root>=x+Skin->typing.wordlist_area.x[0])&&(event->x_root<=x+Skin->typing.wordlist_area.x[0]+Skin->typing.wordlist_area.w)
		&&(event->y_root>=y+Skin->typing.wordlist_area.y[0])&&(event->y_root<=y+Skin->typing.wordlist_area.y[0]+Skin->typing.wordlist_area.h))
	{
		if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		{
			int dialog_x=x+((Skin->reciteword.width - Skin->chooseword.chooseword.w) / 2);
			int dialog_y=y+((Skin->reciteword.height - Skin->chooseword.chooseword.h) / 2);
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->chooseword.chooseword.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->chooseword.chooseword.h))
				return;
		}		
		if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
		{
			int dialog_x=x+((Skin->reciteword.width - Skin->mark.mark.w) / 2);
			int dialog_y=y+((Skin->reciteword.height - Skin->mark.mark.h) / 2);
			if ((event->x_root>=dialog_x)&&(event->x_root<=dialog_x+Skin->mark.mark.w)
				&&(event->y_root>=dialog_y)&&(event->y_root<=dialog_y+Skin->mark.mark.h))
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
			on_typing_wordlistadjust_valuechanged (adj,this);
		}
	}
}

void
CTyping::init ()
{
	g_pReciteWord->status = STATUS_TYPING;
	status = FALSE;
	started = FALSE;
	wordlist_vscroll = NULL;
}

void
CTyping::init_2 ()
{
	status = TRUE;
	key_pixmap_o_a = 53;	//right space key
	key_pixmap_o_b = 1;	//left shirft key
	key_pixmap_s_a = 0;
	key_pixmap_s_b = 0;
	speed_timer = 0;
}

static void
typing_show_2 ()
{
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->typing->show_2 ();
}

void
CTyping::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->typing.typing.p[0]);

	return_button.create (g_pReciteWord->fixed,
			      Skin->typing.return_button.x[0],
			      Skin->typing.return_button.y[0],
			      Skin->typing.return_button.p[0],
			      Skin->typing.return_button.p[1],
			      Skin->typing.return_button.p[2],
			      (GdkPixmap *) NULL, on_typing_return_clicked);

	g_pReciteWord->PopupChooseword (typing_show_2,
					on_typing_return_clicked, &(g_pReciteWord->typing_chooseword_info));
}

void
CTyping::show_2 ()
{
	init_2 ();

	wordlist_item[0].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[0].x[0],
				 Skin->typing.wordlist_item[0].y[0],
				 Skin->typing.wordlist_item[0].w,
				 Skin->typing.wordlist_item[0].h);
	wordlist_item[1].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[1].x[0],
				 Skin->typing.wordlist_item[1].y[0],
				 Skin->typing.wordlist_item[1].w,
				 Skin->typing.wordlist_item[1].h);
	wordlist_item[2].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[2].x[0],
				 Skin->typing.wordlist_item[2].y[0],
				 Skin->typing.wordlist_item[2].w,
				 Skin->typing.wordlist_item[2].h);
	wordlist_item[3].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[3].x[0],
				 Skin->typing.wordlist_item[3].y[0],
				 Skin->typing.wordlist_item[3].w,
				 Skin->typing.wordlist_item[3].h);
	wordlist_item[4].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[4].x[0],
				 Skin->typing.wordlist_item[4].y[0],
				 Skin->typing.wordlist_item[4].w,
				 Skin->typing.wordlist_item[4].h);
	wordlist_item[5].create (g_pReciteWord->fixed,
				 Skin->typing.wordlist_item[5].x[0],
				 Skin->typing.wordlist_item[5].y[0],
				 Skin->typing.wordlist_item[5].w,
				 Skin->typing.wordlist_item[5].h);

	wordlist_adjust =
		gtk_adjustment_new (0, 0, g_pReciteWord->now_zu_wordcount, 1,
				    6, 6);
	g_signal_connect (G_OBJECT (wordlist_adjust), "value_changed",
			  G_CALLBACK (on_typing_wordlistadjust_valuechanged),
			  this);

	wordlist_vscroll =
		gtk_vscrollbar_new ((GtkAdjustment *) wordlist_adjust);
	gtk_widget_set_size_request (wordlist_vscroll,
			      Skin->typing.wordlist_vscroll.w,
			      Skin->typing.wordlist_vscroll.h);
	gtk_widget_show (wordlist_vscroll);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_vscroll,
		       Skin->typing.wordlist_vscroll.x[0],
		       Skin->typing.wordlist_vscroll.y[0]);


	wordtest_label_1 = gtk_label_new ("");
	SetFont(wordtest_label_1,true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordtest_label_1,
		       Skin->typing.wordtest_label_1.x[0],
		       Skin->typing.wordtest_label_1.y[0]);
	gtk_widget_show (wordtest_label_1);
	recited_word_count_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), recited_word_count_label,
		       Skin->typing.recited_word_count_label.x[0],
		       Skin->typing.recited_word_count_label.y[0]);
	gtk_widget_show (recited_word_count_label);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (aa);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->typing.word_count_label.x[0],
		       Skin->typing.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	readword_ckbutton.create (g_pReciteWord->fixed,
			      Skin->typing.readword_ckbutton.x[0],
			      Skin->typing.readword_ckbutton.y[0],
			      Skin->typing.readword_ckbutton.p[0],
			      Skin->typing.readword_ckbutton.p[1],
			      Skin->typing.readword_ckbutton.p[2],
			      Skin->typing.readword_ckbutton.p[3], NULL);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "readword",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	phonetic_ckbutton.create (g_pReciteWord->fixed,
			      Skin->typing.phonetic_ckbutton.x[0],
			      Skin->typing.phonetic_ckbutton.y[0],
			      Skin->typing.phonetic_ckbutton.p[0],
			      Skin->typing.phonetic_ckbutton.p[1],
			      Skin->typing.phonetic_ckbutton.p[2],
			      Skin->typing.phonetic_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showphonetic",
			     &tmp_b);
	phonetic_ckbutton.set_status(tmp_b);
	meaning_ckbutton.create (g_pReciteWord->fixed,
			      Skin->typing.meaning_ckbutton.x[0],
			      Skin->typing.meaning_ckbutton.y[0],
			      Skin->typing.meaning_ckbutton.p[0],
			      Skin->typing.meaning_ckbutton.p[1],
			      Skin->typing.meaning_ckbutton.p[2],
			      Skin->typing.meaning_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showmean",
			     &tmp_b);
	meaning_ckbutton.set_status(tmp_b);
	sndeffect_ckbutton.create (g_pReciteWord->fixed,
			      Skin->typing.sndeffect_ckbutton.x[0],
			      Skin->typing.sndeffect_ckbutton.y[0],
			      Skin->typing.sndeffect_ckbutton.p[0],
			      Skin->typing.sndeffect_ckbutton.p[1],
			      Skin->typing.sndeffect_ckbutton.p[2],
			      Skin->typing.sndeffect_ckbutton.p[3],
				  on_typing_sndeffect_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "sndeffect",
			     &tmp_b);
	sndeffect_ckbutton.set_status(tmp_b);
	fingering_ckbutton.create (g_pReciteWord->fixed,
			      Skin->typing.fingering_ckbutton.x[0],
			      Skin->typing.fingering_ckbutton.y[0],
			      Skin->typing.fingering_ckbutton.p[0],
			      Skin->typing.fingering_ckbutton.p[1],
			      Skin->typing.fingering_ckbutton.p[2],
			      Skin->typing.fingering_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showfingering",
			     &tmp_b);
	fingering_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->typing.readword_button.x[0],
			      Skin->typing.readword_button.y[0],
			      Skin->typing.readword_button.p[0],
			      Skin->typing.readword_button.p[1],
			      Skin->typing.readword_button.p[2],
			      on_typing_readword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), readword_button.event_box,"Tab","");
	chooseword_button.create (g_pReciteWord->fixed, Skin->typing.chooseword_button.x[0],
			      Skin->typing.chooseword_button.y[0],
			      Skin->typing.chooseword_button.p[0],
			      Skin->typing.chooseword_button.p[1],
			      Skin->typing.chooseword_button.p[2],
			      on_typing_chooseword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), chooseword_button.event_box,"Ctrl-W","");
	
	wordtest_entry.create (g_pReciteWord->fixed,
			       Skin->typing.wordtest_entry.x[0],
			       Skin->typing.wordtest_entry.y[0],
			       Skin->typing.wordtest_entry.w,
			       Skin->typing.wordtest_entry.h, "", NULL, 0,
			       on_typing_wordtestentry_keypress);
	wordtest_entry.set_editable (FALSE);
	wordtest_entry.enablesnd = sndeffect_ckbutton.status;
	gtk_widget_grab_focus (wordtest_entry.text_area);

	start_button.create (g_pReciteWord->fixed, Skin->typing.stop_button.x[0],
			     Skin->typing.stop_button.y[0],
			     Skin->typing.stop_button.p[0],
			     Skin->typing.stop_button.p[1],
			     Skin->typing.stop_button.p[2],
			     Skin->typing.stop_button.p[3],
			     on_typing_start_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed, Skin->typing.pause_button.x[0],
			     Skin->typing.pause_button.y[0],
			     Skin->typing.pause_button.p[0],
			     Skin->typing.pause_button.p[1],
			     Skin->typing.pause_button.p[2],
			     Skin->typing.pause_button.p[3],
			     on_typing_pause_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), pause_button.event_box,"Ctrl-P","");
	pressanykey_tip =
		gtk_image_new_from_pixmap (Skin->typing.pressanykey_tip.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), pressanykey_tip,
		       Skin->typing.pressanykey_tip.x[0],
		       Skin->typing.pressanykey_tip.y[0]);

	speed_info_pixmap =	gtk_image_new_from_pixmap (Skin->typing.speed_info_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_info_pixmap,
		       Skin->typing.speed_info_pixmap.x[0],
		       Skin->typing.speed_info_pixmap.y[0]);
	speed_info_bar_event_box = gtk_event_box_new ();
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_info_bar_event_box, Skin->typing.speed_info_bar.x[0],
		       Skin->typing.speed_info_bar.y[0]);
	speed_info_bar =	gtk_image_new_from_pixmap (Skin->typing.speed_info_bar.p[0], NULL);
	gtk_container_add (GTK_CONTAINER (speed_info_bar_event_box), speed_info_bar);
	gtk_widget_show (speed_info_bar);
	gtk_widget_show (speed_info_bar_event_box);
	g_signal_connect (G_OBJECT (speed_info_bar_event_box), "button_press_event",
			  G_CALLBACK (on_typing_speedinfobar_button_press), this);
			   
	key_pixmap_a = gtk_image_new_from_pixmap (Skin->typing.key_pixmap[54].p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), key_pixmap_a,
		       Skin->typing.key_pixmap[54].x[0],
		       Skin->typing.key_pixmap[54].y[0]);
	key_pixmap_b = gtk_image_new_from_pixmap (Skin->typing.key_pixmap[41].p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), key_pixmap_b,
		       Skin->typing.key_pixmap[41].x[0],
		       Skin->typing.key_pixmap[41].y[0]);

	speed_ld_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_ld_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_ld_pixmap,
		       Skin->typing.speed_ld_pixmap.x[0],
		       Skin->typing.speed_ld_pixmap.y[0]);
	speed_lm_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_lm_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_lm_pixmap,
		       Skin->typing.speed_lm_pixmap.x[0],
		       Skin->typing.speed_lm_pixmap.y[0]);
	speed_lu_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_lu_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_lu_pixmap,
		       Skin->typing.speed_lu_pixmap.x[0],
		       Skin->typing.speed_lu_pixmap.y[0]);
	speed_ru_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_ru_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_ru_pixmap,
		       Skin->typing.speed_ru_pixmap.x[0],
		       Skin->typing.speed_ru_pixmap.y[0]);
	speed_rm_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_rm_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_rm_pixmap,
		       Skin->typing.speed_rm_pixmap.x[0],
		       Skin->typing.speed_rm_pixmap.y[0]);
	speed_rd_pixmap =
		gtk_image_new_from_pixmap (Skin->typing.speed_rd_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_rd_pixmap,
		       Skin->typing.speed_rd_pixmap.x[0],
		       Skin->typing.speed_rd_pixmap.y[0]);

	speed_number_a =
		gtk_image_new_from_pixmap (Skin->typing.number_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_number_a,
		       Skin->typing.number_pixmap.x[0],
		       Skin->typing.number_pixmap.y[0]);
	speed_number_b =
		gtk_image_new_from_pixmap (Skin->typing.number_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_number_b,
		       Skin->typing.number_pixmap.x[0] -
		       Skin->typing.number_pixmap.w,
		       Skin->typing.number_pixmap.y[0]);
	speed_number_c =
		gtk_image_new_from_pixmap (Skin->typing.number_pixmap.p[0], NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), speed_number_c,
		       Skin->typing.number_pixmap.x[0] -
		       (2 * Skin->typing.number_pixmap.w),
		       Skin->typing.number_pixmap.y[0]);

	on_typing_start_clicked ();
}

void
CTyping::set_key_pixmap (gchar ch)
{
	gint a = -1, b = -1;
	switch (ch)
	{
	case '\0':
		a = 54;
		break;
	case '\b':
		a = 14;
		break;
	case '\t':
		a = 15;
		break;
	case '\r':
		a = 40;
		break;
	case ' ':
		a = 53;
		break;

	case '`':
		a = 0;
		break;
	case '1':
		a = 1;
		break;
	case '2':
		a = 2;
		break;
	case '3':
		a = 3;
		break;
	case '4':
		a = 4;
		break;
	case '~':
		a = 0;
		b = 2;
		break;
	case '!':
		a = 1;
		b = 2;
		break;
	case '@':
		a = 2;
		b = 2;
		break;
	case '#':
		a = 3;
		b = 2;
		break;
	case '$':
		a = 4;
		b = 2;
		break;
	case '5':
		a = 5;
		break;
	case '6':
		a = 6;
		break;
	case '7':
		a = 7;
		break;
	case '8':
		a = 8;
		break;
	case '9':
		a = 9;
		break;
	case '%':
		a = 5;
		b = 2;
		break;
	case '^':
		a = 6;
		b = 1;
		break;
	case '&':
		a = 7;
		b = 1;
		break;
	case '*':
		a = 8;
		b = 1;
		break;
	case '(':
		a = 9;
		b = 1;
		break;
	case '0':
		a = 10;
		break;
	case '-':
		a = 11;
		break;
	case '=':
		a = 12;
		break;
	case '\\':
		a = 13;
		break;
	case 'q':
		a = 16;
		break;
	case ')':
		a = 10;
		b = 1;
		break;
	case '_':
		a = 11;
		b = 1;
		break;
	case '+':
		a = 12;
		b = 1;
		break;
	case '|':
		a = 13;
		b = 1;
		break;
	case 'Q':
		a = 16;
		b = 2;
		break;
	case 'w':
		a = 17;
		break;
	case 'e':
		a = 18;
		break;
	case 'r':
		a = 19;
		break;
	case 't':
		a = 20;
		break;
	case 'y':
		a = 21;
		break;
	case 'W':
		a = 17;
		b = 2;
		break;
	case 'E':
		a = 18;
		b = 2;
		break;
	case 'R':
		a = 19;
		b = 2;
		break;
	case 'T':
		a = 20;
		b = 2;
		break;
	case 'Y':
		a = 21;
		b = 1;
		break;
	case 'u':
		a = 22;
		break;
	case 'i':
		a = 23;
		break;
	case 'o':
		a = 24;
		break;
	case 'p':
		a = 25;
		break;
	case '[':
		a = 26;
		break;
	case 'U':
		a = 22;
		b = 1;
		break;
	case 'I':
		a = 23;
		b = 1;
		break;
	case 'O':
		a = 24;
		b = 1;
		break;
	case 'P':
		a = 25;
		b = 1;
		break;
	case '{':
		a = 26;
		b = 1;
		break;
	case ']':
		a = 27;
		break;
	case 'a':
		a = 29;
		break;
	case 's':
		a = 30;
		break;
	case 'd':
		a = 31;
		break;
	case 'f':
		a = 32;
		break;
	case '}':
		a = 27;
		b = 1;
		break;
	case 'A':
		a = 29;
		b = 2;
		break;
	case 'S':
		a = 30;
		b = 2;
		break;
	case 'D':
		a = 31;
		b = 2;
		break;
	case 'F':
		a = 32;
		b = 2;
		break;
	case 'g':
		a = 33;
		break;
	case 'h':
		a = 34;
		break;
	case 'j':
		a = 35;
		break;
	case 'k':
		a = 36;
		break;
	case 'l':
		a = 37;
		break;
	case 'G':
		a = 33;
		b = 2;
		break;
	case 'H':
		a = 34;
		b = 1;
		break;
	case 'J':
		a = 35;
		b = 1;
		break;
	case 'K':
		a = 36;
		b = 1;
		break;
	case 'L':
		a = 37;
		b = 1;
		break;
	case ';':
		a = 38;
		break;
	case '\'':
		a = 39;
		break;
	case 'z':
		a = 42;
		break;
	case 'x':
		a = 43;
		break;
	case 'c':
		a = 44;
		break;
	case ':':
		a = 38;
		b = 1;
		break;
	case '\"':
		a = 39;
		b = 1;
		break;
	case 'Z':
		a = 42;
		b = 2;
		break;
	case 'X':
		a = 43;
		b = 2;
		break;
	case 'C':
		a = 44;
		b = 2;
		break;
	case 'v':
		a = 45;
		break;
	case 'b':
		a = 46;
		break;
	case 'n':
		a = 47;
		break;
	case 'm':
		a = 48;
		break;
	case ',':
		a = 49;
		break;
	case 'V':
		a = 45;
		b = 2;
		break;
	case 'B':
		a = 46;
		b = 2;
		break;
	case 'N':
		a = 47;
		b = 1;
		break;
	case 'M':
		a = 48;
		b = 1;
		break;
	case '<':
		a = 49;
		b = 1;
		break;
	case '.':
		a = 50;
		break;
	case '/':
		a = 51;
		break;
	case '>':
		a = 50;
		b = 1;
		break;
	case '?':
		a = 51;
		b = 1;
		break;
	default:
		break;
	}
	if (a == -1)
	{
		if (key_pixmap_s_a)
		{
			gtk_widget_hide (key_pixmap_a);
			key_pixmap_s_a = FALSE;
		}
	}
	else
	{
		if (a != key_pixmap_o_a)
		{
			gtk_widget_hide (key_pixmap_a);
			gtk_fixed_move (GTK_FIXED (g_pReciteWord->fixed),
					key_pixmap_a,
					Skin->typing.key_pixmap[a].x[0],
					Skin->typing.key_pixmap[a].y[0]);
			gtk_image_set_from_pixmap (GTK_IMAGE (key_pixmap_a),
					Skin->typing.key_pixmap[a].p[0],
					(GdkBitmap *) NULL);
			gtk_widget_show (key_pixmap_a);
			key_pixmap_s_a = TRUE;
			key_pixmap_o_a = a;
		}
		else if (!key_pixmap_s_a)
		{
			gtk_widget_show (key_pixmap_a);
			key_pixmap_s_a = TRUE;
		}
	}
	if (b == -1)
	{
		if (key_pixmap_s_b)
		{
			gtk_widget_hide (key_pixmap_b);
			key_pixmap_s_b = FALSE;
		}
	}
	else
	{
		if (b != key_pixmap_o_b)
		{
			gtk_widget_hide (key_pixmap_b);
			if (b == 1)
			{
				gtk_fixed_move (GTK_FIXED
						(g_pReciteWord->fixed),
						key_pixmap_b,
						Skin->typing.key_pixmap[41].x[0],
						Skin->typing.key_pixmap[41].y[0]);
				gtk_image_set_from_pixmap (GTK_IMAGE (key_pixmap_b),
						Skin->typing.key_pixmap[41].p[0],
						(GdkBitmap *) NULL);
			}
			else if (b == 2)
			{
				gtk_fixed_move (GTK_FIXED
						(g_pReciteWord->fixed),
						key_pixmap_b,
						Skin->typing.key_pixmap[52].x[0],
						Skin->typing.key_pixmap[52].y[0]);
				gtk_image_set_from_pixmap (GTK_IMAGE (key_pixmap_b),
						Skin->typing.key_pixmap[52].p[0],
						(GdkBitmap *) NULL);
			}
			gtk_widget_show (key_pixmap_b);
			key_pixmap_s_b = TRUE;
			key_pixmap_o_b = b;
		}
		else if (!key_pixmap_s_b)
		{
			gtk_widget_show (key_pixmap_b);
			key_pixmap_s_b = TRUE;
		}
	}
}

void
CTyping::set_speed_pixmap (gint speed)
{
	if ((speed < 0) || (speed > 36))
		return;
	gint a;
	a = (speed - 1) % 6;
	if (speed == 0)
	{
		gtk_widget_hide (speed_ld_pixmap);
		gtk_widget_hide (speed_lm_pixmap);
		gtk_widget_hide (speed_lu_pixmap);
		gtk_widget_hide (speed_ru_pixmap);
		gtk_widget_hide (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else if (speed <= 6)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[a], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_widget_hide (speed_lm_pixmap);
		gtk_widget_hide (speed_lu_pixmap);
		gtk_widget_hide (speed_ru_pixmap);
		gtk_widget_hide (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else if (speed <= 12)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[5], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lm_pixmap),
				Skin->typing.speed_lm_pixmap.p[a], NULL);
		gtk_widget_show (speed_lm_pixmap);
		gtk_widget_hide (speed_lu_pixmap);
		gtk_widget_hide (speed_ru_pixmap);
		gtk_widget_hide (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else if (speed <= 18)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[5], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lm_pixmap),
				Skin->typing.speed_lm_pixmap.p[5], NULL);
		gtk_widget_show (speed_lm_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lu_pixmap),
				Skin->typing.speed_lu_pixmap.p[a], NULL);
		gtk_widget_show (speed_lu_pixmap);
		gtk_widget_hide (speed_ru_pixmap);
		gtk_widget_hide (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else if (speed <= 24)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[5], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lm_pixmap),
				Skin->typing.speed_lm_pixmap.p[5], NULL);
		gtk_widget_show (speed_lm_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lu_pixmap),
				Skin->typing.speed_lu_pixmap.p[5], NULL);
		gtk_widget_show (speed_lu_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ru_pixmap),
				Skin->typing.speed_ru_pixmap.p[a], NULL);
		gtk_widget_show (speed_ru_pixmap);
		gtk_widget_hide (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else if (speed <= 30)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[5], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lm_pixmap),
				Skin->typing.speed_lm_pixmap.p[5], NULL);
		gtk_widget_show (speed_lm_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lu_pixmap),
				Skin->typing.speed_lu_pixmap.p[5], NULL);
		gtk_widget_show (speed_lu_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ru_pixmap),
				Skin->typing.speed_ru_pixmap.p[5], NULL);
		gtk_widget_show (speed_ru_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_rm_pixmap),
				Skin->typing.speed_rm_pixmap.p[a], NULL);
		gtk_widget_show (speed_rm_pixmap);
		gtk_widget_hide (speed_rd_pixmap);
	}
	else
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ld_pixmap),
				Skin->typing.speed_ld_pixmap.p[5], NULL);
		gtk_widget_show (speed_ld_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lm_pixmap),
				Skin->typing.speed_lm_pixmap.p[5], NULL);
		gtk_widget_show (speed_lm_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_lu_pixmap),
				Skin->typing.speed_lu_pixmap.p[5], NULL);
		gtk_widget_show (speed_lu_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_ru_pixmap),
				Skin->typing.speed_ru_pixmap.p[5], NULL);
		gtk_widget_show (speed_ru_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_rm_pixmap),
				Skin->typing.speed_rm_pixmap.p[5], NULL);
		gtk_widget_show (speed_rm_pixmap);
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_rd_pixmap),
				Skin->typing.speed_rd_pixmap.p[a], NULL);
		gtk_widget_show (speed_rd_pixmap);
	}
}

void
CTyping::set_speed_number (gint speed)
{
	if ((speed < 0) || (speed > 999))
		return;
	gint a, b, c;
	a = speed % 10;
	b = (speed % 100) / 10;
	c = speed / 100;
	if (a)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_number_a),
				Skin->typing.number_pixmap.p[a], NULL);
		gtk_widget_show (speed_number_a);
	}
	else
	{
		gtk_widget_hide (speed_number_a);
	}
	if (b)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_number_b),
				Skin->typing.number_pixmap.p[b], NULL);
		gtk_widget_show (speed_number_b);
	}
	else
	{
		if (c)
		{
			gtk_image_set_from_pixmap (GTK_IMAGE (speed_number_b),
					Skin->typing.number_pixmap.p[0], NULL);
			gtk_widget_show (speed_number_b);
		}
		else
			gtk_widget_hide (speed_number_b);
	}
	if (c)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (speed_number_c),
				Skin->typing.number_pixmap.p[c], NULL);
		gtk_widget_show (speed_number_c);
	}
	else
	{
		gtk_widget_hide (speed_number_c);
	}
}

void
CTyping::set_wordtest_label (gint index)
{
	gtk_label_set_text (GTK_LABEL (wordtest_label_1),
			    g_pReciteWord->now_zu_words.w[index]);
	gchar aa[5];
	sprintf(aa,"%2d",index);
	gtk_label_set_text (GTK_LABEL (recited_word_count_label),
			    aa);
}

void
CTyping::set_wordlist_item (gint index)
{
	wordlist_item[0].set_label (g_pReciteWord->now_zu_words.w[0 + index],
				    g_pReciteWord->now_zu_words.m[0 + index]);
	wordlist_item[1].set_label (g_pReciteWord->now_zu_words.w[1 + index],
				    g_pReciteWord->now_zu_words.m[1 + index]);
	wordlist_item[2].set_label (g_pReciteWord->now_zu_words.w[2 + index],
				    g_pReciteWord->now_zu_words.m[2 + index]);
	wordlist_item[3].set_label (g_pReciteWord->now_zu_words.w[3 + index],
				    g_pReciteWord->now_zu_words.m[3 + index]);
	wordlist_item[4].set_label (g_pReciteWord->now_zu_words.w[4 + index],
				    g_pReciteWord->now_zu_words.m[4 + index]);
	wordlist_item[5].set_label (g_pReciteWord->now_zu_words.w[5 + index],
				    g_pReciteWord->now_zu_words.m[5 + index]);

}

void
CTyping::close ()
{
	return_button.destroy ();

	if (status == FALSE)
	{
		g_pReciteWord->chooseword->close ();
		return;
	}

	if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		g_pReciteWord->chooseword->close ();
	if (g_pReciteWord->mark && g_pReciteWord->mark->fixed)
		g_pReciteWord->mark->close ();

	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "readword",
			     &tmp_b);
	if (tmp_b!=readword_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "typing", "readword",
			     readword_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showphonetic",
			     &tmp_b);
	if (tmp_b!=phonetic_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "typing", "showphonetic",
			     phonetic_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showmean",
			     &tmp_b);
	if (tmp_b!=meaning_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "typing", "showmean",
			     meaning_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "sndeffect",
			     &tmp_b);
	if (tmp_b!=sndeffect_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "typing", "sndeffect",
			     sndeffect_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "typing", "showfingering",
			     &tmp_b);
	if (tmp_b!=fingering_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "typing", "showfingering",
			     fingering_ckbutton.status);
	}
	
	if (g_pReciteWord->typing->speed_timer)
	{
		g_source_remove (g_pReciteWord->typing->speed_timer);
		g_pReciteWord->typing->speed_timer = 0;
	}
	wordlist_item[0].destroy ();
	wordlist_item[1].destroy ();
	wordlist_item[2].destroy ();
	wordlist_item[3].destroy ();
	wordlist_item[4].destroy ();
	wordlist_item[5].destroy ();
	gtk_widget_destroy (wordlist_vscroll);

	gtk_widget_destroy (pressanykey_tip);
	gtk_widget_destroy (speed_info_pixmap);
	gtk_widget_destroy (speed_info_bar_event_box);
	gtk_widget_destroy (key_pixmap_a);
	gtk_widget_destroy (key_pixmap_b);
	gtk_widget_destroy (speed_ld_pixmap);
	gtk_widget_destroy (speed_lm_pixmap);
	gtk_widget_destroy (speed_lu_pixmap);
	gtk_widget_destroy (speed_ru_pixmap);
	gtk_widget_destroy (speed_rm_pixmap);
	gtk_widget_destroy (speed_rd_pixmap);
	gtk_widget_destroy (speed_number_a);
	gtk_widget_destroy (speed_number_b);
	gtk_widget_destroy (speed_number_c);

	gtk_widget_destroy (wordtest_label_1);
	gtk_widget_destroy (recited_word_count_label);
	gtk_widget_destroy (word_count_label);
	wordtest_entry.destroy ();

	readword_ckbutton.destroy ();
	phonetic_ckbutton.destroy ();
	meaning_ckbutton.destroy ();
	sndeffect_ckbutton.destroy ();
	fingering_ckbutton.destroy ();
	readword_button.destroy ();
	chooseword_button.destroy ();

	start_button.destroy ();
	pause_button.destroy ();
}
