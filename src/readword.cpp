#include "readword.h"
#include "reciteword.h"
#include <cstring>

extern CReciteWord* g_pReciteWord;


Cwyabdc::Cwyabdc ()
{
#ifdef G_OS_WIN32
	datapath = reciteword_data_dir + std::string(G_DIR_SEPARATOR_S "WyabdcRealPeopleTTS");
	havedatafile = g_file_test(datapath.c_str(), G_FILE_TEST_EXISTS);
	if (!havedatafile) {
		datapath = "C:\\Program Files\\WyabdcRealPeopleTTS";
		havedatafile = g_file_test(datapath.c_str(), G_FILE_TEST_EXISTS);
		if (!havedatafile)
			datapath.clear();
	}
#else
	havedatafile = g_file_test("/usr/share/WyabdcRealPeopleTTS", G_FILE_TEST_EXISTS);
	if (!havedatafile)
		g_print("RealPeopleTTS files not found!\n");
#endif
}

Cwyabdc::~Cwyabdc ()
{

}

void Cwyabdc::read(const char *word,PLAY_METHOD method)
{
	if (havedatafile) {
		int n=strlen(word);
		gchar *lowerword = (gchar *)g_malloc(n+1);
		for (int i=0;i<n;i++)
			lowerword[i]= g_ascii_tolower(word[i]);
		lowerword[n] = '\0';
#ifdef G_OS_WIN32		
		gchar *filename = g_strdup_printf("%s\\%c\\%s.wav", datapath.c_str(), lowerword[0], lowerword);		
#else
		gchar *filename = g_strdup_printf("/usr/share/WyabdcRealPeopleTTS/%c/%s.wav", lowerword[0],lowerword);		
#endif		
		if (g_file_test(filename, G_FILE_TEST_EXISTS))
			play_file (filename,method);
		g_free(filename);
		g_free(lowerword);
	}
}

// if use global static class,the class's constructor will run before main(),it means that the constructor run before gtk_init(),
// so,if use any gtk,glib function,i.e. g_printf(),it will make some strange things.it is better not to use global static class
// in gtk program.

//static Cwyabdc wyabdc;  

void
readword (const char *word,PLAY_METHOD method)
{
	if (g_pReciteWord->wyabdc.disable)
		return;
	if (word && word[0]!='\0' && (!strchr(word,' '))) // if the string contain a space character,then no sound.
		g_pReciteWord->wyabdc.read (word,method);
}
