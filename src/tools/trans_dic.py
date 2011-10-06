#!/usr/bin/python
  
#This program changes the WordsLoverWordsLib's *.bok file into the kind of
#recitewordbookfile. 

#Usage: "trans_dic.py cet-4.bok"  or "trans_dic.py *.bok" 
#Notice: You must have python installed!
#        File name must be *.bok


import sys
import os
import errno
import string

def tans_file(desc,src) :
#deal with the 0th line
    line_buffer=src.readline()
    tmp="[H]WordsLoverWordsLib"
    re="[H]recitewordbookfile"
    if line_buffer[0:len(re)] == re :
        os.remove(src.name+".tmp")
        return
    if line_buffer[0:len(tmp)] != tmp :
        print "%s Not a valid WordsLover file!" % src.name
        sys.exit(1)
    if line_buffer[-2:] == "\r\n":
        line_buffer=re + line_buffer[len(tmp):-2] + "[A]come from wordslover"+"\r\n"
    elif line_buffer[-1:]=="\n":
        line_buffer=re + line_buffer[len(tmp):-1] + "[A]come from wordslover"+"\r\n"
    desc.write(line_buffer)  
#deal with the 1th line of word
    line_buffer=f.readline()
    line_buffer=line_buffer[4:]
    while 1:
        line_buffer=src.readline()
        if line_buffer == '':
            break
        desc.write(line_buffer)
    src.close()
    desc.close()
    os.remove(src.name)
    os.rename(desc.name,src.name)
#    return 1
########################################################

if __name__ == "__main__" :
    if len(sys.argv) > 1:
        for i in sys.argv:
            if i == sys.argv[0]:
                continue
            try:
            	f=open(i,"rb")
            except:
                x=sys.exc_info()
                print "Can't Open file %s!" % (sys.argv[1],)
                sys.exit(1)
            t=open(i+".tmp","w+")
            tans_file(t,f)
            
