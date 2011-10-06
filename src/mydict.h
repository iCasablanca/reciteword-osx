#ifndef __MYDICT_H__
#define __MYDICT_H__
#include <stdio.h>
#include <string.h>
#define MAXLEN 32764
class mydict
{
 private: 
  FILE *f1,*en;
  unsigned char buf[MAXLEN];
  long offset;
  int cont;
  long num;
  char nearEnglishItem[10][512];  
  char nearMarkItem[10][30];  
  char nearChineseItem[10][512];
  int nearNum;  
  int indexnum;
  long indexlen;
  char indexitem[200][16];
  long indexoffset[200];
  void mystrcpy(char *dest,char *src);
 public: 
  int working;
  mydict(const char *path);
  ~mydict();
  long nextitem(char *itemEnglish,char *itemMark,char *itemChinese);
  long previtem(char *itemEnglish,char *itemMark,char *itemChinese);
  int query(const char *word);
  int getNearNum() { return nearNum; }
  const char *getNearEnglishItem(int k) { if (!working) return NULL; if(k>=nearNum) return NULL; return nearEnglishItem[k]; }
  const char *getNearMarkItem(int k) { if (!working) return NULL; if(k>=nearNum) return NULL; return nearMarkItem[k]; }
  const char *getNearChineseItem(int k) { if (!working) return NULL; if(k>=nearNum) return NULL; return nearChineseItem[k]; }
};

#endif
