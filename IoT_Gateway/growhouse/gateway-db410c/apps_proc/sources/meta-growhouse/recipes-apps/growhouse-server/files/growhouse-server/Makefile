#
#  Makefile for compilation of growhouse-server
#  Copyright (c) 2018
#

all : growhouse-server

growhouse-server : growhouse-server.o factory-reset-monitor.o end-device-monitor.o
	${CC} $^ -o $@ -lmosquitto -lcjson -lpthread -lrt $(LINKER_FLAGS)

growhouse-server.o : growhouse-server.c
	${CC} -c $< -o $@
factory-reset-monitor.o : factory-reset-monitor.c
	${CC} -c $< -o $@
end-device-monitor.o : end-device-monitor.c
	${CC} -c $< -o $@

clean :
	-rm -f *.o growhouse-server

#
#  End Makefile
#
