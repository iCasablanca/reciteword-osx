#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "option.h"
#include "reciteword.h"
#include "configfile.h"
#include "skin.h"
#include <glib/gi18n.h>

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;
extern ConfigFile *usercfgfile;


#define LOGO			0
#define	SKINS_SETTINGS 	1
#define FONTS_SETTINGS		2
#define FIRSTRECITE_SKIM_SETTINGS		3
#define FIRSTRECITE_TEST_SETTINGS		4
#define REVISE_GROUP_SETTINGS		5
#define REVISE_TEST_SETTINGS		6
#define REVISE_SKIM_SETTINGS		7
#define MISC_SOUND_SETTINGS	8

enum
{
	CATEGORY_COLUMN = 0,
	PAGE_NUM_COLUMN,
	NUM_COLUMNS
};

typedef struct _CategoriesTreeItem	CategoriesTreeItem;

struct _CategoriesTreeItem
{
	gchar			*category;
	
	CategoriesTreeItem 	*children;

	gint			notebook_page;
};

static CategoriesTreeItem rw_behavior [] =
{
	{N_("Skins"), NULL, SKINS_SETTINGS},

	{N_("Fonts"), NULL, FONTS_SETTINGS},

	{ NULL }
};

static CategoriesTreeItem form_behavior [] =
{
	{N_("Firstrecite_skim"), NULL, FIRSTRECITE_SKIM_SETTINGS},
	
	{N_("Firstrecite_test"), NULL, FIRSTRECITE_TEST_SETTINGS},
	
	{N_("Revise_Group"), NULL, REVISE_GROUP_SETTINGS},

	{N_("Revise_Test"), NULL, REVISE_TEST_SETTINGS},

	{N_("Revise_Skim"), NULL, REVISE_SKIM_SETTINGS},

	{ NULL }
};

static CategoriesTreeItem misc_behavior [] =
{
	{N_("Sound"), NULL, MISC_SOUND_SETTINGS},

	{ NULL }
};

static CategoriesTreeItem toplevel [] =
{
	{N_("ReciteWord"), rw_behavior, LOGO},
	{N_("Form"), form_behavior, LOGO},
	{N_("Misc"), misc_behavior, LOGO},	

	{ NULL }
};

static gint last_selected_page_num = SKINS_SETTINGS;

static gint
on_window_delete_event (GtkWidget * window, GdkEvent *event, COption * parent)
{
	parent->close();
	return TRUE;
}

static void
rw_preferences_dialog_response_handler (GtkDialog *dialog, gint res_id,  
		COption *parent)
{
	switch (res_id) 
	{
		default:
			parent->close();
	}
}

static GtkTreeModel*
rw_preferences_dialog_create_categories_tree_model ()
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	CategoriesTreeItem *category = toplevel;

      	/* create tree store */
	model = gtk_tree_store_new (NUM_COLUMNS,
			      G_TYPE_STRING,
			      G_TYPE_INT);
  
	/* add data to the tree store */		
	while (category->category)
    	{
      		CategoriesTreeItem *sub_category = category->children;
		
		gtk_tree_store_append (model, &iter, NULL);
		
		gtk_tree_store_set (model, &iter,
			  CATEGORY_COLUMN, gettext (category->category),
			  PAGE_NUM_COLUMN, category->notebook_page,
			  -1);

		/* add children */
		while (sub_category->category)
		{
	  		GtkTreeIter child_iter;
	  
	  		gtk_tree_store_append (model, &child_iter, &iter);
	  
			gtk_tree_store_set (model, &child_iter,
				CATEGORY_COLUMN, gettext (sub_category->category),
				PAGE_NUM_COLUMN, sub_category->notebook_page,
			      -1);

	  		sub_category++;
		}
      
		category++;
	}	

	return GTK_TREE_MODEL (model);	
}

static void
rw_preferences_dialog_categories_tree_selection_cb (GtkTreeSelection *selection, 
		COption *parent)
{
 	GtkTreeIter iter;
	GValue value = {0, };

	if (! gtk_tree_selection_get_selected (selection, NULL, &iter))
		return;

	gtk_tree_model_get_value (parent->categories_tree_model, &iter,
			    PAGE_NUM_COLUMN,
			    &value);

	last_selected_page_num = g_value_get_int (&value);

	if (parent->notebook != NULL)
		gtk_notebook_set_current_page (GTK_NOTEBOOK (parent->notebook), 
					       last_selected_page_num);
      	
	g_value_unset (&value);
}

static gboolean 
rw_preferences_dialog_selection_init (
		GtkTreeModel *model, 
		GtkTreePath *path, 
		GtkTreeIter *iter, 
		COption *parent)
{
	GValue value = {0, };
	gint page_num;

	gtk_tree_model_get_value (parent->categories_tree_model, iter,
			    PAGE_NUM_COLUMN,
			    &value);

	page_num = g_value_get_int (&value);

	g_value_unset (&value);

	if (page_num == last_selected_page_num)
	{
		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (parent->categories_tree));

		gtk_tree_selection_select_iter (selection, iter);

		gtk_notebook_set_current_page (GTK_NOTEBOOK (parent->notebook), page_num);
    	
		return TRUE;
	}
	return FALSE;
}

static void
rw_preferences_dialog_categories_tree_realize (GtkWidget		 *widget, 
						  COption *parent)
{
	gtk_tree_view_expand_all (GTK_TREE_VIEW (widget));

	gtk_tree_model_foreach (parent->categories_tree_model, 
			(GtkTreeModelForeachFunc) rw_preferences_dialog_selection_init,
			(gpointer)parent);
}

static GtkWidget*
rw_preferences_dialog_create_categories_tree (COption *parent)
{
	GtkWidget *sw;
	GtkTreeModel *model;
	GtkWidget *treeview;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtkTreeViewColumn *column;
 	gint col_offset;
	
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
					   GTK_SHADOW_ETCHED_IN);
      	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				      GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
	
	gtk_widget_set_size_request (sw, 160, 240);
	
	model = rw_preferences_dialog_create_categories_tree_model ();
	
	treeview = gtk_tree_view_new_with_model (model);
	
	parent->categories_tree = treeview;
	parent->categories_tree_model = model;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	gtk_tree_selection_set_mode (selection,
				   GTK_SELECTION_SINGLE);

	 /* add column for category */
	renderer = gtk_cell_renderer_text_new ();
  	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
  
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
							    -1, _("Categories"),
							    renderer, "text",
							    CATEGORY_COLUMN,
							    NULL);
	
  	column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  	gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), FALSE);

	g_signal_connect (selection, "changed", 
			  G_CALLBACK (rw_preferences_dialog_categories_tree_selection_cb), 
			  parent);

	gtk_container_add (GTK_CONTAINER (sw), treeview);

      	g_signal_connect (G_OBJECT (treeview), "realize", 
			  G_CALLBACK (rw_preferences_dialog_categories_tree_realize), 
			  parent);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
		
	return sw;
}

static gboolean 
rw_preferences_dialog_setup_logo_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *image;
	image = gtk_image_new_from_pixbuf(Skin->face.icon.p[0]);
	gtk_notebook_append_page(nb,image,NULL);
	return true;
}

static void
on_setup_skins_radio_toggled(GtkWidget *widget, gpointer data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	{
		gchar *ab;
		ab = (gchar *)g_object_get_data(G_OBJECT(widget),"skin_dir");
		rw_cfg_write_string (usercfgfile, "reciteword", "skin", ab);
	}
}

static gboolean 
rw_preferences_dialog_setup_skins_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *vbox1, *label, *radio=NULL;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("Choose the skin.You need to restart the program\nto get effect.\n\nSkin:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	vbox1 = gtk_vbox_new(false,6);
	gchar *ab;
	if (!(rw_cfg_read_string (usercfgfile, "reciteword", "skin", &ab)))
		ab=g_strdup("bdc");
	GDir *dp;
	gchar *filename;
	filename = g_build_filename(reciteword_data_dir, "skins", NULL);
	dp = g_dir_open(filename,0,NULL);
	g_free(filename);
	if (dp)
	{
		const gchar *item;
		gchar *fullname;
		while ((item = g_dir_read_name(dp)) != NULL)
		{
			fullname = g_build_filename(reciteword_data_dir, "skins", item, NULL);			
			if (g_file_test(fullname,G_FILE_TEST_IS_DIR))
			{
				g_free(fullname);
				fullname = g_build_filename(reciteword_data_dir, "skins", item, "skin.inf", NULL);				
				if (g_file_test(fullname,G_FILE_TEST_EXISTS))
				{
					if (!radio) // first radio button.
						radio = gtk_radio_button_new_with_label(NULL,item);
					else
						radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (radio),item);
					g_object_set_data_full(G_OBJECT(radio),"skin_dir",g_strdup(item),g_free);
					gtk_box_pack_start(GTK_BOX(vbox1),radio,false,false,0);
					if (strcmp(ab,item)==0)
						gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio),true);		
					g_signal_connect (G_OBJECT (radio), "toggled", G_CALLBACK (on_setup_skins_radio_toggled), NULL);
				}
				g_free(fullname);
			}
			else
				g_free(fullname);
		}
		g_dir_close(dp);
	}
	g_free(ab);
	gtk_box_pack_start(GTK_BOX(vbox),vbox1,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_fonts_ckbutton_toggled(GtkWidget *widget, COption *parent)
{
	gboolean b = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
	gtk_widget_set_sensitive(parent->font_table,b);
	rw_cfg_write_boolean (usercfgfile, "reciteword", "use_custom_font", b);
}

static void
on_setup_fonts_button_clicked(GtkWidget *widget, COption *parent)
{
	GtkWidget *dlg;
	gint result;
	gchar *font_name;
	if (widget==parent->font_button1)
	{
		dlg = gtk_font_selection_dialog_new(_("Choose the normal english font"));
		gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (parent->window));
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dlg),gtk_button_get_label(GTK_BUTTON(widget)));
		gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(dlg),"Normal english font");
		result = gtk_dialog_run (GTK_DIALOG (dlg));
		switch (result)
		{
		case GTK_RESPONSE_OK:
			font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dlg));
			if (font_name)
			{
				gtk_button_set_label(GTK_BUTTON(widget),font_name);
				rw_cfg_write_string (usercfgfile, "reciteword", "normal_english_font", font_name);
			}
			break;
		default:
			break;
		}
		gtk_widget_destroy (dlg);
	}
	else if (widget==parent->font_button2)
	{
		dlg = gtk_font_selection_dialog_new(_("Choose the big english font"));
		gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (parent->window));
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dlg),gtk_button_get_label(GTK_BUTTON(widget)));
		gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(dlg),"Big english font");
		result = gtk_dialog_run (GTK_DIALOG (dlg));
		switch (result)
		{
		case GTK_RESPONSE_OK:
			font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dlg));
			if (font_name)
			{
				gtk_button_set_label(GTK_BUTTON(widget),font_name);
				rw_cfg_write_string (usercfgfile, "reciteword", "big_english_font", font_name);
			}
			break;
		default:
			break;
		}
		gtk_widget_destroy (dlg);
	}
	else if (widget==parent->font_button3)
	{
		dlg = gtk_font_selection_dialog_new(_("Choose the local language font"));
		gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (parent->window));
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dlg),gtk_button_get_label(GTK_BUTTON(widget)));
		gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(dlg),_("Local language font"));
		result = gtk_dialog_run (GTK_DIALOG (dlg));
		switch (result)
		{
		case GTK_RESPONSE_OK:
			font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dlg));
			if (font_name)
			{
				gtk_button_set_label(GTK_BUTTON(widget),font_name);
				rw_cfg_write_string (usercfgfile, "reciteword", "local_language_font", font_name);
			}
			break;
		default:
			break;
		}
		gtk_widget_destroy (dlg);
	}
}

static gboolean 
rw_preferences_dialog_setup_fonts_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *vbox1, *vbox2, *hbox, *label, *check_button;
	vbox = gtk_vbox_new(false,18);
		label=gtk_label_new(_("Seting the fonts.You need to restart the program\nto get effect."));
		gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
		gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
		vbox1 = gtk_vbox_new(false,6);
			label=gtk_label_new(_("Font:"));
			gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
			gtk_box_pack_start(GTK_BOX(vbox1),label,false,false,0);
			hbox=gtk_hbox_new(false,0);
				label=gtk_label_new("    ");
				gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
				gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
				vbox2=gtk_vbox_new(false,6);
					check_button=gtk_check_button_new_with_label(_("Use Custom font."));
					gboolean use_custom_font;
					rw_cfg_read_boolean (usercfgfile, "reciteword", "use_custom_font", &use_custom_font);
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button),use_custom_font);
					g_signal_connect (G_OBJECT (check_button), "toggled", G_CALLBACK (on_setup_fonts_ckbutton_toggled), (gpointer)parent);
					gtk_box_pack_start(GTK_BOX(vbox2),check_button,false,false,0);
					parent->font_table=gtk_table_new(2,3,false);
					gtk_widget_set_sensitive(parent->font_table,use_custom_font);
					gtk_table_set_row_spacings(GTK_TABLE(parent->font_table),6);
					gtk_table_set_col_spacings(GTK_TABLE(parent->font_table),12);
						label=gtk_label_new(_("Normal english font:"));
						gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),label,0,1,0,1);
						gchar *bb;
						if (rw_cfg_read_string (usercfgfile, "reciteword", "normal_english_font", &bb))
						{
							parent->font_button1=gtk_button_new_with_label(bb);
							g_free(bb);
						}
						else
							parent->font_button1=gtk_button_new_with_label(_("Choose"));
						g_signal_connect (G_OBJECT (parent->font_button1), "clicked", G_CALLBACK (on_setup_fonts_button_clicked), (gpointer)parent);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),parent->font_button1,1,2,0,1);
						label=gtk_label_new(_("Big english font:"));
						gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),label,0,1,1,2);
						if (rw_cfg_read_string (usercfgfile, "reciteword", "big_english_font", &bb))
						{
							parent->font_button2=gtk_button_new_with_label(bb);
							g_free(bb);
						}
						else
							parent->font_button2=gtk_button_new_with_label(_("Choose"));
						g_signal_connect (G_OBJECT (parent->font_button2), "clicked", G_CALLBACK (on_setup_fonts_button_clicked), (gpointer)parent);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),parent->font_button2,1,2,1,2);
						label=gtk_label_new(_("Local language's font:"));
						gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),label,0,1,2,3);
						if (rw_cfg_read_string (usercfgfile, "reciteword", "local_language_font", &bb))
						{
							parent->font_button3=gtk_button_new_with_label(bb);
							g_free(bb);
						}
						else
							parent->font_button3=gtk_button_new_with_label(_("Choose"));
						g_signal_connect (G_OBJECT (parent->font_button3), "clicked", G_CALLBACK (on_setup_fonts_button_clicked), (gpointer)parent);
						gtk_table_attach_defaults(GTK_TABLE(parent->font_table),parent->font_button3,1,2,2,3);
					gtk_box_pack_start(GTK_BOX(vbox2),parent->font_table,false,false,0);
				gtk_box_pack_start(GTK_BOX(hbox),vbox2,false,false,0);
			gtk_box_pack_start(GTK_BOX(vbox1),hbox,false,false,0);
		gtk_box_pack_start(GTK_BOX(vbox),vbox1,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_revise_group_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "revise_group", "max_revise_times", i);
	if (g_pReciteWord->revise_group)
		g_pReciteWord->revise_group->max_revise_times = i;
}

static gboolean 
rw_preferences_dialog_setup_revise_group_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *hbox, *label, *spin_button;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("The revise_group's settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	hbox= gtk_hbox_new(false,6);
	label=gtk_label_new(_("Revise"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gint i;
	rw_cfg_read_int (usercfgfile, "revise_group", "max_revise_times", &i);
	if (i==0)
		i=6;
	spin_button = gtk_spin_button_new_with_range(1,10,1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_revise_group_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("times for echo group.(default:6)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_firstrecite_skim_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "firstrecite_skim", "wait_time", i);
	if (g_pReciteWord->firstrecite_skim)
	{
		g_pReciteWord->firstrecite_skim->wait_time = i;
	}
}

static gboolean 
rw_preferences_dialog_setup_firstrecite_skim_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *hbox, *label, *spin_button;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("The Firstrecite_skim's settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	hbox= gtk_hbox_new(false,6);
	label=gtk_label_new(_("Wait"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gint i;
	rw_cfg_read_int (usercfgfile, "firstrecite_skim", "wait_time", &i);
	if (i<=0)
		i=4000;
	spin_button = gtk_spin_button_new_with_range(1000,20000,500);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_firstrecite_skim_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("milliseconds for echo word.(default:4000)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_firstrecite_test_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "firstrecite_test", "headchar_len", i);
	if (g_pReciteWord->firstrecite_test)
	{
		g_pReciteWord->firstrecite_test->headchar_len = i;
		if (g_pReciteWord->firstrecite_test->headchar_label)
		{
			gchar headchar[6]; //i should between 1 and 5.
			strncpy(headchar,g_pReciteWord->now_zu_words.w[g_pReciteWord->firstrecite_test->selected_wordlist_item],i);
			headchar[i]='\0';
			gtk_label_set_text (GTK_LABEL (g_pReciteWord->firstrecite_test->headchar_label),headchar);
		}
	}
}

static gboolean 
rw_preferences_dialog_setup_firstrecite_test_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *hbox, *label, *spin_button;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("The Firstrecite_test's settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	hbox= gtk_hbox_new(false,6);
	label=gtk_label_new(_("Show"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gint i;
	rw_cfg_read_int (usercfgfile, "firstrecite_test", "headchar_len", &i);
	if (i<=0)
		i=2;
	spin_button = gtk_spin_button_new_with_range(1,5,1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_firstrecite_test_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("chars as the head-char.(default:2)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_revise_test_ckbutton_toggled(GtkWidget *widget, COption *parent)
{
	gboolean b = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
	rw_cfg_write_boolean (usercfgfile, "revise_test", "readword_when_answerright", b);
	if (g_pReciteWord->revise_test)
		g_pReciteWord->revise_test->readword_when_answerright = b;
}

static void
on_setup_revise_test_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "revise_test", "headchar_len", i);
	if (g_pReciteWord->revise_test)
	{
		g_pReciteWord->revise_test->headchar_len = i;
		if (g_pReciteWord->revise_test->headchar_label)
		{
			gchar headchar[6]; //i should between 1 and 5.
			strncpy(headchar,g_pReciteWord->now_zu_words.w[g_pReciteWord->revise_test->selected_wordlist_item],i);
			headchar[i]='\0';
			gtk_label_set_text (GTK_LABEL (g_pReciteWord->revise_test->headchar_label),headchar);
		}
	}
}

static gboolean 
rw_preferences_dialog_setup_revise_test_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *hbox, *label, *check_button, *spin_button;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("The revise_test's settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	check_button = gtk_check_button_new_with_label(_("Read word when answer is right."));
	gboolean b;
	rw_cfg_read_boolean (usercfgfile, "revise_test", "readword_when_answerright", &b);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button),b);
	g_signal_connect (G_OBJECT (check_button), "toggled", G_CALLBACK (on_setup_revise_test_ckbutton_toggled), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(vbox),check_button,false,false,0);
	hbox= gtk_hbox_new(false,6);
	label=gtk_label_new(_("Show"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gint i;
	rw_cfg_read_int (usercfgfile, "revise_test", "headchar_len", &i);
	if (i<=0)
		i=2;
	spin_button = gtk_spin_button_new_with_range(1,5,1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_revise_test_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("chars as the head-char.(default:2)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_revise_skim_word_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "revise_skim", "word_show_time", i);
	if (g_pReciteWord->revise_skim)
	{
		g_pReciteWord->revise_skim->viewer.w_show_time = i;
	}
}

static void
on_setup_revise_skim_meaning_spinbutton_changed(GtkSpinButton *widget, COption *parent)
{
	gint i= gtk_spin_button_get_value_as_int(widget);
	rw_cfg_write_int (usercfgfile, "revise_skim", "meaning_show_time", i);
	if (g_pReciteWord->revise_skim)
	{
		g_pReciteWord->revise_skim->viewer.m_show_time = i;
	}
}

static gboolean 
rw_preferences_dialog_setup_revise_skim_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *hbox, *label, *spin_button;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("The Revise_skim's settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	hbox= gtk_hbox_new(false,6);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	label=gtk_label_new(_("Show word for"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	gint i;
	rw_cfg_read_int (usercfgfile, "revise_skim", "word_show_time", &i);
	if (i<=0)
		i=1000;
	spin_button = gtk_spin_button_new_with_range(500,10000,500);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_revise_skim_word_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("milliseconds.(default:1000)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	hbox= gtk_hbox_new(false,6);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	label=gtk_label_new(_("Show meaning for"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	rw_cfg_read_int (usercfgfile, "revise_skim", "meaning_show_time", &i);
	if (i<=0)
		i=2000;
	spin_button = gtk_spin_button_new_with_range(500,10000,500);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button),i);
	g_signal_connect (G_OBJECT (spin_button), "value-changed", G_CALLBACK (on_setup_revise_skim_meaning_spinbutton_changed), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(hbox),spin_button,false,false,0);
	label=gtk_label_new(_("milliseconds.(default:2000)"));
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);

	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static void
on_setup_misc_sound_ckbutton_toggled(GtkWidget *widget, COption *parent)
{
	gboolean b = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
	if (widget==parent->misc_sound_ckbutton1)
	{
		rw_cfg_write_boolean (usercfgfile, "reciteword", "disable_sound", !b);
		g_pReciteWord->sndserv.disable = !b;
		if (b)
		{
			gtk_widget_set_sensitive(parent->misc_sound_ckbutton2,true);
			gtk_widget_set_sensitive(parent->misc_sound_ckbutton3,true);
		}
		else
		{
			gtk_widget_set_sensitive(parent->misc_sound_ckbutton2,false);
			gtk_widget_set_sensitive(parent->misc_sound_ckbutton3,false);
		}
	}
	else if (widget==parent->misc_sound_ckbutton2)
	{
		rw_cfg_write_boolean (usercfgfile, "reciteword", "disable_soundevent", !b);
		g_pReciteWord->soundevent.disable = !b;
	}
	else if (widget==parent->misc_sound_ckbutton3)
	{
		rw_cfg_write_boolean (usercfgfile, "reciteword", "disable_readword", !b);
		g_pReciteWord->wyabdc.disable = !b;
	}
}

static gboolean 
rw_preferences_dialog_setup_misc_sound_page(GtkNotebook *nb, COption *parent)
{
	GtkWidget *vbox, *vbox1, *hbox, *label;
	vbox = gtk_vbox_new(false,18);
	label=gtk_label_new(_("Sound settings."));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);
	parent->misc_sound_ckbutton1 = gtk_check_button_new_with_label(_("Play sound."));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(parent->misc_sound_ckbutton1),!g_pReciteWord->sndserv.disable);
	g_signal_connect (G_OBJECT (parent->misc_sound_ckbutton1), "toggled", G_CALLBACK (on_setup_misc_sound_ckbutton_toggled), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(vbox),parent->misc_sound_ckbutton1,false,false,0);
	hbox=gtk_hbox_new(false,0);
	label=gtk_label_new("    ");
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(hbox),label,false,false,0);
	vbox1=gtk_vbox_new(false,6);
	parent->misc_sound_ckbutton2 = gtk_check_button_new_with_label(_("Play sound event."));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(parent->misc_sound_ckbutton2),!g_pReciteWord->soundevent.disable);
	g_signal_connect (G_OBJECT (parent->misc_sound_ckbutton2), "toggled", G_CALLBACK (on_setup_misc_sound_ckbutton_toggled), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(vbox1),parent->misc_sound_ckbutton2,false,false,0);
	parent->misc_sound_ckbutton3 = gtk_check_button_new_with_label(_("Play read word's sound."));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(parent->misc_sound_ckbutton3),!g_pReciteWord->wyabdc.disable);
	g_signal_connect (G_OBJECT (parent->misc_sound_ckbutton3), "toggled", G_CALLBACK (on_setup_misc_sound_ckbutton_toggled), (gpointer)parent);
	gtk_box_pack_start(GTK_BOX(vbox1),parent->misc_sound_ckbutton3,false,false,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox1,false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,false,0);
	if (g_pReciteWord->sndserv.disable)
	{
		gtk_widget_set_sensitive(parent->misc_sound_ckbutton2,false);
		gtk_widget_set_sensitive(parent->misc_sound_ckbutton3,false);
	}
	gtk_notebook_append_page(nb,vbox,NULL);
	return true;
}

static GtkWidget*
rw_preferences_dialog_create_notebook (COption *parent)
{
	parent->notebook = gtk_notebook_new();
	GtkNotebook *nb = (GtkNotebook *)parent->notebook;
	gtk_notebook_set_show_tabs(nb,false);
	gtk_notebook_set_show_border(nb,false);
	
	rw_preferences_dialog_setup_logo_page (nb,parent);
	rw_preferences_dialog_setup_skins_page (nb,parent);
	rw_preferences_dialog_setup_fonts_page (nb,parent);
	rw_preferences_dialog_setup_firstrecite_skim_page (nb,parent);
	rw_preferences_dialog_setup_firstrecite_test_page (nb,parent);
	rw_preferences_dialog_setup_revise_group_page (nb,parent);
	rw_preferences_dialog_setup_revise_test_page (nb,parent);
	rw_preferences_dialog_setup_revise_skim_page (nb,parent);
	rw_preferences_dialog_setup_misc_sound_page (nb,parent);
	
	gtk_notebook_set_current_page (GTK_NOTEBOOK (parent->notebook), LOGO);	
	return parent->notebook;
}

COption::COption ()
{
}

COption::~COption ()
{
}

void
COption::init ()
{
	g_pReciteWord->option_button.set_enable (FALSE);
}

void
COption::show ()
{
	init ();
	playsound(SND_WIND);

	showing = TRUE;

	GtkWidget *hbox;
	GtkWidget *r;
    GtkWidget *l;
	GtkWidget *ct;
	GtkWidget *label;
	
	window = gtk_dialog_new();
	g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (on_window_delete_event), this);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (g_pReciteWord->window));
	
	gtk_dialog_add_button (GTK_DIALOG (window),
                             	GTK_STOCK_CLOSE,
                             	GTK_RESPONSE_CLOSE);
	gtk_dialog_set_default_response (GTK_DIALOG (window), GTK_RESPONSE_CLOSE);
	g_signal_connect(G_OBJECT (window), "response",
			 G_CALLBACK (rw_preferences_dialog_response_handler), this);
	hbox = gtk_hbox_new (FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 10);
	r = gtk_vbox_new (FALSE, 6);
	
	label = gtk_label_new_with_mnemonic (_("Cat_egories:"));
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	g_object_set (G_OBJECT (label), "xalign", 0.0, NULL);

	ct = rw_preferences_dialog_create_categories_tree (this);
		
	gtk_box_pack_start (GTK_BOX (r), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (r), ct, TRUE, TRUE, 0);

	l = rw_preferences_dialog_create_notebook (this);

	gtk_box_pack_start (GTK_BOX (hbox), r, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (hbox), l, TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), hbox,
			FALSE, FALSE, 0);
	
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), categories_tree);

	gtk_widget_show_all (GTK_DIALOG (window)->vbox);	
	
	gtk_window_set_title (GTK_WINDOW (window), _("Preferences"));
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_show(window);
}

void
COption::close ()
{
	playsound(SND_WIND);
	g_object_unref (G_OBJECT (categories_tree_model));
	gtk_widget_destroy (window);
	
	showing = FALSE;
	g_pReciteWord->option_button.set_enable (TRUE);
}
