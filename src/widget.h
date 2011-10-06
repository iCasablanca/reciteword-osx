#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <gtk/gtk.h>

#include "sound.h"

struct tPixButton
{
	GtkWidget *event_box;
	GtkWidget *pixmap_widget;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_down;
	GdkPixmap *pix_disable;
	int mousein;
	int enable;
	void (*runfunc) (gpointer);
	gpointer funcdata;

	  tPixButton ();
	void init ();
	void create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		     GdkPixmap * pix_2, GdkPixmap * pix_3,
		     void (*func) (gpointer), gpointer funcdata);
	void destroy ();
	void set_enable (int);
	void do_clicked(gboolean playsnd=true);
	 ~tPixButton ();
};

struct tPixButton2
{
	GtkWidget *event_box;
	GtkWidget *pixmap_widget;
	GdkPixmap *pix_normal;
	GdkPixmap *pix_active;
	GdkPixmap *pix_down;
	GdkPixmap *pix_disable;
	int mousein;
	int enable;
	gboolean pressed;
	void (*runfunc) ();

	  tPixButton2 ();
	void init ();
	void create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		     GdkPixmap * pix_2, GdkPixmap * pix_3, GdkPixmap * pix_4,
		     void (*func) ());
	void reset (GdkPixmap * pix_1, GdkPixmap * pix_2, GdkPixmap * pix_3,
		    GdkPixmap * pix_4);
	void destroy ();
	void set_enable (int e);
	void do_clicked(gboolean playsnd=true);
	 ~tPixButton2 ();
};

struct tCheckButton
{
	GtkWidget *event_box;
	GtkWidget *pixmap_widget;
	GdkPixmap *pix_t_normal;
	GdkPixmap *pix_t_down;
	GdkPixmap *pix_f_normal;
	GdkPixmap *pix_f_down;
	int mousein;
	gboolean enable;
	int status;
	void (*runfunc) ();

	  tCheckButton ();
	void init ();
	void create (GtkWidget * fixed, gint x, gint y, GdkPixmap * pix_1,
		     GdkPixmap * pix_2, GdkPixmap * pix_3, GdkPixmap * pix_4,
		     void (*func) ());
	void set_status (int newstatus);
	void set_enable (gboolean e);
	void destroy ();
	 ~tCheckButton ();
};

struct tRadioButton
{
	GtkWidget *event_box;
	GtkWidget *pixmap_widget;
	GdkPixmap *pix_t_normal;
	GdkPixmap *pix_t_down;
	GdkPixmap *pix_f_normal;
	GdkPixmap *pix_f_down;
	int mousein;
	int selected;
	GSList *group;
	void (*runfunc) (const gpointer);
	gpointer funcdata;

	  tRadioButton ();
	void init ();
	void create (GSList * group, GtkWidget * fixed, gint x, gint y,
		     GdkPixmap * pix_1, GdkPixmap * pix_2, GdkPixmap * pix_3,
		     GdkPixmap * pix_4, void (*func) (const gpointer),const gpointer funcdata);
	void set_selected (int a);
	void do_clicked();
	void set_group (GSList * group);
	void destroy ();
	 ~tRadioButton ();
};

struct tEntry
{
	GtkWidget *text_area;
	GdkPixmap *draw_pixmap;
	GdkGC *gc1, *gc2;
	gchar text[256];
	PangoLayout *layout;
	gint width, height;
	gint max_length;
	guint position;
	gint editable;
	gint show_length;
	gint underline_w;
	gint show_cursor;
	gint show_pen;
	gboolean enablesnd;
	guint32 timer;
	  gint (*runfunc) (gint, gint);

	  tEntry ();
	void init ();
	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		     gchar * str, gchar * right_text, gint show_pen0,
		     gint (*func) (gint, gint));
	void draw ();
	void draw_cursor (gint a);
	void changed (gchar * str, guint pos, gint keyval);
	void set_text (gchar * str);
	void set_showlength (gint a);
	void set_righttext (gchar * right_text);
	void set_showpen (gint a);
	void set_editable (gint a);
	void check_text (gchar * str,SOUND_EVENTS right_snd_event,SOUND_EVENTS wrong_snd_event);
	gchar *get_text ();
	void destroy ();
	 ~tEntry ();
};

/*
struct tPhoneticLabel
{
	GtkWidget *text_area;
	GdkPixmap *draw_pixmap;
	gchar text[256];
	gint width, height;
	struct _Skin_phonetic *ptc;

	void create (GtkWidget * pfixed, gint x, gint y, gint w, gint h,
		     struct _Skin_phonetic *phonetic);
	void draw ();
	void set_text (gchar * str);
	void destroy ();

	  tPhoneticLabel ();
	 ~tPhoneticLabel ();
};
*/

class CPhoneticLabel
{
	gchar text[256];
	struct _Skin_phonetic *ptc;

	void refresh ();
	  public:
	GtkWidget *hbox;
	void create (GtkWidget * pfixed, gint x, gint y, struct _Skin_phonetic *phonetic);
	void set_text (gchar * str);
	void destroy ();

	  CPhoneticLabel ();
	 ~CPhoneticLabel ();
};

#endif
