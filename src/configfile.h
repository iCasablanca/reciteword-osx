#ifndef __RW_CONFIGFILE_H__
#define __RW_CONFIGFILE_H__

#include <glib.h>
#include <time.h>


	typedef struct
	{
		gchar *key;
		gchar *value;
	}
	ConfigLine;

	typedef struct
	{
		gchar *name;
		GList *lines;
	}
	ConfigSection;


	typedef struct
	{
		GList *sections;
		gchar *cfgfilename;
		gboolean changed;
	}
	ConfigFile;

	ConfigFile *rw_cfg_new (void);
	ConfigFile *rw_cfg_open_file (gchar * filename);
	gboolean rw_cfg_write_file (ConfigFile * cfg, gchar * filename);
	void rw_cfg_free (ConfigFile * cfg);

	gboolean rw_cfg_read_string (ConfigFile * cfg, gchar * section,
				     gchar * key, gchar ** value);
	gboolean rw_cfg_read_int (ConfigFile * cfg, gchar * section,
				  gchar * key, gint * value);
	gboolean rw_cfg_read_long (ConfigFile * cfg, gchar * section,
				  gchar * key, glong * value);
	gboolean rw_cfg_read_time (ConfigFile * cfg, gchar * section,
				   gchar * key, time_t * value);
	gboolean rw_cfg_read_boolean (ConfigFile * cfg, gchar * section,
				      gchar * key, gboolean * value);
	void rw_cfg_write_string (ConfigFile * cfg, gchar * section,
				  gchar * key, gchar * value);
	void rw_cfg_write_int (ConfigFile * cfg, gchar * section, gchar * key,
			       gint value);
	void rw_cfg_write_long (ConfigFile * cfg, gchar * section, gchar * key,
			       glong value);
	void rw_cfg_write_time (ConfigFile * cfg, gchar * section,
				gchar * key, time_t value);
	void rw_cfg_write_boolean (ConfigFile * cfg, gchar * section,
				   gchar * key, gboolean value);

	void rw_cfg_rename_section (ConfigFile * cfg, gchar * section,
				    gchar * section_name);
	void rw_cfg_remove_key (ConfigFile * cfg, gchar * section,
				gchar * key);
	void rw_cfg_remove_section (ConfigFile * cfg, gchar * section);

#endif
