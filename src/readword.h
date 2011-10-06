#ifndef __RW_READWORD_H__
#define __RW_READWORD_H__

#include "sndserv.h"
#ifdef G_OS_WIN32
  #include <string>
#endif

class Cwyabdc
{
public:
	gboolean disable;	
	Cwyabdc ();
	~Cwyabdc ();
	void read(const char *word,PLAY_METHOD method);
private:
	bool havedatafile;
#ifdef G_OS_WIN32
	std::string datapath;
#endif
};

void readword (const char *word,PLAY_METHOD method=PM_MIX);

#endif
