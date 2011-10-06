#include "shooting.h"
#include "reciteword.h"
#include "skin.h"
#include "sound.h"
#include "readword.h"
#include "rwconfig.h"
#include <cstring>
#include <cstdlib>

extern CReciteWord *g_pReciteWord;
extern ConfigFile *usercfgfile;
extern struct _Skin *Skin;


static gint
on_labelitem_expose (GtkWidget * widget, GdkEventExpose * event,
		     tLabelItem * parent)
{
	gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 parent->draw_pixmap,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	return FALSE;
}

static gboolean
on_labelitem_buttonpress (GtkWidget * widget, GdkEventButton * event,
			  tLabelItem * parent)
{
	return TRUE;
}

static gint
on_labelitem_buttonrelease (GtkWidget * widget, GdkEventButton * event,
			    tLabelItem * parent)
{
	if (parent->mousein)
	{
		if (parent->enable)
		{
			(*(parent->runfunc1)) (parent->funcdata);
		}
	}
	return TRUE;
}

static gboolean
on_labelitem_enter_notify (GtkWidget * widget, GdkEventCrossing * event,
			   tLabelItem * parent)
{
//	if (event->mode!=GDK_CROSSING_NORMAL) //event happen not because mouse move.
//		return true;
	parent->mousein = TRUE;
	
	if (!parent->enable)
		return true;
	(*(parent->runfunc2)) (parent->funcdata);
	parent->active = TRUE;
	parent->draw ();
	return true;
}

static gboolean
on_labelitem_leave_notify (GtkWidget * widget, GdkEventCrossing * event,
			   tLabelItem * parent)
{
//	if (event->mode!=GDK_CROSSING_NORMAL) //event happen not because mouse move.
//		return true;
	
	parent->mousein = FALSE;
	if (!parent->enable)
		return true;
	parent->active = FALSE;
	parent->draw ();
	return true;
}

tLabelItem::tLabelItem ()
{
}

void
tLabelItem::init ()
{
	layout= NULL;
	mousein = FALSE;
	active = FALSE;
	enable = TRUE;
}

void
tLabelItem::create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		    GdkPixmap * pix_1, GdkPixmap * pix_2, gchar * str,
		    void (*func1) (glong), void (*func2) (glong), gint data)
{
	init ();

	pix_normal = pix_1;
	pix_active = pix_2;
	width = w;
	height = h;
	runfunc1 = func1;
	runfunc2 = func2;
	funcdata = data;

	draw_area = gtk_drawing_area_new ();
	gtk_widget_set_size_request (draw_area, width, height);

	gtk_widget_set_events (draw_area,
			       GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
			       | GDK_ENTER_NOTIFY_MASK |
			       GDK_LEAVE_NOTIFY_MASK);
	g_signal_connect (G_OBJECT (draw_area), "expose_event",
			  G_CALLBACK (on_labelitem_expose), this);
	g_signal_connect (G_OBJECT (draw_area), "button_press_event",
			  G_CALLBACK (on_labelitem_buttonpress), this);
	g_signal_connect (G_OBJECT (draw_area), "button_release_event",
			  G_CALLBACK (on_labelitem_buttonrelease), this);
	g_signal_connect (G_OBJECT (draw_area), "enter_notify_event",
			  G_CALLBACK (on_labelitem_enter_notify), this);
	g_signal_connect (G_OBJECT (draw_area), "leave_notify_event",
			  G_CALLBACK (on_labelitem_leave_notify), this);

	gtk_fixed_put (GTK_FIXED (pfixed), draw_area, x, y);
	gtk_widget_show (draw_area);
	gdk_window_set_cursor (draw_area->window, Skin->reciteword.gun_cursor.cursor);
	draw_pixmap =
		gdk_pixmap_new (draw_area->window, width, height,
				gdk_rgb_get_visual ()->depth);
	set_label(str);
}

void
tLabelItem::draw ()
{
	if (active)
		gdk_draw_drawable (draw_pixmap, draw_area->style->white_gc,
				 pix_active, 0, 0, 0, 0, width, height);
	else
		gdk_draw_drawable (draw_pixmap, draw_area->style->white_gc,
				 pix_normal, 0, 0, 0, 0, width, height);
	if (layout)
	{
		gint h,y;
		pango_layout_get_pixel_size(layout,NULL,&h);
		y = (height - h) / 2;
		gdk_draw_layout(draw_pixmap,draw_area->style->white_gc,5,y,layout);
	}
/*	gdk_draw_string (draw_pixmap, gtk_style_get_font (draw_area->style),
			 draw_area->style->white_gc, 5, y, text);
*/
	gtk_widget_queue_draw (draw_area);
}

void
tLabelItem::set_label (gchar * str)
{
	if (str)
		std::strcpy (text, str);
	else
		text[0]='\0';
	if (layout)
		pango_layout_set_text(layout,text,-1);
	else
		layout=gtk_widget_create_pango_layout (draw_area, text);
	draw ();
}

void
tLabelItem::set_enable (gint a)
{
	if (enable != a)
	{
		if (a)
		{
			gdk_window_set_cursor (draw_area->window, Skin->reciteword.gun_cursor.cursor);
		}
		else
		{
			gdk_window_set_cursor (draw_area->window, Skin->reciteword.disable_cursor.cursor);
			if (active)
			{
				active = FALSE;
				draw ();
			}
		}
		enable = a;
	}
}

void
tLabelItem::destroy ()
{
	if (layout)
		g_object_unref (G_OBJECT (layout));
	gtk_widget_destroy (draw_area);
}

tLabelItem::~tLabelItem ()
{
}

static void do_shooting_pause ()
{
	if (g_pReciteWord->shooting->shoot_pixmap_timer)
	{
		g_source_remove (g_pReciteWord->shooting->shoot_pixmap_timer);
		g_pReciteWord->shooting->shoot_pixmap_timer = 0;
		for (int i=0;i<4;i++)
		{
			gtk_widget_hide (g_pReciteWord->shooting->shoot_pixmap[i]);
			gtk_widget_hide (g_pReciteWord->shooting->right_pixmap[i]);
			gtk_widget_show (g_pReciteWord->shooting->wordlist_head[i].event_box);
		}
	}
	g_pReciteWord->shooting->wordlist_head[0].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_head[1].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_head[2].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_head[3].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_item[0].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_item[1].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_item[2].set_enable (FALSE);
	g_pReciteWord->shooting->wordlist_item[3].set_enable (FALSE);
}

static void do_shooting_continue ()
{
	g_pReciteWord->shooting->wordlist_head[0].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_head[1].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_head[2].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_head[3].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_item[0].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_item[1].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_item[2].set_enable (TRUE);
	g_pReciteWord->shooting->wordlist_item[3].set_enable (TRUE);
}

static void
on_shooting_start_clicked ()
{
	if (g_pReciteWord->shooting->started)
	{
		g_pReciteWord->shooting->started = FALSE;
		do_shooting_pause();
		g_pReciteWord->shooting->pause_button.set_enable (FALSE);
		g_pReciteWord->shooting->start_button.reset (Skin->
							     shooting.
							     start_button.
							     p[0],
							     Skin->
							     shooting.
							     start_button.
							     p[1],
							     Skin->
							     shooting.
							     start_button.
							     p[2],
							     (GdkPixmap *)
							     NULL);
	}
	else
	{
		gint i;
		for (i = 0; i < (g_pReciteWord->now_zu_wordcount); i++)
			g_pReciteWord->shooting->tested_wordlist_item[i] = 0;	// != -1 means didn't tested
		g_pReciteWord->shooting->recited_word_count = 0;
		gtk_label_set_text(GTK_LABEL(g_pReciteWord->shooting->recited_word_count_label)," 0");

		g_pReciteWord->shooting->testing_word_index = 0;

		do_shooting_continue();

		g_pReciteWord->shooting->set_wordlist_label (0);
		g_pReciteWord->shooting->set_wordview_label (0);

		g_pReciteWord->shooting->pause_button.set_enable (TRUE);
		g_pReciteWord->shooting->start_button.reset (Skin->
							     shooting.
							     stop_button.
							     p[0],
							     Skin->
							     shooting.
							     stop_button.
							     p[1],
							     Skin->
							     shooting.
							     stop_button.
							     p[2],
							     Skin->
							     shooting.
							     stop_button.p[3]);
		g_pReciteWord->shooting->started = TRUE;
	}
}

static void
on_shooting_pause_clicked ()
{
	if (g_pReciteWord->shooting->started)
	{
		do_shooting_pause ();
		g_pReciteWord->shooting->start_button.
			set_enable (FALSE);
		g_pReciteWord->shooting->pause_button.reset (Skin->
								     shooting.
								     continue_button.
								     p[0],
								     Skin->
								     shooting.
								     continue_button.
								     p[1],
								     Skin->
								     shooting.
								     continue_button.
								     p[2],
								     (GdkPixmap
								      *)
								     NULL);
		g_pReciteWord->shooting->started = FALSE;
	}
	else
	{
		do_shooting_continue();
		g_pReciteWord->shooting->start_button.
			set_enable (TRUE);
		g_pReciteWord->shooting->pause_button.reset (Skin->
								     shooting.
								     pause_button.
								     p[0],
								     Skin->
								     shooting.
								     pause_button.
								     p[1],
								     Skin->
								     shooting.
								     pause_button.
								     p[2],
								     Skin->
								     shooting.
								     pause_button.
								     p[3]);
		g_pReciteWord->shooting->started = TRUE;
	}
}

static void
on_shooting_return_clicked ()
{
	g_pReciteWord->shooting->close ();
	g_pReciteWord->show ();
}

static void
on_shooting_readword_button_clicked (gpointer data)
{
	CShooting *parent = (CShooting *)data;
	if (parent->testing_word_index>=0 && parent->testing_word_index<g_pReciteWord->now_zu_wordcount)
		readword(g_pReciteWord->now_zu_words.w[parent->testing_word_index]);
}

static void 
on_shooting_chooseword_ok_clicked ()
{
	if (g_pReciteWord->shooting->start_pause_button_status==0)
	{
		g_pReciteWord->shooting->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->shooting->start_pause_button_status==1)
	{
		g_pReciteWord->shooting->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->shooting->start_pause_button_status==2)
	{
		g_pReciteWord->shooting->start_button.set_enable(true);
	}
	if (!g_pReciteWord->shooting->start_button.enable)
		on_shooting_pause_clicked();  //continue
	if (g_pReciteWord->shooting->started)
		on_shooting_start_clicked();  //stop

	g_free(g_pReciteWord->shooting->tested_wordlist_item);
	g_pReciteWord->shooting->tested_wordlist_item =
		(int *) g_malloc (g_pReciteWord->now_zu_wordcount *
				  (sizeof (int)));
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	gtk_label_set_text(GTK_LABEL(g_pReciteWord->shooting->word_count_label),aa);	
	on_shooting_start_clicked();  //start
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->shooting->chooseword_button.set_enable(true);
}

static void 
on_shooting_chooseword_cancel_clicked ()
{
	g_pReciteWord->chooseword->close ();
	if (g_pReciteWord->shooting->start_pause_button_status==0)
	{
		g_pReciteWord->shooting->pause_button.set_enable(true);
		on_shooting_pause_clicked();  //continue
	}
	else if (g_pReciteWord->shooting->start_pause_button_status==1)
	{
		g_pReciteWord->shooting->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->shooting->start_pause_button_status==2)
	{
		g_pReciteWord->shooting->start_button.set_enable(true);
	}
	g_pReciteWord->shooting->chooseword_button.set_enable(true);
}

static void
on_shooting_chooseword_button_clicked (gpointer data)
{	
	if (!g_pReciteWord->shooting->start_button.enable)
	{
		g_pReciteWord->shooting->start_pause_button_status=1;
		g_pReciteWord->shooting->pause_button.set_enable(false);
	}
	else if (!g_pReciteWord->shooting->pause_button.enable)
	{
		g_pReciteWord->shooting->start_pause_button_status=2;
		g_pReciteWord->shooting->start_button.set_enable(false);
	}
	else
	{
		g_pReciteWord->shooting->start_pause_button_status=0;
		on_shooting_pause_clicked();  //pause
		g_pReciteWord->shooting->pause_button.set_enable(false);
	}
	
	g_pReciteWord->PopupChooseword (on_shooting_chooseword_ok_clicked,
					on_shooting_chooseword_cancel_clicked, &(g_pReciteWord->shooting_chooseword_info));
	g_pReciteWord->shooting->chooseword_button.set_enable(false);
}

static void
on_shooting_meaning_ckbutton_clicked ()
{
	if (g_pReciteWord->shooting->meaning_ckbutton.status)
		gtk_widget_show(g_pReciteWord->shooting->meaning_label);
	else
		gtk_widget_hide(g_pReciteWord->shooting->meaning_label);
}

static void
on_shooting_word_ckbutton_clicked ()
{
	if (g_pReciteWord->shooting->word_ckbutton.status)
		gtk_widget_show(g_pReciteWord->shooting->word_label);
	else
		gtk_widget_hide(g_pReciteWord->shooting->word_label);
}

static void
shooting_test_next_word ()
{
	do
	{
		g_pReciteWord->shooting->testing_word_index++;
		if (g_pReciteWord->shooting->testing_word_index >=
		    g_pReciteWord->now_zu_wordcount)
		{
			int i, b;
			b = 1;
			for (i = 0; i < g_pReciteWord->now_zu_wordcount; i++)
			{
				if (g_pReciteWord->shooting->
				    tested_wordlist_item[i] != -1)
				{
					b = 0;
					break;
				}
			}
			if (b)
			{
				on_shooting_start_clicked ();	//stop
				if (g_pReciteWord->now_zu_index >= 0) // come in shooting by chooseword's by_group
				{
					g_pReciteWord->shooting_chooseword_info.by = 0; // by group next time.
					g_pReciteWord->shooting_chooseword_info.zu_index = g_pReciteWord->now_zu_index+1;
				}
				else // now_zu_index == -1,it must be set by chooseword dialog's by_scope.
				{
					g_pReciteWord->shooting_chooseword_info.by = 1; // by scope
					g_pReciteWord->shooting_chooseword_info.scope_start = g_pReciteWord->chooseword->last_scope_start + g_pReciteWord->chooseword->last_scope_count;
					g_pReciteWord->shooting_chooseword_info.scope_count = g_pReciteWord->chooseword->last_scope_count;
				}
				return;
			}
			else
			{
				g_pReciteWord->shooting->testing_word_index =
					0;
			}
		}
	}
	while (g_pReciteWord->shooting->
	       tested_wordlist_item[g_pReciteWord->shooting->
				    testing_word_index] == -1);

	g_pReciteWord->shooting->set_wordlist_label (g_pReciteWord->shooting->
						     testing_word_index);
	g_pReciteWord->shooting->set_wordview_label (g_pReciteWord->shooting->
						     testing_word_index);
}

static gint
draw_right_pixmap (gpointer data)
{
	CShooting *parent = g_pReciteWord->shooting;
	glong index = (glong) data;
	GtkWidget *right_pixmap =
		parent->right_pixmap[parent->
				     tested_wordlist_item[parent->
							  testing_word_index]];
	GtkWidget *wordlist_head =
		parent->wordlist_head[parent->
				      tested_wordlist_item[parent->
							   testing_word_index]].
		event_box;

	if (parent->timer_count == 8)
	{
		gtk_widget_hide (right_pixmap);
		shooting_test_next_word ();
		gtk_widget_show (wordlist_head);
		gtk_widget_show (parent->wordlist_head[index].event_box);
		parent->shoot_pixmap_timer = 0;
		return FALSE;
	}
	if (parent->timer_count == -1)
	{
		gtk_widget_hide (wordlist_head);
		parent->timer_count = 0;
	}
	else
		parent->timer_count++;

	if (parent->timer_count % 2)
		gtk_image_set_from_pixmap (GTK_IMAGE (right_pixmap),
				Skin->shooting.right_pixmap[parent->
							   tested_wordlist_item
							   [parent->
							    testing_word_index]].
				p[0], (GdkBitmap *) NULL);
	else
		gtk_image_set_from_pixmap (GTK_IMAGE (right_pixmap),
				Skin->shooting.right_pixmap[parent->
							   tested_wordlist_item
							   [parent->
							    testing_word_index]].
				p[1], (GdkBitmap *) NULL);
	if (parent->timer_count == 0)
		gtk_widget_show (right_pixmap);
	return TRUE;
}

static gint
draw_shooting (gpointer data)
{
	CShooting *parent = g_pReciteWord->shooting;
	glong index = (glong) data;
	GtkWidget *shoot_pixmap = parent->shoot_pixmap[index];
	GtkWidget *wordlist_head = parent->wordlist_head[index].event_box;

	if (parent->timer_type)
	{
		if (parent->timer_count == 8)
		{
			parent->timer_count++;
			gtk_widget_hide (shoot_pixmap);
			return TRUE;
		}
		if (parent->timer_count == 9)
		{
			shooting_test_next_word ();
			gtk_widget_show (wordlist_head);
			parent->shoot_pixmap_timer = 0;
			return FALSE;
		}
		if (parent->timer_count == -1)
		{
			gtk_widget_hide (wordlist_head);
			parent->timer_count = 0;
		}
		else
			parent->timer_count++;
	}
	else
	{
		if (parent->timer_count == 13)
		{
			gtk_widget_hide (shoot_pixmap);
			parent->timer_count = -1;
			parent->shoot_pixmap_timer =
				g_timeout_add (200, draw_right_pixmap,
						 (void *) index);
			return FALSE;
		}
		if (parent->timer_count == -1)
		{
			gtk_widget_hide (wordlist_head);
			parent->timer_count = 9;
		}
		else
			parent->timer_count++;
	}

	gtk_image_set_from_pixmap (GTK_IMAGE (shoot_pixmap),
			Skin->shooting.shoot_pixmap[index].p[parent->
							    timer_count],
			(GdkBitmap *) NULL);
	if ((parent->timer_count == 0) || (parent->timer_count == 9))
		gtk_widget_show (shoot_pixmap);
	return TRUE;
}

static void
draw_shoot_pixmap (gint type, glong index)
{
	CShooting *parent = g_pReciteWord->shooting;
	parent->timer_type = type;
	parent->timer_count = -1;
	parent->shoot_pixmap_timer =
		g_timeout_add (40, draw_shooting, (void *) index);
}

static void
on_shooting_entocn_robutton_clicked (gpointer data)
{
	CShooting *parent = g_pReciteWord->shooting;
	if (!parent->start_button.enable)
		on_shooting_pause_clicked();  //continue
	if (parent->started)
		on_shooting_start_clicked();  //stop
	parent->method = 0;
	gtk_widget_hide(parent->meaning_label);
	if (parent->word_ckbutton.status)
		gtk_widget_show(parent->word_label);
	parent->meaning_ckbutton.set_enable(false);
	parent->word_ckbutton.set_enable(true);
	on_shooting_start_clicked();  //start
}

static void
on_shooting_cntoen_robutton_clicked (gpointer data)
{
	CShooting *parent = g_pReciteWord->shooting;
	if (!parent->start_button.enable)
		on_shooting_pause_clicked();  //continue
	if (parent->started)
		on_shooting_start_clicked();  //stop
	parent->method = 1;
	gtk_widget_hide(parent->word_label);
	if (parent->meaning_ckbutton.status)
		gtk_widget_show(parent->meaning_label);
	parent->meaning_ckbutton.set_enable(true);
	parent->word_ckbutton.set_enable(false);
	on_shooting_start_clicked();  //start
}

static void
on_shooting_wordlistitem_clicked (glong index)
{
	CShooting *parent = g_pReciteWord->shooting;

	if (parent->shoot_pixmap_timer)
		return;

	if (index ==
	    g_pReciteWord->shooting->tested_wordlist_item[g_pReciteWord->
							  shooting->
							  testing_word_index])
	{
		g_pReciteWord->shooting->tested_wordlist_item[g_pReciteWord->
							      shooting->
							      testing_word_index]
			= -1;
		g_pReciteWord->shooting->recited_word_count ++;
		gchar aa[5];
		sprintf(aa,"%2d",g_pReciteWord->shooting->recited_word_count);
		gtk_label_set_text(GTK_LABEL(g_pReciteWord->shooting->recited_word_count_label),aa);
		if (g_pReciteWord->shooting->shotsnd_ckbutton.status)
			playsound(SND_GUNOK);
		draw_shoot_pixmap (TRUE, index);
	}
	else
	{
		if (g_pReciteWord->shooting->shotsnd_ckbutton.status)
			playsound(SND_GUNNO);
		draw_shoot_pixmap (FALSE, index);
	}
}

static void
on_shooting_wordlistitem_active (glong index)
{
	if (g_pReciteWord->shooting->wordlist_item[g_pReciteWord->shooting->active_wordlist_item].active)
	{
		g_pReciteWord->shooting->wordlist_item[g_pReciteWord->shooting->active_wordlist_item].active = false;
		g_pReciteWord->shooting->wordlist_item[g_pReciteWord->shooting->active_wordlist_item].draw ();
	}
	g_pReciteWord->shooting->active_wordlist_item=index;
	switch (index)
	{
	case 0:
		gtk_image_set_from_pixmap (GTK_IMAGE
				(g_pReciteWord->shooting->tank_pixmap),
				Skin->shooting.tank.p[0], (GdkBitmap *) NULL);
		break;
	case 1:
		gtk_image_set_from_pixmap (GTK_IMAGE
				(g_pReciteWord->shooting->tank_pixmap),
				Skin->shooting.tank.p[1], (GdkBitmap *) NULL);
		break;
	case 2:
		gtk_image_set_from_pixmap (GTK_IMAGE
				(g_pReciteWord->shooting->tank_pixmap),
				Skin->shooting.tank.p[2], (GdkBitmap *) NULL);
		break;
	case 3:
		gtk_image_set_from_pixmap (GTK_IMAGE
				(g_pReciteWord->shooting->tank_pixmap),
				Skin->shooting.tank.p[3], (GdkBitmap *) NULL);
		break;
	}
}

static void
on_shooting_wordlisthead_button_press (gpointer data)
{
	on_shooting_wordlistitem_clicked ((glong) data);
}


static gboolean
on_shooting_wordlisthead_enter_notify (GtkWidget * widget, GdkEvent * event,
				       gpointer func_data)
{
	if (g_pReciteWord->shooting->shoot_pixmap_timer)	//when mouse button press and release,the enter_notify and leave_notift event will happen too.
		return true;
	glong index = (glong) func_data;
	if (!g_pReciteWord->shooting->wordlist_head[index].enable)
		return true;
	on_shooting_wordlistitem_active (index);
	g_pReciteWord->shooting->wordlist_item[index].active = TRUE;
	g_pReciteWord->shooting->wordlist_item[index].draw ();
	return true;
}

static gboolean
on_shooting_wordlisthead_leave_notify (GtkWidget * widget, GdkEvent * event,
				       gpointer func_data)
{
	if (g_pReciteWord->shooting->shoot_pixmap_timer)
		return true;
	glong index = (glong) func_data;
	if (!g_pReciteWord->shooting->wordlist_head[index].enable)
		return true;
	g_pReciteWord->shooting->wordlist_item[index].active = FALSE;
	g_pReciteWord->shooting->wordlist_item[index].draw ();
	return true;
}

CShooting::CShooting ()
{
	skin_load_shooting (g_pReciteWord->window);
}

void
CShooting::init ()
{
	g_pReciteWord->status = STATUS_SHOOTING;
	status = FALSE;
	started = FALSE;
}
void
CShooting::init_2 ()
{
	shoot_pixmap_timer = 0;
	status = TRUE;
	method = 0; // 0 means entocn,1 means cntoen.
	active_wordlist_item = 0;
	tested_wordlist_item =
		(int *) g_malloc (g_pReciteWord->now_zu_wordcount *
				  (sizeof (int)));
}

static void
shooting_show_2 ()
{
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->shooting->show_2 ();
}

void
CShooting::show ()
{
	init ();
	SetBackPixmap (g_pReciteWord->fixed, Skin->shooting.shooting.p[0]);
	return_button.create (g_pReciteWord->fixed,
			      Skin->shooting.return_button.x[0],
			      Skin->shooting.return_button.y[0],
			      Skin->shooting.return_button.p[0],
			      Skin->shooting.return_button.p[1],
			      Skin->shooting.return_button.p[2],
			      (GdkPixmap *) NULL, on_shooting_return_clicked);
	g_pReciteWord->PopupChooseword (shooting_show_2,
					on_shooting_return_clicked,&(g_pReciteWord->shooting_chooseword_info));
}

void
CShooting::show_2 ()
{
	init_2 ();

	entocn_robutton.create (NULL, g_pReciteWord->fixed,
				Skin->shooting.entocn_robutton.x[0],
				Skin->shooting.entocn_robutton.y[0],
				Skin->shooting.entocn_robutton.p[0],
				Skin->shooting.entocn_robutton.p[1],
				Skin->shooting.entocn_robutton.p[2],
				Skin->shooting.entocn_robutton.p[3],
				on_shooting_entocn_robutton_clicked,NULL);
	cntoen_robutton.create (entocn_robutton.group, g_pReciteWord->fixed,
				Skin->shooting.cntoen_robutton.x[0],
				Skin->shooting.cntoen_robutton.y[0],
				Skin->shooting.cntoen_robutton.p[0],
				Skin->shooting.cntoen_robutton.p[1],
				Skin->shooting.cntoen_robutton.p[2],
				Skin->shooting.cntoen_robutton.p[3],
				on_shooting_cntoen_robutton_clicked,NULL);

	word_label = gtk_label_new ("");
	gtk_widget_set_name (word_label, "shooting-word_label");
	SetFont(word_label,true);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_label,
		       Skin->shooting.word_label.x[0],
		       Skin->shooting.word_label.y[0]);
	gtk_widget_show (word_label);
	meaning_label = gtk_label_new ("");
	gtk_widget_set_name (meaning_label, "shooting-meaning_label");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), meaning_label,
		       Skin->shooting.meaning_label.x[0],
		       Skin->shooting.meaning_label.y[0]);
	recited_word_count_label = gtk_label_new ("");
	gtk_widget_set_name (recited_word_count_label, "shooting-wordindex_label");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), recited_word_count_label,
		       Skin->shooting.recited_word_count_label.x[0],
		       Skin->shooting.recited_word_count_label.y[0]);
	gtk_widget_show (recited_word_count_label);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (aa);
	gtk_widget_set_name (word_count_label, "shooting-wordcount_label");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->shooting.word_count_label.x[0],
		       Skin->shooting.word_count_label.y[0]);
	gtk_widget_show (word_count_label);

	readword_ckbutton.create (g_pReciteWord->fixed,
			      Skin->shooting.readword_ckbutton.x[0],
			      Skin->shooting.readword_ckbutton.y[0],
			      Skin->shooting.readword_ckbutton.p[0],
			      Skin->shooting.readword_ckbutton.p[1],
			      Skin->shooting.readword_ckbutton.p[2],
			      Skin->shooting.readword_ckbutton.p[3], NULL);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "readword",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	phonetic_ckbutton.create (g_pReciteWord->fixed,
			      Skin->shooting.phonetic_ckbutton.x[0],
			      Skin->shooting.phonetic_ckbutton.y[0],
			      Skin->shooting.phonetic_ckbutton.p[0],
			      Skin->shooting.phonetic_ckbutton.p[1],
			      Skin->shooting.phonetic_ckbutton.p[2],
			      Skin->shooting.phonetic_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showphonetic",
			     &tmp_b);
	phonetic_ckbutton.set_status(tmp_b);
	meaning_ckbutton.create (g_pReciteWord->fixed,
			      Skin->shooting.meaning_ckbutton.x[0],
			      Skin->shooting.meaning_ckbutton.y[0],
			      Skin->shooting.meaning_ckbutton.p[0],
			      Skin->shooting.meaning_ckbutton.p[1],
			      Skin->shooting.meaning_ckbutton.p[2],
			      Skin->shooting.meaning_ckbutton.p[3],
				  on_shooting_meaning_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showmean",
			     &tmp_b);
	meaning_ckbutton.set_status(tmp_b);
	meaning_ckbutton.set_enable(false);
	word_ckbutton.create (g_pReciteWord->fixed,
			      Skin->shooting.word_ckbutton.x[0],
			      Skin->shooting.word_ckbutton.y[0],
			      Skin->shooting.word_ckbutton.p[0],
			      Skin->shooting.word_ckbutton.p[1],
			      Skin->shooting.word_ckbutton.p[2],
			      Skin->shooting.word_ckbutton.p[3],
				  on_shooting_word_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showword",
			     &tmp_b);
	word_ckbutton.set_status(tmp_b);
	if (!word_ckbutton.status)
		gtk_widget_hide(word_label);
	shotsnd_ckbutton.create (g_pReciteWord->fixed,
			      Skin->shooting.shotsnd_ckbutton.x[0],
			      Skin->shooting.shotsnd_ckbutton.y[0],
			      Skin->shooting.shotsnd_ckbutton.p[0],
			      Skin->shooting.shotsnd_ckbutton.p[1],
			      Skin->shooting.shotsnd_ckbutton.p[2],
			      Skin->shooting.shotsnd_ckbutton.p[3], NULL);
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "shotsnd",
			     &tmp_b);
	shotsnd_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->shooting.readword_button.x[0],
			      Skin->shooting.readword_button.y[0],
			      Skin->shooting.readword_button.p[0],
			      Skin->shooting.readword_button.p[1],
			      Skin->shooting.readword_button.p[2],
			      on_shooting_readword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), readword_button.event_box,"Tab","");
	chooseword_button.create (g_pReciteWord->fixed, Skin->shooting.chooseword_button.x[0],
			      Skin->shooting.chooseword_button.y[0],
			      Skin->shooting.chooseword_button.p[0],
			      Skin->shooting.chooseword_button.p[1],
			      Skin->shooting.chooseword_button.p[2],
			      on_shooting_chooseword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), chooseword_button.event_box,"Ctrl-W","");

	wordlist_item[0].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_item_1.x[0],
				 Skin->shooting.wordlist_item_1.y[0],
				 Skin->shooting.wordlist_item_1.w,
				 Skin->shooting.wordlist_item_1.h,
				 Skin->shooting.wordlist_item_1.p[0],
				 Skin->shooting.wordlist_item_1.p[1], "",
				 on_shooting_wordlistitem_clicked,
				 on_shooting_wordlistitem_active, 0);
	wordlist_item[1].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_item_2.x[0],
				 Skin->shooting.wordlist_item_2.y[0],
				 Skin->shooting.wordlist_item_2.w,
				 Skin->shooting.wordlist_item_2.h,
				 Skin->shooting.wordlist_item_2.p[0],
				 Skin->shooting.wordlist_item_2.p[1], "",
				 on_shooting_wordlistitem_clicked,
				 on_shooting_wordlistitem_active, 1);
	wordlist_item[2].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_item_3.x[0],
				 Skin->shooting.wordlist_item_3.y[0],
				 Skin->shooting.wordlist_item_3.w,
				 Skin->shooting.wordlist_item_3.h,
				 Skin->shooting.wordlist_item_3.p[0],
				 Skin->shooting.wordlist_item_3.p[1], "",
				 on_shooting_wordlistitem_clicked,
				 on_shooting_wordlistitem_active, 2);
	wordlist_item[3].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_item_4.x[0],
				 Skin->shooting.wordlist_item_4.y[0],
				 Skin->shooting.wordlist_item_4.w,
				 Skin->shooting.wordlist_item_4.h,
				 Skin->shooting.wordlist_item_4.p[0],
				 Skin->shooting.wordlist_item_4.p[1], "",
				 on_shooting_wordlistitem_clicked,
				 on_shooting_wordlistitem_active, 3);

	wordlist_head[0].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_head[0].x[0],
				 Skin->shooting.wordlist_head[0].y[0],
				 Skin->shooting.wordlist_head[0].p[0], NULL, NULL,
				 on_shooting_wordlisthead_button_press,
				 GINT_TO_POINTER (0));
	wordlist_head[1].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_head[1].x[0],
				 Skin->shooting.wordlist_head[1].y[0],
				 Skin->shooting.wordlist_head[1].p[0], NULL, NULL,
				 on_shooting_wordlisthead_button_press,
				 GINT_TO_POINTER (1));
	wordlist_head[2].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_head[2].x[0],
				 Skin->shooting.wordlist_head[2].y[0],
				 Skin->shooting.wordlist_head[2].p[0], NULL, NULL,
				 on_shooting_wordlisthead_button_press,
				 GINT_TO_POINTER (2));
	wordlist_head[3].create (g_pReciteWord->fixed,
				 Skin->shooting.wordlist_head[3].x[0],
				 Skin->shooting.wordlist_head[3].y[0],
				 Skin->shooting.wordlist_head[3].p[0], NULL, NULL,
				 on_shooting_wordlisthead_button_press,
				 GINT_TO_POINTER (3));
	glong i;
	for (i = 0; i < 4; i++)
	{
		g_signal_connect (G_OBJECT (wordlist_head[i].event_box),
				  "enter_notify_event",
				  G_CALLBACK
				  (on_shooting_wordlisthead_enter_notify),
				  (void *) i);
		g_signal_connect (G_OBJECT (wordlist_head[i].event_box),
				  "leave_notify_event",
				  G_CALLBACK
				  (on_shooting_wordlisthead_leave_notify),
				  (void *) i);
	}
/*
	glong i;
	GtkWidget *pixmap_widget;
	for (i=0;i<4;i++)
	{
		wordlist_head[i] = gtk_event_box_new ();
		gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), wordlist_head[i],
		       Skin->shooting.wordlist_head[i].x[0],
		       Skin->shooting.wordlist_head[i].y[0]);
		pixmap_widget=gtk_image_new_from_pixmap(Skin->shooting.wordlist_head[i].p,(GdkBitmap *) NULL);
		gtk_container_add (GTK_CONTAINER (wordlist_head[i]), pixmap_widget);
		gtk_widget_show(pixmap_widget);
		gtk_widget_show(wordlist_head[i]);

		g_signal_connect (G_OBJECT(wordlist_head[i]), "button_press_event",
			 G_CALLBACK(on_shooting_wordlisthead_button_press), (void *)i);
		g_signal_connect (G_OBJECT(wordlist_head[i]), "enter_notify_event",
			 G_CALLBACK(on_shooting_wordlisthead_enter_notify), (void *)i);
		g_signal_connect (G_OBJECT(wordlist_head[i]), "leave_notify_event",
			 G_CALLBACK(on_shooting_wordlisthead_leave_notify), (void *)i);	
	}
*/
	tank_pixmap =
		gtk_image_new_from_pixmap (Skin->shooting.tank.p[0], (GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), tank_pixmap,
		       Skin->shooting.tank.x[0], Skin->shooting.tank.y[0]);
	gtk_widget_show (tank_pixmap);
	shoot_pixmap[0] =
		gtk_image_new_from_pixmap (Skin->shooting.shoot_pixmap[0].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), shoot_pixmap[0],
		       Skin->shooting.shoot_pixmap[0].x[0],
		       Skin->shooting.shoot_pixmap[0].y[0]);
	shoot_pixmap[1] =
		gtk_image_new_from_pixmap (Skin->shooting.shoot_pixmap[1].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), shoot_pixmap[1],
		       Skin->shooting.shoot_pixmap[1].x[0],
		       Skin->shooting.shoot_pixmap[1].y[0]);
	shoot_pixmap[2] =
		gtk_image_new_from_pixmap (Skin->shooting.shoot_pixmap[2].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), shoot_pixmap[2],
		       Skin->shooting.shoot_pixmap[2].x[0],
		       Skin->shooting.shoot_pixmap[2].y[0]);
	shoot_pixmap[3] =
		gtk_image_new_from_pixmap (Skin->shooting.shoot_pixmap[3].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), shoot_pixmap[3],
		       Skin->shooting.shoot_pixmap[3].x[0],
		       Skin->shooting.shoot_pixmap[3].y[0]);

	right_pixmap[0] =
		gtk_image_new_from_pixmap (Skin->shooting.right_pixmap[0].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_pixmap[0],
		       Skin->shooting.right_pixmap[0].x[0],
		       Skin->shooting.right_pixmap[0].y[0]);
	right_pixmap[1] =
		gtk_image_new_from_pixmap (Skin->shooting.right_pixmap[1].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_pixmap[1],
		       Skin->shooting.right_pixmap[1].x[0],
		       Skin->shooting.right_pixmap[1].y[0]);
	right_pixmap[2] =
		gtk_image_new_from_pixmap (Skin->shooting.right_pixmap[2].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_pixmap[2],
		       Skin->shooting.right_pixmap[2].x[0],
		       Skin->shooting.right_pixmap[2].y[0]);
	right_pixmap[3] =
		gtk_image_new_from_pixmap (Skin->shooting.right_pixmap[3].p[0],
				(GdkBitmap *) NULL);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), right_pixmap[3],
		       Skin->shooting.right_pixmap[3].x[0],
		       Skin->shooting.right_pixmap[3].y[0]);

	start_button.create (g_pReciteWord->fixed,
			     Skin->shooting.stop_button.x[0],
			     Skin->shooting.stop_button.y[0],
			     Skin->shooting.stop_button.p[0],
			     Skin->shooting.stop_button.p[1],
			     Skin->shooting.stop_button.p[2],
			     Skin->shooting.stop_button.p[3],
			     on_shooting_start_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed,
			     Skin->shooting.pause_button.x[0],
			     Skin->shooting.pause_button.y[0],
			     Skin->shooting.pause_button.p[0],
			     Skin->shooting.pause_button.p[1],
			     Skin->shooting.pause_button.p[2],
			     Skin->shooting.pause_button.p[3],
			     on_shooting_pause_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), pause_button.event_box,"Ctrl-P","");

	on_shooting_start_clicked ();
}

void
CShooting::set_wordlist_label (gint index)
{
	gint tmp[4];
	gint a,i;

	tmp[0] = std::rand () % (g_pReciteWord->now_zu_wordcount);
	i = 0;
	tmp[1] = std::rand () % (g_pReciteWord->now_zu_wordcount);
	while ((tmp[1]==tmp[0])&&(i<100))
	{
		tmp[1] = std::rand () % (g_pReciteWord->now_zu_wordcount);
		i++;
	}
	tmp[2] = std::rand () % (g_pReciteWord->now_zu_wordcount);
	while ((tmp[2]==tmp[0] || tmp[2]==tmp[1])&&(i<100))
	{
		tmp[2] = std::rand () % (g_pReciteWord->now_zu_wordcount);
		i++;
	}
	tmp[3] = std::rand () % (g_pReciteWord->now_zu_wordcount);
	while ((tmp[3]==tmp[0] || tmp[3]==tmp[1] || tmp[3]==tmp[2])&&(i<100))
	{
		tmp[3] = std::rand () % (g_pReciteWord->now_zu_wordcount);
		i++;
	}
	if (i>=100) //i think those while should end up soon,but it seems may be not.after all,i added the i to end the circle in sure.
	{
		tmp[1] = tmp[0]+1;
		tmp[2] = tmp[0]+2;
		tmp[3] = tmp[0]+3;
		tmp[1] = tmp[1] % (g_pReciteWord->now_zu_wordcount);
		tmp[2] = tmp[2] % (g_pReciteWord->now_zu_wordcount);
		tmp[3] = tmp[3] % (g_pReciteWord->now_zu_wordcount);
	}
	
	if ((tmp[0]!=index)&&(tmp[1]!=index)&&(tmp[2]!=index)&&(tmp[3]!=index))
	{
		a = std::rand () % 4;	//the right answer's position
		g_pReciteWord->shooting->tested_wordlist_item[index] = a;
		tmp[a] = index;
	}
	else
	{
		if (tmp[0]==index)
			g_pReciteWord->shooting->tested_wordlist_item[index] = 0;
		else if (tmp[1]==index)
			g_pReciteWord->shooting->tested_wordlist_item[index] = 1;
		else if (tmp[2]==index)
			g_pReciteWord->shooting->tested_wordlist_item[index] = 2;
		else
			g_pReciteWord->shooting->tested_wordlist_item[index] = 3;
	}

	if (method==0) // entocn
	{
		wordlist_item[0].set_label (g_pReciteWord->now_zu_words.m[tmp[0]]);
		wordlist_item[1].set_label (g_pReciteWord->now_zu_words.m[tmp[1]]);
		wordlist_item[2].set_label (g_pReciteWord->now_zu_words.m[tmp[2]]);
		wordlist_item[3].set_label (g_pReciteWord->now_zu_words.m[tmp[3]]);
	}
	else  // method==1,means cntoen.
	{
		wordlist_item[0].set_label (g_pReciteWord->now_zu_words.w[tmp[0]]);
		wordlist_item[1].set_label (g_pReciteWord->now_zu_words.w[tmp[1]]);
		wordlist_item[2].set_label (g_pReciteWord->now_zu_words.w[tmp[2]]);
		wordlist_item[3].set_label (g_pReciteWord->now_zu_words.w[tmp[3]]);
	}
}

void
CShooting::set_wordview_label (gint index)
{
	if (method==0)
	{
		gtk_label_set_text (GTK_LABEL (word_label),
			    g_pReciteWord->now_zu_words.w[index]);
		if (readword_ckbutton.status)
			readword(g_pReciteWord->now_zu_words.w[index]);
	}
	else
	{
		gtk_label_set_text (GTK_LABEL (meaning_label),
			    g_pReciteWord->now_zu_words.m[index]);
	}
}

void
CShooting::close ()
{
	return_button.destroy ();

	if (status == FALSE)
	{
		g_pReciteWord->chooseword->close ();
		return;
	}

	if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		g_pReciteWord->chooseword->close ();
	
	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "readword",
			     &tmp_b);
	if (tmp_b!=readword_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "shooting", "readword",
			     readword_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showphonetic",
			     &tmp_b);
	if (tmp_b!=phonetic_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "shooting", "showphonetic",
			     phonetic_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showmean",
			     &tmp_b);
	if (tmp_b!=meaning_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "shooting", "showmean",
			     meaning_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "showword",
			     &tmp_b);
	if (tmp_b!=word_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "shooting", "showword",
			     word_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "shooting", "shotsnd",
			     &tmp_b);
	if (tmp_b!=shotsnd_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "shooting", "shotsnd",
			     shotsnd_ckbutton.status);
	}

	if (shoot_pixmap_timer)
	{
		g_source_remove (shoot_pixmap_timer);
		shoot_pixmap_timer = 0;
	}

	entocn_robutton.destroy ();
	cntoen_robutton.destroy ();
	readword_ckbutton.destroy ();
	phonetic_ckbutton.destroy ();
	meaning_ckbutton.destroy ();
	word_ckbutton.destroy ();
	shotsnd_ckbutton.destroy ();
	readword_button.destroy ();
	chooseword_button.destroy ();
	wordlist_item[0].destroy ();
	wordlist_item[1].destroy ();
	wordlist_item[2].destroy ();
	wordlist_item[3].destroy ();

	gtk_widget_destroy (word_label);
	gtk_widget_destroy (meaning_label);
	gtk_widget_destroy (recited_word_count_label);
	gtk_widget_destroy (word_count_label);

	gtk_widget_destroy (tank_pixmap);
	gtk_widget_destroy (shoot_pixmap[0]);
	gtk_widget_destroy (shoot_pixmap[1]);
	gtk_widget_destroy (shoot_pixmap[2]);
	gtk_widget_destroy (shoot_pixmap[3]);
	gtk_widget_destroy (right_pixmap[0]);
	gtk_widget_destroy (right_pixmap[1]);
	gtk_widget_destroy (right_pixmap[2]);
	gtk_widget_destroy (right_pixmap[3]);

	wordlist_head[0].destroy ();
	wordlist_head[1].destroy ();
	wordlist_head[2].destroy ();
	wordlist_head[3].destroy ();

	start_button.destroy ();
	pause_button.destroy ();

	g_free (tested_wordlist_item);
}

void
CShooting::do_up()
{
	if (active_wordlist_item==0)
		return;
	if (!wordlist_head[active_wordlist_item-1].enable)
		return;
	on_shooting_wordlistitem_active (active_wordlist_item-1); //after this fuc,active_wordlist_item=active_wordlist_item-1.
	wordlist_item[active_wordlist_item].active = true;
	wordlist_item[active_wordlist_item].draw ();
}

void
CShooting::do_down()
{
	if (active_wordlist_item==3)
		return;
	if (!wordlist_head[active_wordlist_item+1].enable)
		return;
	on_shooting_wordlistitem_active (active_wordlist_item+1);
	wordlist_item[active_wordlist_item].active = true;
	wordlist_item[active_wordlist_item].draw ();
}

void
CShooting::do_pageup()
{
	if (active_wordlist_item==0)
		return;
	if (!wordlist_head[0].enable)
		return;
	on_shooting_wordlistitem_active (0);
	wordlist_item[0].active = true;
	wordlist_item[0].draw ();
}

void
CShooting::do_pagedown()
{
	if (active_wordlist_item==3)
		return;
	if (!wordlist_head[3].enable)
		return;
	on_shooting_wordlistitem_active (3);
	wordlist_item[3].active = true;
	wordlist_item[3].draw ();
}

void
CShooting::do_fire()
{
	if (!wordlist_head[active_wordlist_item].enable)
		return;	
	on_shooting_wordlistitem_clicked (active_wordlist_item);
}

CShooting::~CShooting ()
{
}
