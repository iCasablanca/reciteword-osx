#include "widget.h"
#include "skin.h"
#include "reciteword.h"
#include <gdk/gdkkeysyms.h>
#include <cstring>
#include <cstdio>

extern struct _Skin *Skin;
extern CReciteWord *g_pReciteWord;


static gboolean
on_pixbutton_button_press (GtkWidget * widget, GdkEvent * event,
			   tPixButton * parent)
{
	if (!parent->enable)
		return TRUE;
	playsound (SND_BUTTONDOWN);
	if (parent->pix_down)  // in shooting form,there are 4 button without pix_down.
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_down, (GdkBitmap *) NULL);
	}
	return TRUE;
}

static gboolean
on_pixbutton_button_release (GtkWidget * widget, GdkEvent * event,
			     tPixButton * parent)
{
	if (!parent->enable)
		return TRUE;

	playsound (SND_BUTTONUP);

	if (parent->pix_down)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_normal, (GdkBitmap *) NULL);
	}
	if (parent->mousein)
	{
		(*(parent->runfunc)) (parent->funcdata);
	}
	return TRUE;
}

static gboolean
on_pixbutton_enter_notify (GtkWidget * widget, GdkEvent * event,
			   tPixButton * parent)
{
	parent->mousein = TRUE;
	return false;
}

static gboolean
on_pixbutton_leave_notify (GtkWidget * widget, GdkEvent * event,
			   tPixButton * parent)
{
	parent->mousein = FALSE;
	return false;
}

tPixButton::tPixButton ()
{

}

tPixButton::~tPixButton ()
{
	
}

void
tPixButton::do_clicked (gboolean playsnd)
{
	if (!enable)
		return;
	if (playsnd)
		playsound (SND_BUTTONDOWN);
	(*(runfunc)) (funcdata);
}

void
tPixButton::init ()
{
	enable = TRUE;
}

void
tPixButton::create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		    GdkPixmap * pix_2, GdkPixmap * pix_3,
		    void (*func) (gpointer), gpointer funcdata0)
{
	init ();

	pix_normal = pix_1;
	pix_down = pix_2;
	pix_disable = pix_3;
	runfunc = func;
	funcdata = funcdata0;

	event_box = gtk_event_box_new ();
	gtk_fixed_put (GTK_FIXED (fixed), event_box, x, y);
	pixmap_widget = gtk_image_new_from_pixmap (pix_normal, (GdkBitmap *) NULL);
	gtk_container_add (GTK_CONTAINER (event_box), pixmap_widget);
	g_signal_connect (G_OBJECT (event_box), "button_press_event",
			  G_CALLBACK (on_pixbutton_button_press), this);
	g_signal_connect (G_OBJECT (event_box), "button_release_event",
			  G_CALLBACK (on_pixbutton_button_release), this);
	g_signal_connect (G_OBJECT (event_box), "enter_notify_event",
			  G_CALLBACK (on_pixbutton_enter_notify), this);
	g_signal_connect (G_OBJECT (event_box), "leave_notify_event",
			  G_CALLBACK (on_pixbutton_leave_notify), this);

	gtk_widget_realize (event_box);
	gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
	gtk_widget_show_all (event_box);
}

void
tPixButton::set_enable (int a)
{
	if (enable != a)
	{
		if (a)
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
			gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_normal, (GdkBitmap *) NULL);
		}
		else
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.disable_cursor.cursor);
			if (pix_disable)
				gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
						pix_disable,
						(GdkBitmap *) NULL);
		}
		enable = a;
	}
}

void
tPixButton::destroy ()
{
	gtk_widget_destroy (event_box);
}

static gint
on_checkbutton_button_press (GtkWidget * widget, GdkEvent * event,
			     tCheckButton * parent)
{
	if (!parent->enable)
		return TRUE;
	playsound (SND_BUTTONDOWN);
	if (parent->status)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_t_down, (GdkBitmap *) NULL);
	}
	else
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_f_down, (GdkBitmap *) NULL);
	}
	return TRUE;
}

static gint
on_checkbutton_button_release (GtkWidget * widget, GdkEvent * event,
			       tCheckButton * parent)
{
	if (!parent->enable)
		return TRUE;	
	playsound (SND_BUTTONUP);
	if (parent->mousein)
	{
		parent->set_status (!parent->status);
		if (parent->runfunc)
			(*(parent->runfunc)) ();
	}
	else
	{
		if (parent->status)
		{
			gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
					parent->pix_t_normal, (GdkBitmap *) NULL);
		}
		else
		{
			gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
					parent->pix_f_normal, (GdkBitmap *) NULL);
		}
	}
	return TRUE;
}

static gboolean
on_checkbutton_enter_notify (GtkWidget * widget, GdkEvent * event,
			     tCheckButton * parent)
{
	parent->mousein = TRUE;
	return true;
}

static gboolean
on_checkbutton_leave_notify (GtkWidget * widget, GdkEvent * event,
			     tCheckButton * parent)
{
	parent->mousein = FALSE;
	return true;
}

tCheckButton::tCheckButton ()
{
}

tCheckButton::~tCheckButton ()
{
}

void
tCheckButton::init ()
{
	status = TRUE;
	enable = true;
}

void
tCheckButton::create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		      GdkPixmap * pix_2, GdkPixmap * pix_3, GdkPixmap * pix_4,
		      void (*func) ())
{
	init ();

	pix_t_normal = pix_1;
	pix_t_down = pix_2;
	pix_f_normal = pix_3;
	pix_f_down = pix_4;
	runfunc = func;


	event_box = gtk_event_box_new ();
	gtk_fixed_put (GTK_FIXED (fixed), event_box, x, y);
	pixmap_widget = gtk_image_new_from_pixmap (pix_t_normal, (GdkBitmap *) NULL);
	gtk_container_add (GTK_CONTAINER (event_box), pixmap_widget);
	g_signal_connect (G_OBJECT (event_box), "button_press_event",
			  G_CALLBACK (on_checkbutton_button_press), this);
	g_signal_connect (G_OBJECT (event_box), "button_release_event",
			  G_CALLBACK (on_checkbutton_button_release), this);
	g_signal_connect (G_OBJECT (event_box), "enter_notify_event",
			  G_CALLBACK (on_checkbutton_enter_notify), this);
	g_signal_connect (G_OBJECT (event_box), "leave_notify_event",
			  G_CALLBACK (on_checkbutton_leave_notify), this);

	gtk_widget_realize (event_box);
	gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
	gtk_widget_show_all (event_box);

}

void
tCheckButton::destroy ()
{
	gtk_widget_destroy (event_box);
}

void
tCheckButton::set_enable (gboolean e)
{
	if (enable != e)
	{
		if (e)
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
		}
		else
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.disable_cursor.cursor);
		}
		enable = e;
	}
}

void
tCheckButton::set_status (int newstatus)
{
	if (status != newstatus)
	{
		status = newstatus;
		if (newstatus)
			gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_t_normal, (GdkBitmap *) NULL);
		else
			gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_f_normal, (GdkBitmap *) NULL);
	}
}

static gboolean
on_pixbutton2_button_press (GtkWidget * widget, GdkEvent * event,
			    tPixButton2 * parent)
{
	if (!parent->enable)
		return TRUE;

	parent->pressed=true;
	playsound (SND_BUTTONDOWN);
	gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget), parent->pix_down,
			(GdkBitmap *) NULL);
	return TRUE;
}

static gboolean
on_pixbutton2_button_release (GtkWidget * widget, GdkEvent * event,
			      tPixButton2 * parent)
{
	if (!parent->enable)
		return TRUE;
	parent->pressed=false;
	playsound (SND_BUTTONUP);
	if (parent->mousein)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_active,
				(GdkBitmap *) NULL);
		(*(parent->runfunc)) ();
	}
	else
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_normal,
				(GdkBitmap *) NULL);
	}
	return TRUE;
}

static gboolean
on_pixbutton2_enter_notify (GtkWidget * widget, GdkEventCrossing * event,
			    tPixButton2 * parent)
{
	parent->mousein = TRUE;
	if (event->mode!=GDK_CROSSING_NORMAL) //event happen not because mouse move.
		return true;
	
	if (!parent->enable)
		return true;
	if (!parent->pressed)
	{
		playsound (SND_BUTTONACTIVE);
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_active, (GdkBitmap *) NULL);
	}
	return true;
}

static gboolean
on_pixbutton2_leave_notify (GtkWidget * widget, GdkEventCrossing * event,
			    tPixButton2 * parent)
{
	parent->mousein = FALSE;
	if (event->mode!=GDK_CROSSING_NORMAL) //event happen not because mouse move.
		return true;
	
	if (!parent->enable)
		return true;
	if (!parent->pressed)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_normal, (GdkBitmap *) NULL);
	}
	return true;
}

tPixButton2::tPixButton2 ()
{
}

tPixButton2::~tPixButton2 ()
{
}

void
tPixButton2::do_clicked (gboolean playsnd)
{
	if (!enable)
		return;
	if (playsnd)
		playsound (SND_BUTTONDOWN);
	(*(runfunc)) ();
}

void
tPixButton2::init ()
{
	enable = TRUE;
	pressed = false;
}

void
tPixButton2::create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		     GdkPixmap * pix_2, GdkPixmap * pix_3, GdkPixmap * pix_4,
		     void (*func) ())
{
	pix_normal = pix_1;
	pix_active = pix_2;
	pix_down = pix_3;
	pix_disable = pix_4;
	runfunc = func;

	init ();

	event_box = gtk_event_box_new ();
	gtk_fixed_put (GTK_FIXED (fixed), event_box, x, y);
	gtk_widget_show (event_box);


	pixmap_widget = gtk_image_new_from_pixmap (pix_normal, (GdkBitmap *) NULL);
	gtk_container_add (GTK_CONTAINER (event_box), pixmap_widget);
	gtk_widget_show (pixmap_widget);

	g_signal_connect (G_OBJECT (event_box), "button_press_event",
			  G_CALLBACK (on_pixbutton2_button_press), this);
	g_signal_connect (G_OBJECT (event_box), "button_release_event",
			  G_CALLBACK (on_pixbutton2_button_release), this);
	g_signal_connect (G_OBJECT (event_box), "enter_notify_event",
			  G_CALLBACK (on_pixbutton2_enter_notify), this);
	g_signal_connect (G_OBJECT (event_box), "leave_notify_event",
			  G_CALLBACK (on_pixbutton2_leave_notify), this);

	gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
}

void
tPixButton2::reset (GdkPixmap * pix_1, GdkPixmap * pix_2, GdkPixmap * pix_3,
		    GdkPixmap * pix_4)
{
	if ((pix_normal == pix_1) && (pix_active == pix_2)
	    && (pix_down == pix_3) && (pix_disable == pix_4))
		return;

	pix_normal = pix_1;
	pix_active = pix_2;
	pix_down = pix_3;
	pix_disable = pix_4;


	if (mousein)
		gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget), pix_active,
				(GdkBitmap *) NULL);
	else
		gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget), pix_normal,
				(GdkBitmap *) NULL);
}

void
tPixButton2::set_enable (int a)
{
	if (enable != a)
	{
		if (a)
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
			if (mousein)
				gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
						pix_active,
						(GdkBitmap *) NULL);
			else
				gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
						pix_normal,
						(GdkBitmap *) NULL);
		}
		else
		{
			gdk_window_set_cursor (event_box->window, Skin->reciteword.disable_cursor.cursor);
			if (pix_disable)
				gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_disable, (GdkBitmap *) NULL);
		}
		enable = a;
	}
}

void
tPixButton2::destroy ()
{
	gtk_widget_destroy (event_box);
}


static gboolean
on_radiobutton_button_press (GtkWidget * widget, GdkEvent * event,
			     tRadioButton * parent)
{
	playsound (SND_BUTTONDOWN);
	if (parent->selected)
	{
		if (parent->pix_t_down)
			gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
					parent->pix_t_down, (GdkBitmap *) NULL);
	}
	else
	{
		if (parent->pix_f_down)
			gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
					parent->pix_f_down, (GdkBitmap *) NULL);
	}
	return TRUE;
}

static gboolean
on_radiobutton_button_release (GtkWidget * widget, GdkEvent * event,
			       tRadioButton * parent)
{
	playsound (SND_BUTTONUP);
	if (parent->selected)
	{
		gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
				parent->pix_t_normal, (GdkBitmap *) NULL);
	}
	else
	{
		if (parent->mousein)
		{
			parent->set_selected (TRUE);
			if (parent->runfunc)
				(*(parent->runfunc)) (parent->funcdata);
		}
		else
		{
			gtk_image_set_from_pixmap (GTK_IMAGE (parent->pixmap_widget),
					parent->pix_f_normal, (GdkBitmap *) NULL);
		}
	}
	return TRUE;
}

static gboolean
on_radiobutton_enter_notify (GtkWidget * widget, GdkEvent * event,
			     tRadioButton * parent)
{
	parent->mousein = TRUE;
	return true;
}

static gboolean
on_radiobutton_leave_notify (GtkWidget * widget, GdkEvent * event,
			     tRadioButton * parent)
{
	parent->mousein = FALSE;
	return true;
}

tRadioButton::tRadioButton ()
{
}

tRadioButton::~tRadioButton ()
{
}

void
tRadioButton::init ()
{
	selected = FALSE;
	this->group = g_slist_prepend ((GSList *) NULL, this);
}

void
tRadioButton::create (GSList * group, GtkWidget * fixed, gint x, gint y,
		      GdkPixmap * pix_1, GdkPixmap * pix_2, GdkPixmap * pix_3,
		      GdkPixmap * pix_4, void (*func) (const gpointer), const gpointer funcdata1)
{
	init ();

	pix_t_normal = pix_1;
	pix_t_down = pix_2;
	pix_f_normal = pix_3;
	pix_f_down = pix_4;
	runfunc = func;
	funcdata = funcdata1;


	event_box = gtk_event_box_new ();
	gtk_fixed_put (GTK_FIXED (fixed), event_box, x, y);
	pixmap_widget = gtk_image_new_from_pixmap (pix_f_normal, (GdkBitmap *) NULL);
	gtk_container_add (GTK_CONTAINER (event_box), pixmap_widget);
	g_signal_connect (G_OBJECT (event_box), "button_press_event",
			  G_CALLBACK (on_radiobutton_button_press), this);
	g_signal_connect (G_OBJECT (event_box), "button_release_event",
			  G_CALLBACK(on_radiobutton_button_release),
			  this);
	g_signal_connect (G_OBJECT (event_box), "enter_notify_event",
			  G_CALLBACK(on_radiobutton_enter_notify), this);
	g_signal_connect (G_OBJECT (event_box), "leave_notify_event",
			  G_CALLBACK(on_radiobutton_leave_notify), this);

	gtk_widget_realize (event_box);
	gdk_window_set_cursor (event_box->window, Skin->reciteword.hand_cursor.cursor);
	gtk_widget_show_all (event_box);

	if (group)
		set_group (group);
	else
		set_selected (TRUE);
}

void
tRadioButton::set_group (GSList * group)
{
	if (this->group)
	{
		GSList *slist;

		this->group = g_slist_remove (this->group, this);

		for (slist = this->group; slist; slist = slist->next)
		{
			tRadioButton *tmp_button;
			tmp_button = (tRadioButton *) slist->data;
			tmp_button->group = this->group;
		}
	}

	this->group = g_slist_prepend (group, this);

	if (group)
	{
		GSList *slist;

		for (slist = group; slist; slist = slist->next)
		{
			tRadioButton *tmp_button;
			tmp_button = (tRadioButton *) slist->data;

			tmp_button->group = this->group;
		}
	}

	set_selected (group == NULL);
}

void
tRadioButton::set_selected (int a)
{
	if (this->selected != a)
	{
		if (a)
		{
			GSList *tmp_list;
			tRadioButton *tmp_button;
			tmp_list = this->group;
			while (tmp_list)
			{
				tmp_button = (tRadioButton *) tmp_list->data;
				tmp_list = tmp_list->next;
				if (tmp_button->selected)
				{
					tmp_button->set_selected (FALSE);
					break;
				}
			}
			gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_t_normal, (GdkBitmap *) NULL);
		}
		else
			gtk_image_set_from_pixmap (GTK_IMAGE (pixmap_widget),
					pix_f_normal, (GdkBitmap *) NULL);
		this->selected = a;
	}
}

void
tRadioButton::do_clicked ()
{
	set_selected(true);
	if (runfunc)
		(*(runfunc)) (funcdata);
}

void
tRadioButton::destroy ()
{
	GSList *tmp_list;
	tRadioButton *tmp_button;

	this->group = g_slist_remove (this->group, this);
	tmp_list = this->group;

	while (tmp_list)
	{
		tmp_button = (tRadioButton *) tmp_list->data;
		tmp_list = tmp_list->next;

		tmp_button->group = this->group;
	}


	gtk_widget_destroy (event_box);
}


static gint
on_entry_expose (GtkWidget * widget, GdkEventExpose * event, tEntry * parent)
{
	gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 parent->draw_pixmap,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	return FALSE;
}

static gint
on_entry_timer_active (gpointer data)
{
	tEntry *parent = (tEntry *) data;

	static gint drawed = FALSE;
	if (drawed)
	{
		parent->draw_cursor (0);
	}
	else
	{
		parent->draw_cursor (1);
	}
	drawed = !drawed;
	return TRUE;
}

static gboolean
on_entry_buttonpress (GtkWidget * widget, GdkEventButton * event,
		      tEntry * parent)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (!GTK_WIDGET_HAS_FOCUS (widget))
			gtk_widget_grab_focus (widget);
	}
	return true;
}

static gboolean
on_entry_keypress (GtkWidget * widget, GdkEventKey * event, tEntry * parent)
{
	if (parent->runfunc)
	{
		if ((*(parent->runfunc)) (0, event->keyval))
			return TRUE;
	}

	if (parent->editable == FALSE)
		return FALSE;

	gint return_val;
	gchar text0[256], text[256];
	gint position, length;

	return_val = FALSE;
	position = parent->position;
	strcpy (text, parent->text);
	strcpy (text0, parent->text);
	length = strlen (text0);
	
	gboolean only_ctrl_pressed = ((event->state & GDK_CONTROL_MASK)&&(!(event->state & GDK_MOD1_MASK))&&(!(event->state & GDK_SHIFT_MASK)));

	switch (event->keyval)
	{
	case GDK_BackSpace:
		return_val = TRUE;
		if (position == 0)
			return TRUE;
		if (only_ctrl_pressed) {
			strcpy(text, text0 + position);
			position = 0;
		}
		else {
			text0[position - 1] = '\0';
			sprintf (text, "%s%s", text0, text0 + position);
			position--;
		}
		break;
	case GDK_Delete:
		return_val = TRUE;
		if (position == length)
			return TRUE;
		text0[position] = '\0';
		sprintf (text, "%s%s", text0, text0 + position + 1);
		break;
	case GDK_Home:
		return_val = TRUE;
		position = 0;
		break;
	case GDK_End:
		return_val = TRUE;
		position = length;
		break;
	case GDK_Left:
		return_val = TRUE;
		if (position != 0)
			position--;
		break;
	case GDK_Right:
		return_val = TRUE;
		if (position != length)
			position++;
		break;
	case GDK_Return:
		return TRUE;
		break;
	case GDK_Tab:
	case GDK_Escape:
		break;
	default:
		return_val = TRUE;
		if ((length < parent->max_length) && (event->keyval >= 0x20)
		    && (event->keyval <= 0x7E))
		{
			gchar key;
			key = event->keyval;

			text0[position] = '\0';
			sprintf (text, "%s%c%s", text0, key,
				 parent->text + position);
			position++;
		}
		break;
	}
	parent->changed (text, position, event->keyval);

	return return_val;
}

tEntry::tEntry ()
{
}

tEntry::~tEntry ()
{
}

void
tEntry::init ()
{
	editable = TRUE;
	enablesnd = TRUE;
	show_cursor = TRUE;
	max_length = 40;
	position = 0;
	timer = 0;
	gc1 = gdk_gc_new (g_pReciteWord->window->window);
	gc2 = gdk_gc_new (g_pReciteWord->window->window);
	GdkColormap *cmap = gdk_rgb_get_colormap ();
	GdkColor color1 = { 0, 48896, 48896, 48896 }, color2 =
	{
	0, 0xFFFF, 0x8000, 0x8000};
	gdk_colormap_alloc_color (cmap, &(color1), FALSE, TRUE);
	gdk_colormap_alloc_color (cmap, &(color2), FALSE, TRUE);
	gdk_gc_set_foreground (gc1, &color1);
	gdk_gc_set_foreground (gc2, &color2);
	gdk_colormap_free_colors (cmap, &color1, 1);
	gdk_colormap_free_colors (cmap, &color2, 1);
}

void
tEntry::create (GtkWidget * pfixed, gint x, gint y, gint w,
		gint h, gchar * str, gchar * right_text, gint show_pen0,
		gint (*func) (gint, gint))
{
	init ();

	runfunc = func;
	strcpy (text, str);
	width = w;
	height = h;
	show_pen = show_pen0;

	text_area = gtk_drawing_area_new ();
	SetFont(text_area,true);
	gtk_widget_set_size_request (text_area, width, height);
	GTK_WIDGET_SET_FLAGS (text_area, GTK_CAN_FOCUS);

	gtk_widget_set_events (text_area, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (G_OBJECT (text_area), "expose_event",
			  G_CALLBACK(on_entry_expose), this);
	g_signal_connect (G_OBJECT (text_area), "key_press_event",
			  G_CALLBACK(on_entry_keypress), this);
	g_signal_connect (G_OBJECT (text_area),
			  "button_press_event",
			  G_CALLBACK(on_entry_buttonpress), this);
	gtk_fixed_put (GTK_FIXED (pfixed), text_area, x, y);
	gtk_widget_show (text_area);
	layout=gtk_widget_create_pango_layout (text_area, text);
	if (right_text)
	{
		show_length = TRUE;
		pango_layout_set_text(layout,right_text,-1);
		pango_layout_get_pixel_size(layout,&underline_w,NULL);
		//underline_w = gdk_string_width (Skin->text.font, right_text);
		//pango_layout_set_text(layout,text,-1);
	}
	else
	{
		show_length = FALSE;
		underline_w = 0;
	}

	draw_pixmap =
		gdk_pixmap_new (text_area->window, width, height,
				gdk_rgb_get_visual ()->depth);
	draw ();
	timer = g_timeout_add (500, on_entry_timer_active, this);

	//gdk_window_set_back_pixmap (text_area->window,draw_pixmap,0);
}

void
tEntry::draw ()
{
	gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
			    TRUE, 0, 0, width, height);
	if (strcmp(pango_layout_get_text(layout),text)!=0)
		pango_layout_set_text(layout,text,-1);
	gdk_draw_layout(draw_pixmap,text_area->style->black_gc,Skin->text.entry_text.x[0],Skin->text.entry_text.y[0],layout);
	/*gdk_draw_string (draw_pixmap, Skin->text.font,
			 text_area->style->black_gc, Skin->text.entry_text.x[0],
			 Skin->text.entry_text.y[0], text);*/
	if (show_length)
		gdk_draw_rectangle (draw_pixmap, gc1,
				    TRUE, Skin->text.entry_underline.x[0],
				    Skin->text.entry_underline.y[0], underline_w,
				    Skin->text.entry_underline.h);
	if (show_pen)
	{
		gint w;
		pango_layout_get_pixel_size(layout,&w,NULL);
		gdk_draw_drawable (draw_pixmap, text_area->style->white_gc,
				 Skin->text.entry_pen.p[0], 0, 0,
				 w /*gdk_string_width (Skin->text.font,
						   text)*/ +
				 Skin->text.entry_pen.x[0], Skin->text.entry_pen.y[0],
				 Skin->text.entry_pen.w,
				 Skin->text.entry_pen.h);
	}
	gtk_widget_queue_draw (text_area);
}

void
tEntry::draw_cursor (gint a)
{
	if ((!editable) || (!show_cursor))
		return;

	gint w;
	pango_layout_set_text(layout,text,position);
	pango_layout_get_pixel_size(layout,&w,NULL);

	if (a)
	{
		gdk_draw_rectangle (draw_pixmap, text_area->style->black_gc,
				    TRUE, w /*gdk_text_width (Skin->text.font,
							  text, position)*/,
				    Skin->text.entry_cursor.y[0],
				    Skin->text.entry_cursor.w,
				    Skin->text.entry_cursor.h);

	}
	else
	{
		gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
				    TRUE, w /*gdk_text_width (Skin->text.font,
							  text, position)*/,
				    Skin->text.entry_cursor.y[0],
				    Skin->text.entry_cursor.w,
				    Skin->text.entry_cursor.h);
	/*if (position > 0)
		{
			gdk_draw_text (draw_pixmap, Skin->text.font,
				       text_area->style->black_gc,
				       Skin->text.entry_text.x[0] +
				       gdk_text_width (Skin->text.font, text,
						       position - 1),
				       Skin->text.entry_text.y[0],
				       text + position - 1, 1);
		}
		if (position < strlen (text))
		{
			gdk_draw_text (draw_pixmap, Skin->text.font,
				       text_area->style->black_gc,
				       Skin->text.entry_text.x[0] +
				       gdk_text_width (Skin->text.font, text,
						       position),
				       Skin->text.entry_text.y[0],
				       text + position, 1);
		}*/		
		pango_layout_set_text(layout,text,-1);
		gdk_draw_layout(draw_pixmap,text_area->style->black_gc,Skin->text.entry_text.x[0],Skin->text.entry_text.y[0],layout);
	}
	gtk_widget_queue_draw (text_area);
}

void
tEntry::check_text (gchar * str,SOUND_EVENTS right_snd_event,SOUND_EVENTS wrong_snd_event)
{
	show_cursor = FALSE;

	if (strcmp (g_strstrip (text), str))
	{
		if (enablesnd)
			playsound(wrong_snd_event);
		gint w1,w2,w3;
		gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
				    TRUE, 0, 0, width, height);

		int i, j, k;
		j = strlen (str);
		k = strlen (text);		
		if (j >= k)
		{
			for (i = 0; i < k; i++)
			{
				if (text[i] != *(str + i))
				{
					pango_layout_set_text(layout,text,i);
					pango_layout_get_pixel_size(layout,&w1,NULL);
					pango_layout_set_text(layout,text+i,1);
					pango_layout_get_pixel_size(layout,&w2,NULL);
					gdk_draw_rectangle (draw_pixmap, gc2,
							    FALSE,
							     w1 /*gdk_text_width
							    (Skin->text.font,
							     text, i)*/,
							    Skin->text.
							    entry_cursor.y[0],
							    w2 /*gdk_char_width
							    (Skin->text.font,
							     text[i])*/,
							    Skin->text.
							    entry_cursor.h -
							    1);
				}
			}
			for (i = k; i < j; i++)
			{
				pango_layout_set_text(layout,text,-1);
				pango_layout_get_pixel_size(layout,&w1,NULL);
				pango_layout_set_text(layout,str+k,i-k);
				pango_layout_get_pixel_size(layout,&w2,NULL);				
				pango_layout_set_text(layout,str+i,1);
				pango_layout_get_pixel_size(layout,&w3,NULL);
				gdk_draw_rectangle (draw_pixmap, gc2,
						    FALSE,
						    w1+w2 /*gdk_string_width (Skin->
								      text.
								      font,
								      text) +
						    gdk_text_width (Skin->text.
								    font,
								    str + k,
								    i - k)*/,
						    Skin->text.entry_cursor.y[0],
						    w3 /*gdk_char_width (Skin->text.
								    font,
								    *(str +
								      i))*/,
						    Skin->text.entry_cursor.h -
						    1);
			}
		}
		else
		{
			for (i = 0; i < j; i++)
			{
				if (text[i] != *(str + i))
				{
					pango_layout_set_text(layout,text,i);
					pango_layout_get_pixel_size(layout,&w1,NULL);
					pango_layout_set_text(layout,text+i,1);
					pango_layout_get_pixel_size(layout,&w2,NULL);
					gdk_draw_rectangle (draw_pixmap, gc2,
							    FALSE,
							    w1 /*gdk_text_width
							    (Skin->text.font,
							     text, i)*/,
							    Skin->text.
							    entry_cursor.y[0],
							    w2 /*gdk_char_width
							    (Skin->text.font,
							     text[i])*/,
							    Skin->text.
							    entry_cursor.h -
							    1);
				}
			}
			pango_layout_set_text(layout,text,j);
			pango_layout_get_pixel_size(layout,&w1,NULL);
			pango_layout_set_text(layout,text+j,-1);
			pango_layout_get_pixel_size(layout,&w2,NULL);
			gdk_draw_rectangle (draw_pixmap, gc2,
					    FALSE,
					    w1 /*gdk_text_width (Skin->text.font,
							    text, j)*/,
					    Skin->text.entry_cursor.y[0],
					    w2 /*gdk_string_width (Skin->text.font,
							      text + j)*/,
					    Skin->text.entry_cursor.h - 1);
		}
		pango_layout_set_text(layout,text,-1);
		gdk_draw_layout(draw_pixmap,text_area->style->black_gc,Skin->text.entry_text.x[0],Skin->text.entry_text.y[0],layout);
		/*gdk_draw_string (draw_pixmap, Skin->text.font,
				 text_area->style->black_gc,
				 Skin->text.entry_text.x[0],
				 Skin->text.entry_text.y[0], text);*/
		pango_layout_get_pixel_size(layout,&w1,NULL);
		gdk_draw_drawable (draw_pixmap, text_area->style->white_gc,
				 Skin->text.entry_wrong.p[0], 0, 0,
				 w1 /*gdk_string_width (Skin->text.font,
						   text)*/ +
				 Skin->text.entry_wrong.x[0],
				 Skin->text.entry_wrong.y[0],
				 Skin->text.entry_wrong.w,
				 Skin->text.entry_wrong.h);
	}
	else
	{
		if (enablesnd)
			playsound(right_snd_event);
		gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
					TRUE, 0, 0, width, height);
		if (strcmp(pango_layout_get_text(layout),text)!=0)
			pango_layout_set_text(layout,text,-1);
		gdk_draw_layout(draw_pixmap,text_area->style->black_gc,Skin->text.entry_text.x[0],Skin->text.entry_text.y[0],layout);
		/*gdk_draw_string (draw_pixmap, Skin->text.font,
				 text_area->style->black_gc,
				 Skin->text.entry_text.x[0],
				 Skin->text.entry_text.y[0], text);*/
		gint w;
		pango_layout_get_pixel_size(layout,&w,NULL);
		gdk_draw_drawable (draw_pixmap, text_area->style->white_gc,
				 Skin->text.entry_right.p[0], 0, 0,
				 w /*gdk_string_width (Skin->text.font,
						   text)*/ +
				 Skin->text.entry_right.x[0],
				 Skin->text.entry_right.y[0],
				 Skin->text.entry_right.w,
				 Skin->text.entry_right.h);
	}
	gtk_widget_queue_draw (text_area);
}

void
tEntry::changed (gchar * str, guint pos, gint keyval)
{
	if (strcmp (str, text) != 0)
	{
		if (enablesnd)
		{
			if (strlen(str)>=strlen(text))
				playsound (SND_TYPE);
			else
				playsound (SND_BACK);
		}
		strcpy (text, str);
		//pango_layout_set_text(layout,str,-1);
		draw ();

		g_source_remove (timer);
		timer = g_timeout_add (500, on_entry_timer_active, this);

		position = pos;
		draw_cursor (1);
		(*(runfunc)) (1, keyval);
	}
	else
	{
		if (position != pos)
		{
			g_source_remove (timer);
			timer = g_timeout_add (500, on_entry_timer_active,
						 this);

			draw_cursor (0);
			position = pos;
			draw_cursor (1);
		}
	}
}

void
tEntry::set_text (gchar * str)
{
	strcpy (text, str);
	//pango_layout_set_text(layout,str,-1);
	position = 0;
	draw ();
}

void
tEntry::set_editable (gint a)
{
	if (editable == a)
		return;
	editable = a;
}

void
tEntry::set_showlength (gint a)
{
	if (a == show_length)
		return;
	if (a)
		gdk_draw_rectangle (draw_pixmap, gc1,
				    TRUE, Skin->text.entry_underline.x[0],
				    Skin->text.entry_underline.y[0], underline_w,
				    Skin->text.entry_underline.h);
	else
		gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
				    TRUE, Skin->text.entry_underline.x[0],
				    Skin->text.entry_underline.y[0], underline_w,
				    Skin->text.entry_underline.h);
	gtk_widget_queue_draw (text_area);
	show_length = a;
}

void
tEntry::set_righttext (gchar * right_text)
{
	pango_layout_set_text(layout,right_text,-1);
	if (show_length)
	{
		set_showlength (FALSE);
		pango_layout_get_pixel_size(layout,&underline_w,NULL);
		//underline_w = gdk_string_width (Skin->text.font, right_text);
		set_showlength (TRUE);
	}
	else
	{
		pango_layout_get_pixel_size(layout,&underline_w,NULL);
		//underline_w = gdk_string_width (Skin->text.font, right_text);
	}
}

void
tEntry::set_showpen (gint a)
{
	if (a != show_pen)
	{
		show_pen = a;
		draw ();
	}
}

gchar *
tEntry::get_text ()
{
	return text;
}

void
tEntry::destroy ()
{
	g_source_remove (timer);
	if (layout)
		g_object_unref (G_OBJECT (layout));
	gtk_widget_destroy (text_area);
	//gdk_gc_unref (gc1);
	//gdk_gc_unref (gc2);
	g_object_unref (G_OBJECT (gc1));
	g_object_unref (G_OBJECT (gc2));
}

/*
tPhoneticLabel::tPhoneticLabel ()
{
}

tPhoneticLabel::~tPhoneticLabel ()
{
}


static gboolean
on_phoneticlabel_expose (GtkWidget * widget, GdkEventExpose * event,
			 tPhoneticLabel * parent)
{
	gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 parent->draw_pixmap,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	return TRUE;
}

void
tPhoneticLabel::create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
			struct _Skin_phonetic *phonetic)
{
	text[0] = '\0';
	width = w;
	if (h > phonetic->h)
		height = h;
	else
		height = phonetic->h;
	ptc = phonetic;

	text_area = gtk_drawing_area_new ();
	gtk_widget_set_size_request (text_area, width, height);
	g_signal_connect (G_OBJECT (text_area), "expose_event",
			  G_CALLBACK (on_phoneticlabel_expose), this);
	gtk_fixed_put (GTK_FIXED (pfixed), text_area, x, y);
	gtk_widget_show (text_area);
	draw_pixmap =
		gdk_pixmap_new (text_area->window, width, height,
				gdk_rgb_get_visual ()->depth);
}

void
tPhoneticLabel::set_text (gchar * str)
{
	if (str)
		strcpy (text, str);
	else
		text[0] = '\0';
	draw ();
}

void
tPhoneticLabel::draw ()
{
	gdk_draw_rectangle (draw_pixmap, text_area->style->white_gc,
			    TRUE, 0, 0, width, height);
	if (text[0])
	{
	gint i = 0, x = 0, w = ptc->w[34], index;
	GdkPixmap *p = ptc->p[34];
	gdk_draw_drawable (draw_pixmap, text_area->style->white_gc, p, 0, 0, x,
			 0, w, ptc->h);
	x += w;
	while (text[i])
	{
		switch (text[i])
		{
		case 'i':
			index = 0;
			break;
		case ':':
			index = 1;
			break;
		case 'e':
			index = 2;
			break;
		case '5':
			index = 3;
			break;
		case '1':
			index = 4;
			break;
		case '6':
			index = 5;
			break;
		case 'u':
			index = 6;
			break;
		case '2':
			index = 7;
			break;
		case '3':
			index = 8;
			break;
		case 'a':
			index = 9;
			break;
		case 'o':
			index = 10;
			break;
		case '4':
			index = 11;
			break;
		case 'j':
			index = 12;
			break;
		case 'p':
			index = 13;
			break;
		case 'b':
			index = 14;
			break;
		case 't':
			index = 15;
			break;
		case 'd':
			index = 16;
			break;
		case 'k':
			index = 17;
			break;
		case 'g':
			index = 18;
			break;
		case 'm':
			index = 19;
			break;
		case 'n':
			index = 20;
			break;
		case '9':
			index = 21;
			break;
		case 'l':
			index = 22;
			break;
		case 'f':
			index = 23;
			break;
		case 'v':
			index = 24;
			break;
		case '0':
			index = 25;
			break;
		case '8':
			index = 26;
			break;
		case 's':
			index = 27;
			break;
		case 'z':
			index = 28;
			break;
		case '7':
			index = 29;
			break;
		case '=':
			index = 30;
			break;
		case 'r':
			index = 31;
			break;
		case 'h':
			index = 32;
			break;
		case 'w':
			index = 33;
			break;
		case '[':
			index = 34;
			break;
		case '\'':
			index = 35;
			break;
		case ';':
			index = 36;
			break;
		case ']':
			index = 37;
			break;
		case '-':
			index = 38;
			break;
		case '(':
			index = 39;
			break;
		case ')':
			index = 40;
			break;
		case ',':
			index = 41;
			break;
		case ' ':
			index = 42;
			break;
		default:
			index = 42;
			break;
		}
		w = ptc->w[index];
		p = ptc->p[index];
		gdk_draw_drawable (draw_pixmap, text_area->style->white_gc, p,
				 0, 0, x, 0, w, ptc->h);
		x += w;
		i++;
	}
	w = ptc->w[37];
	p = ptc->p[37];
	gdk_draw_drawable (draw_pixmap, text_area->style->white_gc, p, 0, 0, x,
			 0, w, ptc->h);
	}
	gtk_widget_queue_draw (text_area);
}

void
tPhoneticLabel::destroy ()
{
	gtk_widget_destroy (text_area);
}
*/

CPhoneticLabel::CPhoneticLabel ()
{
}

CPhoneticLabel::~CPhoneticLabel ()
{
}

void
CPhoneticLabel::create (GtkWidget * pfixed, gint x, gint y, struct _Skin_phonetic *phonetic)
{
	text[0] = '\0';
	ptc = phonetic;

	hbox = gtk_hbox_new (false,0);
	gtk_fixed_put (GTK_FIXED (pfixed), hbox, x, y);
	gtk_widget_show (hbox);
}

void
CPhoneticLabel::set_text (gchar * str)
{
	if (str)
		strcpy (text, str);
	else
		text[0] = '\0';
	refresh ();
}

void
CPhoneticLabel::refresh ()
{
	GList *children,*list;
	children = gtk_container_get_children(GTK_CONTAINER(hbox));
	list = children;
	while (list)
	{
		gtk_widget_destroy(GTK_WIDGET(list->data));
		list = list->next;
	}
	g_list_free(children);	
	
	if (!text[0])
		return;	
	
	GtkWidget *image;
	
	gint i = 0, index;
	GdkPixbuf *p = ptc->p[34];
	
	image = gtk_image_new_from_pixbuf(p);
	gtk_box_pack_start(GTK_BOX(hbox),image,false,false,0);
	gtk_widget_show(image);
	
	while (text[i])
	{
		switch (text[i])
		{
		case 'i':
			index = 0;
			break;
		case ':':
			index = 1;
			break;
		case 'e':
			index = 2;
			break;
		case '5':
			index = 3;
			break;
		case '1':
			index = 4;
			break;
		case '6':
			index = 5;
			break;
		case 'u':
			index = 6;
			break;
		case '2':
			index = 7;
			break;
		case '3':
			index = 8;
			break;
		case 'a':
			index = 9;
			break;
		case 'o':
			index = 10;
			break;
		case '4':
			index = 11;
			break;
		case 'j':
			index = 12;
			break;
		case 'p':
			index = 13;
			break;
		case 'b':
			index = 14;
			break;
		case 't':
			index = 15;
			break;
		case 'd':
			index = 16;
			break;
		case 'k':
			index = 17;
			break;
		case 'g':
			index = 18;
			break;
		case 'm':
			index = 19;
			break;
		case 'n':
			index = 20;
			break;
		case '9':
			index = 21;
			break;
		case 'l':
			index = 22;
			break;
		case 'f':
			index = 23;
			break;
		case 'v':
			index = 24;
			break;
		case '0':
			index = 25;
			break;
		case '8':
			index = 26;
			break;
		case 's':
			index = 27;
			break;
		case 'z':
			index = 28;
			break;
		case '7':
			index = 29;
			break;
		case '=':
			index = 30;
			break;
		case 'r':
			index = 31;
			break;
		case 'h':
			index = 32;
			break;
		case 'w':
			index = 33;
			break;
		case '[':
			index = 34;
			break;
		case '\'':
			index = 35;
			break;
		case ';':
			index = 36;
			break;
		case ']':
			index = 37;
			break;
		case '-':
			index = 38;
			break;
		case '(':
			index = 39;
			break;
		case ')':
			index = 40;
			break;
		case ',':
			index = 41;
			break;
		case ' ':
			index = 42;
			break;
		default:
			index = 42;
			break;
		}
		p = ptc->p[index];
		image = gtk_image_new_from_pixbuf(p);
		gtk_box_pack_start(GTK_BOX(hbox),image,false,false,0);
		gtk_widget_show(image);
		i++;
	}
	p = ptc->p[37];
	image = gtk_image_new_from_pixbuf(p);
	gtk_box_pack_start(GTK_BOX(hbox),image,false,false,0);
	gtk_widget_show(image);
}

void
CPhoneticLabel::destroy ()
{
	gtk_widget_destroy (hbox);
}
