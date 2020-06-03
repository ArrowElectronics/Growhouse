#
#  Makefile for compilation of ble-server
#  Copyright (c) 2018
#

all : bleServer bleApp

bleServer : bleServer.o
	${CC} $^ -o $@ -lmosquitto -lcjson

bleApp : bleApp.o
	${CC} $^ -o $@ -lmosquitto -lcjson

bleServer.o : bleServer.c
	${CC} -c $< -o $@

bleApp.o : bleApp.c
	${CC} -c $< -o $@

clean :
	rm -f *.o bleServer bleApp

#
#  End Makefile
#
