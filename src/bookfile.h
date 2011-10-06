#ifndef __RW_BOOKFILE_H__
#define __RW_BOOKFILE_H__

#include <glib.h>

typedef struct
{
	gchar **w;
	gchar **t;
	gchar **m;
}
Rw_Word;

typedef struct
{
	gchar *bookfilename;

	Rw_Word words;

	gint wordcount;
	gchar *name;
	gchar *count;
	gchar *author;
	gchar *email;
	gchar *website;
	gchar *about;
}
BookFile;

gint rw_book_get_file_info(gchar *filename,gchar **name,gchar **count,gchar **author,gchar **website,gchar **email,gchar **about);
BookFile *rw_book_open_file (gchar * filename);
void rw_book_free (BookFile * book);

#endif
