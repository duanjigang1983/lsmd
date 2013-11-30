#author:duanjigang@20070318<duanjigang1983@126.com>
#add runtime to shut down illegal task @ 2008-07-29 by duanjigang
#updated to added it as an example for multipkg  @ 2013-11-30 by duanjigang
#!/bin/sh
tskeeper:procKeeper.o process.o
	gcc -o tskeeper procKeeper.o process.o
procKeeper.o:procKeeper.c
	gcc -c procKeeper.c
process.o:process.c
	gcc -c process.c
clean:
	rm -fr *.o tskeeper

