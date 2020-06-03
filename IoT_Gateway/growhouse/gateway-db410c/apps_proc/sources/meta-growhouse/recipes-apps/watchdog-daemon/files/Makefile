#
#  Makefile for compilation of growhouse-server
#  Copyright (c) 2018
#

all : watchdog-daemon

watchdog-daemon : watchdog_daemon.o
	${CC} $^ -o watchdogDaemon $(LINKER_FLAGS)

watchdog-daemon.o : watchdog_daemon.c
	${CC} -c $< -o $@

clean :
	-rm -f *.o watchdogDaemon

#
#  End Makefile
#
