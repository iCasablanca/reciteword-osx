//This program changes the WordsLoverWordsLib's *.bok file into the kind of
//recitewordbookfile. 

//Usage: "wordlover cet-4.bok"  or "wordlover *.bok" 
//Notice: it will create the backup file which name oldfile.bok.bak


#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

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
	gchar *author;
	gchar *email;
	gchar *website;
	gchar *about;
}
BookFile;

void wordlover_book_backup (BookFile *book)
{
	gchar filename[256];
	sprintf(filename,"%s.bak",book->bookfilename);

	rename(book->bookfilename,filename);
}

void
convert_phonetic(char *str)
{
	gint i,len;
	len=strlen(str);
	for (i=0;i<len;i++)
	{
		switch (str[i])
		{
			case '2': str[i]='3';break;
			case '3': str[i]='2'; break;
			case '4': str[i]='0'; break;
			case '5': str[i]='8'; break;
			case '6': str[i]='7'; break;
			case '7': str[i]='='; break;
			case '8': str[i]='9'; break;
			case '9': str[i]='5'; break;
			case '0': str[i]='6'; break;
			case '/': str[i]='4'; break;
			case '=': str[i]=' '; break;
			case ',': str[i]=';'; break;
			case ';': str[i]=' '; break;
			default: break;
		}
	}
}
void wordlover_book_to_reciteword (BookFile *book)
{
	FILE *file;
	
	if (!(file = fopen (book->bookfilename, "w")))
		return;
	fprintf(file,"%s","[H]recitewordbookfile");
	if (book->name)
		fprintf(file,"[N]%s",book->name);
	if (book->author)
		fprintf(file,"[R]%s",book->author);
	if (book->website)
		fprintf(file,"[P]%s",book->website);
	if (book->email)
		fprintf(file,"[E]%s",book->email);
	if (book->about)
		fprintf(file,"[A]%s",book->about);
	fprintf(file,"%s","\r\n");
	if (book->words.w)
	{
		int i;
		i=0;
		while (book->words.w[i])
		{
			fprintf(file,"[W]%s",book->words.w[i]);
			if (book->words.t[i])
			{
				convert_phonetic(book->words.t[i]);
				fprintf(file,"[T]%s",book->words.t[i]);
			}
			if (book->words.m[i])
				fprintf(file,"[M]%s",book->words.m[i]);
			fprintf(file,"%s","\r\n");
			
			i++;
		}
	}
	fclose(file);
}

void wordlover_book_free (BookFile *book)
{
	if (book->bookfilename)
		g_free(book->bookfilename);
	
	if (book->words.w)
	{
		int i;
		i=0;
		while (book->words.w[i])
		{
			g_free(book->words.w[i]);
			if (book->words.t[i])
				g_free(book->words.t[i]);
			if (book->words.m[i])
				g_free(book->words.m[i]);
			i++;
		}
		g_free(book->words.w);
		g_free(book->words.t);
		g_free(book->words.m);
	}
	if (book->name)
		g_free(book->name);
	if (book->author)
		g_free(book->author);
	if (book->email)
		g_free(book->email);
	if (book->website)
		g_free(book->website);
	if (book->about)
		g_free(book->about);
}

static gchar *rw_book_get_value(gchar *str1,gchar *str2)
{
	gchar tmp,*tmp1,*tmp2;
	
	tmp1=strstr(str1,str2);
	if (!tmp1)
		return NULL;
	else
		tmp1+=strlen(str2);
	tmp2=tmp1;
	while ((*tmp2!='\0')&&(*tmp2!='\n')&&(*tmp2!='\r')&&(*tmp2!='['))
	{
		tmp2++;
	}
	if (tmp2==tmp1)
		return NULL;
	tmp=*tmp2;
	*tmp2='\0';
	tmp1=g_strdup(tmp1);
	*tmp2=tmp;
	return tmp1;
}

BookFile *wordlover_book_open_file(gchar *filename)
{
	BookFile *book;

	FILE *file;
	gchar *buffer,**lines,*tmp;
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
	if (!((strncmp(buffer,"[H]WordsLoverWordsLib",21)==0) || (strncmp(buffer,"[H]WordsLoverWordsBok",21)==0)))
	{
		g_free (buffer);
		g_print("%s is not wordlover's file\n",filename);
		return NULL; 
	}
	
	book = (BookFile *)g_malloc0 (sizeof (BookFile));
	book->bookfilename = g_strdup (filename);
	
	book->name=rw_book_get_value(buffer,"[N]");
	book->author=rw_book_get_value(buffer,"[R]");
	book->website=rw_book_get_value(buffer,"[P]");
	book->email=rw_book_get_value(buffer,"[E]");
	book->about=rw_book_get_value(buffer,"[A]");
	if (!book->about)
		book->about=g_strdup("come from wordlover");
	tmp=strchr(buffer,'\n');
	tmp++;
	for (i=0;i<4;i++) //there may be some NULL char;
	{
		if (*(tmp+i)=='\0')
			*(tmp+i)='0';
	}
	lines = g_strsplit (tmp, "\n", 0);
	g_free (buffer);
	
	a=0;
	while (lines[a])
	{
		a++;
	}
	book->words.w=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	book->words.t=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	book->words.m=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	
	i=0;
	j=0;
	while (lines[i])
	{
		tmp=rw_book_get_value(lines[i],"[W]");
		if (tmp)
		{
			book->words.w[j]=tmp;
			book->words.t[j]=rw_book_get_value(lines[i],"[T]");
			book->words.m[j]=rw_book_get_value(lines[i],"[M]");
			j++;
		}
		i++;
	}
	book->words.w[j]=NULL;
	book->wordcount=j;
	
	g_strfreev (lines);
	
	return book;	
}

int
main(int argc,char * argv [])
{
	int file_index;
	for (file_index=1;file_index<argc;file_index++)
	{
		BookFile* wordloverfile=wordlover_book_open_file(argv[file_index]);
		if (wordloverfile)
		{
			wordlover_book_backup (wordloverfile);
			wordlover_book_to_reciteword (wordloverfile);
			wordlover_book_free (wordloverfile);
		}
	}
	return FALSE;
}
