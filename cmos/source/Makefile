#test:tcpserver -l 0 -R  -H  -X 0 43698 ./cmos -f test-module
#CFLAGS=-Wall -c -Werror -g -static -static-libgcc
CFLAGS=-Wall -c -Werror -g
#LDFLAGS=libreadline.a libncurses.a -static
#LDFLAGS=libreadline.a libncurses.a   -static
#LDFLAGS=-static 
#LDFLAGS=-lreadline -lncurses -static -static-libgcc
#LDFLAGS=-lreadline -lncurses -static
timeout:
	gcc -g -Wall  -static-libgcc timeout.c -o cmos-timeout
	#gcc -g -Wall  -static-libgcc -static timeout.c -o timeout
cmos:
	gcc  $(CFLAGS)  cmos.c
	gcc  $(CFLAGS)  cm_help.c
	gcc  $(CFLAGS)  cm_opt.c
	gcc  $(CFLAGS)  cm_ls.c
	gcc  $(CFLAGS)  cm_signal.c
	gcc  $(CFLAGS)  cm_wizard.c
	gcc  $(CFLAGS)  cm_settings.c
	gcc  $(CFLAGS)  cm_command.c
	gcc  $(CFLAGS)  cm_loadtree.c
	gcc  $(CFLAGS)  readline_utils.c
	gcc  $(CFLAGS)  IniHelper.c
	gcc  $(CFLAGS)  rbtree.c
	gcc  $(CFLAGS)  cm_utils.c
	gcc -o cmos cmos.o cm_help.o cm_opt.o readline_utils.o cm_wizard.o cm_ls.o cm_utils.o cm_command.o cm_signal.o IniHelper.o rbtree.o cm_loadtree.o cm_settings.o $(LDFLAGS)
clean:
	rm -fr *.o cmos cmos-timeout

