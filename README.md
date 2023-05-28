Makefile works for linux. 

on mac can compile like this:

gcc -undefined suppress -flat_namespace -bundle  -arch x86_64 -mmacosx-version-min=10.6  -o pparp.pd_darwin pparp.c  -lc
