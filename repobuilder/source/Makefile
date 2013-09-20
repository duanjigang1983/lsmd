LIBS=  -g  -Wl,--enable-new-dtags -Wl,-rpath, -lpthread  
INC= -g   -D__LINUX__ -c -Wall -I./ -Wall -O2
CC=gcc
EXE=repobuilder
$(EXE):IniHelper.o LogHelper.o main.o configserver.o NetHelper.o \
	IniHelper.o LogHelper.o   cmthread.o comm_utils.o  daemon.o
	$(CC) -o $(EXE) $(LIBS) IniHelper.o  NetHelper.o \
	LogHelper.o main.o configserver.o  cmthread.o comm_utils.o  daemon.o
cmdserver.o:main.c *.h
	$(CC) $(INC) main.c
configserver.o:configserver.c *.h
	$(CC) $(INC) configserver.c
LogHelper.o:LogHelper.c *.h
	$(CC) $(INC) LogHelper.c
IniHelper.o:IniHelper.c *.h
	$(CC) $(INC) IniHelper.c
cmthread.o:cmthread.c *.h
	$(CC) $(INC) cmthread.c
NetHelper.o:NetHelper.c *.h
	$(CC) $(INC) NetHelper.c
comm_utils.o:comm_utils.c *.h
	$(CC) $(INC) comm_utils.c
daemon.o:daemon.c *.h
	$(CC) $(INC) daemon.c
clean:
	rm -fr *.o $(EXE)
