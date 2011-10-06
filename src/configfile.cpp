/*
this file is copy form x11amp,i changed it for some place.
这个文件是我从x11amp(现在叫xmms)拷来的,增删了几个函数.
*/

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configfile.h"

static ConfigSection *rw_cfg_create_section (ConfigFile * cfg, gchar * name);
static ConfigLine *rw_cfg_create_string (ConfigSection * section, gchar * key,
					 gchar * value);
static ConfigSection *rw_cfg_find_section (ConfigFile * cfg, gchar * name);
static ConfigLine *rw_cfg_find_string (ConfigSection * section, gchar * key);

ConfigFile *
rw_cfg_new (void)
{
	ConfigFile *cfg;

	cfg = (ConfigFile *)g_malloc0 (sizeof (ConfigFile));

	return cfg;
}


ConfigFile *
rw_cfg_open_file (gchar * filename)
{
	ConfigFile *cfg;

	FILE *file;
	gchar *buffer, **lines, *tmp;
	gint i;
	struct stat stats;
	ConfigSection *section = NULL;

	if (stat (filename, &stats) == -1)
		return NULL;
	if (!(file = fopen (filename, "rb")))
		return NULL;

	buffer = (gchar *)g_malloc (stats.st_size + 1);
	fread (buffer, 1, stats.st_size, file);
/*	if (fread (buffer, 1, stats.st_size, file) != stats.st_size)
	{
		g_free (buffer);
		fclose (file);
		return NULL;
	}
*/
	fclose (file);
	buffer[stats.st_size] = '\0';

	cfg = (ConfigFile *)g_malloc0 (sizeof (ConfigFile));
	cfg->cfgfilename = g_strdup (filename);
	cfg->changed = 0;
	lines = g_strsplit (buffer, "\n", 0);
	g_free (buffer);
	i = 0;
	while (lines[i])
	{
		if (lines[i][0] == '[')
		{
			if ((tmp = strchr (lines[i], ']')))
			{
				*tmp = '\0';
				section =
					rw_cfg_create_section (cfg,
							       &lines[i][1]);
			}
		}
		else if (lines[i][0] != '#' && section)
		{
			if ((tmp = strchr (lines[i], '=')))
			{
				*tmp = '\0';
				tmp++;
				rw_cfg_create_string (section, lines[i], tmp);
			}
		}
		i++;
	}
	g_strfreev (lines);
	return cfg;
}


gboolean
rw_cfg_write_file (ConfigFile * cfg, gchar * filename)
{
	FILE *file;
	GList *section_list, *line_list;
	ConfigSection *section;
	ConfigLine *line;

	if (!(file = fopen (filename, "wb")))
		return FALSE;

	section_list = cfg->sections;
	while (section_list)
	{
		section = (ConfigSection *) section_list->data;
		if (section->lines)
		{
			fprintf (file, "[%s]\n", section->name);
			line_list = section->lines;
			while (line_list)
			{
				line = (ConfigLine *) line_list->data;
				fprintf (file, "%s=%s\n", line->key,
					 line->value);
				line_list = g_list_next (line_list);
			}
			fprintf (file, "\n");
		}
		section_list = g_list_next (section_list);
	}
	fclose (file);
	return TRUE;
}

gboolean
rw_cfg_read_string (ConfigFile * cfg, gchar * section, gchar * key,
		    gchar ** value)
{
	ConfigSection *sect;
	ConfigLine *line;

	*value = NULL;
	if (!(sect = rw_cfg_find_section (cfg, section)))
		return FALSE;
	if (!(line = rw_cfg_find_string (sect, key)))
		return FALSE;
	*value = g_strdup (line->value);
	return TRUE;
}

gboolean
rw_cfg_read_int (ConfigFile * cfg, gchar * section, gchar * key, gint * value)
{
	gchar *str;

	if (!rw_cfg_read_string (cfg, section, key, &str))
	{
		*value = 0;
		return FALSE;
	}
	*value = atoi (str);
	g_free (str);

	return TRUE;
}

gboolean
rw_cfg_read_long (ConfigFile * cfg, gchar * section, gchar * key, glong * value)
{
	gchar *str;

	if (!rw_cfg_read_string (cfg, section, key, &str))
	{
		*value = 0;
		return FALSE;
	}
	*value = atol (str);
	g_free (str);

	return TRUE;
}

gboolean
rw_cfg_read_time (ConfigFile * cfg, gchar * section, gchar * key,
		  time_t * value)
{
	gchar *str;

	if (!rw_cfg_read_string (cfg, section, key, &str))
	{
		*value = 0;
		return FALSE;
	}
	*value = atol (str);
	g_free (str);

	return TRUE;
}

gboolean
rw_cfg_read_boolean (ConfigFile * cfg, gchar * section, gchar * key,
		     gboolean * value)
{
	gchar *str;

	if (!rw_cfg_read_string (cfg, section, key, &str))
	{
		*value = FALSE;
		return FALSE;
	}
	if (!strcmp (str, "0"))
		*value = FALSE;
	else
		*value = TRUE;
	g_free (str);
	return TRUE;
}

void
rw_cfg_write_string (ConfigFile * cfg, gchar * section, gchar * key,
		     gchar * value)
{
	ConfigSection *sect;
	ConfigLine *line;

	cfg->changed = 1;
	sect = rw_cfg_find_section (cfg, section);
	if (!sect)
		sect = rw_cfg_create_section (cfg, section);
	if ((line = rw_cfg_find_string (sect, key)))
	{
		g_free (line->value);
		line->value = g_strchug (g_strchomp (g_strdup (value)));
	}
	else
		rw_cfg_create_string (sect, key, value);
}

void
rw_cfg_write_int (ConfigFile * cfg, gchar * section, gchar * key, gint value)
{
	gchar *strvalue;
	strvalue = g_strdup_printf ("%d", value);
	rw_cfg_write_string (cfg, section, key, strvalue);
	g_free (strvalue);
}

void
rw_cfg_write_long (ConfigFile * cfg, gchar * section, gchar * key, glong value)
{
	gchar *strvalue;
	strvalue = g_strdup_printf ("%ld", value);
	rw_cfg_write_string (cfg, section, key, strvalue);
	g_free (strvalue);
}

void
rw_cfg_write_time (ConfigFile * cfg, gchar * section, gchar * key,
		   time_t value)
{
	gchar *strvalue;
	strvalue = g_strdup_printf ("%ld", value);
	rw_cfg_write_string (cfg, section, key, strvalue);
	g_free (strvalue);
}

void
rw_cfg_write_boolean (ConfigFile * cfg, gchar * section, gchar * key,
		      gboolean value)
{
	if (value)
		rw_cfg_write_string (cfg, section, key, "1");
	else
		rw_cfg_write_string (cfg, section, key, "0");
}

void
rw_cfg_rename_section (ConfigFile * cfg, gchar * section,
		       gchar * section_name)
{
	ConfigSection *sect;

	sect = rw_cfg_find_section (cfg, section);
	if (sect)
	{
		cfg->changed = 1;
		g_free (sect->name);
		sect->name = g_strdup (section_name);
	}
}

void
rw_cfg_remove_key (ConfigFile * cfg, gchar * section, gchar * key)
{
	ConfigSection *sect;
	ConfigLine *line;

	sect = rw_cfg_find_section (cfg, section);
	if (sect)
	{
		line = rw_cfg_find_string (sect, key);
		if (line)
		{
			cfg->changed = 1;
			g_free (line->key);
			g_free (line->value);
			g_free (line);
			sect->lines = g_list_remove (sect->lines, line);
		}
	}
}

void
rw_cfg_remove_section (ConfigFile * cfg, gchar * section)	/* this was added by me 我加的,应该没错误 */
{
	ConfigSection *sect;
	ConfigLine *line;
	GList *line_list;

	sect = rw_cfg_find_section (cfg, section);
	if (sect)
	{
		cfg->changed = 1;
		g_free (sect->name);
		line_list = sect->lines;
		while (line_list)
		{
			line = (ConfigLine *) line_list->data;
			g_free (line->key);
			g_free (line->value);
			g_free (line);
			line_list = g_list_next (line_list);
		}
		g_list_free (sect->lines);
		g_free (sect);
		cfg->sections = g_list_remove (cfg->sections, sect);
	}
}


void
rw_cfg_free (ConfigFile * cfg)
{
	ConfigSection *section;
	ConfigLine *line;
	GList *section_list, *line_list;

	g_free (cfg->cfgfilename);
	section_list = cfg->sections;
	while (section_list)
	{
		section = (ConfigSection *) section_list->data;
		g_free (section->name);

		line_list = section->lines;
		while (line_list)
		{
			line = (ConfigLine *) line_list->data;
			g_free (line->key);
			g_free (line->value);
			g_free (line);
			line_list = g_list_next (line_list);
		}
		g_list_free (section->lines);
		g_free (section);

		section_list = g_list_next (section_list);
	}
	g_list_free (cfg->sections);
	cfg->sections = NULL;	/*这两行我加的,rw_cfg_free后可回到rw_cfg_new状态,可继续使用. */
	cfg->cfgfilename = NULL;
}

static ConfigSection *
rw_cfg_create_section (ConfigFile * cfg, gchar * name)
{
	ConfigSection *section;
	section = (ConfigSection *)g_malloc0 (sizeof (ConfigSection));
	section->name = g_strdup (name);
	cfg->sections = g_list_append (cfg->sections, section);

	return section;
}

static ConfigLine *
rw_cfg_create_string (ConfigSection * section, gchar * key, gchar * value)
{
	ConfigLine *line;
	line = (ConfigLine *)g_malloc0 (sizeof (ConfigLine));
	line->key = g_strchug (g_strchomp (g_strdup (key)));
	line->value = g_strchug (g_strchomp (g_strdup (value)));
	section->lines = g_list_append (section->lines, line);

	return line;
}

static ConfigSection *
rw_cfg_find_section (ConfigFile * cfg, gchar * name)
{
	ConfigSection *section;
	GList *list;

	list = cfg->sections;
	while (list)
	{
		section = (ConfigSection *) list->data;
		if (!strcasecmp (section->name, name))
			return section;
		list = g_list_next (list);
	}
	return NULL;
}

static ConfigLine *
rw_cfg_find_string (ConfigSection * section, gchar * key)
{
	ConfigLine *line;
	GList *list;

	list = section->lines;
	while (list)
	{
		line = (ConfigLine *) list->data;
		if (!strcasecmp (line->key, key))
			return line;
		list = g_list_next (list);
	}
	return NULL;
}
