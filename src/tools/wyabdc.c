//This program changes the wyabdc 's *.km file into the kind of
//
//recitewordbookfile. 

//Usage: "./wyabdc cet4.km"  or "./wyabdc *.km" 
//Notice: it will create the new file which name file.bok


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

void
convert_phonetic(char *str)
{
	gint i,len;
	len=strlen(str);
	for (i=0;i<len;i++)
	{
		switch (str[i])
		{
			case 'A': str[i]='5';break;
			case 'B': str[i]='1'; break;
			case 'R': str[i]='6'; break;
			case 'E': str[i]='3'; break;
			case 'F': str[i]='7'; break;
			case 'Q': str[i]='2'; break;
			case 'Z': str[i]='4'; break;
			case 'T': str[i]='8'; break;
			case 'N': str[i]='9'; break;
			case 'W': str[i]='0'; break;
			case 'V': str[i]='='; break;
			case ',': str[i]=';'; break;
			case '_': str[i]=' '; break;
			default: break;
		}
	}
}

void km_book_to_reciteword (BookFile *book)
{
	FILE *file;
	gchar newfilename[256],*a;
	strcpy(newfilename,book->bookfilename);
	a=strrchr(newfilename,'.');
	*a='\0';
		
	sprintf(newfilename,"%s.bok",newfilename);

	if (!(file = fopen (newfilename, "w")))
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

void km_book_free (BookFile *book)
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

gchar* get_book_name(gchar *filename)
{
	if (!strcasecmp(filename,"GRE.km"))
		return "GRE 词汇";
	else 	if (!strcasecmp(filename,"GMAT.km"))
		return "GMAT 词汇";
	else 	if (!strcasecmp(filename,"Taisha200207.km"))
		return "太傻单词7月精简版";
	else 	if (!strcasecmp(filename,"CET4.km"))
		return "大学四级英语";
		else 	if (!strcasecmp(filename,"CET56cz.km"))
		return "大学英语词组（5、6级）";else 	if (!strcasecmp(filename,"CET6.km"))
		return "大学六级英语";else 	if (!strcasecmp(filename,"COMMERCE.km"))
		return "金融英语";else 	if (!strcasecmp(filename,"COMPUTER.km"))
		return "计算机英语";else 	if (!strcasecmp(filename,"CZ1.KM"))
		return "初中英语1";else 	if (!strcasecmp(filename,"CZ1_new.km"))
		return "初中英语1（新版）";else 	if (!strcasecmp(filename,"CZ2.km"))
		return "初中英语2";else 	if (!strcasecmp(filename,"CZ2_new.km"))
		return "初中英语2（新版）";else 	if (!strcasecmp(filename,"CZ3.km"))
		return "初中英语3";else 	if (!strcasecmp(filename,"CZ3_new.km"))
		return "初中英语3（新版）";else 	if (!strcasecmp(filename,"CZ4.km"))
		return "初中英语4";else 	if (!strcasecmp(filename,"CZ5.km"))
		return "初中英语5";else 	if (!strcasecmp(filename,"CZ6.km"))
		return "初中英语6";else 	if (!strcasecmp(filename,"DXcz.km"))
		return "大学英语词组";else 	if (!strcasecmp(filename,"GJZX.km"))
		return "高教自学考试英语";else 	if (!strcasecmp(filename,"GRECP.km"))
		return "GRE 词频表词频序";else 	if (!strcasecmp(filename,"GRECPZM.km"))
		return "GRE 词频表字母序";else 	if (!strcasecmp(filename,"GREDYC.km"))
		return "GRE 同义词";else 	if (!strcasecmp(filename,"GRENXAll.km"))
		return "GRE 逆序";else 	if (!strcasecmp(filename,"GZ1.km"))
		return "高中英语1";else 	if (!strcasecmp(filename,"GZ2.km"))
		return "高中英语2";else 	if (!strcasecmp(filename,"GZ3.km"))
		return "高中英语3";else 	if (!strcasecmp(filename,"IELTS.km"))
		return "雅思英语";else 	if (!strcasecmp(filename,"LasyWord.km"))
		return "懒人单词";else 	if (!strcasecmp(filename,"NCE1.km"))
		return "新概念1";else 	if (!strcasecmp(filename,"NCE2.km"))
		return "新概念2";else 	if (!strcasecmp(filename,"NCE3.km"))
		return "新概念3";else 	if (!strcasecmp(filename,"NCE4.km"))
		return "新概念4";else 	if (!strcasecmp(filename,"PP2.km"))
		return "PP2 中的单词";else 	if (!strcasecmp(filename,"pp2_lbfyc.km"))
		return "PP2 类比反义词汇";else 	if (!strcasecmp(filename,"PP2_TK.km"))
		return "PP2 填空词汇";else 	if (!strcasecmp(filename,"SJJD1.km"))
		return "上教精读第一册";else 	if (!strcasecmp(filename,"SJJD2.km"))
		return "上教精读第二册";else 	if (!strcasecmp(filename,"TAISHA.km"))
		return "太傻单词";else 	if (!strcasecmp(filename,"TOEFL.km"))
		return "托福单词";else 	if (!strcasecmp(filename,"TOEFLCiHui.km"))
		return "托福词汇";else 	if (!strcasecmp(filename,"TOEFLCZ.km"))
		return "托福词组";else 	if (!strcasecmp(filename,"WM1.km"))
		return "外贸英语一";else 	if (!strcasecmp(filename,"WM2.km"))
		return "外贸英语二";else 	if (!strcasecmp(filename,"WM3.km"))
		return "外贸英语三";else 	if (!strcasecmp(filename,"WM4.km"))
		return "外贸英语四";else 	if (!strcasecmp(filename,"XX1.km"))
		return "小学英语第一册";else 	if (!strcasecmp(filename,"XX1_new.km"))
		return "新编小学英语第一册";else 	if (!strcasecmp(filename,"XX2.km"))
		return "小学英语第二册";else 	if (!strcasecmp(filename,"XX2_new.km"))
		return "新编小学英语第二册";else 	if (!strcasecmp(filename,"XX3.km"))
		return "小学英语第三册";else 	if (!strcasecmp(filename,"XX3_new.km"))
		return "新编小学英语第三册";else 	if (!strcasecmp(filename,"XX4.km"))
		return "小学英语第四册";else 	if (!strcasecmp(filename,"XX4_new.km"))
		return "新编小学英语第四册";else 	if (!strcasecmp(filename,"XX5_new.km"))
		return "小学英语第五册";else 	if (!strcasecmp(filename,"XX6_NEW.km"))
		return "新编小学英语第五册";else 	if (!strcasecmp(filename,"YJS.km"))
		return "研究生入学词汇";else 	if (!strcasecmp(filename,"YJScz.km"))
		return "研究生入学词组";else 	if (!strcasecmp(filename,"YX1.km"))
		return "医学英语第一册";else 	if (!strcasecmp(filename,"YX2.km"))
		return "医学英语第二册";else 	if (!strcasecmp(filename,"YX3.km"))
		return "医学英语第三册";else 	if (!strcasecmp(filename,"ZCa.km"))
		return "职称英语考试A";else 	if (!strcasecmp(filename,"ZCb.km"))
		return "职称英语考试B";else 	if (!strcasecmp(filename,"ZCc.km"))
		return "职称英语考试C";else 	if (!strcasecmp(filename,"lyyu365.km"))
		return "李扬疯狂英语三百六十五句";else {g_print("name not find\n");return "noname";}
}

BookFile *km_book_open_file(gchar *filename)
{
	BookFile *book;

	FILE *file;
	gchar *buffer,**lines,*tmp,tmpstr[1024],*tmp1;
	gint i,j,a;
	struct stat stats;

	if (lstat (filename, &stats) == -1)
		return NULL;
	if (!(file = fopen (filename, "r")))
		return NULL;
g_print("%s\n",filename);
	buffer = (gchar *) g_malloc (stats.st_size + 1);
	if (fread (buffer, 1, stats.st_size, file) != (guint) stats.st_size)
	{
		g_free (buffer);
		fclose (file);
		return NULL;
	}
	fclose (file);
	buffer[stats.st_size] = '\0';
	
	book = (BookFile *)g_malloc0 (sizeof (BookFile));
	book->bookfilename = g_strdup (filename);
	tmp=strrchr(filename,G_DIR_SEPARATOR);
	if (tmp)
	{
		book->name=g_strdup(get_book_name(tmp+1));
	}
	else
	{
		book->name=g_strdup(get_book_name(filename));
	}
	book->author=g_strdup("小芝麻");;
	book->website=g_strdup("http://www.zhimajie.net");
	book->email=g_strdup("admin@zhimajie.net");
	book->about=g_strdup("来自《我也爱背单词》");
	lines = g_strsplit (buffer, "\r\n", 0);
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
		//g_print("%d ",i);
		/*if (i>=1804)
		{
			g_print("%d %s",i,lines[i]);
		}*/
		strcpy(tmpstr,lines[i]);
		tmp=strchr(tmpstr,'\t');
		if (tmp)
		{
			*tmp='\0';
			g_strstrip(tmpstr);
			book->words.w[j]=g_strdup(tmpstr);
			if (*(tmp+1)=='\t')
			{
				book->words.t[j]=NULL;
				tmp1=tmp;
			}
			else
			{
				tmp1=strchr(tmp+2,'/');
				*tmp1='\0';
				g_strstrip(tmp+2);
				book->words.t[j]=g_strdup(tmp+2);
			}
			tmp1=strchr(tmp1+1,'\t');
			if (tmp1)
			{
				g_strstrip(tmp1+1);
				book->words.m[j]=g_strdup(tmp1+1);
			}
			else
			{
				book->words.m[j]=NULL;
			}
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
		BookFile* kmfile=km_book_open_file(argv[file_index]);
		if (kmfile)
		{
			km_book_to_reciteword (kmfile);
			km_book_free (kmfile);
		}
	}
	return FALSE;
}
