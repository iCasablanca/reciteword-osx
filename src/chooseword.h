#ifndef __RW_CHOOSEWORD_H__
#define __RW_CHOOSEWORD_H__

#include <gtk/gtk.h>
#include "widget.h"

class CChooseword
{
      public:

	GtkWidget * fixed;
	tPixButton ok_button;
	tPixButton cancel_button;

	gint total_zucount;
	gint scope_start, scope_count;
	gint last_scope_start, last_scope_count; //only change when clicked ok button.
	gint tip_timer;
	GtkWidget *group_entry, *group_s_label, *scope_entry, *scope_label,
		*scopecount_entry, *scopecount_label, *scope_s_label;
	GtkWidget *noclass_tip, *scopeerr_tip;
	tRadioButton bygroup_robutton;
	tRadioButton byscope_robutton;
	tRadioButton byclass_robutton;

	tCheckButton order_ckbutton;

	void (*func1) ();
	void (*func2) ();

	  CChooseword ();
	 ~CChooseword ();
	void init ();
	void show (void (*ok_func) (), void (*cancel_func) (), struct chooseword_info *info);
	void show_tip (gint type);
	void close ();
	void do_up();
	void do_down();
};


#endif
