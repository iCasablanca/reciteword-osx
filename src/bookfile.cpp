#include "bookfile.h"
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <fstream>
#include <string>

using namespace std;

void
rw_book_free (BookFile * book)
{
	if (book->bookfilename)
		g_free (book->bookfilename);

	if (book->words.w)
	{
		int i;
		i = 0;
		while (book->words.w[i])
		{
			g_free (book->words.w[i]);
			if (book->words.t[i])
				g_free (book->words.t[i]);
			if (book->words.m[i])
				g_free (book->words.m[i]);
			i++;
		}
		g_free (book->words.w);
		g_free (book->words.t);
		g_free (book->words.m);
	}
	if (book->name)
		g_free (book->name);
	if (book->count)
		g_free (book->count);
	if (book->author)
		g_free (book->author);
	if (book->email)
		g_free (book->email);
	if (book->website)
		g_free (book->website);
	if (book->about)
		g_free (book->about);
}

static gint
rw_book_get_word( gchar *src, gchar **w, gchar **t, gchar **m)//w:[W], t:[T], m:[M]
{
	static const gchar	wf[]="[W]", tf[]="[T]", mf[]="[M]";
	static const gint	wfl=sizeof(wf)-1, tfl=sizeof(tf)-1, mfl=sizeof(mf)-1;
	gchar	*tmp;
	
	for(tmp=src;*tmp;tmp++){
		if(*tmp=='\r'||*tmp=='\n'){
			*tmp='\0';
			break;
		}
	}

	*w=strstr(src, wf);
	if(*w){
		*w+=wfl;
	}else{
		return false;
	}
	*t=strstr(src, tf);
	*m=strstr(src, mf);

	if(*t){
		**t='\0';
		*t+=tfl;
	}
	if(*m){
		**m='\0';
		*m+=mfl;
	}

	*w=g_strdup(*w);
	if(*t){
		*t=g_strdup(*t);
	}
	if(*m){
		*m=g_strdup(*m);
	}
	return true;
}
	

static gchar *
rw_book_get_value (const gchar * str1, gchar * str2,gint utf8)
{
	gchar tmp, *tmp1, *tmp2;

	//tmp1 = strstr (str1, str2);
	tmp1 = (gchar*)strstr (str1, str2);
	if (!tmp1)
		return NULL;
	else
		tmp1 += strlen (str2);
	tmp2 = tmp1;
	while ((*tmp2 != '\0') && (*tmp2 != '\n') && (*tmp2 != '\r')
	       && (*tmp2 != '['))
	{
		tmp2++;
	}
	if (tmp2 == tmp1)
		return NULL;
	tmp = *tmp2;
	*tmp2 = '\0';
//	if (utf8)
//	{		
		tmp1 = g_strdup (tmp1);
		// the new reciteword's bok file format is converted to utf8,so needn't and shouldn't convert again.
		//tmp1 = g_convert(tmp1,-1,"UTF-8","GBK",NULL,NULL,NULL);
//	}
//	else
//	{
//		tmp1 = g_strdup (tmp1);
//	}
	*tmp2 = tmp;
	return tmp1;
}

gint
rw_book_get_file_info(gchar *filename,gchar **name,gchar **count,gchar **author,gchar **website,gchar **email,gchar **about)
{
	std::ifstream file(filename);
	if (file.fail())
		return false; 
	gchar linechar[256];
	if (!file.getline(linechar,256,'\n'))
	{
		file.close();
		return false;
	}
	file.close();
	if (strncmp(linechar,"[H]recitewordbookfile",21))
		return false;
	gchar *wordcount;
	wordcount=rw_book_get_value(linechar,"[C]",0);
	if (wordcount)
	{
		*count=wordcount;
	}
	else
	{
		BookFile *book;
		book=rw_book_open_file(filename); //it will rebuild the bok file,make it have the "[C]" word count record.
		*count=g_strdup_printf("%d",book->wordcount);
		rw_book_free(book);
		g_free(book);
	}
	*name=rw_book_get_value(linechar,"[N]",1);
	*author=rw_book_get_value(linechar,"[R]",1);
	*website=rw_book_get_value(linechar,"[P]",1);
	*email=rw_book_get_value(linechar,"[E]",1);
	*about=rw_book_get_value(linechar,"[A]",1);
	return TRUE;
}

static void rw_book_saveto_file(BookFile *book,gchar *filename)
{
	FILE *file;
	file=fopen(filename,"wb");
	if (!file)
		return;
	fprintf(file,"[H]recitewordbookfile");
	if (book->name)
		fprintf(file,"[N]%s",book->name);
	fprintf(file,"[C]%d",book->wordcount);
	if (book->author)
		fprintf(file,"[R]%s",book->author);
	if (book->website)
		fprintf(file,"[P]%s",book->website);
	if (book->email)
		fprintf(file,"[E]%s",book->email);
	if (book->about)
		fprintf(file,"[A]%s",book->about);
	if (book->words.w)
	{
		int i = 0;
		while (book->words.w[i])
		{
			fprintf(file,"\n[W]%s",book->words.w[i]);
			//if (book->words.t[i])//就算没有内容也把标志输出
				fprintf(file,"[T]%s",book->words.t[i]?book->words.t[i]:"");
			//if (book->words.m[i])
				fprintf(file,"[M]%s",book->words.m[i]?book->words.m[i]:"");
			i++;
		}
	}
	fclose(file);
}

BookFile *
rw_book_open_file (gchar * filename)
{
	BookFile *book;
	FILE *file;
	gchar *buffer, **lines, *tmp;
	gint i, j, a;
	struct stat stats;
	if (stat (filename, &stats))
		return NULL;
	if (!(file = fopen (filename, "rb")))
		return NULL;

	buffer = (gchar *) g_malloc (stats.st_size + 1);
	fread (buffer, 1, stats.st_size, file);
/*	if (fread (buffer, 1, stats.st_size, file) != (guint) stats.st_size)
	{
		g_free (buffer);
		fclose (file);
		return NULL;
	}
*/
	fclose (file);
	buffer[stats.st_size] = '\0';
	if (strncmp (buffer, "[H]recitewordbookfile", 21))
	{
		g_free (buffer);
		return NULL;
	}

	book = (BookFile *) g_malloc0 (sizeof (BookFile));
	book->bookfilename = g_strdup (filename);

	book->name = rw_book_get_value (buffer, "[N]",1);
	book->count = rw_book_get_value (buffer, "[C]",1);
	book->author = rw_book_get_value (buffer, "[R]",1);
	book->website = rw_book_get_value (buffer, "[P]",1);
	book->email = rw_book_get_value (buffer, "[E]",1);
	book->about = rw_book_get_value (buffer, "[A]",1);
	tmp = strchr (buffer, '\n');
	tmp++;
	lines = g_strsplit (tmp, "\n", 0);
	g_free (buffer);
	a = 0;
	while (lines[a])
	{
		a++;
	}
	book->words.w = (gchar **) g_malloc ((a + 1) * sizeof (gchar *));
	book->words.t = (gchar **) g_malloc ((a + 1) * sizeof (gchar *));
	book->words.m = (gchar **) g_malloc ((a + 1) * sizeof (gchar *));

	i = 0;
	j = 0;
	while (lines[i])
	{
		/*tmp = rw_book_get_value (lines[i], "[W]",0);
		if (tmp)
		{
			book->words.w[j] = tmp;
			book->words.t[j] =
				rw_book_get_value (lines[i], "[T]",0);
			book->words.m[j] =
				rw_book_get_value (lines[i], "[M]",1);
			j++;
		}*/
		if(rw_book_get_word(lines[i], &(book->words.w[j]), &(book->words.t[j]), &(book->words.m[j])))
		{//参考上面给j增1
			j++;
		}
		i++;
	}
	book->words.w[j] = NULL;
	book->wordcount = j;
	g_strfreev (lines);
	gchar aa[5];
	sprintf(aa,"%d",j);
	if ((!book->count)||(strcmp(book->count,aa)))  // the word count is not recorded or wrong,rebuid this bok file
	{
		rw_book_saveto_file(book,filename);
		if (book->count)
		{
			g_free(book->count);
		}
		book->count=g_strdup(aa);
	}
	return book;
}
