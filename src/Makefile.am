## Process this file with automake to produce Makefile.in
SUBDIRS= tools modules skins win32

bin_PROGRAMS = reciteword

INCLUDES =							\
	$(RECITEWORD_CFLAGS)						\
	-DRECITEWORD_DATA_DIR=\""$(datadir)/reciteword"\"		\
	-DRECITEWORD_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

reciteword_LDFLAGS = 

reciteword_LDADD = 				\
	$(RECITEWORD_LIBS)			

reciteword_SOURCES = 			\
	about.cpp			\
	about.h			\
	bookfile.cpp			\
	bookfile.h			\
	choosebook.cpp	\
	choosebook.h	\
	rwconfig.cpp		\
	rwconfig.h		\
	configfile.cpp 		\
	configfile.h 		\
	chooseword.cpp 			\
	chooseword.h 			\
	collectword.cpp			\
	collectword.h			\
	dict.cpp			\
	dict.h			\
	firstrecite.cpp 			\
	firstrecite.h 			\
	firstrecite_test.cpp 			\
	firstrecite_test.h 			\
	help.cpp		\
	help.h			\
	intl.h			\
	mark.cpp		\
	mark.h			\
	menu.cpp 			\
	menu.h 			\
	mydict.cpp		\
	mydict.h		\
	reciteword.cpp			\
	reciteword.h			\
	record.cpp		\
	record.h		\
	revise.cpp		\
	revise.h		\
	revise_test.cpp	\
	revise_test.h	\
	rest.cpp		\
	rest.h			\
	readword.cpp		\
	readword.h		\
	know.cpp		\
	know.h		\
	option.cpp option.h	\
	shooting.cpp			\
	shooting.h			\
	skin.cpp			\
	skin.h			\
	sound.cpp			\
	sound.h			\
	sndserv.cpp		\
	sndserv.h		\
	typing.cpp		\
	typing.h		\
	widget.cpp		\
	widget.h

EXTRA_DIST =
