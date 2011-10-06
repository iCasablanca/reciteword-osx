#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>

void verify_word(gchar *word, int linenum)
{
	if (g_ascii_isspace(*word)) {
        	g_print("Warning: begin with space, %s %d\n", word, linenum);
	}
	if (g_ascii_isspace(*(word+strlen(word)-1))) {
		g_print("Warning: end with space, %s %d\n", word, linenum);
	}
	gchar *p = word;
	while (*p) {
		if (!(*p >= 0x20 && *p <= 0x7E)) {
			g_print("Invalid character: %s %d\n", word, linenum);
			return;
		}
		p++;
	}
}

void verify_file(gchar *bokfilename)
{
	gchar *buffer;
	g_file_get_contents(bokfilename, &buffer, NULL, NULL);
	if (!buffer) {
		g_print("Read file %s failed!\n", bokfilename);
		return;
	}
	if (strncmp(buffer,"[H]recitewordbookfile",21)) {
		g_print("%s is not a bok file.\n", bokfilename);
		return;
	}
	g_print("Verifying %s...\n", bokfilename);
	gchar *p;
	p = strchr(buffer, '\n');
	if (!p) {
		g_print("No first ending line.\n");
		g_free(buffer);
		return;
	}
	p++;
	gchar *p1, *p2, *p3;
	int linenum = 1;
	while (TRUE) {
		if (*p=='\0')
			break;
		linenum++;
		p1 = strchr(p, '\n');
		if (!p1) {
			g_print("No ending new line.\n");
			p1 = p + strlen(p)-1;
		} else {
			*p1='\0';
		}
		p2=strstr(p, "[W]");
		if (!p) {
			g_print("No [W].\n");
			g_free(buffer);
			return;
		}
		p2+=3;
		p3 = strchr(p2, '[');
		if (!p3) {
			g_print("No [, %s.\n", p2);
			p = p1+1;
			continue;
		}
		*p3='\0';
		verify_word(p2, linenum);
		p = p1+1;
	}
	g_free(buffer);
}

void verify_dir(gchar *dirname)
{
	GDir *dir = g_dir_open(dirname, 0, NULL);
	if (dir) {
		const gchar *filename;
		gchar fullfilename[256];
		while ((filename = g_dir_read_name(dir))!=NULL) {
			sprintf(fullfilename, "%s/%s", dirname, filename);
			if (g_file_test(fullfilename, G_FILE_TEST_IS_DIR)) {
                                verify_dir(fullfilename);
                        }
                        else if (strcmp(filename, "dirname")!=0) {
                                verify_file(fullfilename);
				if (!g_str_has_suffix(filename, ".bok"))
					g_print("Invalid filename.\n");
                        }
		}
		g_dir_close(dir);
	}
}

int main(int argc,char * argv [])
{
	gtk_set_locale ();
	g_type_init ();
	verify_dir ("/usr/share/reciteword/books");
	g_print("Done!\n");
	return FALSE;
}

