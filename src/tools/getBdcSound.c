// read bdc's word.idx and word.wav and output echo word's wave file.
// the crack algorithm is come from http://www.chinafoxsoft.net; sound data file structure crack by Alex Jiang,myBdc develop by Fox Jiang.
// this program is develop by huzheng <huzheng_001@163.com> http://forlinux.yeah.net

//note: this crack algorithm still have some fault,make only 20000 words of the totol 30000 words can be check out.

#include "stdio.h"
#include "stdlib.h"
#include <sys/stat.h>
#include "string.h"
#include "ctype.h"

unsigned char *get_encryt_str(unsigned char *str)
{
	unsigned char c3[14]={13,10,26,128,127,128,128,40,121,127,128,128,123,07};	
	static unsigned char c2[21],b1[21];
	int i,j,len;	
	
	strcpy((char *)b1,(char *)str);
	len = strlen((char *)b1);

	if (len==1)
    {
    	len++;
		b1[1]=b1[0];
		b1[2]='\0';
    }

  j=0;
  c2[j]=32;
  for (i=1;((i<=len) && (j<20)) ;i++)
    {
    j++;
    switch (i)
      {
      case 1:;
      case 3:;
      case 5:;
      case 7:;
      case 9:;
      case 11:;
      case 13:;
      case 15:;
      case 17:;
      case 19:c2[j]=(char)(b1[i-1]-30);break;

      case 2:;
      case 4:;
      case 6:;
      case 8:;
      case 10:;
      case 12:;
      case 14:;
      case 16:;
      case 18:;
      case 20:c2[j]=(char)(128-b1[i-1]);break;
      }
    }
  if (j<20)
    {
    j++;
    c2[j]=7;
    }
    for (i=1;((i<=len) && (j<20));i++)
      {
      j++;
      switch (i)
        {
        case 1:c2[j]=(char)(c2[i]+128);break;
        case 2:c2[j]=(char)(c2[i]+127);break;
        case 3:c2[j]=(char)(c2[i]+128);break;
        case 4:c2[j]=(char)(c2[i]+128);break;
        case 5:c2[j]=(char)(c2[i]+127);break;
        case 6:c2[j]=(char)(c2[i]+40);break;
        case 7:c2[j]=(char)(c2[i]+121);break;
        case 8: c2[j]=(char)(c2[i]+81);break;
        case 9: c2[j]=(char)(c2[i]+127);break;
        }
      }
    if (j<20)
      {
      j++;
      switch (j)
        {
        case 6:c2[j]=135;break;
        case 8:c2[j]=135;break;
        case 10:c2[j]=134;break;
        case 12:c2[j]=47;break;
        case 14:c2[j]=128;break;
        case 16:c2[j]=88;break;
        case 18:c2[j]=135;break;
        case 20:c2[j]=136;break;
        }
      }
    i=0;
    j++;
    for (;(j<=20);j++)
      {
      c2[j]=c3[i];
      i++;
      }
	return c2;
}

unsigned char *get_word_str(unsigned char *idxbuf)
{
	unsigned char b1[21],b2[20];
	static unsigned char c2[20];
	int i,j;	
	
	memcpy(b1,idxbuf,21);
	j=0;
  	for (i=1;j<=20;i++)
    {
		if (b1[i]==7) //may be means str end or 'y'
		{
			int equal,k;
			unsigned char *str;
			memcpy(b2,c2,j);
			b2[j] = '\0';			
			str = get_encryt_str(b2);
			equal = 1;
			for (k=0;k<=20;k++)
			{
				if (str[k]!=idxbuf[k])
				{
					equal = 0;
					printf("find word %s at %d :",b2,j);
					printf("not match at %d: %d %d\n",k,idxbuf[k],str[k]);
					break;
				}
			}
			if (equal)
			{
				c2[j] = '\0';
				return c2;
			}
		}
    switch (i)
      {
		case 1:;
		case 3:;
		case 5:;
		case 7:;
		case 9:;
		case 11:;
		case 13:;
		case 15:;
		case 17:;
		case 19:c2[j]=(char)(b1[i]+30);break;

      case 2:;
      case 4:;
      case 6:;
      case 8:;
      case 10:;
      case 12:;
      case 14:;
      case 16:;
      case 18:;
      case 20:c2[j]=(char)(128-b1[i]);break;
      }
	  j++;
    }	
	c2[19] = '\0';
	return c2;
}

int main(int argc,char * argv [])
{
	FILE *wordidxFile,*wordwavFile,*wordFile;
	int i,wordcount,wavbegin,wavweight;
	unsigned char idxbuf[29],*wav;
	char wavfilename[64],*str,dirchar;
	struct stat st;
	
	if (argc!=3)
	{
		printf("please give the path of Word.idx and word.wav!\n");
		return 0;
	}
	if (stat ("BdcSound", &st))
	{
		mkdir("BdcSound",00755);
		mkdir("BdcSound/a",00755);mkdir("BdcSound/b",00755);mkdir("BdcSound/c",00755);mkdir("BdcSound/d",00755);mkdir("BdcSound/e",00755);
		mkdir("BdcSound/f",00755);mkdir("BdcSound/g",00755);mkdir("BdcSound/h",00755);mkdir("BdcSound/i",00755);mkdir("BdcSound/j",00755);
		mkdir("BdcSound/k",00755);mkdir("BdcSound/l",00755);mkdir("BdcSound/m",00755);mkdir("BdcSound/n",00755);mkdir("BdcSound/o",00755);
		mkdir("BdcSound/p",00755);mkdir("BdcSound/q",00755);mkdir("BdcSound/r",00755);mkdir("BdcSound/s",00755);mkdir("BdcSound/t",00755);
		mkdir("BdcSound/u",00755);mkdir("BdcSound/v",00755);mkdir("BdcSound/w",00755);mkdir("BdcSound/x",00755);mkdir("BdcSound/y",00755);
		mkdir("BdcSound/z",00755);
	}
	
	wordidxFile = fopen(argv[1],"r");
	wordwavFile = fopen(argv[2],"r");
	stat(argv[1], &st);
	wordcount = (st.st_size - 129)/29;
	printf("find %d words\n",wordcount);
	for (i=0;i<wordcount;i++)
	{
		fseek(wordidxFile,i*29+129,0);
		fread(idxbuf,sizeof(idxbuf),1,wordidxFile);
		wavbegin=((((((idxbuf[24]*256)+idxbuf[23])*256)+idxbuf[22])*256)+idxbuf[21]);
    	wavweight=((((((idxbuf[28]*256)+idxbuf[27])*256)+idxbuf[26])*256)+idxbuf[25])-wavbegin;
		wav = malloc(wavweight+0x3a);	
	    wav[0]=0x52;
        wav[1]=0x49;
        wav[2]=0x46;
        wav[3]=0x46;
        wav[4]= ((wavweight+0x32) & 0x000000ff);
        wav[5]= (((wavweight+0x32) & 0x00ff00)>>8);
        wav[6]= (((wavweight+0x32) & 0x00ff0000)>>16);
        wav[7]= (((wavweight+0x32) & 0xff000000)>>24);
        wav[8]=0x57;
        wav[9]=0x41;
        wav[10]=0x56;
        wav[11]=0x45;
        wav[12]=0x66;
        wav[13]=0x6d;
        wav[14]=0x74;
        wav[15]=0x20;
        wav[16]=0x12;
        wav[17]=0x00;
        wav[18]=0x00;
        wav[19]=0x00;
        wav[20]=0x01;
        wav[21]=0x00;
        wav[22]=0x01;
        wav[23]=0x00;
        wav[24]=0x11;
        wav[25]=0x2b;
        wav[26]=0x00;
        wav[27]=0x00;
        wav[28]=0x11;
        wav[29]=0x2b;
        wav[30]=0x00;
        wav[31]=0x00;
        wav[32]=0x01;
        wav[33]=0x00;
        wav[34]=0x08;
        wav[35]=0x00;
        wav[36]=0x00;
        wav[37]=0x00;
        wav[38]=0x66;
        wav[39]=0x61;
        wav[40]=0x63;
        wav[41]=0x74;
        wav[42]=0x04;
        wav[43]=0x00;
        wav[44]=0x00;
        wav[45]=0x00;
        wav[46]= ((wavweight) & 0x000000ff);
        wav[47]= (((wavweight) & 0x0000ff00)>>8);
        wav[48]= (((wavweight) & 0x00ff0000)>>16);
        wav[49]= (((wavweight) & 0xff000000)>>24);
        wav[50]=0x64;
        wav[51]=0x61;
        wav[52]=0x74;
        wav[53]=0x61;
        wav[54]= ((wavweight) & 0x000000ff);
        wav[55]= (((wavweight) & 0x0000ff00)>>8);
        wav[56]= (((wavweight) & 0x00ff0000)>>16);
        wav[57]= (((wavweight) & 0xff000000)>>24);		
		fseek(wordwavFile,wavbegin,0);
		fread(&wav[0x3a], wavweight,1,wordwavFile);
		str = (char *)get_word_str(idxbuf);
		dirchar = tolower(str[0]);	
		sprintf(wavfilename,"BdcSound/%c/%s.wav",dirchar,str);
		if (!stat(wavfilename, &st))
			printf("word file overwrite: %s\n",wavfilename);
		wordFile = fopen(wavfilename,"w");
		fwrite(wav,wavweight+0x3a,1,wordFile);
		free(wav);
		fclose(wordFile);
	}
	fclose(wordwavFile);
	fclose(wordidxFile);
	printf("done!\n");
	
	return 0;
}
