#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <glib.h>

gchar* convert_file(gchar * filename)
{
	FILE *file;
	gchar *buffer,**lines,*tmp,tmpstr[1024],*tmp1;
	gint i,j,a;
	struct stat stats;

	if (lstat (filename, &stats) == -1)
		return NULL;
	if (!(file = fopen (filename, "r")))
		return NULL;
	buffer = (gchar *) g_malloc (stats.st_size + 1);
	if (fread (buffer, 1, stats.st_size, file) != (guint) stats.st_size)
	{
		g_free (buffer);
		fclose (file);
		return NULL;
	}
	fclose (file);
	buffer[stats.st_size] = '\0';
    tmp=buffer;
	if (!(file = fopen ("adf.km", "w")))
		return NULL;
	while (*tmp)
	{
		if (*tmp=='\t')
			fprintf(file,"\t\t");
		else
			fprintf(file,"%c",*tmp);
		tmp++;
	}
	fclose(file);
}

int
main(int argc,char * argv [])
{
	int file_index;
	for (file_index=1;file_index<argc;file_index++)
	{
		convert_file(argv[file_index]);
	}
	return FALSE;
}
