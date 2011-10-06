#include "sndserv.h"
#include "reciteword.h"
#include <cstdio>
#include <cstdlib>
//#include <string>

#ifdef G_OS_WIN32
    #include "windows.h"
#else
	#include <esd.h>
#endif

extern CReciteWord* g_pReciteWord;

CSndserv::CSndserv()
{
#ifdef G_OS_WIN32
	canplay = true;
#else
	//fd=esd_open_sound(NULL);//"localhost";
	//canplay = (fd>=0);
	canplay = true;
#endif
	if (!canplay)
		g_print("esd initialization failed,no sound will be play!\n");
}

CSndserv::~CSndserv()
{
#ifdef G_OS_WIN32
#else
	//if (fd>=0) esd_close(fd);
#endif
}

void
CSndserv::play(const gchar *filename)
{
#ifdef G_OS_WIN32
#else
	//if (fd>=0) {
	  //if (g_file_test(filename, G_FILE_TEST_EXISTS)) {
	    //esd_play_file(NULL,filename,0); // ???
	    gchar command[256];
	    //int pos = filename.find_last_of(".");
	    //filename = substr(filename, 0, pos);
	    sprintf(command,"/usr/bin/afplay %s ",filename);
	    system(command);
	  //} else {
	  //  gchar command[256];
	  //  //int pos = filename.find_last_of(".");
	  //  //filename = substr(filename, 0, pos);
	  //  sprintf(command,"/usr/bin/espeak %s ",filename);
	  //  system(command);
	  //}
	//}
//		//esd_play_file ("reciteword", filename, 1);

	//gchar command[256];
	//sprintf(command,"/usr/bin/play %s ",filename);
	//system(command);
#endif
}

//static CSndserv sndserv;

#ifndef G_OS_WIN32
static gpointer play_file_mix(gpointer data)
{
	g_pReciteWord->sndserv.play((gchar *)data);
	g_free(data);
	return NULL;
}
#endif

/*
static void play_file_after_pre(gpointer data,gpointer user_data)
{
	g_pReciteWord->sndserv.play((gchar *)user_data);
	return NULL;
}

class CThreadPool
{
	GThreadPool *pool;
public:
	CThreadPool();
	~CThreadPool();
};

CThreadPool::CThreadPool()
{
	pool = g_thread_pool_new(play_file_after_pre,NULL,1,FALSE,NULL);
}

CThreadPool::~CThreadPool()
{
	g_thread_pool_free(pool,);
}
*/


void play_file(const char *filename,PLAY_METHOD method)
{
	if ((!g_pReciteWord->sndserv.canplay)||(g_pReciteWord->sndserv.disable))
		return;

	//gchar command[256];
	//sprintf(command,"/usr/bin/play %s &",filename);
	//system(command);


	//esd_play_file ("reciteword", filename, 1);  //it can't return quickly :( when in typing,press wrong key will cause freezing.
		
	// the system() function is inefficiency, should change to use thread to call esd_play_file().
	//gchar command[256];
	//sprintf(command,"esdplay %s &",filename);
	//system(command);
	
	//I tried to use gnome_sound_play() without threading, but it is too slow when you type words fast.
	
	switch (method)
	{
		case PM_MIX:
		{
#ifdef G_OS_WIN32
    	PlaySound(filename, 0, SND_ASYNC | SND_FILENAME);
#else
			GThread * thread;
			gchar *dup_filename = g_strdup(filename); //as in the new thread, filename may have already be freeed.
			thread = g_thread_create(play_file_mix,(gpointer)dup_filename,false,NULL);  //use GThreadPool may be more efficient.
#endif
			break;
		}
		case PM_STOP_PRE:   // it is hard to done,may need write my own esd_play_file.
		{
			break;
		}
		case PM_AFTER_PRE:  // use GThreadPool can do this,but,it does seems very useful before PM_STOP_PRE is done.
		{
			//GThreadPool *threadpool;
			
			break;
		}
	}

}
