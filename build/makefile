#				Make main process of mssw.

#	Macro with gcc,set std=c99.
CC=c99

#	Finally traget
all:mssw

#	Make binary file mssw
mssw:mssw-main.o mssw.o

#	Make object file mssw-main.o and mssw.o
mssw-main.o:mssw-main.c mssw.h

mssw.o:mssw.c mssw.h

#	Move mssw to ../bin/main/
mvdir:
	-mv mssw ../bin/main/

#	Label,fake target
.PHONY:clean
clean:
	-rm -f *.o

