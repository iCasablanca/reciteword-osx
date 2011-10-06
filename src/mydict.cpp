#include "mydict.h"

mydict::mydict(const char *path)
{
   char filepath[1048];
   working = true;
   sprintf(filepath,"%s/xdict.lib",path);
   offset=0;cont=0; num=0;
   f1=fopen(filepath,"rb");
   if(f1!=NULL)
     fread(buf,MAXLEN,1,f1);
   else 
   {
     fprintf(stderr,"%s\n","Cannot open dictionary file!");
     //exit(-1);
	 working = false;
	 return;
   }
   sprintf(filepath,"%s/xdicten.idx",path);
   en=fopen(filepath,"rb");
   if(en!=NULL)
   {
      fread(&indexnum,2,1,en);
      fread(&indexlen,4,1,en);
      fread(indexitem,indexlen,1,en);
      fread(indexoffset,indexnum*4,1,en);
      fclose(en);
   }
   else
   {
      fprintf(stderr,"%s\n","Cannot open index file!");
      fclose(f1);
	   working = false;
	   return;
      //exit(-1);
   } 
}

mydict::~mydict()
{
   if(f1!=NULL)
      fclose(f1);
}

void mydict::mystrcpy(char *dest,char *src)
{
   int i,len;
   unsigned char *s;
   char *ln="\xa";
   const char *tmp;
   static char * prop[]={" "," "," ","a. ","ad. ","art. ","conj. ","int. ","n. "," "," ","num. ",
              "prep. "," ","pron. ","v. ","aux. ","vi. ","vt. ","vbl. "," ","st. ",
              "pr. ","pp. ","pl. ","ing. "," ","adj. ","adv. ","pla. ","pn. "," "};
   s=(unsigned char *)src;
   len=strlen(src);
   for(i=0;i<len;i++)
   {
     if(s[i]<0x20)
     {
         tmp=prop[int(src[i])];
         if(i>0) strcat(dest,ln);
         strcat(dest,tmp); //(const char *)prop[int(src[i])]);
     }
     else strncat(dest,(src+i),1);
   }
}
//--------------------------------------
long mydict::nextitem(char *itemEnglish,char *itemMark,char *itemChinese)
{
	if (!working) return 0;
   int i,status;
   int k;
   char item1[512],item2[30],item4[512];
   char tmp[512];
   if(f1==NULL ) return 0;
   if((cont+1024)>MAXLEN)
   {
     offset=offset+long(cont);
     if((fseek(f1,offset,0))!=0) return 0;
     k=fread(buf,1,MAXLEN,f1);
     if(k<MAXLEN)
     {
       buf[k]=buf[k+1]=0x0;
       if(k<1)
         return -1;  //end of file
     }
     cont=0;
   }
   i=0; status=0; item1[0]=item2[0]=item4[0]=0x0;
   while(1)
   {
      switch(status)
      {
         case 0:
                if(buf[cont]==0x0 && buf[cont+1]==0x0) return -1;
                if(buf[cont]==0x0) { cont++; i=0; status=1; }
                else cont++;
                break;
         case 1:
                if(buf[cont]==0x1)
                { item1[i]=0x0; cont++; i=0; status=2; }
                else
                { item1[i]=buf[cont]; cont++; i++; if(cont>=MAXLEN) goto err; }
                break;
         case 2:
                if(buf[cont]>0x2)
                { tmp[i]=buf[cont]; cont++; i++; if(cont>=MAXLEN) goto err;}
                else if(buf[cont]==0x0)
                { tmp[i]=0x0; mystrcpy(item4,tmp);   goto out1; }
                else if(buf[cont]==0x2)
                {
                  tmp[i]=0x0; sprintf(item2,"[%s]",tmp); i=0; cont++;
                  status=3;
                }
                break;
         case 3:
                if(buf[cont]==0x0)
                { tmp[i]=0x0; mystrcpy(item4,tmp);  goto out1; }
                else
                { tmp[i]=buf[cont]; i++; cont++; if(cont>=MAXLEN) goto err;}
                break;
      }
   }
out1:
   num++;
   sprintf(itemEnglish,"%s",item1);
   sprintf(itemMark,"%s",item2);
   sprintf(itemChinese,"%s",item4);
   return num;
err:
   return -1;
}
//---------------------------------------
long mydict::previtem(char *itemEnglish,char *itemMark,char *itemChinese)
{
	if (!working) return 0;
   int i,status;
   int k;
   char item1[512],item2[30],item4[512];
   char tmp[512];
   if(f1==NULL) return 0;
   if(cont<1024 && offset>0)
   {
     if(offset<MAXLEN){ cont=int(offset)+cont; offset=0; }
     else {offset=offset+long(cont)-MAXLEN; cont=MAXLEN;}
     if((fseek(f1,offset,0))!=0) return 0;
     k=fread(buf,1,MAXLEN,f1);
     if(k<MAXLEN)
     {
       buf[k]=buf[k+1]=0x0;
       if(k<1)
         return -1;  //end of file
     }
     //cont=0;
   }
   if(cont<=0) return -1;
   while(1) //back to prev item
   {
      //if(offset==0)
      //{
        if(cont<=0) break;
        if(buf[cont-1]==0) { cont--; break; }
        else cont--;
      //}
   }
   while(1) //back to prev item
   {
      //if(offset==0)
      //{
        if(cont<=0) break;
        if(buf[cont-1]==0) { cont--; break; }
        else cont--;
      //}
   }
   if(cont==0 && offset==0) num=1;
   else num--;
   i=0; status=0; item1[0]=item2[0]=item4[0]=0x0;

   while(1)
   {
      switch(status)
      {
         case 0:
                if(buf[cont]==0x0 && buf[cont+1]==0x0) return -1;
                if(buf[cont]==0x0) { cont++; i=0; status=1; }
                else cont++;
                break;
         case 1:
                if(buf[cont]==0x1)
                { item1[i]=0x0; cont++; i=0; status=2; }
                else
                { item1[i]=buf[cont]; cont++; i++; if(cont>=MAXLEN) goto err; }
                break;
         case 2:
                if(buf[cont]>0x2)
                { tmp[i]=buf[cont]; cont++; i++; if(cont>=MAXLEN) goto err;}
                else if(buf[cont]==0x0)
                { tmp[i]=0x0; mystrcpy(item4,tmp);   goto out1; }
                else if(buf[cont]==0x2)
                {
                  tmp[i]=0x0; sprintf(item2,"[%s]",tmp); i=0; cont++;
                  status=3;
                }
                break;
         case 3:
                if(buf[cont]==0x0)
                { tmp[i]=0x0; mystrcpy(item4,tmp);  goto out1; }
                else
                { tmp[i]=buf[cont]; i++; cont++; if(cont>=MAXLEN) goto err;}
                break;
      }
   }
out1:
   //num--;
   sprintf(itemEnglish,"%s",item1);
   sprintf(itemMark,"%s",item2);
   sprintf(itemChinese,"%s",item4);
   return num;
err:
   return -1;
}

int mydict::query(const char *word)
{
	if (!working) return 0;
   int i;
   long index;
   const char *tmp;
   char item1[512],item2[30],item3[512];
   tmp=word;
   if(tmp==NULL) return 0;
   for(i=0;i<indexnum;i++)
   {
     if(strcasecmp(tmp,indexitem[i])<=0) break;
   }
   if(i==indexnum) i--;
   if(offset!=indexoffset[i])
   {
     offset=indexoffset[i];
     fseek(f1,offset,0);
     fread(buf,MAXLEN,1,f1);
   }
   cont=0;
   nextitem(item1,item2,item3);
   previtem(item1,item2,item3);
   while(1)
   {
     if(strcasecmp(tmp,item1)<=0) break;
     index=nextitem(item1,item2,item3);
     if(index<=0) break;
   }
   previtem(nearEnglishItem[0],nearMarkItem[0],nearChineseItem[0]);
   for(i=1;i<10;i++)
   {
     index=nextitem(nearEnglishItem[i],nearMarkItem[i],nearChineseItem[i]);
     if(index<=0) break;
   }
   nearNum=i;
   for(;i>1;i--)
   {
     index=previtem(item1,item2,item3);
     if(index<=0) break;
   }
   return nearNum;
}

