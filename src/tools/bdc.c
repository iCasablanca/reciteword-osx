//this file's codeset is utf8
//This program changes the BDC's *.gds file into the kind of
//recitewordbookfile. 

//Usage: "bdc ck-xd.gds"  or "bdc *.gds" 
//Notice: the newfile's name will come from the content of the gds file.


#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <gtk/gtk.h>
#include <glib.h>

typedef struct
{
	gchar **w;  // word
	gchar **t;  // photic
	gchar **m;  // meaning
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
	gchar *type;
}
BookFile;

void bdc_book_to_reciteword (BookFile *book)
{
	FILE *file;
	struct stat st;
	gchar newfilename[256],*a,dir[256],*bb;
	strcpy(newfilename,book->bookfilename);
	a=strrchr(newfilename,G_DIR_SEPARATOR);
	*a='\0';
	gchar filebasename[256];
	strcpy(filebasename, a+1);
	gchar *b=strrchr(filebasename, '.');
        *b='\0';

	sprintf(dir,"%s/bdc",newfilename);
	if (stat (dir, &st))
		mkdir (dir, 00755);
	gchar *utype;
	utype = g_convert(book->type,-1,"UTF-8","GB18030",NULL,NULL,NULL);
	if (!utype)
		g_print("Wrong type!\n");
	sprintf(dir,"%s/%s",dir,utype);
	if (stat (dir, &st))
		mkdir (dir, 00755);
	gchar bookfilename[256];
	sprintf(bookfilename,"%s/bdc/%s/%s.bok",newfilename,utype,filebasename);
	g_free(utype);
	if (!(file = fopen (bookfilename, "w")))
		return;
	fprintf(file,"%s","[H]recitewordbookfile");
	if (book->name)
	{
		bb = g_convert(book->name,-1, "UTF-8","GB18030",NULL,NULL,NULL);
		fprintf(file,"[N]%s",bb);
		g_free(bb);		
	}
	fprintf(file,"[C]%d",book->wordcount);
	if (book->author)
		fprintf(file,"[R]%s",book->author);
	if (book->website)
		fprintf(file,"[P]%s",book->website);
	if (book->email)
		fprintf(file,"[E]%s",book->email);
	if (book->about)
		fprintf(file,"[A]%s",book->about);
	fprintf(file,"%s","\n");
	if (book->words.w)
	{
		int i;
		i=0;
		while (book->words.w[i])
		{
			fprintf(file,"[W]%s",book->words.w[i]);
			if (book->words.t[i])
			{
				fprintf(file,"[T]%s",book->words.t[i]);
			}
			if (book->words.m[i])
				fprintf(file,"[M]%s",book->words.m[i]);
			fprintf(file,"%s","\n");
			i++;
		}
	}
	fclose(file);
}

void bdc_book_free (BookFile *book)
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
	if (book->type)
		g_free(book->type);
	if (book->author)
		g_free(book->author);
	if (book->email)
		g_free(book->email);
	if (book->website)
		g_free(book->website);
	if (book->about)
		g_free(book->about);
}

BookFile *bdc_book_open_file(gchar *filename)
{
	BookFile *book;

	FILE *file;
	gchar *buffer;
	gchar tmpstr[1024],tmpchar;
	gint i,j,a;
	struct stat stats;

	if (lstat (filename, &stats) == -1)
		return NULL;
	if (!(file = fopen (filename, "rb")))
		return NULL;

	//g_print("Converting: %s\n", filename);

	buffer = (gchar *) g_malloc (stats.st_size + 1);
	if (fread (buffer, 1, stats.st_size, file) != (guint) stats.st_size) {
		g_free (buffer);
		fclose (file);
		return NULL;
	}
	fclose (file);
	buffer[stats.st_size] = '\0';
/*	if (!((strncmp(buffer,"[H]WordsLoverWordsLib",21)==0) || (strncmp(buffer,"[H]WordsLoverWordsBok",21)==0)))
	{
		g_free (buffer);
		g_print("%s is not wordlover's file\n",filename);
		return NULL; 
	}*/
	
	book = (BookFile *)g_malloc0 (sizeof (BookFile));
	book->bookfilename = g_strdup (filename);

	buffer[32]='\0'; //单词 
	tmpchar=buffer[58];
	buffer[58]='\0';
	sprintf(tmpstr,"%s%s",buffer+12,buffer+50);
	g_strstrip(tmpstr);
	book->name=g_strdup(tmpstr);
	buffer[64]='\0'; //音标 
	buffer[88]='\0'; //词意 	
	buffer[58]=tmpchar;
	sprintf(tmpstr,"%s%s",buffer+58,buffer+82);
	g_strstrip(tmpstr);
	book->type=g_strdup(tmpstr);

	
	
	book->author=g_strdup("蒲公英公司");
	book->website=g_strdup("http://www.pgy.com.cn");
	book->email=NULL;
	book->about=g_strdup("Come from bdc");
	
	a=stats.st_size/128; //每个单词块大小为128字节 
	book->words.w=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	book->words.t=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	book->words.m=(gchar **)g_malloc((a+1)*sizeof(gchar *));
	
	i=290; //第一个单词的偏移 
	j=0;
	while (i<stats.st_size)
	{
		*(buffer+i+29)='\0';
	    snprintf(tmpstr,sizeof(tmpstr), "%s",buffer+i);
		g_strstrip(tmpstr);
		book->words.w[j]= g_convert(tmpstr,-1,"UTF-8","GB18030",NULL,NULL,NULL);
		if (!book->words.w[j])
		{
			printf("wrong word %d\n",j);
			i+=128;
			continue;
		}
		*(buffer+i+59)='\0';
		snprintf(tmpstr,sizeof(tmpstr),"%s",buffer+i+30);
		g_strstrip(tmpstr);
		book->words.t[j]=g_strdup(tmpstr);
		
		*(buffer+i+100)='\0';
		//*(buffer+i+127)='\0';
		snprintf(tmpstr,sizeof(tmpstr),"%s",buffer+i+60);
	    //snprintf(tmpstr,sizeof(tmpstr),"%s%s",buffer+i+60,buffer+i+60);
		g_strstrip(tmpstr);
		book->words.m[j]=g_convert(tmpstr,-1,"UTF-8","GB18030",NULL,NULL,NULL);
		if (!book->words.m[j])
			printf("wrong meaning %d %s file: %s\n",j,book->words.w[j],filename);

		j++;		
		i+=128;
	}
	book->words.w[j]=NULL;
	book->wordcount=j;
	return book;	
}

int
main(int argc,char * argv [])
{
	int file_index;
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	BookFile* bdcfile;
	for (file_index=1;file_index<argc;file_index++)	{
		bdcfile=bdc_book_open_file(argv[file_index]);
		if (bdcfile) {
			bdc_book_to_reciteword (bdcfile);
			bdc_book_free (bdcfile);
		}
	}
	return FALSE;
}
