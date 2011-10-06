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
#include <cstring>
#include <cstdio>
#include <ctime>
#include <string>
#include <fstream>


extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;

static gint
on_textbutton_expose (GtkWidget * widget, GdkEventExpose * event,
		     tTextButton * parent)
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
on_textbutton_buttonpress (GtkWidget * widget, GdkEventButton * event,
			  tTextButton * parent)
{
	if (!parent->enable)
		return TRUE;
	playsound (SND_BUTTONDOWN);
	parent->draw(true);
	return TRUE;
}

static gint
on_textbutton_buttonrelease (GtkWidget * widget, GdkEventButton * event,
			    tTextButton * parent)
{
	if (!parent->enable)
		return TRUE;
	playsound (SND_BUTTONUP);
	parent->draw(false);
	if (parent->mousein)
	{
		(*(parent->runfunc)) (parent->funcdata);
	}
	return TRUE;
}

static void
on_textbutton_enter_notify (GtkWidget * widget, GdkEvent * event,
			   tTextButton * parent)
{
	parent->mousein = TRUE;
}

static void
on_textbutton_leave_notify (GtkWidget * widget, GdkEvent * event,
			   tTextButton * parent)
{
	parent->mousein = FALSE;
}

tTextButton::tTextButton ()
{
}

void
tTextButton::init ()
{
	layout= NULL;
	mousein = FALSE;
	enable = true;
}

void
tTextButton::create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,gint tx1, gint ty1,gint tx2, gint ty2,
		     GdkPixmap * pix_1, GdkPixmap * pix_2, gchar * str,
		     void (*func) (gpointer), gpointer data)
{
	init ();

	pix_normal = pix_1;
	pix_down = pix_2;
	width = w;
	height = h;
	text_x1 = tx1;
	text_y1 = ty1;
	text_x2 = tx2;
	text_y2 = ty2;
	runfunc = func;
	funcdata = data;

	draw_area = gtk_drawing_area_new ();
	gtk_widget_set_size_request (draw_area, width, height);

	gtk_widget_set_events (draw_area,
			       GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
			       | GDK_ENTER_NOTIFY_MASK |
			       GDK_LEAVE_NOTIFY_MASK);
	g_signal_connect (G_OBJECT (draw_area), "expose_event",
			  G_CALLBACK (on_textbutton_expose), this);
	g_signal_connect (G_OBJECT (draw_area), "button_press_event",
			  G_CALLBACK (on_textbutton_buttonpress), this);
	g_signal_connect (G_OBJECT (draw_area), "button_release_event",
			  G_CALLBACK (on_textbutton_buttonrelease), this);
	g_signal_connect (G_OBJECT (draw_area), "enter_notify_event",
			  G_CALLBACK (on_textbutton_enter_notify), this);
	g_signal_connect (G_OBJECT (draw_area), "leave_notify_event",
			  G_CALLBACK (on_textbutton_leave_notify), this);

	gtk_fixed_put (GTK_FIXED (pfixed), draw_area, x, y);
	gtk_widget_show (draw_area);
	gdk_window_set_cursor (draw_area->window, Skin->reciteword.hand_cursor.cursor);
	draw_pixmap =
		gdk_pixmap_new (draw_area->window, width, height,
				gdk_rgb_get_visual ()->depth);
	strcpy (text, str);
	layout=gtk_widget_create_pango_layout (draw_area, text);
	draw (false);
}

void
tTextButton::draw (gboolean pressed)
{
	if (pressed)
	{
		gdk_draw_drawable (draw_pixmap, draw_area->style->white_gc,
				 pix_down, 0, 0, 0, 0, width, height);
		gdk_draw_layout(draw_pixmap,draw_area->style->black_gc,text_x2,text_y2,layout);
	}
	else
	{
		gdk_draw_drawable (draw_pixmap, draw_area->style->white_gc,
				 pix_normal, 0, 0, 0, 0, width, height);
		gdk_draw_layout(draw_pixmap,draw_area->style->black_gc,text_x1,text_y1,layout);
	}
	gtk_widget_queue_draw (draw_area);
}

void
tTextButton::set_enable (gboolean a)
{
	if (enable != a)
	{
		if (a)
		{
			gdk_window_set_cursor (draw_area->window, Skin->reciteword.hand_cursor.cursor);
		}
		else
		{
			gdk_window_set_cursor (draw_area->window,
					       Skin->reciteword.disable_cursor.cursor);
		}
		enable = a;
	}
}

void
tTextButton::destroy ()
{
	g_object_unref (G_OBJECT (layout));
	gtk_widget_destroy (draw_area);
}

tTextButton::~tTextButton ()
{
}


static void
on_revise_group_return_clicked ()
{
	g_pReciteWord->revise_group->close ();
	g_pReciteWord->show ();
}

static void 
on_revise_group_chooseword_ok_clicked ()
{
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->revise_group->close ();
	g_pReciteWord->ShowRevise_test ();
}

static void 
on_revise_group_chooseword_cancel_clicked ()
{
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->revise_group->chooseword_button.set_enable(true);
	g_pReciteWord->revise_group->order_ckbutton.set_enable(true);
	gint i;
	for (i = 0; i < 5; i++)
	{
		if (g_pReciteWord->revise_group->plane[i])
		{
			g_pReciteWord->revise_group->plane[i]->set_enable (true);
		}
		if (g_pReciteWord->revise_group->plane_button[i])
		{
			g_pReciteWord->revise_group->plane_button[i]->set_enable (true);
		}
	}
}

static void
on_revise_group_chooseword_clicked (gpointer data)
{
	g_pReciteWord->PopupChooseword (on_revise_group_chooseword_ok_clicked,
					on_revise_group_chooseword_cancel_clicked, &(g_pReciteWord->revise_test_chooseword_info));
	g_pReciteWord->revise_group->chooseword_button.set_enable(false);
	g_pReciteWord->revise_group->order_ckbutton.set_enable(false);
	gint i;
	for (i = 0; i < 5; i++)
	{
		if (g_pReciteWord->revise_group->plane[i])
		{
			g_pReciteWord->revise_group->plane[i]->set_enable (false);
		}
		if (g_pReciteWord->revise_group->plane_button[i])
		{
			g_pReciteWord->revise_group->plane_button[i]->set_enable (false);
		}
	}
}

static void
on_revise_group_plane_clicked (gpointer data)
{
	glong index = (glong) data;
	gint zu_index = g_pReciteWord->revise_group->revise_zu_index[index];

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

	g_pReciteWord->set_now_zu_words(zu_index,last_zu_wordcount,
					g_pReciteWord->now_book->words.w + now_wordcount,
					g_pReciteWord->now_book->words.t + now_wordcount,
					g_pReciteWord->now_book->words.m + now_wordcount,
					g_pReciteWord->revise_group->order_ckbutton.status);
	g_pReciteWord->revise_group->close ();
	g_pReciteWord->ShowRevise_test ();
}

CRevise_group::CRevise_group ()
{
	skin_load_revise_group (g_pReciteWord->window);
	rw_cfg_read_int (usercfgfile, "revise_group", "max_revise_times", &max_revise_times);
	if (max_revise_times==0)
		max_revise_times=6;
}

CRevise_group::~CRevise_group ()
{
}

void
CRevise_group::init ()
{
	g_pReciteWord->status = STATUS_REVISE_GROUP;
	gint i;
	for (i = 0; i < 5; i++)
	{
		plane_button[i] = NULL;
		plane[i] = NULL;
		plane_h[i] = NULL;
	}
}

void
CRevise_group::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed,
		       Skin->revise_group.revise_group.p[0]);
	return_button.create (g_pReciteWord->fixed,
			      Skin->revise_group.return_button.x[0],
			      Skin->revise_group.return_button.y[0],
			      Skin->revise_group.return_button.p[0],
			      Skin->revise_group.return_button.p[1],
			      Skin->revise_group.return_button.p[2],
			      (GdkPixmap *) NULL,
			      on_revise_group_return_clicked);
	chooseword_button.create (g_pReciteWord->fixed,
			      Skin->revise_group.chooseword_button.x[0],
			      Skin->revise_group.chooseword_button.y[0],
			      Skin->revise_group.chooseword_button.p[0],
			      Skin->revise_group.chooseword_button.p[1],
			      Skin->revise_group.chooseword_button.p[2],
			      on_revise_group_chooseword_clicked,this);
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, chooseword_button.event_box,"Ctrl-W","");
	order_ckbutton.create (g_pReciteWord->fixed,
			      Skin->revise_group.order_ckbutton.x[0],
			      Skin->revise_group.order_ckbutton.y[0],
			      Skin->revise_group.order_ckbutton.p[0],
			      Skin->revise_group.order_ckbutton.p[1],
			      Skin->revise_group.order_ckbutton.p[2],
				  Skin->revise_group.order_ckbutton.p[3],
			      NULL);				  
   	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "revise_group", "disorder",
			     &order);
	order_ckbutton.set_status(order);
	glong found_group = 0;
	gint found = TRUE;
	gint zu_index = 0, revise_times;
	gchar tmpstr[256];
	std::time_t last_time;
	double between_time;
	while (found)
	{
		sprintf (tmpstr, "zu%d_time0", zu_index);
		if (rw_cfg_read_time (usercfgfile, g_pReciteWord->now_book->bookfilename, tmpstr, &last_time))	// have firstrecited 
		{
			sprintf (tmpstr, "zu%d_revisetimes", zu_index);
			rw_cfg_read_int (usercfgfile,
					 g_pReciteWord->now_book->
					 bookfilename, tmpstr, &revise_times);
			if (revise_times < max_revise_times)
			{
				sprintf (tmpstr, "zu%d_time%d", zu_index,
					 revise_times);
				rw_cfg_read_time (usercfgfile,
						  g_pReciteWord->now_book->
						  bookfilename, tmpstr,
						  &last_time);
				between_time = (revise_times * (24 * 60 * 60)) - (12 * 60 * 60);
				std::time_t now = std::time (0);
				if (difftime (now, last_time) > between_time)
				{
					found_group++;
					if (found_group <= 5)  //only show the first 5 groups.
					{
						this->revise_zu_index[found_group-1] =
							zu_index;						
						if (revise_times > 4)
							create_plane (4,found_group-1);
						else
							create_plane (revise_times,found_group-1);
					}
				}
			}
		}
		zu_index++;
		if (zu_index > g_pReciteWord->max_zu_index)	//the current book is search over....will try old books.
			found = FALSE;
	}
	sprintf(tmpstr,_("found %ld groups in total"),found_group);
	total_groups_label = gtk_label_new(tmpstr);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), total_groups_label,
		       Skin->revise_group.total_groups_label.x[0], Skin->revise_group.total_groups_label.y[0]);
	gtk_widget_show (total_groups_label);
}

void
CRevise_group::create_plane (glong x, glong y)
{
	gchar tmpstr[246];
	sprintf(tmpstr,_("G%d"),revise_zu_index[y]+1);
	plane_button[y] = new tTextButton;
	plane_button[y]->create (g_pReciteWord->fixed,
				 Skin->revise_group.plane_button[y].x[0],
				 Skin->revise_group.plane_button[y].y[0],
				 Skin->revise_group.plane_button_p.w,
				 Skin->revise_group.plane_button_p.h,
				 Skin->revise_group.plane_button_text_1.x[0],
				 Skin->revise_group.plane_button_text_1.y[0],
				 Skin->revise_group.plane_button_text_2.x[0],
				 Skin->revise_group.plane_button_text_2.y[0],
				 Skin->revise_group.plane_button_p.p[0],
				 Skin->revise_group.plane_button_p.p[1],
				 tmpstr,
				 on_revise_group_plane_clicked,
				 (gpointer)y);
	plane[y] = new tPixButton;
	plane[y]->create (g_pReciteWord->fixed,
			  Skin->revise_group.plane[x][y].x[0],
			  Skin->revise_group.plane[x][y].y[0],
			  Skin->revise_group.plane[x][y].p[0],
			  Skin->revise_group.plane[x][y].p[1],
			  (GdkPixmap *) NULL, on_revise_group_plane_clicked,
			  (gpointer)y);
	const gchar tooltips[5][3]={"F1","F2","F3","F4","F5"};
	gtk_tooltips_set_tip (g_pReciteWord->tooltips, plane[y]->event_box,tooltips[y],"");
	if (Skin->revise_group.plane_h[x][y].w)
	{
		plane_h[y] =
			gtk_image_new_from_pixmap (Skin->revise_group.plane_h[x][y].p[0],
					NULL);
		gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), plane_h[y],
			       Skin->revise_group.plane_h[x][y].x[0],
			       Skin->revise_group.plane_h[x][y].y[0]);
		gtk_widget_show (plane_h[y]);
	}
}

void
CRevise_group::close ()
{
	if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		g_pReciteWord->chooseword->close ();
	
	gboolean order;
	rw_cfg_read_boolean(usercfgfile,
			     "revise_group", "disorder",
			     &order);
	if (order!=order_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "revise_group", "disorder",
			     order_ckbutton.status);
	}

	gint i;
	for (i = 0; i < 5; i++)
	{
		if (plane_h[i])
			gtk_widget_destroy (plane_h[i]);
		if (plane[i])
		{
			plane[i]->destroy ();
			delete plane[i];
		}
		if (plane_button[i])
		{
			plane_button[i]->destroy ();
			delete plane_button[i];
		}
	}
	chooseword_button.destroy();
	order_ckbutton.destroy();
	gtk_widget_destroy(total_groups_label);
	return_button.destroy ();
}


static gboolean
on_viewer_expose (GtkWidget * widget, GdkEventExpose * event,
		  tViewer * parent)
{
	gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 parent->draw_pixmap,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	return true;
}

static gint viewer_disappear_timer_func (gpointer data);
static gint viewer_m_show_timer_func (gpointer data);
static gint viewer_hide_timer_func (gpointer data);

static gint
viewer_w_appear_timer_func (gpointer data)
{
	tViewer *parent = (tViewer *) data;
	if ((parent->color.red) == 0xF800)
	{
		parent->color.red = 0xFFFF;
		parent->color.green = 0xFFFF;
		parent->color.blue = 0xFFFF;
	}
	else
	{
		parent->color.red += 0x800;
		parent->color.green += 0x800;
		parent->color.blue += 0x800;
	}
	gdk_colormap_alloc_color (parent->cmap, &(parent->color), FALSE,
				  TRUE);
	//gdk_color_change(parent->cmap,&(parent->color));
	if (parent->now_count < parent->count)
		parent->draw (1);
	else
		parent->draw (4);
	if ((parent->color.red) == 0xFFFF)
	{
		parent->w_appear_timer = 0;
		if (parent->now_count < parent->count)
			parent->m_show_timer =
				g_timeout_add (parent->w_show_time,
						 viewer_m_show_timer_func,
						 data);
		else
			(*(parent->runfunc)) ();
		return FALSE;
	}
	else
		return TRUE;
}

static gint
viewer_m_appear_timer_func (gpointer data)
{
	tViewer *parent = (tViewer *) data;
	if ((parent->color.red) == 0xF000)
	{
		parent->color.red = 0xFFFF;
		parent->color.green = 0xFFFF;
		parent->color.blue = 0xFFFF;
	}
	else
	{
		parent->color.red += 0x1000;
		parent->color.green += 0x1000;
		parent->color.blue += 0x1000;
	}
	gdk_colormap_alloc_color (parent->cmap, &(parent->color), FALSE,
				  TRUE);
	parent->draw (2);
	if ((parent->color.red) == 0xFFFF)
	{
		parent->hide_timer =
			g_timeout_add (parent->m_show_time, viewer_hide_timer_func, data);
		parent->m_appear_timer = 0;
		return FALSE;
	}
	else
		return TRUE;

}

static gint
viewer_m_show_timer_func (gpointer data)
{
	tViewer *parent = (tViewer *) data;
	parent->color.red = 0x0000;
	parent->color.green = 0x0000;
	parent->color.blue = 0x0000;

	parent->m_appear_timer =
		g_timeout_add (20, viewer_m_appear_timer_func, data);
	parent->m_show_timer = 0;
	return FALSE;
}
static gint
viewer_hide_timer_func (gpointer data)
{
	tViewer *parent = (tViewer *) data;
	parent->disappear_timer =
		g_timeout_add (20, viewer_disappear_timer_func, data);
	parent->hide_timer = 0;
	return FALSE;
}

static gint
viewer_disappear_timer_func (gpointer data)
{
	tViewer *parent = (tViewer *) data;
	if ((parent->color.red) == 0xFFFF)
	{
		parent->color.red = 0xF800;
		parent->color.green = 0xF800;
		parent->color.blue = 0xF800;
	}
	else
	{
		parent->color.red -= 0x800;
		parent->color.green -= 0x800;
		parent->color.blue -= 0x800;
	}
	gdk_colormap_alloc_color (parent->cmap, &(parent->color), FALSE,
				  TRUE);
	//gdk_color_change(parent->cmap,&(parent->color));
	if (parent->color.red == 0x0000)
	{
		parent->refresh();
		parent->now_count++;
		if (parent->now_count < parent->count)
		{
			parent->set_text(parent->str_1[parent->now_count],parent->str_2[parent->now_count]);
			/*parent->str1 = parent->str_1[parent->now_count];
			parent->str2 = parent->str_2[parent->now_count];*/
		}
		else
		{
			parent->set_text("","Game Over! :-)");
			//parent->str2 = "Game Over! :-)";
		}
		parent->w_appear_timer =
			g_timeout_add (20, viewer_w_appear_timer_func,
					 data);
		parent->disappear_timer = 0;
		return FALSE;
	}
	else {
		parent->draw (3);
		return TRUE;
	}
}

tViewer::tViewer ()
{
	rw_cfg_read_int (usercfgfile, "revise_skim", "word_show_time", &w_show_time);
	if (w_show_time <= 0)
		w_show_time = 1000; // 1 seconds.
	rw_cfg_read_int (usercfgfile, "revise_skim", "meaning_show_time", &m_show_time);
	if (m_show_time <= 0)
		m_show_time = 2000; // 2 seconds.
}

tViewer::~tViewer ()
{
}

void
tViewer::init ()
{
	layout_w=NULL;
	layout_m=NULL;
	gc = gdk_gc_new (g_pReciteWord->window->window);
	cmap = gdk_rgb_get_colormap();
}

void
tViewer::create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		 gchar ** str_w, gchar ** str_m, gint _count, void (*func) ())
{
	init ();

	str_1 = str_w;
	str_2 = str_m;
	count = _count;
	runfunc = func;

	text_area = gtk_drawing_area_new ();
	gtk_widget_set_size_request (text_area, w, h);

	g_signal_connect (G_OBJECT (text_area), "expose_event",
			  G_CALLBACK(on_viewer_expose), this);
	gtk_fixed_put (GTK_FIXED (pfixed), text_area, x, y);
	gtk_widget_show (text_area);
	draw_pixmap =
		gdk_pixmap_new (text_area->window, w, h,
				gdk_rgb_get_visual ()->depth);
	refresh ();
}

void
tViewer::set_words(gchar ** str_w, gchar ** str_m, gint _count)
{
	str_1 = str_w;
	str_2 = str_m;
	count = _count;
}

void
tViewer::set_text(gchar * str_w, gchar * str_m)
{
	str1 = str_w;
	str2 = str_m;
	if (layout_w)
		pango_layout_set_text(layout_w,str1,-1);
	else {
		layout_w=gtk_widget_create_pango_layout (text_area, str1);
		pango_layout_set_font_description(layout_w,Skin->reciteword.b_font);
	}
	if (layout_m) {
		if (str2) {
			pango_layout_set_text(layout_m,str2,-1);
		} else {
			pango_layout_set_text(layout_m,"",-1);
		}
	}
	else
		layout_m=gtk_widget_create_pango_layout (text_area, str2);
	if (g_pReciteWord->revise_skim->readword_ckbutton.status)
		readword(str_w);
	if (str1[0]!='\0')
	{
		gchar aa[5];
		sprintf(aa,"%2d",now_count+1);
		gtk_label_set_text (GTK_LABEL (g_pReciteWord->revise_skim->word_index_label),
					aa);
	}
}

void
tViewer::refresh ()
{

	gdk_draw_rectangle (draw_pixmap, text_area->style->black_gc,
			    TRUE, 0, 0, Skin->revise_skim.viewer.w,
			    Skin->revise_skim.viewer.h);
	gtk_widget_queue_draw (text_area);
}

void
tViewer::draw (gint status)
{
	gint x1=0, y1=0, x2, y2, h1, h2;

	gdk_gc_set_foreground (gc, &color);
	gint w;
	pango_layout_get_pixel_size(layout_m,&w,&h2);
	x2 = (Skin->revise_skim.viewer.w -
			w
	      /*gdk_string_width (gtk_style_get_font (text_area->style),
				str2)*/) / 2;
	//h2 = gdk_string_height (gtk_style_get_font (text_area->style), str2);
	if (status == 4)
	{
		y2 = (Skin->revise_skim.viewer.h - h2) / 2;
	}
	else
	{
		pango_layout_get_pixel_size(layout_w,&w,&h1);
		x1 = (Skin->revise_skim.viewer.w -
		      w) / 2;
		/*x1 = (Skin->revise_skim.viewer.w -
		      gdk_string_width (Skin->text.font, str1)) / 2;
		h1 = gdk_string_height (Skin->text.font, str1);*/		
		y1 = (Skin->revise_skim.viewer.h - (h1 + h2 + 10)) / 2 ;
		y2 = y1 + h1 + 10;
	}
	if ((status == 1) || (status == 3))
	{
		gdk_draw_layout(draw_pixmap,gc,x1,y1,layout_w);
		/*gdk_draw_string (draw_pixmap, Skin->text.font,
				 gc, x1, y1, str1);*/
	}
	if ((status == 2) || (status == 3) || (status == 4))
	{
		gdk_draw_layout(draw_pixmap,gc,x2,y2,layout_m);
		/*gdk_draw_string (draw_pixmap,
				 gtk_style_get_font (text_area->style), gc,
				 x2, y2, str2);*/
	}
	gtk_widget_queue_draw (text_area);
}

void
tViewer::set_show_meaning (gint a)
{
	if (a != show_meaning)
	{
		show_meaning = a;
	}
}

void
tViewer::start_skim ()
{
	now_count = 0;
	set_text(str_1[0],str_2[0]);
	/*str1 = str_1[0];
	str2 = str_2[0];*/
	refresh ();
	color.red = 0x0000;
	color.green = 0x0000;
	color.blue = 0x0000;
	m_show_timer = m_appear_timer = hide_timer = disappear_timer = 0;
	w_appear_timer =
		g_timeout_add (20, viewer_w_appear_timer_func, this);
}

void
tViewer::stop_skim ()
{
	if (w_appear_timer)
	{
		g_source_remove (w_appear_timer);
	}
	else if (m_show_timer)
	{
		g_source_remove (m_show_timer);
	}
	else if (m_appear_timer)
	{
		g_source_remove (m_appear_timer);
	}
	else if (hide_timer)
	{
		g_source_remove (hide_timer);
	}
	else if (disappear_timer)
	{
		g_source_remove (disappear_timer);
	}
}

void
tViewer::pause_skim ()
{
	stop_skim ();
}

void
tViewer::continue_skim ()
{
	if (w_appear_timer)
	{
		w_appear_timer =
			g_timeout_add (20, viewer_w_appear_timer_func,
					 this);
	}
	else if (m_show_timer)
	{
		m_show_timer =
			g_timeout_add (w_show_time, viewer_m_show_timer_func,
					 this);
	}
	else if (m_appear_timer)
	{
		m_appear_timer =
			g_timeout_add (20, viewer_m_appear_timer_func,
					 this);
	}
	else if (hide_timer)
	{
		hide_timer =
			g_timeout_add (m_show_time, viewer_hide_timer_func, this);
	}
	else if (disappear_timer)
	{
		disappear_timer =
			g_timeout_add (20, viewer_disappear_timer_func,
					 this);
	}
}

void
tViewer::destroy ()
{
	stop_skim ();
	if (layout_w)
		g_object_unref (G_OBJECT (layout_w));
	if (layout_m)
		g_object_unref (G_OBJECT (layout_m));
	gtk_widget_destroy (text_area);
	g_object_unref (G_OBJECT (gc));
	//gdk_gc_unref (gc);
}


static void
on_revise_skim_readword_ckbutton_clicked ()
{
}

static void
on_revise_skim_meaning_ckbutton_clicked ()
{
}

static void
on_revise_skim_animate_ckbutton_clicked ()
{
}

static void
on_revise_skim_readword_button_clicked (gpointer data)
{
	readword(g_pReciteWord->revise_skim->viewer.str1);
}

static void on_revise_skim_pause_clicked ();
static void on_revise_skim_start_clicked();

static void 
on_revise_skim_chooseword_ok_clicked ()
{
	if (g_pReciteWord->revise_skim->start_pause_button_status==0)
	{
		g_pReciteWord->revise_skim->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_skim->start_pause_button_status==1)
	{
		g_pReciteWord->revise_skim->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_skim->start_pause_button_status==2)
	{
		g_pReciteWord->revise_skim->start_button.set_enable(true);
	}	
	if (!g_pReciteWord->revise_skim->start_button.enable)
		on_revise_skim_pause_clicked();  //continue
	if (g_pReciteWord->revise_skim->started)
		on_revise_skim_start_clicked();  //stop
	g_pReciteWord->revise_skim->viewer.set_words(g_pReciteWord->now_zu_words.w,
		       g_pReciteWord->now_zu_words.m,
		       g_pReciteWord->now_zu_wordcount);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	gtk_label_set_text(GTK_LABEL(g_pReciteWord->revise_skim->word_count_label),aa);
	on_revise_skim_start_clicked();  //start
	g_pReciteWord->chooseword->close ();
	g_pReciteWord->revise_skim->chooseword_button.set_enable(true);
}

static void 
on_revise_skim_chooseword_cancel_clicked ()
{
	g_pReciteWord->chooseword->close ();
	if (g_pReciteWord->revise_skim->start_pause_button_status==0)
	{
		g_pReciteWord->revise_skim->pause_button.set_enable(true);
		on_revise_skim_pause_clicked();  //continue
	}
	else if (g_pReciteWord->revise_skim->start_pause_button_status==1)
	{
		g_pReciteWord->revise_skim->pause_button.set_enable(true);
	}
	else if (g_pReciteWord->revise_skim->start_pause_button_status==2)
	{
		g_pReciteWord->revise_skim->start_button.set_enable(true);
	}
	g_pReciteWord->revise_skim->chooseword_button.set_enable(true);
}

static void
on_revise_skim_chooseword_button_clicked (gpointer data)
{
	if (!g_pReciteWord->revise_skim->start_button.enable)
	{
		g_pReciteWord->revise_skim->start_pause_button_status=1;
		g_pReciteWord->revise_skim->pause_button.set_enable(false);
	}
	else if (!g_pReciteWord->revise_skim->pause_button.enable)
	{
		g_pReciteWord->revise_skim->start_pause_button_status=2;
		g_pReciteWord->revise_skim->start_button.set_enable(false);
	}
	else
	{
		g_pReciteWord->revise_skim->start_pause_button_status=0;
		on_revise_skim_pause_clicked();  //pause
		g_pReciteWord->revise_skim->pause_button.set_enable(false);
	}
	
	g_pReciteWord->PopupChooseword (on_revise_skim_chooseword_ok_clicked,
					on_revise_skim_chooseword_cancel_clicked,&(g_pReciteWord->revise_skim_chooseword_info));
	g_pReciteWord->revise_skim->chooseword_button.set_enable(false);
}

static void
on_revise_skim_over ()
{
	g_pReciteWord->revise_skim->started = FALSE;
	g_pReciteWord->revise_skim->pause_button.set_enable (FALSE);
	g_pReciteWord->revise_skim->start_button.reset (Skin->revise_skim.
							start_button.p[0],
							Skin->revise_skim.
							start_button.p[1],
							Skin->revise_skim.
							start_button.p[2],
							(GdkPixmap *) NULL);
	if (g_pReciteWord->now_zu_index >= 0) // come in shooting by chooseword's by_group
	{
		g_pReciteWord->revise_skim_chooseword_info.by = 0; // by group next time.
		g_pReciteWord->revise_skim_chooseword_info.zu_index = g_pReciteWord->now_zu_index+1;
	}
	else // now_zu_index == -1,it must be set by chooseword dialog's by_scope.
	{
		g_pReciteWord->revise_skim_chooseword_info.by = 1; // by scope
		g_pReciteWord->revise_skim_chooseword_info.scope_start = g_pReciteWord->chooseword->last_scope_start + g_pReciteWord->chooseword->last_scope_count;
		g_pReciteWord->revise_skim_chooseword_info.scope_count = g_pReciteWord->chooseword->last_scope_count;
	}
}

static void
on_revise_skim_start_clicked ()
{
	if (g_pReciteWord->revise_skim->started)
	{
		g_pReciteWord->revise_skim->started = FALSE;
		g_pReciteWord->revise_skim->pause_button.set_enable (FALSE);
		g_pReciteWord->revise_skim->start_button.reset (Skin->
								revise_skim.
								start_button.
								p[0],
								Skin->
								revise_skim.
								start_button.
								p[1],
								Skin->
								revise_skim.
								start_button.
								p[2],
								(GdkPixmap *)
								NULL);
		g_pReciteWord->revise_skim->viewer.stop_skim ();
	}
	else
	{
		g_pReciteWord->revise_skim->pause_button.set_enable (TRUE);
		g_pReciteWord->revise_skim->start_button.reset (Skin->
								revise_skim.
								stop_button.
								p[0],
								Skin->
								revise_skim.
								stop_button.
								p[1],
								Skin->
								revise_skim.
								stop_button.
								p[2],
								Skin->
								revise_skim.
								stop_button.
								p[3]);
		g_pReciteWord->revise_skim->started = TRUE;

		g_pReciteWord->revise_skim->viewer.start_skim ();
	}
}

static void
on_revise_skim_pause_clicked ()
{
	if (g_pReciteWord->revise_skim->started)
	{
		g_pReciteWord->revise_skim->started = FALSE;
		g_pReciteWord->revise_skim->start_button.set_enable (FALSE);
		g_pReciteWord->revise_skim->pause_button.reset (Skin->
								revise_skim.
								continue_button.
								p[0],
								Skin->
								revise_skim.
								continue_button.
								p[1],
								Skin->
								revise_skim.
								continue_button.
								p[2],
								(GdkPixmap *)
								NULL);
		g_pReciteWord->revise_skim->viewer.pause_skim ();
	}
	else
	{
		g_pReciteWord->revise_skim->start_button.set_enable (TRUE);
		g_pReciteWord->revise_skim->pause_button.reset (Skin->
								revise_skim.
								pause_button.
								p[0],
								Skin->
								revise_skim.
								pause_button.
								p[1],
								Skin->
								revise_skim.
								pause_button.
								p[2],
								Skin->
								revise_skim.
								pause_button.
								p[3]);
		g_pReciteWord->revise_skim->started = TRUE;

		g_pReciteWord->revise_skim->viewer.continue_skim ();
	}
}

static void
on_revise_skim_group_clicked ()
{
	g_pReciteWord->revise_skim->close ();
	g_pReciteWord->ShowRevise_group ();
}

static void
on_revise_skim_test_clicked ()
{
	g_pReciteWord->revise_skim->close ();
	g_pReciteWord->ShowRevise_test ();
}

static void
on_revise_skim_return_clicked ()
{
	g_pReciteWord->revise_skim->close ();
	g_pReciteWord->show ();
}

CRevise_skim::CRevise_skim ()
{
	skin_load_revise_skim (g_pReciteWord->window);
}

CRevise_skim::~CRevise_skim ()
{
}

void
CRevise_skim::init ()
{
	g_pReciteWord->status = STATUS_REVISE_SKIM;
	started = FALSE;
}

void
CRevise_skim::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->revise_skim.revise_skim.p[0]);

	viewer.create (g_pReciteWord->fixed, Skin->revise_skim.viewer.x[0],
		       Skin->revise_skim.viewer.y[0], Skin->revise_skim.viewer.w,
		       Skin->revise_skim.viewer.h,
		       g_pReciteWord->now_zu_words.w,
		       g_pReciteWord->now_zu_words.m,
		       g_pReciteWord->now_zu_wordcount, on_revise_skim_over);

	start_button.create (g_pReciteWord->fixed,
			     Skin->revise_skim.stop_button.x[0],
			     Skin->revise_skim.stop_button.y[0],
			     Skin->revise_skim.stop_button.p[0],
			     Skin->revise_skim.stop_button.p[1],
			     Skin->revise_skim.stop_button.p[2],
			     (GdkPixmap *) NULL,
			     on_revise_skim_start_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), start_button.event_box,"Ctrl-S","");
	pause_button.create (g_pReciteWord->fixed,
			     Skin->revise_skim.pause_button.x[0],
			     Skin->revise_skim.pause_button.y[0],
			     Skin->revise_skim.pause_button.p[0],
			     Skin->revise_skim.pause_button.p[1],
			     Skin->revise_skim.pause_button.p[2],
			     Skin->revise_skim.pause_button.p[3],
			     on_revise_skim_pause_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), pause_button.event_box,"Ctrl-P","");
	group_button.create (g_pReciteWord->fixed,
			     Skin->revise_skim.group_button.x[0],
			     Skin->revise_skim.group_button.y[0],
			     Skin->revise_skim.group_button.p[0],
			     Skin->revise_skim.group_button.p[1],
			     Skin->revise_skim.group_button.p[2],
			     (GdkPixmap *) NULL,
			     on_revise_skim_group_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), group_button.event_box,"Ctrl-C","");
	test_button.create (g_pReciteWord->fixed,
			    Skin->revise_skim.test_button.x[0],
			    Skin->revise_skim.test_button.y[0],
			    Skin->revise_skim.test_button.p[0],
			    Skin->revise_skim.test_button.p[1],
			    Skin->revise_skim.test_button.p[2],
			    (GdkPixmap *) NULL, on_revise_skim_test_clicked);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), test_button.event_box,"Ctrl-B","");
	return_button.create (g_pReciteWord->fixed,
			      Skin->revise_skim.return_button.x[0],
			      Skin->revise_skim.return_button.y[0],
			      Skin->revise_skim.return_button.p[0],
			      Skin->revise_skim.return_button.p[1],
			      Skin->revise_skim.return_button.p[2],
			      (GdkPixmap *) NULL,
			      on_revise_skim_return_clicked);
	word_index_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_index_label,
		       Skin->revise_skim.word_index_label.x[0],
		       Skin->revise_skim.word_index_label.y[0]);
	gtk_widget_show (word_index_label);
	gchar aa[5];
	sprintf(aa,"%2d",g_pReciteWord->now_zu_wordcount);
	word_count_label = gtk_label_new (aa);
	gtk_fixed_put (GTK_FIXED (g_pReciteWord->fixed), word_count_label,
		       Skin->revise_skim.word_count_label.x[0],
		       Skin->revise_skim.word_count_label.y[0]);
	gtk_widget_show (word_count_label);
	readword_ckbutton.create (g_pReciteWord->fixed, Skin->revise_skim.readword_ckbutton.x[0],
			      Skin->revise_skim.readword_ckbutton.y[0],
			      Skin->revise_skim.readword_ckbutton.p[0],
			      Skin->revise_skim.readword_ckbutton.p[1],
			      Skin->revise_skim.readword_ckbutton.p[2],
				  Skin->revise_skim.readword_ckbutton.p[3],
			      on_revise_skim_readword_ckbutton_clicked);
   	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "readword",
			     &tmp_b);
	readword_ckbutton.set_status(tmp_b);
	meaning_ckbutton.create (g_pReciteWord->fixed, Skin->revise_skim.meaning_ckbutton.x[0],
			      Skin->revise_skim.meaning_ckbutton.y[0],
			      Skin->revise_skim.meaning_ckbutton.p[0],
			      Skin->revise_skim.meaning_ckbutton.p[1],
			      Skin->revise_skim.meaning_ckbutton.p[2],
				  Skin->revise_skim.meaning_ckbutton.p[3],
			      on_revise_skim_meaning_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "showmean",
			     &tmp_b);
	meaning_ckbutton.set_status(tmp_b);
	animate_ckbutton.create (g_pReciteWord->fixed, Skin->revise_skim.animate_ckbutton.x[0],
			      Skin->revise_skim.animate_ckbutton.y[0],
			      Skin->revise_skim.animate_ckbutton.p[0],
			      Skin->revise_skim.animate_ckbutton.p[1],
			      Skin->revise_skim.animate_ckbutton.p[2],
				  Skin->revise_skim.animate_ckbutton.p[3],
			      on_revise_skim_animate_ckbutton_clicked);
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "animate",
			     &tmp_b);
	animate_ckbutton.set_status(tmp_b);
	readword_button.create (g_pReciteWord->fixed, Skin->revise_skim.readword_button.x[0],
			      Skin->revise_skim.readword_button.y[0],
			      Skin->revise_skim.readword_button.p[0],
			      Skin->revise_skim.readword_button.p[1],
			      Skin->revise_skim.readword_button.p[2],
			      on_revise_skim_readword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), readword_button.event_box,"Tab","");
	chooseword_button.create (g_pReciteWord->fixed, Skin->revise_skim.chooseword_button.x[0],
			      Skin->revise_skim.chooseword_button.y[0],
			      Skin->revise_skim.chooseword_button.p[0],
			      Skin->revise_skim.chooseword_button.p[1],
			      Skin->revise_skim.chooseword_button.p[2],
			      on_revise_skim_chooseword_button_clicked,this);
	gtk_tooltips_set_tip (GTK_TOOLTIPS (g_pReciteWord->tooltips), chooseword_button.event_box,"Ctrl-W","");
	on_revise_skim_start_clicked ();
}

void
CRevise_skim::close ()
{
	if (g_pReciteWord->chooseword && g_pReciteWord->chooseword->fixed)
		g_pReciteWord->chooseword->close ();

	gboolean tmp_b;
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "readword",
			     &tmp_b);
	if (tmp_b!=readword_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "revise_skim", "readword",
			     readword_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "showmean",
			     &tmp_b);
	if (tmp_b!=meaning_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "revise_skim", "showmean",
			     readword_ckbutton.status);
	}
	rw_cfg_read_boolean(usercfgfile,
			     "revise_skim", "animate",
			     &tmp_b);
	if (tmp_b!=animate_ckbutton.status)
	{
		rw_cfg_write_boolean(usercfgfile,
			     "revise_skim", "animate",
			     animate_ckbutton.status);
	}

	viewer.destroy ();

	start_button.destroy ();
	pause_button.destroy ();
	group_button.destroy ();
	test_button.destroy ();
	return_button.destroy ();
	gtk_widget_destroy (word_index_label);
	gtk_widget_destroy (word_count_label);
	readword_ckbutton.destroy ();
	meaning_ckbutton.destroy ();
	animate_ckbutton.destroy ();
	readword_button.destroy ();
	chooseword_button.destroy ();
}
