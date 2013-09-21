LIBS=  -g  -Wl,--enable-new-dtags -Wl,-rpath, -lpthread  
INC= -g   -D__LINUX__ -c -Wall -I./ -Wall -O2
CC=gcc
EXE=repobuilder
$(EXE):inihelper.o loghelper.o main.o configserver.o nethelper.o main.o inihelper.o loghelper.o   cmthread.o daemon.o
	$(CC) -o $(EXE) $(LIBS) inihelper.o  nethelper.o loghelper.o main.o configserver.o  cmthread.o daemon.o
main.o:main.c *.h
	$(CC) $(INC) main.c 
configserver.o:configserver.c *.h
	$(CC) $(INC) configserver.c
loghelper.o:loghelper.c *.h
	$(CC) $(INC) loghelper.c
inihelper.o:inihelper.c *.h
	$(CC) $(INC) inihelper.c
cmthread.o:cmthread.c *.h
	$(CC) $(INC) cmthread.c
nethelper.o:nethelper.c *.h
	$(CC) $(INC) nethelper.c
daemon.o:daemon.c *.h
	$(CC) $(INC) daemon.c
clean:
	rm -fr *.o $(EXE)
